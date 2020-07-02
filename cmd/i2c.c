// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2009
 * Sergey Kubushyn, himself, ksi@koi8.net
 *
 * Changes for unified multibus/multiadapter I2C support.
 *
 * (C) Copyright 2001
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com.
 */

/*
 * I2C Functions similar to the standard memory functions.
 *
 * There are several parameters in many of the commands that bear further
 * explanations:
 *
 * {i2c_chip} is the I2C chip address (the first byte sent on the bus).
 *   Each I2C chip on the bus has a unique address.  On the I2C data bus,
 *   the address is the upper seven bits and the LSB is the "read/write"
 *   bit.  Note that the {i2c_chip} address specified on the command
 *   line is not shifted up: e.g. a typical EEPROM memory chip may have
 *   an I2C address of 0x50, but the data put on the bus will be 0xA0
 *   for write and 0xA1 for read.  This "non shifted" address notation
 *   matches at least half of the data sheets :-/.
 *
 * {addr} is the address (or offset) within the chip.  Small memory
 *   chips have 8 bit addresses.  Large memory chips have 16 bit
 *   addresses.  Other memory chips have 9, 10, or 11 bit addresses.
 *   Many non-memory chips have multiple registers and {addr} is used
 *   as the register index.  Some non-memory chips have only one register
 *   and therefore don't need any {addr} parameter.
 *
 *   The default {addr} parameter is one byte (.1) which works well for
 *   memories and registers with 8 bits of address space.
 *
 *   You can specify the length of the {addr} field with the optional .0,
 *   .1, or .2 modifier (similar to the .b, .w, .l modifier).  If you are
 *   manipulating a single register device which doesn't use an address
 *   field, use "0.0" for the address and the ".0" length field will
 *   suppress the address in the I2C data stream.  This also works for
 *   successive reads using the I2C auto-incrementing memory pointer.
 *
 *   If you are manipulating a large memory with 2-byte addresses, use
 *   the .2 address modifier, e.g. 210.2 addresses location 528 (decimal).
 *
 *   Then there are the unfortunate memory chips that spill the most
 *   significant 1, 2, or 3 bits of address into the chip address byte.
 *   This effectively makes one chip (logically) look like 2, 4, or
 *   8 chips.  This is handled (awkwardly) by #defining
 *   CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW and using the .1 modifier on the
 *   {addr} field (since .1 is the default, it doesn't actually have to
 *   be specified).  Examples: given a memory chip at I2C chip address
 *   0x50, the following would happen...
 *     i2c md 50 0 10   display 16 bytes starting at 0x000
 *                      On the bus: <S> A0 00 <E> <S> A1 <rd> ... <rd>
 *     i2c md 50 100 10 display 16 bytes starting at 0x100
 *                      On the bus: <S> A2 00 <E> <S> A3 <rd> ... <rd>
 *     i2c md 50 210 10 display 16 bytes starting at 0x210
 *                      On the bus: <S> A4 10 <E> <S> A5 <rd> ... <rd>
 *   This is awfully ugly.  It would be nice if someone would think up
 *   a better way of handling this.
 *
 * Adapted from cmd_mem.c which is copyright Wolfgang Denk (wd@denx.de).
 */
#include <common.h>
#include <cli.h>
#include <command.h>
#include <errno.h>
#include <module/i2c.h>
#include <linux/compiler.h>

#define DEFAULT_ADDR_LEN	1

/* Display values from last command.
 * Memory modify remembered values are different from display memory.
 */
static uint	i2c_dp_last_chip;
static uint	i2c_dp_last_addr;
static uint	i2c_dp_last_alen;
static uint	i2c_dp_last_length = 0x10;

#define DISP_LINE_LEN	16

/**
 * get_alen() - Small parser helper function to get address length
 *
 * Returns the address length.
 */
