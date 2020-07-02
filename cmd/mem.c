// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/*
 * Memory Functions
 *
 * Copied from FADS ROM, Dan Malek (dmalek@jlc.net)
 */
#include <common.h>
#include <cli.h>
#include <command.h>
#include <errno.h>
#include <display_options.h>
#include <linux/compiler.h>

/* Display values from last command.
 * Memory modify remembered values are different from display memory.
 */
static ulong dp_last_addr, dp_last_size;
static ulong dp_last_length = 0x40;

static ulong base_address = 0;

/* Memory Display
 *
 * Syntax:
 *	md{.b, .w, .l, .q} {addr} {len}
 */
#define DISP_LINE_LEN	16
static int do_mem_md(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong	addr, length, bytes;
	const void *buf;
	int	size;
	int rc = 0;

	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	addr = dp_last_addr;
	size = dp_last_size;
	length = dp_last_length;

	if (argc < 2)
		return CMD_RET_USAGE;

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/* New command specified.  Check for a size specification.
		 * Defaults to long if no or incorrect specification.
		 */
		if ((size = cmd_get_data_size(argv[0], 4)) < 0)
			return 1;

		/* Address is specified since argc > 1
		*/
		addr = simple_strtoul(argv[1], NULL, 16);
		addr += base_address;

		/* If another parameter, it is the length to display.
		 * Length is the number of objects, not number of bytes.
		 */
		if (argc > 2)
			length = simple_strtoul(argv[2], NULL, 16);
	}

	bytes = size * length;
	buf = (void *)addr;

	/* Print the lines. */
	print_buffer(addr, buf, size, length, DISP_LINE_LEN / size);
	addr += bytes;

	dp_last_addr = addr;
	dp_last_length = length;
	dp_last_size = size;
	return (rc);
}

static int do_mem_mw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	u64 writeval;
#else
	ulong writeval;
#endif
	ulong	addr, count;
	int	size;
	void *buf, *start;
	ulong bytes;

	if ((argc < 3) || (argc > 4))
		return CMD_RET_USAGE;

	/* Check for size specification.
	*/
	if ((size = cmd_get_data_size(argv[0], 4)) < 1)
		return 1;

	/* Address is specified since argc > 1
	*/
	addr = simple_strtoul(argv[1], NULL, 16);
	addr += base_address;

	/* Get the value to write.
	*/
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	writeval = simple_strtoull(argv[2], NULL, 16);
#else
	writeval = simple_strtoul(argv[2], NULL, 16);
#endif

	/* Count ? */
	if (argc == 4) {
		count = simple_strtoul(argv[3], NULL, 16);
	} else {
		count = 1;
	}

	bytes = size * count;
	start = (void *)addr;
	buf = start;
	while (count-- > 0) {
		if (size == 4)
			*((u32 *)buf) = (u32)writeval;
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
		else if (size == 8)
			*((u64 *)buf) = (u64)writeval;
#endif
		else if (size == 2)
			*((u16 *)buf) = (u16)writeval;
		else
			*((u8 *)buf) = (u8)writeval;
		buf += size;
	}
	return 0;
}

static int do_mem_cmp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong	addr1, addr2, count, ngood, bytes;
	int	size;
	int     rcode = 0;
	const char *type;
	const void *buf1, *buf2, *base;
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	u64 word1, word2;
#else
	ulong word1, word2;
#endif

	if (argc != 4)
		return CMD_RET_USAGE;

	/* Check for size specification.
	*/
	if ((size = cmd_get_data_size(argv[0], 4)) < 0)
		return 1;
	type = size == 8 ? "double word" :
	       size == 4 ? "word" :
	       size == 2 ? "halfword" : "byte";

	addr1 = simple_strtoul(argv[1], NULL, 16);
	addr1 += base_address;

	addr2 = simple_strtoul(argv[2], NULL, 16);
	addr2 += base_address;

	count = simple_strtoul(argv[3], NULL, 16);

	bytes = size * count;
	base = buf1 = (void *)addr1;
	buf2 = (void *)addr2;
	for (ngood = 0; ngood < count; ++ngood) {
		if (size == 4) {
			word1 = *(u32 *)buf1;
			word2 = *(u32 *)buf2;
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
		} else if (size == 8) {
			word1 = *(u64 *)buf1;
			word2 = *(u64 *)buf2;
#endif
		} else if (size == 2) {
			word1 = *(u16 *)buf1;
			word2 = *(u16 *)buf2;
		} else {
			word1 = *(u8 *)buf1;
			word2 = *(u8 *)buf2;
		}
		if (word1 != word2) {
			ulong offset = buf1 - base;
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
			printf("%s at 0x%p (%#0*llx) != %s at 0x%p (%#0*llx)\n",
			       type, (void *)(addr1 + offset), size, word1,
			       type, (void *)(addr2 + offset), size, word2);
#else
			printf("%s at 0x%08lx (%#0*lx) != %s at 0x%08lx (%#0*lx)\n",
				type, (ulong)(addr1 + offset), size, word1,
				type, (ulong)(addr2 + offset), size, word2);
#endif
			rcode = 1;
			break;
		}

		buf1 += size;
		buf2 += size;

		/* reset watchdog from time to time */
		if ((ngood % (64 << 10)) == 0)
			printf("wdtdog rest!\n");
	}

	printf("Total of %ld %s(s) were the same\n", ngood, type);
	return rcode;
}

static int do_mem_cp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ulong	addr, dest, count;
	void	*src, *dst;
	int	size;

	if (argc != 4)
		return CMD_RET_USAGE;

	/* Check for size specification.
	*/
	if ((size = cmd_get_data_size(argv[0], 4)) < 0)
		return 1;

	addr = simple_strtoul(argv[1], NULL, 16);
	addr += base_address;

	dest = simple_strtoul(argv[2], NULL, 16);
	dest += base_address;

	count = simple_strtoul(argv[3], NULL, 16);

	if (count == 0) {
		puts ("Zero length ???\n");
		return 1;
	}

	src = (void *)addr;
	dst = (void *)dest;

	memcpy(dst, src, count * size);

	return 0;
}

static int do_mem_base(cmd_tbl_t *cmdtp, int flag, int argc,
		       char * const argv[])
{
	if (argc > 1) {
		/* Set new base address.
		*/
		base_address = simple_strtoul(argv[1], NULL, 16);
	}
	/* Print the current base address.
	*/
	printf("Base Address: 0x%08lx\n", base_address);
	return 0;
}

/**************************************************/
BST_CMD(
	md,	3,	1,	do_mem_md,
	"memory display",
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	"[.b, .w, .l, .q] address [# of objects]"
#else
	"[.b, .w, .l] address [# of objects]"
#endif
);

BST_CMD(
	mw,	4,	1,	do_mem_mw,
	"memory write (fill)",
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	"[.b, .w, .l, .q] address value [count]"
#else
	"[.b, .w, .l] address value [count]"
#endif
);

BST_CMD(
	cp,	4,	1,	do_mem_cp,
	"memory copy",
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	"[.b, .w, .l, .q] source target count"
#else
	"[.b, .w, .l] source target count"
#endif
);

BST_CMD(
	cmp,	4,	1,	do_mem_cmp,
	"memory compare",
#ifdef CONFIG_SYS_SUPPORT_64BIT_DATA
	"[.b, .w, .l, .q] addr1 addr2 count"
#else
	"[.b, .w, .l] addr1 addr2 count"
#endif
);

BST_CMD(
	base,	2,	1,	do_mem_base,
	"print or set address offset",
	"\n    - print address offset for memory commands\n"
	"base off\n    - set address offset for memory commands to 'off'"
);
