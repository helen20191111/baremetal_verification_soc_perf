/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <common.h>
#include <command.h>
#include <a1000/sysreg_base.h>
#include <utility_lite.h>
#include <RegTest.h>
#include <module/uart.h>
#include <vsprintf.h>
#include <tty_io.h>

/* Match uart port base addr
 * Param : User input parameters on the command line
 * return : Base address of the uart port
 */
static u32 uart_port_match(char *const port){

	if(strcmp(port, "port0") == 0)
		return UART0_BASE_ADDR;
	else if(strcmp(port, "port1") == 0)
		return UART1_BASE_ADDR;
	else if(strcmp(port, "port2") == 0)
		return UART2_BASE_ADDR;
	else if(strcmp(port, "port3") == 0)
		return UART3_BASE_ADDR;
	else 
		return 0;
}

static int do_uart_init(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]){
	u32 reg_base =  0;

	if(argc != 2)
		return CMD_RET_USAGE;

	reg_base = uart_port_match(argv[1]);
	if(reg_base){
		printf("Set uart %s 8 bit data length, 1 stop bit, no parity, 38400 baud.\r\n", argv[1]);
		uart_init(reg_base);
	}else 
		return CMD_RET_USAGE;
	
	return CMD_RET_SUCCESS;
}

static int do_uart_baud(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]){
	u32 reg_base =  0;
	u32 baud = 0;

	if(argc != 3)
		return CMD_RET_USAGE;

	reg_base = uart_port_match(argv[1]);
	if(reg_base){
		baud = simple_strtoul(argv[2], NULL, 10);
		printf("Set uart %s baud : %d.\r\n", argv[1], baud);
		while(!(REG32_READ(UART_PRINT_REG_LSR) & (0x1 << 6))); //wait transmit complete
		udelay(10);
	    
		uart_baud_set(reg_base, baud);

	}else 
		return CMD_RET_USAGE;
	
	return CMD_RET_SUCCESS;

}

static int do_uart_echo(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]){	
	u32 reg_base =  0;
	char *p = argv[2];
	
	if(argc != 3)
		return CMD_RET_USAGE;

	reg_base = uart_port_match(argv[1]);
	if(reg_base){
		if(*p == '"'){
			p++;
			while(*p != '"'){
				while (!(REG32_READ(reg_base + UART_REG_LSR) & TXD0READY)); 
				REG32_WRITE(reg_base + UART_REG_THR, *p);
				p++;
			}
		}else
			return CMD_RET_USAGE;
	
	}else 
		return CMD_RET_USAGE;
	return CMD_RET_SUCCESS;
}

static int do_uart_recv(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]){	
	u32 reg_base =  0;
	unsigned char c = 0;
	
	if(argc != 2)
		return CMD_RET_USAGE;

	reg_base = uart_port_match(argv[1]);
	if(reg_base){
		while (1){
			//if console has input, exit
			if(REG32_READ(UART_PRINT_REG_LSR) & RXD0READY){	
				c = (unsigned char)(REG32_READ(UART_PRINT_REG_RBR));
				printf("\r\nReceve a INTR, test exited.\r\n");
				break;
			}
			
			if((REG32_READ(reg_base + UART_REG_LSR) & RXD0READY)){
				c = (unsigned char)(REG32_READ(reg_base + UART_REG_RBR));
				REG32_WRITE(reg_base + UART_REG_THR, c);
				putc(c);
			}
		}
	}else 
		return CMD_RET_USAGE;
	
	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_uart_sub[] = {
	BST_CMD_MKENT(init, 1, 1, do_uart_init, "", ""),
	BST_CMD_MKENT(baud, 1, 1, do_uart_baud, "", ""),
	BST_CMD_MKENT(echo, 1, 1, do_uart_echo, "", ""),
	BST_CMD_MKENT(recv, 1, 1, do_uart_recv, "", ""),
};


static int do_uart(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	cmd_tbl_t *c;

	if(argc < 3)
		return CMD_RET_USAGE;

	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_uart_sub[0], ARRAY_SIZE(cmd_uart_sub));

	if(c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}


BST_CMD(uart, CONFIG_SYS_MAXARGS, 1, do_uart,
	   "uart module test", 
	   "\r\n"
	   "\tuart init portx (x:0-3)\r\n"
	   "\tuart baud portx xxx\r\n"
	   "\tuart echo portx \"xxx\"\r\n"
	   "\tuart recv portx\r\n");