static uint get_alen(char *arg, int default_len)
{
	int	j;
	int	alen;

	alen = default_len;
	for (j = 0; j < 8; j++) {
		if (arg[j] == '.') {
			alen = arg[j+1] - '0';
			break;
		} else if (arg[j] == '\0')
			break;
	}
	return alen;
}

/**
 * do_i2c_bus_num() - Handle the "i2c dev" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 */
static int do_i2c_bus_num(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
    int i = 0, ret;

    if (argc == 1) {
        printf("Current bus is %d\n", i2c_get_cur_bus());
    } else {
        i = simple_strtoul(argv[1], NULL, 10);
        ret = i2c_set_cur_bus(i);
        if (ret) {
            printf("%s\n", errno_str(ret));
            return CMD_RET_USAGE;
        }
    }
    
    return CMD_RET_SUCCESS;
}

/**
 * do_i2c_md() - Handle the "i2c md" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *	i2c md {i2c_chip} {addr}{.0, .1, .2} {len}
 */
static int do_i2c_md ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint	chip;
	uint	addr, length;
	int alen;
	int	j, nbytes, linebytes;
	int ret;

	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	chip   = i2c_dp_last_chip;
	addr   = i2c_dp_last_addr;
	alen   = i2c_dp_last_alen;
	length = i2c_dp_last_length;

	if (argc < 3)
		return CMD_RET_USAGE;

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/*
		 * New command specified.
		 */

		/*
		 * I2C chip address
		 */
		chip = simple_strtoul(argv[1], NULL, 16);

		/*
		 * I2C data address within the chip.  This can be 1 or
		 * 2 bytes long.  Some day it might be 3 bytes long :-).
		 */
		addr = simple_strtoul(argv[2], NULL, 16);
		alen = get_alen(argv[2], DEFAULT_ADDR_LEN);
		if (alen > 3)
			return CMD_RET_USAGE;

		/*
		 * If another parameter, it is the length to display.
		 * Length is the number of objects, not number of bytes.
		 */
		if (argc > 3)
			length = simple_strtoul(argv[3], NULL, 16);
	}

	/*
	 * Print the lines.
	 *
	 * We buffer all read data, so we can make sure data is read only
	 * once.
	 */
	nbytes = length;
	do {
		unsigned char	linebuf[DISP_LINE_LEN];
		unsigned char	*cp;

		linebytes = (nbytes > DISP_LINE_LEN) ? DISP_LINE_LEN : nbytes;

		ret = i2c_read(chip, addr, alen, linebuf, linebytes);

		if (ret) {
			pr_err("i2c%d %s read failed\n", i2c_get_cur_bus(), i2c_get_adap_name());
			return CMD_RET_FAILURE;
		}
		else {
			printf("%04x:", addr);
			cp = linebuf;
			for (j=0; j<linebytes; j++) {
				printf(" %02x", *cp++);
				addr++;
			}
			puts ("    ");
			cp = linebuf;
			for (j=0; j<linebytes; j++) {
				if ((*cp < 0x20) || (*cp > 0x7e))
					puts (".");
				else
					printf("%c", *cp);
				cp++;
			}
			puts("\n\r");
		}
		nbytes -= linebytes;
	} while (nbytes > 0);

	i2c_dp_last_chip   = chip;
	i2c_dp_last_addr   = addr;
	i2c_dp_last_alen   = alen;
	i2c_dp_last_length = length;

	return 0;
}

/**
 * do_i2c_mw() - Handle the "i2c mw" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *	i2c mw {i2c_chip} {addr}{.0, .1, .2} {data} [{count}]
 */
