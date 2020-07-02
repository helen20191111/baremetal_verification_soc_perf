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
#include <RegTest.h>

#include <a1000/sysreg_base.h>
#include <int_num.h>
#include <secure.h>

//*************************************sram firewall***************************************//
void closeSramFirewall(void)
{
	REG32(SECURE_SRAM_FIREWALL) = 3;
}

void adjustLspSspFirewall(int para)
{
	unsigned long val = 0;
	unsigned int cfgVal = 0;

	hprintf(TSENV, "\r\nopen lsp0 wall(0=no, other=yes)?:");
	if (getHex(&val)) {
		val = 1;
	}
	if (val > 1) {
		val = 1;
	}
	cfgVal |= (val << 0);

	hprintf(TSENV, "\r\nopen lsp1 wall(0=no, other=yes)?:");
	if (getHex(&val)) {
		val = 1;
	}
	if (val > 1) {
		val = 1;
	}
	cfgVal |= (val << 1);

	hprintf(TSENV, "\r\nopen qspi0 wall(0=no, other=yes)?:");
	if (getHex(&val)) {
		val = 1;
	}
	if (val > 1) {
		val = 1;
	}
	cfgVal |= (val << 2);

	hprintf(TSENV, "\r\nopen qspi1 wall(0=no, other=yes)?:");
	if (getHex(&val)) {
		val = 1;
	}
	if (val > 1) {
		val = 1;
	}
	cfgVal |= (val << 3);

	hprintf(TSENV, "\r\nopen ssp(crpm) wall(0=no, other=yes)?:");
	if (getHex(&val)) {
		val = 1;
	}
	if (val > 1) {
		val = 1;
	}
	cfgVal |= (val << 4);

	hprintf(TSENV, "\r\nopen a55 sram wall(0=no, other=yes)?:");
	if (getHex(&val)) {
		val = 1;
	}
	if (val > 1) {
		val = 1;
	}
	cfgVal |= (val << 5);

	REG32(SAFETY_A55_FIREWALL) &= ~(0x3f << 16);
	REG32(SAFETY_A55_FIREWALL) |= (cfgVal << 16);

	hprintf(TSENV, "\r\n%x config 0x%x", SAFETY_A55_FIREWALL,
		REG32(SAFETY_A55_FIREWALL));
}

void adjustSramFirewall(int para)
{
	unsigned long val;

	hprintf(TSENV, "\r\ninput FireWall value");
	hprintf(TSENV,
		"\r\n0 = 0~bffff,1 = 0~7ffff, 2 = 0~3ffff,other=unmask \r\n");
	if (getHex(&val)) {
		val = 3;
	}
	if (val > 3) {
		val = 3;
	}

	REG32(SECURE_SRAM_FIREWALL) = val;
}

CMD_DEFINE(firewallram, adjustSramFirewall, 0, "adjust sram firewall", 0);
CMD_DEFINE(firewalla55, adjustLspSspFirewall, 0, "adjust a55 lsp&ssp firewall",
	   0);

//*****************************some exception test case******************************//
//********************************swi************************************************//
void SwiCall(int para)
{
	asm("swi 0");
}
CMD_DEFINE(swi, SwiCall, 0, "swi test", 0);

//*******************************watchdog***********************************
#define WDT_RST 0x76
void open_secureWatchdog(void)
{
	unsigned int val;
	//TORR,Timeout Range Register,0x7 is 8M clock,1=128k
	//REG32(SECURE_WDT_ADDR+0x4) = 0x7;
	REG32(SECURE_WDT_ADDR + 0x4) = 0x5;

	//CRR,watch dog reset（kick dog）
	REG32(SECURE_WDT_ADDR + 0xc) = WDT_RST;

	//CR,bit0 enable,bit1 RMOD(2 intr gen rst)
	REG32(SECURE_WDT_ADDR + 0x0) &= ~3;
	val = REG32(SECURE_WDT_ADDR + 0x0) |= 3;
	//config rst length
	val = REG32(SECURE_WDT_ADDR + 0x0) |= (7 << 2);
	hprintf(TSENV, "\nwdt CR 0x%x", val);

	//CRR,watch dog reset(kick dog)
	REG32(SECURE_WDT_ADDR + 0xc) = WDT_RST;

	//CCVR,Current Counter Value Register
	val = REG32(SECURE_WDT_ADDR + 0x8);
	hprintf(TSENV, "\nwdt ccvr 0x%x", val);
}

