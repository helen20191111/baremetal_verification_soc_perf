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
#include <uart_sync.h>
#include <common.h>
#include <module/i2c.h>
/////////////////////////////////////putc  getc for printf////////////////////////////////////////
#include <a1000/sysreg_base.h>
#include <int_num.h>
#include <RegTest.h>

static unsigned int sPrtBase = 0;
#define UART_PRINT_BASE sPrtBase
//#define UART_PRINT_BASE UART1_BASE_ADDR
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

static unsigned int cpu_getIntrIdx(void);
extern unsigned int Ipc_GetSecureEn(void);
extern void OpenDefaultIntr(void);

/////////////////////////////////////clock & sharepin////////////////////////////////////////

//init clock & sharepin
void hard_init(void)
{
}

void putstr(char *str)
{
	int i = 0;
	while (str[i] != 0) {
		putc(str[i]);
		i++;
	}
	//rx, tx FIFO status @USR register
}

void prj_logo(void)
{
	putc('\r');
	putc('\n');
	putc('s');
	putc('a');
	putc('f');
	putc('e');
	putc('t');
	putc('y');
	putc('R');
	putc('5');
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

extern unsigned long __section_irq_table_start;
extern T_isr_list __section_irq_table_end;
extern unsigned long __section_fiq_table_start;
extern T_isr_list __section_fiq_table_end;

#define NO_ISR 0xfff0ff
static int sIrqTab[IRQ_SAFETY_TOTAL_CNT];
static int sFiqTab[FIQ_SAFETY_TOTAL_CNT];

void isr_init(void)
{
	T_isr_list *pIsrList;
	int i = 0;

	unsigned int cpuIdx = cpu_getIntrIdx();

	for (i = 0; i < IRQ_SAFETY_TOTAL_CNT; i++) {
		sIrqTab[i] = NO_ISR;
	}
	for (i = 0; i < FIQ_SAFETY_TOTAL_CNT; i++) {
		sFiqTab[i] = NO_ISR;
	}

	i = 0;
	pIsrList = (T_isr_list *)(&__section_irq_table_start);

	while (pIsrList < &__section_irq_table_end) {
		if ((pIsrList->attr & ISR_ATTR_SAFETY) &&
		    (pIsrList->isrName < IRQ_SAFETY_TOTAL_CNT)) {
			sIrqTab[pIsrList->isrName] = i;
			hprintf(TSENV, "\nno %d,irq %d: %s,isEdge: %d", i,
				pIsrList->isrName, pIsrList->description,
				IS_EDGE(pIsrList->attr));

			cpu_gic_config_irq(pIsrList->isrName,
					   IS_EDGE(pIsrList->attr), cpuIdx);
		}

		i++;
		pIsrList++;
	}

	i = 0;
	pIsrList = (T_isr_list *)(&__section_fiq_table_start);

	while (pIsrList < &__section_fiq_table_end) {
		if ((pIsrList->attr & ISR_ATTR_SAFETY) &&
		    (pIsrList->isrName < FIQ_SAFETY_TOTAL_CNT)) {
			sFiqTab[pIsrList->isrName] = i;
			hprintf(TSENV, "\nno %d,fiq %d: %s,isEdge: %d", i,
				pIsrList->isrName, pIsrList->description,
				IS_EDGE(pIsrList->attr));

			cpu_gic_config_fiq(pIsrList->isrName,
					   IS_EDGE(pIsrList->attr), cpuIdx);
		}

		i++;
		pIsrList++;
	}

	cpu_gic_config_nonsecure();
}

//flag for test
volatile int irqFlag = 0;
void irq_enter(void)
{
	T_isr_list *pIsrList;
	int intrNum;
	unsigned int data;

	irqFlag = 1;
	hprintf(TSENV, "\nirq comming");
	data = cpu_gicc_get_aiar();
	//cpu_gicd_clear_enable(data);

	hprintf(TSENV, "\nirq aiar %d", data);
	//hprintf(TSENV, "\niar %d",cpu_gicc_get_iar());

	intrNum = data & 0x3ff;
	if (intrNum == 1022) {
		putc('p');
		putc('r');
		putc('m');
		putc('t');
		return;
	} else if (intrNum == 1023) {
		//source disappear
		putc('1');
		putc('0');
		putc('2');
		putc('3');
		return;
	} else if ((sIrqTab[intrNum] != NO_ISR) &&
		   (intrNum < IRQ_SAFETY_TOTAL_CNT)) {
		pIsrList = (T_isr_list *)(&__section_irq_table_start);
		pIsrList += sIrqTab[intrNum];
		pIsrList->isrFn(pIsrList->para);
	} else {
		hprintf(TSENV, "\nirq %d not register", intrNum);
		getc();
		hprintf(TSENV, "\niar %d", cpu_gicc_get_iar());
		while (1)
			;
	}

	hprintf(TSENV, "\nbefore set aeoir");
	cpu_gicc_set_aeoir(data);
	//next delay is for test prmt
	//delaySoft(1000);
	hprintf(TSENV, "\nexit irq");
	irqFlag = 0;
}

void fiq_enter(void)
{
	T_isr_list *pIsrList;
	int intrNum;
	unsigned int data;

	if (irqFlag) {
		hprintf(TSENV, "\n!!!!prmt form irq!!!!");
	}
	hprintf(TSENV, "\nfiq comming");
	//getc();
	data = cpu_gicc_get_iar();
	hprintf(TSENV, "\nfiq iar %d", data);
	//hprintf(TSENV, "\naiar %d",cpu_gicc_get_aiar());

	intrNum = data & 0x3ff;
	if (intrNum == 1022) {
		putc('p');
		putc('r');
		putc('m');
		putc('t');
		return;
	} else if (intrNum == 1023) {
		//source disappear
		putc('1');
		putc('0');
		putc('2');
		putc('3');
		return;
	} else if ((sFiqTab[intrNum] != NO_ISR) &&
		   (intrNum < FIQ_SAFETY_TOTAL_CNT)) {
		hprintf(TSENV, "\nfiq handle");
		pIsrList = (T_isr_list *)(&__section_fiq_table_start);
		pIsrList += sFiqTab[intrNum];
		pIsrList->isrFn(pIsrList->para);
	} else {
		hprintf(TSENV, "\nFiq %d not register", intrNum);
		hprintf(TSENV, "\naiar %d", cpu_gicc_get_aiar());
		while (1)
			;
	}

	cpu_gicc_set_eoir(data);
	hprintf(TSENV, "\nexit fiq");
}

void swi_main(void)
{
	hprintf(TSENV, "\nswi comming");
}

static void (*dAbrtCb)(void) = NULL;

void dAbrtSetCb(void (*CbFn)(void))
{
	dAbrtCb = CbFn;
}
void dAbrtCleanCb(void)
{
	dAbrtCb = NULL;
}

static unsigned int sLstLr = 0;
static unsigned int sRtyCnt = 0;
void dabort_main(unsigned int lr, unsigned int sp)
{
	unsigned int *pSP;
	hprintf(TSENV, "\ndata abort exception error");
	hprintf(TSENV, "\nlr 0x%x, sp 0x%x", lr, sp);

	if (sLstLr == lr) {
		hprintf(TSENV, "\nlr is same ,retry cnt %d", sRtyCnt);
		if (--sRtyCnt > 0) {
			if (dAbrtCb) {
				dAbrtCb();
			}
		} else {
			hprintf(TSENV, "\nlr is same ,retry fail");
			pSP = (unsigned int *)(sp);
			pSP += 13;
			if (*pSP == lr) {
				hprintf(TSENV, "\nfind lr,ignore");
				*pSP += 4;
			} else {
				hprintf(TSENV, "\nlr?=0x%x", *pSP);
			}
		}
	} else {
		hprintf(TSENV, "\nfirst comming ,retry");

		sLstLr = lr;
		sRtyCnt = 2;
		if (dAbrtCb) {
			dAbrtCb();
		}
	}
}

void pabort_main(void)
{
	hprintf(TSENV, "\nprefech abort exception error");
}

void undef_main(void)
{
	hprintf(TSENV, "\nundef exception error");
}

/*
MRC p15, 0, <Rd>, c9, c1, 1 ; Read ATCM Region Register
MCR p15, 0, <Rd>, c9, c1, 1 ; Write ATCM Region Register
*/
void tcm_cpExpt(void)
{
	//base 0x10000000
	asm("MOV R0, #0x10000000");
	//enable
	asm("ORR R0, R0,#0x1");
	//open
	asm("MCR p15, 0, R0, c9, c1, 1");
	asm("DSB");
	//copy data to tcm
	memCpy32((unsigned int *)(0x10000000), (unsigned int *)(0), 20);

	//remap tcm
	//base
	asm("MOV R0, #0x00000000");
	//enable
	asm("ORR R0, R0,#0x1");
	//valid new tcm addr
	asm("MCR p15, 0, R0, c9, c1, 1");
	asm("DSB");
}

void cache_enable(void)
{
	asm("MRC p15, 0, r1, c1, c0, 0"); //Read System Control Register configuration data
	asm("ORR r1, r1, #0x1 <<12"); //instruction cache enable
	asm("ORR r1, r1, #0x1 <<2"); //data cache enable
	asm("DSB"); //
	asm("MCR p15, 0, r0, c15, c5, 0"); // Invalidate entire data cache
	asm("MCR p15, 0, r0, c7, c5, 0"); // Invalidate entire instruction cache
	asm("MCR p15, 0, r1, c1, c0, 0"); // enabled cache RAMs
	asm("ISB"); //
}

void cache_disable(void)
{
	asm("MRC p15, 0, r1, c1, c0, 0"); //Read System Control Register configuration data
	asm("BIC r1, r1, #0x1 <<12"); //instruction cache disable
	asm("BIC r1, r1, #0x1 <<2"); //data cache disable
	asm("DSB");
	asm("MCR p15, 0, r1, c1, c0, 0"); //disabled cache RAMs
	asm("ISB "); // Clean entire data cache. This routine depends on the data cache size. It can be omitted if it is known that the data cache has no dirty data
}

//The following code is an example of enabling the instruction cache:
void cache_openIcache(void)
{
	asm("MRC p15, 0, r1, c1, c0, 0"); // Read System Control Register configuration data
	asm("ORR r1, r1, #0x1 <<12"); // instruction cache enable
	asm("MCR p15, 0, r0, c7, c5, 0"); // Invalidate entire instruction cache
	asm("MCR p15, 0, r1, c1, c0, 0"); // enabled instruction cache
	asm("ISB"); //
}
//The following code is an example of disabling the instruction cache:
void cache_closeIcache(void)
{
	asm("MRC p15, 0, R1, c1, c0, 0"); // Read System Control Register configuration data
	asm("BIC r1, r1, #0x1 <<12"); // instruction cache enable
	asm("MCR p15, 0, r1, c1, c0, 0 "); //
	asm("ISB"); //
}

//The following code is an example of enabling the data cache:
void cache_openDcache(void)
{
	asm("MRC p15, 0, r1, c1, c0, 0"); // Read System Control Register configuration data
	asm("ORR r1, r1, #0x1 <<2"); //
	asm("DSB"); //
	asm("MCR p15, 0, r0, c15, c5, 0"); // Invalidate entire data cache
	asm("MCR p15, 0, r1, c1, c0, 0"); // enabled data cache
}

//The following code is an example of disabling the cache RAMs:
void cache_closeDcache(void)
{
	asm("MRC p15, 0, r1, c1, c0, 0"); // Read System Control Register configuration data
	asm("BIC r1, r1, #0x1 <<2"); //
	asm("DSB"); //
	asm("MCR p15, 0, r1, c1, c0, 0"); // disabled data cache
}

static void cache_report(void)
{
	int ret;

	__asm("MRC p15,0,%0, c1, c0, 0"
	      : "=r"(ret)::"memory"); //Read System Control Register configuration data

	hprintf(TSENV, "\nc1 = 0x%x", ret);
	if (ret & (1 << 2)) {
		hprintf(TSENV, "\nDCache open");
	} else {
		hprintf(TSENV, "\nDCache close");
	}

	if (ret & (1 << 12)) {
		hprintf(TSENV, "\nICache open");
	} else {
		hprintf(TSENV, "\nICache close");
	}
}

static void cpu_report(int para)
{
	int ret;
	putc('c');

	__asm("MRC p15, 0, %0, c0, c0, 5" : "=r"(ret)::"memory");
	putc('p');
	hprintf(TSENV, "\n\rc1 = 0x%x", ret);
	if (ret & (1 << 0)) {
		hprintf(TSENV, "\n\rcpu 1");
	} else {
		hprintf(TSENV, "\n\rcpu 0");
	}
}

/*
1 = cpu 0
2 = cpu 1
*/
static unsigned int cpu_getIntrIdx(void)
{
	unsigned int ret;

	__asm("MRC p15, 0, %0, c0, c0, 5" : "=r"(ret)::"memory");

	if (ret & (1 << 0)) {
		return CPU1_IDX;
	} else {
		return CPU0_IDX;
	}
}

void CacheButton(int para)
{
	char ch;

	cache_report();
	hprintf(TSENV,
		"\nCache selct. 0=close all, 1=open Icache,2=open Dcache,3(other)=open all");
	ch = getc();
	switch (ch) {
	case '0':
		cache_disable();
		break;
	case '1':
		cache_openIcache();
		cache_closeDcache();
		break;
	case '2':
		cache_openDcache();
		cache_closeIcache();
		break;
	case '3':
	default:
		cache_enable();
		break;
	}

	cache_report();
}

void setPrtBase(void)
{
	if (Ipc_GetSecureEn()) { //Secure is running
		sPrtBase = UART1_BASE_ADDR;
	} else { //secure sleep
		sPrtBase = UART0_BASE_ADDR;
	}
}

extern void ddr_rw_test0(void);
extern void ddr_rw_test1(void);
extern int bst_a1000_setup_ddr(void);

//int main(int para1, int para2)
int main(void)
{
	setPrtBase();
	tcm_cpExpt();
	cache_enable();
	putc('c');
	closePrintf();
	isr_init();
	r5intr_open();
	openPrintf();
	sync_uart_init();
	OpenDefaultIntr();
	//cpu_report(0);
	hprintf(TSENV, "\r\n"
		       "------------------------------------\r\n"
		       "|               BST HVTE           |\r\n"
		       "------------------------------------\r\n");

#ifdef CONFIG_DDR_TEST
	bst_a1000_setup_ddr();
	ddr_rw_test0();
	ddr_rw_test1();
#endif
	i2c_init();
	cli_loop();
	return 0;
}

CMD_DEFINE(cpu, cpu_report, 0, "cpu0 or cpu1?", 0);
CMD_DEFINE(cache, CacheButton, 0, "open or close cache", 0);
