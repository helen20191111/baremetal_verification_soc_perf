/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _TTY_IO_H
#define _TTY_IO_H

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

#endif /* _TTY_IO_H */