void wdtIrqOpen(int para)
{
	//wtd resest en
	hprintf(TSENV, "\nSECURE_CTRL_ADDR f0001004 = %x",
		REG32(SECURE_CTRL_ADDR + 0x4));
	//wdt mask,reset all
	hprintf(TSENV, "\nwdt mask 0xc003500c = %x",
		REG32(SAFETY_CRM_ADDR + 0xc));

	SECURE_IRQ_ENABLE(IRQ_SECURE_WATCHDOG);

	open_secureWatchdog();
}

void wdtIrqClose(int para)
{
	SECURE_IRQ_DISABLE(IRQ_SECURE_WATCHDOG);
	REG32(SECURE_WDT_ADDR + 0x0) &= ~3;
}

static int kickCnt = 10;
void irq_secureWatchdog(int para)
{
	//CRR,watch dog reset（kick dog）
	if (kickCnt-- > 7) {
		REG32(SECURE_WDT_ADDR + 0xc) = WDT_RST;
		hprintf(TSENV, "\nkick dog");
	} else if (kickCnt-- > 3) {
		hprintf(TSENV, "\nwdt int,not kick");
	} else {
		SECURE_IRQ_DISABLE(IRQ_SECURE_WATCHDOG);
		hprintf(TSENV, "\nclose wdog int");
	}
	//CCVR,Current Counter Value Register
	//regval = REG32(SECURE_WDT_ADDR+0x8);
	//hprintf(TSENV, "\nwdt ccvr 0x%x",regval);
}

CMD_DEFINE(enwdt, wdtIrqOpen, 0, "watch dog irq enable", 0);
CMD_DEFINE(diswdt, wdtIrqClose, 0, "watch dog irq disable", 0);
IRQ_DEFINE(IRQ_SECURE_WATCHDOG, irq_secureWatchdog, 0, "watch dog isr",
	   ISR_ATTR_SECURE);

//*******************************timer***********************************
#define TIMER_LOAD_VALUE (9 * 1000 * 1000)
void open_secureTimer(void)
{
	//disable timer
	REG32_WRITE(SECURE_TIMER_ADDR + 8, 0);
	//start config
	REG32_WRITE(SECURE_TIMER_ADDR, TIMER_LOAD_VALUE);
	//timer bit2=0,intr unmask
	REG32_WRITE(SECURE_TIMER_ADDR + 8, 3);
}

void irq_secureTimer(int para)
{
	unsigned int regval;

	regval = REG32(SECURE_TIMER_ADDR + 0xc);
	hprintf(TSENV, "\ntimer read 0x%x", regval);
	REG32_WRITE(SECURE_TIMER_ADDR, TIMER_LOAD_VALUE);
}

void timerIrqOpen(int para)
{
	SECURE_IRQ_ENABLE(IRQ_SECURE_TIMER);
	open_secureTimer();
}

void timerIrqClose(int para)
{
	SECURE_IRQ_DISABLE(IRQ_SECURE_TIMER);
	REG32_WRITE(SECURE_TIMER_ADDR + 8, 0);
}

CMD_DEFINE(entimer, timerIrqOpen, 0, "timer irq enabl;e", 0);
CMD_DEFINE(distimer, timerIrqClose, 0, "timer irq disable", 0);
IRQ_DEFINE(IRQ_SECURE_TIMER, irq_secureTimer, 0, "timer isr", ISR_ATTR_SECURE);

