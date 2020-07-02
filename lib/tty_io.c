/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <a1000/sysreg_base.h>
#include <RegTest.h>
#include <utility_lite.h>
#include <module/uart.h>
#include <printf_inc.h>
#include <module/a55_timer.h>
#include <tty_io.h>

#if (defined CONFIG_R5_SECURE) || (defined CONFIG_APP_BOOT) ||                 \
	(!defined CONFIG_ZEBU)
#define UART_PRINT_BASE UART0_BASE_ADDR
#else
#define UART_PRINT_BASE UART1_BASE_ADDR
#endif

#define TXD0READY (1 << 6)
#define RXD0READY 1

#define UART_PRINT_REG_DLL (UART_PRINT_BASE + 0x0)
#define UART_PRINT_REG_THR (UART_PRINT_BASE + 0x0)
#define UART_PRINT_REG_RBR (UART_PRINT_BASE + 0x0)
#define UART_PRINT_REG_DLH (UART_PRINT_BASE + 0x4)
#define UART_PRINT_REG_LCR (UART_PRINT_BASE + 0xC)
#define UART_PRINT_REG_MCR (UART_PRINT_BASE + 0x10)
#define UART_PRINT_REG_LSR (UART_PRINT_BASE + 0x14)
#define UART_PRINT_REG_DLF (UART_PRINT_BASE + 0xC0)

#ifdef CONFIG_ZEBU
void uart_print_init(void)
{
	uart_pinmux_init(UART_PRINT_BASE);
	REG32_WRITE(UART_PRINT_REG_MCR, 0); //disable flow ctrl
	REG32_WRITE(UART_PRINT_REG_MCR,
		    (REG32_READ(UART_PRINT_REG_MCR) | (0x1 << 1))); //clear rts

	REG32_WRITE(UART_PRINT_REG_LCR, (REG32_READ(UART_PRINT_REG_LCR) |
					 (0x1 << 7))); //enable access DLL & DLH
	REG32_WRITE(UART_PRINT_REG_DLL, 1); //set baud rate
	//REG32_WRITE(UART_PRINT_REG_DLH, 0x0); //set baud rate
	//    //REG32_WRITE(UART_PRINT_REG_DLF, 0x21); //fractional part
	REG32_WRITE(UART_PRINT_REG_LCR, (REG32_READ(UART_PRINT_REG_LCR) &
					 (~(0x1 << 7)))); //clear DLAB bit

	REG32_WRITE(
		UART_PRINT_REG_LCR,
		REG32_READ(UART_PRINT_REG_LCR) |
			(0x3)); //set data length to 8 bit, 1 stop bit,no parity

	delaySoft(10);
}
#else

void uart_print_init(void)
{
	uart_pinmux_init(UART_PRINT_BASE);
	uart_irq_set(UART_PRINT_BASE, UART_IRQ_ALL, HVTE_FALSE);
	uart_afc_set(UART_PRINT_BASE, HVTE_FALSE);
	uart_rts_set(UART_PRINT_BASE, HVTE_TRUE);
	uart_baud_set(UART_PRINT_BASE, CONFIG_PRINT_UART_BAUD);
	uart_data_bit_set(UART_PRINT_BASE, DATA_8_BIT);
	uart_stop_bit_set(UART_PRINT_BASE, STOP_1_BIT);
	uart_parity_enable(UART_PRINT_BASE, HVTE_FALSE);

	//udelay(10);
	delaySoft(10);
}
#endif

void putc(unsigned char c)
{
	while (!(REG32_READ(UART_PRINT_REG_LSR) & TXD0READY))
		; //bit set means tx is empty

	//UTXH0 = c;
	REG32_WRITE(UART_PRINT_REG_THR, c);

	//rx, tx FIFO status @USR register
}
unsigned char getc(void)
{
	unsigned char c;
	while (!(REG32_READ(UART_PRINT_REG_LSR) & RXD0READY))
		;

	//UTXH0 = c;
	c = (unsigned char)(REG32_READ(UART_PRINT_REG_RBR));

	//rx, tx FIFO status @USR register
	return c;
}

unsigned char getc_unblock(void)
{
	unsigned char c;
	if (!(REG32_READ(UART_PRINT_REG_LSR) & RXD0READY)) {
		return 0;
	}

	//UTXH0 = c;
	c = (unsigned char)(REG32_READ(UART_PRINT_REG_RBR));

	//rx, tx FIFO status @USR register
	return c;
}

void puts(const char *str)
{
	while (*str)
		putc(*str++);
}
