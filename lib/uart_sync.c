/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <a1000/sysreg_base.h>
#include <printf_inc.h>
#include <printf.h>
#include <RegTest.h>
#include <linux/types.h>
#include <ut_test.h>
#include <linux/string.h>
#include <utility_lite.h>
#include <uart_sync.h>

#define SYNC_UART_BASE_ADDR UART3_BASE_ADDR

#define SYNC_UART_FCR (SYNC_UART_BASE_ADDR + 0x08)
#define SYNC_UART_MCR (SYNC_UART_BASE_ADDR + 0x10)
#define SYNC_UART_LSR (SYNC_UART_BASE_ADDR + 0x14)
#define SYNC_UART_LCR (SYNC_UART_BASE_ADDR + 0x0C)
#define SYNC_UART_DLL (SYNC_UART_BASE_ADDR + 0x00)
#define SYNC_UART_THR (SYNC_UART_BASE_ADDR + 0x00)
#define SYNC_UART_RBR (SYNC_UART_BASE_ADDR + 0x00)

#define TXDATAREADY (1 << 6)
#define RXDATAREADY (1)

void txc(unsigned char c)
{
	while (!(REG32_READ(SYNC_UART_LSR) & TXDATAREADY))
		; //bit set means tx is empty

	REG32_WRITE(SYNC_UART_THR, c);
}

unsigned char rxc(void)
{
	unsigned char c;
	while (!(REG32_READ(SYNC_UART_LSR) & RXDATAREADY))
		;
	c = (unsigned char)(REG32_READ(SYNC_UART_RBR));

	return c;
}

void sync_uart_init(void)
{
	u32 i;
	u32 reg_data;
	REG32_WRITE(SYNC_UART_MCR, 0); //disable flow ctrl
	REG32_WRITE(SYNC_UART_MCR,
		    (REG32_READ(SYNC_UART_MCR) | (0x1 << 1))); //clear rts

	REG32_WRITE(SYNC_UART_LCR, (REG32_READ(SYNC_UART_LCR) |
				    (0x1 << 7))); //enable access DLL & DLH
	REG32_WRITE(SYNC_UART_DLL, 1); //set baud rate
	REG32_WRITE(SYNC_UART_LCR, (REG32_READ(SYNC_UART_LCR) &
				    (~(0x1 << 7)))); //clear DLAB bit

	REG32_WRITE(
		SYNC_UART_LCR,
		REG32_READ(SYNC_UART_LCR) |
			(0x3)); //set data length to 8 bit, 1 stop bit,no parity
	REG32_WRITE(SYNC_UART_FCR, (REG32_READ(SYNC_UART_FCR) & (~(0x1 << 0))) |
					   (1 << 0)); //fifo enable

	for (i = 0; i < 100; i++)
		; //delay

	reg_data = REG32_READ(AON_PMM_REG_BASE_ADDR);
	reg_data = reg_data & 0xe1ffffff; //UART3_SYNC
	REG32_WRITE(AON_PMM_REG_BASE_ADDR, reg_data);
}

void xtor_dut_sync_write(char *str)
{
	u32 syncval = 0;
	char *ptr = str;

	if (NULL == ptr) {
		hprintf(DEBUG, "parameter invalid.\r\n");
		return;
	}

	if (strlen(ptr) > 100) {
		hprintf(DEBUG, "string is too long.\r\n");
		return;
	}

	while (*ptr) {
		putc(*ptr);
		syncval += *(ptr++);
	}
	hprintf(DEBUG, "\r\n");

	//hprintf(DEBUG, "write str:%s\r\n", ptr); //%s in printf do not work
	hprintf(DEBUG, "write syncval:%x\r\n", syncval & 0xFFFFFFFF);
	putc('0');
	txc((syncval & 0xFF000000) >> 24);
	putc('1');
	txc((syncval & 0x00FF0000) >> 16);
	putc('2');
	txc((syncval & 0x0000FF00) >> 8);
	putc('3');
	txc((syncval & 0x000000FF) >> 0);
	putc('4');
}

u32 xtor_dut_sync_read(char *str)
{
	u32 syncval = 0, syncval_read, i;
	char *ptr = str;

	if (NULL == ptr) {
		hprintf(DEBUG, "parameter invalid.\r\n");
		return 0;
	}

	if (strlen(ptr) > 100) {
		hprintf(DEBUG, "string is too long.\r\n");
		return 0;
	}

	while (*ptr) {
		putc(*ptr);
		syncval += *(ptr++);
	}
	hprintf(DEBUG, "\r\n");

	//hprintf(DEBUG, "read str:%s\r\n", ptr); //%s in printf do not work
	//syncval = 0x61616161; //'a''a''a''a'
	hprintf(DEBUG, "expect syncval:%x\n", syncval & 0xFFFFFFFF);

	while (1) {
		for (i = 0, syncval_read = 0; i < 4; i++) {
			syncval_read <<= 8;
			syncval_read |= rxc(); //rxc() is a blocking API
			hprintf(DEBUG, "i:%d, syncval_read:%x\r\n", i,
				syncval_read);
		}
		hprintf(DEBUG, "syncval read:%x\r\n",
			syncval_read & 0xFFFFFFFF);
		if (syncval_read == syncval) {
			break;
		}
	}

	return (syncval_read & 0xFFFFFFFF);
}