//*******************noc parity intr stat0 and stat1*********************//
void irqSecureNocStat0(int para)
{
	hprintf(DEBUG, "\r\naddr 0xf0001068 value is 0x%08x",
		REG32(SECURE_CTRL_ADDR + 0x68));
	if (REG32(SECURE_CTRL_ADDR + 0x68) & (0x3f)) {
		hprintf(DEBUG, "\r\nnoc pty stat0,aes pty intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 17; //clear intr
	}

	if (REG32(SECURE_CTRL_ADDR + 0x68) & (0x3f << 6)) {
		hprintf(DEBUG, "\r\nnoc pty stat0,crc pty intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 18;
	}

	if (REG32(SECURE_CTRL_ADDR + 0x68) & (0x3f << 12)) {
		hprintf(DEBUG, "\r\nnoc pty stat0,msha pty intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 19;
	}

	if (REG32(SECURE_CTRL_ADDR + 0x68) & (0x3f << 18)) {
		hprintf(DEBUG, "\r\nnoc pty stat0,sm pty intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 20;
	}
}

void irqSecureNocStat1(int para)
{
	hprintf(DEBUG, "\r\naddr 0xf000106c value is 0x%08x",
		REG32(SECURE_CTRL_ADDR + 0x6c));
	if (REG32(SECURE_CTRL_ADDR + 0x6c) & (0x3 << 22)) {
		hprintf(DEBUG, "\r\nnoc pty stat1,server wdata parity intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 25; //clear intr
	}
	if (REG32(SECURE_CTRL_ADDR + 0x6c) & (0x3 << 20)) {
		hprintf(DEBUG, "\r\nnoc pty stat1,r5 rdata parity intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 24;
	}
	if (REG32(SECURE_CTRL_ADDR + 0x6c) & (0xf << 16)) {
		hprintf(DEBUG, "\r\nnoc pty stat1,pka parity intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 21;
	}
	if (REG32(SECURE_CTRL_ADDR + 0x6c) & (0xf << 12)) {
		hprintf(DEBUG, "\r\nnoc pty stat1,abus parity intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 23;
	}
	if (REG32(SECURE_CTRL_ADDR + 0x6c) & (0xf << 8)) {
		hprintf(DEBUG, "\r\nnoc pty stat1,pbus parity intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 22;
	}
	if (REG32(SECURE_CTRL_ADDR + 0x6c) & (0xf << 4)) {
		hprintf(DEBUG, "\r\nnoc pty stat1,rom parity intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 16;
	}
	if (REG32(SECURE_CTRL_ADDR + 0x6c) & (0xf)) {
		hprintf(DEBUG, "\r\nnoc pty stat1,r5 eccerr intr");
		REG32(SECURE_CTRL_ADDR + 0x64) = 0x1 << 24;
	}
}

void irqSecureSecCsr(int para)
{
	if (REG32(SECURE_CTRL_ADDR + 0x74) & (0x1 << 2)) {
		hprintf(DEBUG, "\r\nsec csr parity intr");
		REG32(SECURE_CTRL_ADDR + 0xB0) |= 0x3 << 2;
		REG32(SECURE_CTRL_ADDR + 0xC0) = 0;
	}
}

IRQ_DEFINE(IRQ_SECURE_NOC_PARITY_0, irqSecureNocStat0, 0,
	   "noc isr for parity stat0", ISR_ATTR_SECURE);
IRQ_DEFINE(IRQ_SECURE_NOC_PARITY_1, irqSecureNocStat1, 0,
	   "noc isr for parity stat1", ISR_ATTR_SECURE);
IRQ_DEFINE(IRQ_SECURE_SEC_CSR_PARITY, irqSecureSecCsr, 0,
	   "sec csr isr for parity", ISR_ATTR_SECURE);

//******************************ipc event handle***************************//
#define IPC_BASE 0x33100000

//************event********//
#define EVT_MID_A55_0 (0)
#define EVT_MID_SAFETY_0 (8)
#define EVT_MID_SECURE (17)

#define REG_IPC_EN(evt_mid)                                                    \
	((IPC_BASE) | (1 << 13) | (evt_mid << 8) | (1 << 7) | (evt_mid << 2))
#define REG_IPC_SRC(evt_mid, evt_d_mid)                                        \
	((IPC_BASE) | (1 << 13) | (evt_mid << 8) | (evt_d_mid << 2))

//A55 CPU0
#define REG_EN_A550 (REG_IPC_EN(EVT_MID_A55_0))
#define REG_SRC_A550 (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_A55_0))
#define REG_SRC_A550_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_SAFETY_0))
#define REG_SRC_A550_TO_SC (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_SECURE))

//SAFETY CPU0
#define REG_EN_SF0 (REG_IPC_EN(EVT_MID_SAFETY_0))
#define REG_SRC_SF0 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_SAFETY_0))
#define REG_SRC_SF0_TO_SC (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_SECURE))
#define REG_SRC_SF0_TO_A550 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_0))

//SECURE
#define REG_EN_SE (REG_IPC_EN(EVT_MID_SECURE))
#define REG_SRC_SC (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_SECURE))
#define REG_SRC_SC_TO_SF0 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_SAFETY_0))
#define REG_SRC_SC_TO_A550 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_0))

//***************semaphore********//
#define SM_MID_A55 (1)
#define SM_MID_SAFETY (2)
#define SM_MID_SECURE (8)

#define REG_SMPH_ACQ(sm_mid, bank, sm_id)                                      \
	((IPC_BASE) | (sm_mid << 9) | (bank << 7) | (sm_id << 2))
#define REG_SMPH_PND(sm_mid, bank)                                             \
	((IPC_BASE) | (sm_mid << 9) | (bank << 7) | (1 << 6))

#define REG_SMPH_SC_BANK0_PEND (REG_SMPH_PND(SM_MID_SECURE, 0))
#define REG_SMPH_SC_BANK1_PEND (REG_SMPH_PND(SM_MID_SECURE, 1))
#define REG_SMPH_SC_BANK2_PEND (REG_SMPH_PND(SM_MID_SECURE, 2))
#define REG_SMPH_SC_BANK3_PEND (REG_SMPH_PND(SM_MID_SECURE, 3))

#define REG_SMPH_SC_BANK0_ID0 (REG_SMPH_ACQ(SM_MID_SECURE, 0, 0))
#define REG_SMPH_SC_BANK1_ID0 (REG_SMPH_ACQ(SM_MID_SECURE, 1, 0))
#define REG_SMPH_SC_BANK2_ID0 (REG_SMPH_ACQ(SM_MID_SECURE, 2, 0))
#define REG_SMPH_SC_BANK3_ID0 (REG_SMPH_ACQ(SM_MID_SECURE, 3, 0))

#define REG_SMPH_SF_BANK0_PEND (REG_SMPH_PND(SM_MID_SAFETY, 0))
#define REG_SMPH_SF_BANK1_PEND (REG_SMPH_PND(SM_MID_SAFETY, 1))
#define REG_SMPH_SF_BANK2_PEND (REG_SMPH_PND(SM_MID_SAFETY, 2))
#define REG_SMPH_SF_BANK3_PEND (REG_SMPH_PND(SM_MID_SAFETY, 3))

#define REG_SMPH_SF_BANK0_ID0 (REG_SMPH_ACQ(SM_MID_SAFETY, 0, 0))
#define REG_SMPH_SF_BANK1_ID0 (REG_SMPH_ACQ(SM_MID_SAFETY, 1, 0))
#define REG_SMPH_SF_BANK2_ID0 (REG_SMPH_ACQ(SM_MID_SAFETY, 2, 0))
#define REG_SMPH_SF_BANK3_ID0 (REG_SMPH_ACQ(SM_MID_SAFETY, 3, 0))

void SecureIpc_CloseEn(void)
{
	REG32(REG_EN_SE) = 0;
	REG32(REG_EN_SE) |= (3 << 30);
	SECURE_IRQ_DISABLE(10);
	SECURE_IRQ_DISABLE(11);
	SECURE_IRQ_DISABLE(12);
	SECURE_IRQ_DISABLE(13);
	SECURE_IRQ_DISABLE(14);
}

void SecureIpc_OpenIrq(void)
{
	REG32(REG_EN_SE) = 0x7fffffff;
	hprintf(TSENV, "\r\nREG_EN_SE 0x%x = 0x%x", REG_EN_SE,
		REG32(REG_EN_SE));

	SECURE_IRQ_ENABLE(10);
	SECURE_IRQ_ENABLE(11);
	SECURE_IRQ_ENABLE(12);
	SECURE_IRQ_ENABLE(13);
	SECURE_IRQ_ENABLE(14);
}

/*
core info:such as a55 wdt,
*/
void isr_IpcEvent(int para)
{
	volatile unsigned int val;
	hprintf(TSENV, "\n\rpara is %d", para);

	switch (para) {
	case 10:
		val = REG32(REG_SMPH_SC_BANK0_PEND);
		hprintf(TSENV, "\r\nREG_SMPH_SC_BANK0_PEND 0x%x = 0x%x",
			REG_SMPH_SC_BANK0_PEND, REG32(REG_SMPH_SC_BANK0_PEND));
		REG32(REG_SMPH_SC_BANK0_PEND) = val;
		hprintf(TSENV, "\r\nAfter REG_SMPH_SC_BANK0_PEND 0x%x = 0x%x",
			REG_SMPH_SC_BANK0_PEND, REG32(REG_SMPH_SC_BANK0_PEND));
		break;

	case 11:
		val = REG32(REG_SMPH_SC_BANK1_PEND);
		hprintf(TSENV, "\r\nREG_SMPH_SC_BANK1_PEND 0x%x = 0x%x",
			REG_SMPH_SC_BANK1_PEND, REG32(REG_SMPH_SC_BANK1_PEND));
		REG32(REG_SMPH_SC_BANK1_PEND) = val;
		hprintf(TSENV, "\r\nAfter REG_SMPH_SC_BANK1_PEND 0x%x = 0x%x",
			REG_SMPH_SC_BANK1_PEND, REG32(REG_SMPH_SC_BANK1_PEND));
		break;

	case 12:
		val = REG32(REG_SMPH_SC_BANK2_PEND);
		hprintf(TSENV, "\r\nREG_SMPH_SC_BANK2_PEND 0x%x = 0x%x",
			REG_SMPH_SC_BANK2_PEND, REG32(REG_SMPH_SC_BANK2_PEND));
		REG32(REG_SMPH_SC_BANK2_PEND) = val;
		hprintf(TSENV, "\r\nAfter nREG_SMPH_SC_BANK2_PEND 0x%x = 0x%x",
			REG_SMPH_SC_BANK2_PEND, REG32(REG_SMPH_SC_BANK2_PEND));
		break;

	case 13:
		val = REG32(REG_SMPH_SC_BANK3_PEND);
		hprintf(TSENV, "\r\nREG_SMPH_SC_BANK3_PEND 0x%x = 0x%x",
			REG_SMPH_SC_BANK3_PEND, REG32(REG_SMPH_SC_BANK3_PEND));
		REG32(REG_SMPH_SC_BANK3_PEND) = val;
		hprintf(TSENV, "\r\nAfter nREG_SMPH_SC_BANK3_PEND 0x%x = 0x%x",
			REG_SMPH_SC_BANK3_PEND, REG32(REG_SMPH_SC_BANK3_PEND));
		break;

	case 14: //event
		hprintf(TSENV, "\r\nREG_SRC_SC 0x%x = 0x%x", REG_SRC_SC,
			REG32(REG_SRC_SC));
		//hprintf(TSENV, "\r\nREG_SRC_A550_TO_SC 0x%x = 0x%x",REG_SRC_A550_TO_SC,REG32(REG_SRC_A550_TO_SC));
		//hprintf(TSENV, "\r\nREG_SRC_SF0_TO_SC 0x%x = 0x%x",REG_SRC_SF0_TO_SC,REG32(REG_SRC_SF0_TO_SC));
		val = REG32(REG_SRC_SC);
		REG32(REG_SRC_SC) = val;
		hprintf(TSENV, "\r\nAfter REG_SRC_SC 0x%x = 0x%x", REG_SRC_SC,
			REG32(REG_SRC_SC));
		//hprintf(TSENV, "\r\nAfter REG_SRC_A550_TO_SC 0x%x = 0x%x",REG_SRC_A550_TO_SC,REG32(REG_SRC_A550_TO_SC));
		//hprintf(TSENV, "\r\nAfter REG_SRC_SF0_TO_SC 0x%x = 0x%x",REG_SRC_SF0_TO_SC,REG32(REG_SRC_SF0_TO_SC));
		break;
	}
}

IRQ_DEFINE(10, isr_IpcEvent, 10, "sec bank0", ISR_ATTR_SECURE);
IRQ_DEFINE(11, isr_IpcEvent, 11, "sec bank1", ISR_ATTR_SECURE);
IRQ_DEFINE(12, isr_IpcEvent, 12, "sec bank2", ISR_ATTR_SECURE);
IRQ_DEFINE(13, isr_IpcEvent, 13, "sec bank3", ISR_ATTR_SECURE);
IRQ_DEFINE(14, isr_IpcEvent, 14, "sec event", ISR_ATTR_SECURE);

#if 0
void Secure_IpcTst(int para)
{
   unsigned int val;

   hprintf(TSENV, "\r\nsend to Safety value 0x:");
   if (getHex(&val)) {
     val = 2;   
   }
   
   REG32(REG_SRC_SC_TO_SF0) = val;

   hprintf(TSENV, "\r\nsend to a55-0 value 0x:");
   if (getHex(&val)) {
     val = 2;
   }

   REG32(REG_SRC_SC_TO_A550) = val;
}
CMD_DEFINE(ipctst, Secure_IpcTst, 0, "Secure to other ipc tst", 0);
#endif
//******************************end of ipc event handle*******************//

//*******************rom ram ecc  parity intr ****************//
void irq_SysHandle(int para)
{
	hprintf(TSENV, "\nSystem int， intr %d", para);

	//tx intr
	if (IRQ_SECURE_RX_INTR == para) {
		hprintf(TSENV, "\n\r0xc0037060 = 0x%x", REG32(0xc0037060));
		hprintf(TSENV, "\n\r0xf0000014 = 0x%x", REG32(0xf0000014));

		//clean secure tx
		REG32(0xf0000014) = 0;

		//clean safety tx
		if (REG32(0xc0037060) & 0x8) {
			hprintf(TSENV, "\n\r clean intr from Secure");
			REG32(0xc0037060) &= ~0x8;
		}
	}
}

void eccParity_openIrq(void)
{
	hprintf(TSENV, "\r\nopen ecc&parity of sram&rom ");
	SECURE_IRQ_ENABLE(IRQ_SECURE_ROMPARITY);
	SECURE_IRQ_ENABLE(IRQ_SECURE_ROMECC);
	SECURE_IRQ_ENABLE(IRQ_SECURE_RAMECC);
	SECURE_IRQ_ENABLE(IRQ_SECURE_RAMPARITY);
}

void Secure_OpenSysIrq(void)
{
	eccParity_openIrq();
	SECURE_IRQ_ENABLE(IRQ_SECURE_RX_INTR);

	SecureIpc_OpenIrq();
}

void Secure_TxTst(int para)
{
	unsigned long val;

	hprintf(TSENV, "\r\ninput 0xf0000014 value(0~F)?:");
	if (getHex(&val)) {
		val = 2;
	}

	if (val > 0xf) {
		val = 8;
	}

	REG32(0xf0000014) = val;
}

CMD_DEFINE(txtst, Secure_TxTst, 1, "R5 tx tst", 0);

IRQ_DEFINE(IRQ_SECURE_ROMECC, irq_SysHandle, IRQ_SECURE_ROMECC, "rom ecc isr",
	   ISR_ATTR_SECURE);
IRQ_DEFINE(IRQ_SECURE_ROMPARITY, irq_SysHandle, IRQ_SECURE_ROMPARITY,
	   "rom parity isr", ISR_ATTR_SECURE);
IRQ_DEFINE(IRQ_SECURE_RAMECC, irq_SysHandle, IRQ_SECURE_RAMECC, "Ram ecc isr",
	   ISR_ATTR_SECURE);
IRQ_DEFINE(IRQ_SECURE_RAMPARITY, irq_SysHandle, IRQ_SECURE_RAMPARITY,
	   "Ram patity isr", ISR_ATTR_SECURE);
IRQ_DEFINE(IRQ_SECURE_RX_INTR, irq_SysHandle, IRQ_SECURE_RX_INTR, "R5 tx intr",
	   ISR_ATTR_SECURE);
//*****************************************end of tx isr function **********************************//

//******************************************two cmd for debug*****************************************//
void load_safety_r5(void)
{
	//reset safety
	//REG32(0xC0035008) &=  ~(0x1<<2);
	//delaySoft(1000);
	/*
    REG32(0xC0037070) &=  ~(0x1);
    delaySof(0);
    REG32(0xC0037070) |=  (0x1);
    */

	//release safety
	REG32(SAFETY_RELEASE_CTRL) |= (0x1 << SAFETY_RELEASE_BIT);

	hprintf(TSENV, "\nrelease safety r5 Success.\n");
}

void secure_load_A55(void)
{
#define A55_RAW_FLASHC_ADDR 0x80400000

	//dump_memory(SAFETY_LOAD_A55_ADDR,0x100);
	hprintf(TSENV, "\nStart Load A55.\n");
	//closeSramFirewall();
	REG32_WRITE(SAFETY_RELEASE_CTRL, REG32_READ(SAFETY_RELEASE_CTRL) | 2);
	while (!(REG32_READ(SAFETY_RELEASE_CTRL) & (1 << 31)))
		;
	REG32_WRITE(A55_BOOT_RVBARADDR0, A55_RAW_FLASHC_ADDR >> 2);
	REG32_WRITE(SAFETY_RELEASE_CTRL,
		    REG32_READ(SAFETY_RELEASE_CTRL) | (1 << 30));
	while (!(REG32_READ(SAFETY_RELEASE_CTRL) & 0XC0))
		;

	hprintf(TSENV, "\nrelease A55 done\n");
}

void release_dsp(void)
{
#define DSP_RAW_FLASHC_ADDR 0x80000000

	//release noc
	REG32(SAFETY_CRM_ADDR) |= (1 << 2);
	//CV block release
	REG32(A55_CRM_BLOCK_RELEASE) = 0xffff;
	//SET  dsp run addr
	//dsp 1
	REG32(CV_SREG_CORE1_START) = DSP_RAW_FLASHC_ADDR;
	//dsp 2~4
	//REG32(CV_SREG_CORE2_START) = DSP_RAW_FLASHC_ADDR;
	//REG32(CV_SREG_CORE3_START) = DSP_RAW_FLASHC_ADDR;
	//REG32(CV_SREG_CORE4_START) = DSP_RAW_FLASHC_ADDR;

	//release dsp(bit 28~32, dsp 1~4)
	REG32(CV_SREG_CORE_RELEASE) = 0x1f000000;
	//REG32(CV_SREG_ADDR) = 0xff000000;

	hprintf(TSENV, "\nrelease dsp done\n");
}

void free_safety(int para)
{
	load_safety_r5();
}

void free_A55(int para)
{
	secure_load_A55();
}

void free_dsp(int para)
{
	release_dsp();
}

void secure_sleep(int para)
{
	SecureIpc_CloseEn();
	load_safety_r5();
	while (1) {
		asm("wfi");
	}
}

CMD_DEFINE(sfrun, free_safety, 0, "free safety cpu", 0);
CMD_DEFINE(a55run, free_A55, 0, "free a55 cpu", 0);
CMD_DEFINE(dsprun, free_dsp, 0, "free dsp for cv", 0);
CMD_DEFINE(sleep, secure_sleep, 0, "free safety & secure sleep", 0);

//post info
void post_info(void)
{
	//Secure Reset en
	unsigned int val = REG32(SECURE_BLOCK_RELEASE);

	hprintf(TSENV, "\r\nSecure release REG32(0xf0001004) = 0x%x", val);
	if (val & 1 << 31) {
		hprintf(TSENV, ".......from Reset\n");
	} else {
		hprintf(TSENV, ".......from Power On\n");
		REG32(SAFETY_WDT_CTRL) &= ~(1 << 4);
	}
}

extern unsigned long __section_irq_table_start;
extern T_isr_list __section_irq_table_end;
extern unsigned long __section_fiq_table_start;
extern T_isr_list __section_fiq_table_end;

#define NO_ISR 0xfff0ff
static int sIrqTab[IRQ_SECURE_TOTAL_CNT];
static int sFiqTab[FIQ_SECURE_TOTAL_CNT];

void isr_init(void)
{
	T_isr_list *pIsrList;
	int i = 0;

	for (i = 0; i < IRQ_SECURE_TOTAL_CNT; i++) {
		sIrqTab[i] = NO_ISR;
	}
	for (i = 0; i < FIQ_SECURE_TOTAL_CNT; i++) {
		sFiqTab[i] = NO_ISR;
	}

	i = 0;
	pIsrList = (T_isr_list *)(&__section_irq_table_start);

	while (pIsrList < &__section_irq_table_end) {
		if ((pIsrList->attr & ISR_ATTR_SECURE) &&
		    (pIsrList->isrName < IRQ_SECURE_TOTAL_CNT)) {
			sIrqTab[pIsrList->isrName] = i;
			hprintf(TSENV, "\r\nno %d,irq %d: %s", i,
				pIsrList->isrName, pIsrList->description);
		}

		i++;
		pIsrList++;
	}

	i = 0;
	pIsrList = (T_isr_list *)(&__section_fiq_table_start);

	while ((pIsrList->attr & ISR_ATTR_SECURE) &&
	       (pIsrList < &__section_fiq_table_end)) {
		if (pIsrList->isrName < FIQ_SECURE_TOTAL_CNT) {
			sFiqTab[pIsrList->isrName] = i;
			hprintf(TSENV, "\r\nno %d,fiq %d: %s", i,
				pIsrList->isrName, pIsrList->description);
		}
		i++;
		pIsrList++;
	}

	return;
}

void irq_enter(void)
{
	T_isr_list *pIsrList;
	unsigned int status;
	int i;

	status = read_irq_f_status();
	hprintf(TSENV, "\r\nirq status 0x%x", status);

	for (i = 0; i < IRQ_SECURE_TOTAL_CNT; i++) {
		if ((status >> i) & 0x1) {
			if (sIrqTab[i] != NO_ISR) {
				pIsrList =
					(T_isr_list
						 *)(&__section_irq_table_start);
				pIsrList += sIrqTab[i];
				pIsrList->isrFn(pIsrList->para);
			} else {
				hprintf(TSENV, "\r\nirq %d not register", i);
			}
		}
	}
}

void fiq_enter(void)
{
	T_isr_list *pIsrList;
	unsigned int status;
	int i;

	status = read_fiq_f_status();
	hprintf(TSENV, "\r\nfiq status 0x%x", status);

	for (i = 0; i < FIQ_SECURE_TOTAL_CNT; i++) {
		if ((status >> i) & 0x1) {
			if (sFiqTab[i] != NO_ISR) {
				pIsrList =
					(T_isr_list
						 *)(&__section_fiq_table_start);
				pIsrList += sFiqTab[i];
				pIsrList->isrFn(pIsrList->para);
			} else {
				hprintf(TSENV, "\r\nfiq %d not register", i);
			}
		}
	}
}

void swi_main(void)
{
	hprintf(TSENV, "\nswi comming");
}

void undef_main(void)
{
	hprintf(TSENV, "\n\rcpu undefine\n\r");
}

void dabort_main(void)
{
	hprintf(TSENV, "\n\rcpu data abort12\n\r");
	REG32(SECURE_CTRL_ADDR + 0xC0) = 0;
	REG32(OTP_BASE_ADDR + 0x120) = 0xf;
	REG32(AES_BASE_ADDR + 0x48) = 0xd;
	REG32(MSHA_BASE_ADDR + 0x78) = 0xd;
	REG32(CRC_BASE_ADDR + 0x28) = 0xd;
	REG32(SECURE_CTRL_ADDR + 0x40) &= ~(1 << 3);
}

void pabort_main(void)

{
	hprintf(TSENV, "\n\rcpu prefech abort\n\r");
}