static int do_i2c_mw ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint	chip;
	ulong	addr;
	int	alen;
	uchar	byte;
	int	count;
	int ret;

	if ((argc < 4) || (argc > 5))
		return CMD_RET_USAGE;

	/*
	 * Chip is always specified.
	 */
	chip = simple_strtoul(argv[1], NULL, 16);

	/*
	 * Address is always specified.
	 */
	addr = simple_strtoul(argv[2], NULL, 16);
	alen = get_alen(argv[2], DEFAULT_ADDR_LEN);
	if (alen > 3)
		return CMD_RET_USAGE;

	/*
	 * Value to write is always specified.
	 */
	byte = simple_strtoul(argv[3], NULL, 16);

	/*
	 * Optional count
	 */
	if (argc == 5)
		count = simple_strtoul(argv[4], NULL, 16);
	else
		count = 1;

	while (count-- > 0) {
		ret = i2c_write(chip, addr++, alen, &byte, 1);

		if (ret) {
			pr_err("i2c%d %s read failed\n", i2c_get_cur_bus(), i2c_get_adap_name());
			return CMD_RET_FAILURE;
		}
		/*
		 * Wait for the write to complete.  The write can take
		 * up to 10mSec (we allow a little more time).
		 */
		udelay(20);
	}

	return 0;
}

/**
 * do_i2c_probe() - Handle the "i2c probe" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *	i2c probe {addr}
 *
 * Returns zero (success) if one or more I2C devices was found
 */
static int do_i2c_probe (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int j;
	int addr = -1;
	int found = 0;
	int ret;

	if (argc == 2)
		addr = simple_strtol(argv[1], 0, 16);

	puts ("Valid chip addresses:");
	for (j = 0; j < 256; j++) {
		if ((0 <= addr) && (j != addr))
			continue;

		ret = i2c_probe(j);
		if (ret == 0) {
			printf(" %02X", j);
			found++;
		}
	}
	puts("\n\r");
	return (0 == found);
}

/**
 * do_i2c_read() - Handle the "i2c read" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 *
 * Syntax:
 *	i2c read {i2c_chip} {devaddr}{.0, .1, .2} {len} {memaddr}
 */
static int do_i2c_read ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    uint	chip;
	uint	devaddr, length;
	int alen;
	u_char  *memaddr;
	int ret;
    int cur_bus;

    if (argc != 5)
        return CMD_RET_USAGE;

    cur_bus = i2c_get_cur_bus();
	/*
	 * I2C chip address
	 */
	chip = simple_strtoul(argv[1], NULL, 16);

	/*
	 * I2C data address within the chip.  This can be 1 or
	 * 2 bytes long.  Some day it might be 3 bytes long :-).
	 */
	devaddr = simple_strtoul(argv[2], NULL, 16);
	alen = get_alen(argv[2], DEFAULT_ADDR_LEN);
	if (alen > 3)
		return CMD_RET_USAGE;

	/*
	 * Length is the number of objects, not number of bytes.
	 */
	length = simple_strtoul(argv[3], NULL, 16);

	/*
	 * memaddr is the address where to store things in memory
	 */
	memaddr = (u_char *)simple_strtoul(argv[4], NULL, 16);

    ret = i2c_read(chip, devaddr, alen, memaddr, length);

    if (ret) {
        pr_err("i2c%d %s read failed\n", i2c_get_cur_bus(), i2c_get_adap_name());
        return CMD_RET_FAILURE;
    } else
        printf("i2c%d %s read success\n", i2c_get_cur_bus(), i2c_get_adap_name());

    return CMD_RET_SUCCESS;
}

