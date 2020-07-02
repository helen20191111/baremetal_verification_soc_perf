/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <printf_inc.h> //use related path
#include <utility_lite.h>

/////////////////////////////////////putc_f  getc for printf////////////////////////////////////////
#include <a1000/sysreg_base.h>
#include <RegTest.h>

//static unsigned int sPrtBase = 0;
#define UART_PRINT_BASE uartBase
#define TXD0READY (1 << 6)
#define RXD0READY (1)

#define UART_PRINT_REG_DLL (UART_PRINT_BASE + 0x0)
#define UART_PRINT_REG_THR (UART_PRINT_BASE + 0x0)
#define UART_PRINT_REG_RBR (UART_PRINT_BASE + 0x0)
#define UART_PRINT_REG_DLH (UART_PRINT_BASE + 0x4)
#define UART_PRINT_REG_LCR (UART_PRINT_BASE + 0xC)
#define UART_PRINT_REG_MCR (UART_PRINT_BASE + 0x10)
#define UART_PRINT_REG_LSR (UART_PRINT_BASE + 0x14)
#define UART_PRINT_REG_DLF (UART_PRINT_BASE + 0xC0)

extern int ddrc_init(unsigned int ddr_base);
extern void ddr_envInit(void);

void delaySoft_f(int loopcnt)
{
	while (loopcnt-- > 0) {
		asm("nop");
	}
}

static void putc_f(unsigned char c, unsigned int uartBase)
{
	while (!(REG32_READ(UART_PRINT_REG_LSR) & TXD0READY))
		; //bit set means tx is empty

	//UTXH0 = c;
	REG32_WRITE(UART_PRINT_REG_THR, c);

	//rx, tx FIFO status @USR register
}

//init print io
void printf_init(unsigned int uartBase)
{
	//use xx putc_f
	uart_print_init();

	//test putc_f
	putc_f('S', uartBase);
	putc_f('F', uartBase);
}

#define IPC_BASE 0x33100000
#define EVT_MID_A55_0 (0)
#define EVT_MID_SAFETY_0 (8)
#define EVT_MID_SAFETY_1 (9)
#define EVT_MID_SECURE (17)

#define REG_IPC_EN(evt_mid)                                                    \
	((IPC_BASE) | (1 << 13) | (evt_mid << 8) | (1 << 7) | (evt_mid << 2))
#define REG_EN_SE (REG_IPC_EN(EVT_MID_SECURE))
/*
is secure receive ipc. 0 = en, other = open 
*/
#ifndef CONFIG_EVB
static unsigned int Ipc_GetSecureEn(void)
{
	return REG32(REG_EN_SE);
}
#endif

//NOTE : init_f.c �ڲ��ܵ��������ļ���API����Ϊ�ⲿ�ִ�����������QSPI�ڵģ�����API����DDR/SRAM�ڵġ�init_f�׶β�δ���ƴ��뵽DDR��
void init_f(void)
{
#ifndef CONFIG_EVB
	unsigned int uartBase;

	if (Ipc_GetSecureEn()) { //Secure is running
		uartBase = UART1_BASE_ADDR;
		printf_init(uartBase);
	} else { //secure sleep,use uart 0
		uartBase = UART0_BASE_ADDR;
		putc_f('f', uartBase);
		putc_f('0', uartBase);
	}
#endif
}
