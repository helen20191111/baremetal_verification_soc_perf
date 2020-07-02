/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <printf_inc.h> //use related path
#include <printf.h> //use related path
#include <utility_lite.h>
#include <uart_sync.h>

/////////////////////////////////////putc  getc for printf////////////////////////////////////////
#include <a1000/sysreg_base.h>
#include <int_num.h>
#include <secure.h>
#include <RegTest.h>

//******************************************uart api for printf*****************************************//

#define UART_PRINT_BASE UART0_BASE_ADDR
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

extern void ddr_envInit(void);
extern void ddr0ddr1Init(void);

//******************************************start uart api for printf*****************************************//

int putc_nb(unsigned char c, unsigned int delayLoop)
{
	while (1) {
		if (REG32_READ(UART_PRINT_REG_LSR) & TXD0READY) {
			break;
		}

		if (delayLoop--) {
			delaySoft(1);
			continue;
		}
		return -1;
	}

	REG32_WRITE(UART_PRINT_REG_THR, c);

	return 0;
}

int getc_nb(unsigned char *c, unsigned int delayLoop)
{
	while (1) {
		if (REG32_READ(UART_PRINT_REG_LSR) & RXD0READY) {
			break;
		}

		if (delayLoop--) {
			delaySoft(1);
			continue;
		}
		return -1;
	}

	*c = (unsigned char)(REG32_READ(UART_PRINT_REG_RBR));
	return 0;
}

//******************************************end of uart api for printf*****************************************//

//init print io
void printf_init(void)
{
	//use xx putc
	uart_print_init();

	//test putc
	putc('s');
	putc('t');
	putc('a');
	putc('r');
	putc('t');
	putc('6');
}

//init clock & sharepin
unsigned int hard_init(void)
{
	unsigned int ret = 0;
#ifdef CONFIG_ZEBU_HARDWARE_62
	//open all secure clock
	REG32(SECURE_CLOCK_CTRL) = 0x7FF;
	//relase  all secure reset
	ret = REG32(SECURE_BLOCK_RELEASE);
	REG32(SECURE_BLOCK_RELEASE) |= 0x1FFF;
	//PMM open uart1
	REG32(SAFETY_PMM_CFG0) &= ~(0xF << 17);
#endif

	/*******adjust qspi div reopen_qspi xip mode ***********/
	//close xip
	REG32(QSPI0_XIP_ENABLE_REG) &= ~0x1;
	delaySoft(1000);
	//ssi disable
	REG32(FLASE_GRP_BASE + 0x8) = 0;
	delaySoft(1000);
	//change div to 8
	REG32(FLASE_GRP_BASE + 0x14) = 8;
	delaySoft(1000);
	//ssi enable
	REG32(FLASE_GRP_BASE + 0x8) = 1;
	delaySoft(1000);
	//enable xip
	REG32(QSPI0_XIP_ENABLE_REG) |= 0x1;
	delaySoft(1000);
	//*******************end of adjust qspi div*********//

	return ret;
}

void prj_logo(void)
{
	putc('\r');
	putc('\n');
	putc('s');
	putc('e');
	putc('c');
	putc('u');
	putc('r');
	putc('e');
	putc('>');
}

void r5intr_open(void)
{
	asm("MRS  R1,CPSR");
	asm("BIC  R1,#0xC0");
	asm("MSR  CPSR,R1");
}

void r5intr_close(void)
{
	asm("MRS  R1,CPSR");
	asm("ORR  R1,#0xC0");
	asm("MSR  CPSR,R1");
}

void cmdPost(void)
{
	runCmd("openddr");
	runCmd("help");
}

int main(void)
{
	unsigned int ret;

	ret = hard_init();

	printf_init();

#ifdef CONFIG_RELEASE_R5_SAFETY
	free_safety(0);
	/*if secure R5 Only release safety, secure not use uart0, UART0 maybe for A55.c */
	while (1)
		;
#endif

#ifdef CONFIG_RELEASE_A55
	ddr_envInit();
	ddr0ddr1Init();
	free_A55(0);
	/*if secure R5 Only release safety, secure not use uart0, UART0 maybe for A55.c */
	while (1)
		;
#endif
	intr_init();
	isr_init();
	r5intr_open();
	Secure_OpenSysIrq();
	hprintf(TSENV, "\r\n HardInit ret = 0x%x", ret);
	post_info();
	hprintf(TSENV,
		"\r\n"
		"---------------------------------------------------------------------\r\n"
		"|                           BST HVTE                                |\r\n"
		"---------------------------------------------------------------------\r\n");
	cmd_SetPostCb(cmdPost);
	//sync_uart_init();
	cmd_main();

	return 0;
}