static int do_i2c_write(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    uint chip;
    uint	devaddr, length;
	int alen;
    u_char  *memaddr;
    int ret;
    int cur_bus;

    if ((argc < 5) || (argc > 6))
		return cmd_usage(cmdtp);

    cur_bus = i2c_get_cur_bus();
    /*
	 * memaddr is the address where to store things in memory
	 */
	memaddr = (u_char *)simple_strtoul(argv[1], NULL, 16);

	/*
	 * I2C chip address
	 */
	chip = simple_strtoul(argv[2], NULL, 16);

	/*
	 * I2C data address within the chip.  This can be 1 or
	 * 2 bytes long.  Some day it might be 3 bytes long :-).
	 */
	devaddr = simple_strtoul(argv[3], NULL, 16);
	alen = get_alen(argv[3], DEFAULT_ADDR_LEN);
    if (alen > 3)
		return cmd_usage(cmdtp);

    /*
	 * Length is the number of bytes.
	 */
	length = simple_strtoul(argv[4], NULL, 16);

    if (argc == 6 && !strcmp(argv[5], "-s")) {
        /*
		 * Write all bytes in a single I2C transaction. If the target
		 * device is an EEPROM, it is your responsibility to not cross
		 * a page boundary. No write delay upon completion, take this
		 * into account if linking commands.
		 */
        ret = i2c_write(chip, devaddr, alen, memaddr, length);

    if (ret) {
        pr_err("i2c%d %s write failed\n", i2c_get_cur_bus(), i2c_get_adap_name());
        return CMD_RET_FAILURE;
    } else
        printf("i2c%d %s write success\n", i2c_get_cur_bus(), i2c_get_adap_name());

    } else {
        /*
		 * Repeated addressing - perform <length> separate
		 * write transactions of one byte each
		 */
        while (length-- > 0) {
            ret = i2c_write(chip, devaddr++, alen, memaddr++, 1);

        	if (ret) {
            	pr_err("i2c%d %s write failed\n", i2c_get_cur_bus(), i2c_get_adap_name());
            	return CMD_RET_FAILURE;
        	} else
            	printf("i2c%d %s write success\n", i2c_get_cur_bus(), i2c_get_adap_name());

        udelay(10);
     	}
    }

    return CMD_RET_SUCCESS;
}

/**
 * do_i2c_bus_speed() - Handle the "i2c speed" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 */
static int do_i2c_bus_speed(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int speed, ret = 0;

    if (argc == 1) {
        printf("Current bus speed is %d\n", i2c_get_adap_speed());
    } else {
    	speed = simple_strtoul(argv[1], NULL, 10);
		printf("Setting bus speed to %d Hz\n", speed);
		ret = i2c_set_bus_speed(speed);

		if (ret)
			printf("Failure changing bus speed (%d)\n", ret);
    }
    
    return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_i2c_sub[] = {
    BST_CMD_MKENT(dev, 1, 1, do_i2c_bus_num, "", ""),
    BST_CMD_MKENT(md, 3, 1, do_i2c_md, "", ""),
	BST_CMD_MKENT(mw, 3, 1, do_i2c_mw, "", ""),
	BST_CMD_MKENT(probe, 0, 1, do_i2c_probe, "", ""),
	BST_CMD_MKENT(read, 5, 1, do_i2c_read, "", ""),
	BST_CMD_MKENT(write, 6, 0, do_i2c_write, "", ""),
    BST_CMD_MKENT(speed, 1, 1, do_i2c_bus_speed, "", ""),
};

/**
 * do_i2c() - Handle the "i2c" command-line command
 * @cmdtp:	Command data struct pointer
 * @flag:	Command flag
 * @argc:	Command-line argument count
 * @argv:	Array of command-line arguments
 *
 * Returns zero on success, CMD_RET_USAGE in case of misuse and negative
 * on error.
 */
static int do_i2c(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'i2c' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_i2c_sub[0], ARRAY_SIZE(cmd_i2c_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char i2c_help_text[] =
    "dev [dev] - show or set current I2C bus\n\r"
    "i2c md chip address[.0, .1, .2] [# of objects] - read from I2C device\n\r"
    "i2c mw chip address[.0, .1, .2] value [count] - write to I2C device (fill)\n\r"
    "i2c probe [address] - test for and show device(s) on the I2C bus\n\r"
    "i2c read chip address[.0, .1, .2] length memaddress - read to memory\n\r"
    "i2c write memaddress chip address[.0, .1, .2] length [-s] - write memory\n\r"
	"          to I2C; the -s option selects bulk write in a single transaction\n\r"
    "i2c speed [speed] - show or set I2C bus speed";
#endif

BST_CMD(
    i2c, 7, 0, do_i2c,
    "I2C sub-system",
    i2c_help_text
);

