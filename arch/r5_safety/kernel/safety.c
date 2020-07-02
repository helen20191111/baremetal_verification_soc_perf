///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           //
//  This file contains proprietary information that is the sole intellectual property        //
//  of Black Sesame Technologies, Inc. and its affiliates.  No portions of this material     //
//  may be reproduced in any form without the written permission of:                         //
//                                                                                           //
//  Black Sesame Technologies, Inc. and its affiliates                                       //
//  2255 Martin Ave. Suite D                                                                 //
//  Santa Clara, CA  95050                                                                   //
//  Copyright \@2018: all right reserved.                                                    //
//                                                                                           //
//  Notice:                                                                                  //
//                                                                                           //
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

/////////////////////////////////////putc  getc for printf////////////////////////////////////////
#include <a1000/sysreg_base.h>
#include <int_num.h>
#include <main.h>
#include <RegTest.h>

void load_a55(int para)
{
}

//*******************************timer isr example***********************************
//#define LOAD_VALUE0 (1000*1000)
//#define LOAD_VALUE1 (2*1000*1000)
#define LOAD_VALUE0 (600 * 1000)
#define LOAD_VALUE1 (1200 * 1000)
static unsigned int cnt1 = 0, cnt2 = 0;

void irq_timer(unsigned int para)
{
	unsigned int regVal, loadVal;

	regVal = REG32(para + 0xc);
	hprintf(TSENV, "\nirq timer regbase0x%x read 0x%x", para, regVal);
	loadVal = LOAD_VALUE0 * (1 + cnt1++ / 10);
	if (cnt1 > 80) {
		cnt1 = 0;
	}
	REG32_WRITE(para, loadVal);
	delaySoft(10000);
}

void fiq_timer(unsigned int para)
{
	unsigned int regVal, loadVal;

	regVal = REG32(para + 0xc);
	//hprintf(TSENV, "\nfiq timer regbase0x%x read 0x%x",para,regval);
	loadVal = LOAD_VALUE1 * (1 + cnt2++ / 10);
	if (cnt2 > 50) {
		cnt2 = 0;
	}
	REG32_WRITE(para, loadVal);
}

void timerIrqOpen(int para)
{
	//enable two timer interrupt
	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_TIMER0);
	SAFETY_FIQ_ENABLE(FIQ_IRQ_LSP1_TIMER0);

	//*****timer 0*****//
	//disable timer
	REG32_WRITE(TIMER0_PWM0_BASE_ADDR + 8, 0);
	//start config
	REG32_WRITE(TIMER0_PWM0_BASE_ADDR, LOAD_VALUE0);
	//timer bit2=0,intr unmask
	REG32_WRITE(TIMER0_PWM0_BASE_ADDR + 8, 3);

	//*****timer 1*****//
	//disable timer
	REG32_WRITE(TIMER1_PWM1_BASE_ADDR + 8, 0);
	//start config
	REG32_WRITE(TIMER1_PWM1_BASE_ADDR, LOAD_VALUE1);
	//timer bit2=0,intr unmask
	REG32_WRITE(TIMER1_PWM1_BASE_ADDR + 8, 3);
}

void timerIrqClose(int para)
{
	SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_TIMER0);
	SAFETY_FIQ_DISABLE(FIQ_IRQ_LSP1_TIMER0);

	REG32_WRITE(TIMER0_PWM0_BASE_ADDR + 8, 0);
	REG32_WRITE(TIMER1_PWM1_BASE_ADDR + 8, 0);
}

//*********************************************************************************//
void SwiCall(int para)
{
	asm("swi 0");
}
//*********************************************************************************//

//******************************ipc event handle***************************//

#define IPC_BASE 0x33100000

//************event********//
#define EVT_MID_A55_0 (0)
#define EVT_MID_SAFETY_0 (8)
#define EVT_MID_SAFETY_1 (9)
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
//SAFETY CPU1
#define REG_EN_SF1 (REG_IPC_EN(EVT_MID_SAFETY_1))
#define REG_SRC_SF1 (REG_IPC_SRC(EVT_MID_SAFETY_1, EVT_MID_SAFETY_1))
#define REG_SRC_SF1_TO_SC (REG_IPC_SRC(EVT_MID_SAFETY_1, EVT_MID_SECURE))
#define REG_SRC_SF1_TO_A550 (REG_IPC_SRC(EVT_MID_SAFETY_1, EVT_MID_A55_0))

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

/*
is secure receive ipc. 0 = en, other = open 
*/
unsigned int Ipc_GetSecureEn(void)
{
	return REG32(REG_EN_SE);
}

void SafetyIpc_OpenIrq(void)
{
	REG32(REG_EN_SF0) = 0x7fffffff;
	hprintf(TSENV, "\r\nREG_EN_SF0 0x%x = 0x%x", REG_EN_SF0,
		REG32(REG_EN_SF0));
	REG32(REG_EN_SF1) = 0x7fffffff;
	hprintf(TSENV, "\r\nREG_EN_SF1 0x%x = 0x%x", REG_EN_SF0,
		REG32(REG_EN_SF0));

	hprintf(TSENV, "\r\nREG_SMPH_SF_BANK0_ID0 0x%x", REG_SMPH_SF_BANK0_ID0);
	hprintf(TSENV, "\r\nREG_SMPH_SC_BANK0_ID0 0x%x", REG_SMPH_SC_BANK0_ID0);

	SAFETY_IRQ_ENABLE(59);
	SAFETY_IRQ_ENABLE(60);
	SAFETY_IRQ_ENABLE(61);
	SAFETY_IRQ_ENABLE(62);
	SAFETY_IRQ_ENABLE(63);
	SAFETY_IRQ_ENABLE(64);
}

/*
core info:such as a55 wdt,
*/
void isr_IpcEvent(int para)
{
	volatile unsigned int val;
	hprintf(TSENV, "\n\rpara is %d", para);
	switch (para) {
	case 59:
		val = REG32(REG_SMPH_SF_BANK0_PEND);
		hprintf(TSENV, "\r\nREG_SMPH_SF_BANK0_PEND 0x%x = 0x%x",
			REG_SMPH_SF_BANK0_PEND, val);
		REG32(REG_SMPH_SF_BANK0_PEND) = val;
		hprintf(TSENV, "\r\nAfter REG_SMPH_SF_BANK0_PEND 0x%x = 0x%x",
			REG_SMPH_SF_BANK0_PEND, REG32(REG_SMPH_SF_BANK0_PEND));
		break;

	case 60:
		val = REG32(REG_SMPH_SF_BANK1_PEND);
		hprintf(TSENV, "\r\nREG_SMPH_SF_BANK1_PEND 0x%x = 0x%x",
			REG_SMPH_SF_BANK1_PEND, val);
		REG32(REG_SMPH_SF_BANK1_PEND) = val;
		hprintf(TSENV, "\r\nAfter REG_SMPH_SF_BANK1_PEND 0x%x = 0x%x",
			REG_SMPH_SF_BANK1_PEND, REG32(REG_SMPH_SF_BANK1_PEND));
		break;

	case 61:
		val = REG32(REG_SMPH_SF_BANK2_PEND);
		hprintf(TSENV, "\r\nREG_SMPH_SF_BANK2_PEND 0x%x = 0x%x",
			REG_SMPH_SF_BANK2_PEND, val);
		REG32(REG_SMPH_SF_BANK2_PEND) = val;
		hprintf(TSENV, "\r\nAfter nREG_SMPH_SF_BANK2_PEND 0x%x = 0x%x",
			REG_SMPH_SF_BANK2_PEND, REG32(REG_SMPH_SF_BANK2_PEND));
		break;

	case 62:
		val = REG32(REG_SMPH_SF_BANK3_PEND);
		hprintf(TSENV, "\r\nREG_SMPH_SF_BANK3_PEND 0x%x = 0x%x",
			REG_SMPH_SF_BANK3_PEND, val);
		REG32(REG_SMPH_SF_BANK3_PEND) = val;
		hprintf(TSENV, "\r\nAfter nREG_SMPH_SF_BANK3_PEND 0x%x = 0x%x",
			REG_SMPH_SF_BANK3_PEND, REG32(REG_SMPH_SF_BANK3_PEND));
		break;

	case 63:
		hprintf(TSENV, "\r\nEvt REG_SRC_SF0 0x%x = 0x%x", REG_SRC_SF0,
			REG32(REG_SRC_SF0));
		//hprintf(TSENV, "\r\nREG_SRC_SC_TO_SF0 0x%x = 0x%x",REG_SRC_SC_TO_SF0,REG32(REG_SRC_SC_TO_SF0));
		//hprintf(TSENV, "\r\nREG_SRC_A550_TO_SF0 0x%x = 0x%x",REG_SRC_A550_TO_SF0,REG32(REG_SRC_A550_TO_SF0));
		val = REG32(REG_SRC_SF0);
		REG32(REG_SRC_SF0) = val;
		//hprintf(TSENV, "\r\nAfter REG_SRC_SF0 0x%x = 0x%x",REG_SRC_SF0,REG32(REG_SRC_SF0));
		//hprintf(TSENV, "\r\nAfter REG_SRC_SC_TO_SF0 0x%x = 0x%x",REG_SRC_SC_TO_SF0,REG32(REG_SRC_SC_TO_SF0));
		//hprintf(TSENV, "\r\nAfter REG_SRC_A550_TO_SF0 0x%x = 0x%x",REG_SRC_A550_TO_SF0,REG32(REG_SRC_A550_TO_SF0));
		break;

	case 64:
		hprintf(TSENV, "\r\nEvt REG_SRC_SF1 0x%x = 0x%x", REG_SRC_SF1,
			REG32(REG_SRC_SF1));
		val = REG32(REG_SRC_SF1);
		REG32(REG_SRC_SF1) = val;
		break;
	}
}

FIQ_DEFINE(59, isr_IpcEvent, 59, "sec IPC BANK0",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(60, isr_IpcEvent, 60, "sec IPC BANK1",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(61, isr_IpcEvent, 61, "sec IPC BANK2",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(62, isr_IpcEvent, 62, "sec IPC BANK3",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(63, isr_IpcEvent, 63, "sec IPC0 EVENT",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(64, isr_IpcEvent, 64, "sec IPC1 EVENT",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);

//******************************end of ipc event handle*******************//

//******************************core_event handle***************************//
void ResetSecure(int para)
{
	hprintf(TSENV, "\nResetSecure parameter %d", para);
	//mask reset inof
	//REG32(0xc003500c) |= (1<<8);
	REG32(0xc003500c) = 0x11f;

	REG32(0xc0035008) &= ~(1 << 14);
	delaySoft(1000);
	REG32(0xc0035008) |= (1 << 14);
}

void ResetSecure_SetFlag(void)
{
	//close secure wdt reset
	//REG32(0xc003500c) |= (1<<4);
	REG32(0xc003500c) |= (1 << 4);
	hprintf(TSENV, "\n\r0xc003500c 0x%x", REG32(0xc003500c));
}

void CoreEvent_OpenIrq(void)
{
	unsigned i;

	ResetSecure_SetFlag();

	//wdt
	SAFETY_FIQ_ENABLE(57);
	SAFETY_FIQ_ENABLE(58);

	//pll
	SAFETY_FIQ_ENABLE(66);
	SAFETY_FIQ_ENABLE(67);
	SAFETY_FIQ_ENABLE(68);
	SAFETY_FIQ_ENABLE(69);
	SAFETY_FIQ_ENABLE(70);
	SAFETY_FIQ_ENABLE(71);

	//A55 int

	for (i = 72; i <= 111; i++) {
		SAFETY_FIQ_ENABLE(i);
	}

	//secure parity&ecc
	SAFETY_FIQ_ENABLE(112);
	SAFETY_FIQ_ENABLE(113);
	SAFETY_FIQ_ENABLE(114);
	SAFETY_FIQ_ENABLE(115);

	//secure safety tx intr
	SAFETY_FIQ_ENABLE(150);
	SAFETY_FIQ_ENABLE(151);
	SAFETY_FIQ_ENABLE(152);
	SAFETY_FIQ_ENABLE(153);
	SAFETY_FIQ_ENABLE(154);
	SAFETY_FIQ_ENABLE(155);
}

/*
core info:such as a55 wdt,
*/
void isr_CoreEvent(int para)
{
	if (57 == para) {
		hprintf(TSENV, "\n\rSecure WDT Die");
		ResetSecure(para);
	}

	if (58 == para) {
		hprintf(TSENV, "\n\rA55 WDT Die");
	}

	if ((para >= 66) && (para <= 71)) {
		hprintf(TSENV, "\n\rPll intr,para %d", para);
	}

	if ((para >= 72) && (para <= 111)) {
		hprintf(TSENV, "\n\rintr form a55 %d", para);

		if ((para >= 103) && (para <= 108)) {
			hprintf(TSENV, "----ddr phy or controller");
			SAFETY_FIQ_DISABLE(para);
		}
	}

	if ((para >= 112) && (para <= 115)) {
		hprintf(TSENV, "\n\rSecure parity,para %d", para);
		if (para == 115) {
			hprintf(TSENV, "----ecc");
		}
	}

	if ((para >= 150) && (para <= 155)) {
		if ((para == 151) || (para == 154) || (para == 155)) {
			hprintf(TSENV, "\n\rSecure Tx int");
			hprintf(TSENV, "\n\rACK intr to Secure");
			REG32(0xc0037060) = 0x8;
		} else {
			hprintf(TSENV, "\n\rSafety Tx int");
			hprintf(TSENV, "\n\r0xc0037060 = 0x%x",
				REG32(0xc0037060));
			hprintf(TSENV, "\n\rclean intr");
			REG32(0xc0037060) = 0;
		}
	}
}

void Secure_TxTst(int para)
{
	unsigned long val;

	hprintf(TSENV, "\r\ninput 0xc0037060 value(0~F)?:");
	if (getHex(&val)) {
		val = 2;
	}

	if (val > 0xf) {
		val = 2;
	}

	REG32(0xc0037060) = val;
}

//******************************end core_event handle***************************//

//***********************************critical intr handle**********************//
void parityClear(void)
{
	//inject info
	hprintf(TSENV, "\n\r0xc0035130 = 0x%x", REG32(0xc0035130));
	hprintf(TSENV, "\n\r0xc0035134 = 0x%x", REG32(0xc0035134));
	hprintf(TSENV, "\n\r0xc0035118 = 0x%x", REG32(0xc0035118));

	//clean
	hprintf(TSENV, "\n\rclear ram parity inject");
	REG32(0xc0035118) &= ~(1 << 16);
	hprintf(TSENV, "\n\rclear inject1");
	REG32(0xc0035130) = 0;
	REG32(0xc0035134) = 0;

	hprintf(TSENV, "\n\r0xc0035130 = 0x%x", REG32(0xc0035130));
	hprintf(TSENV, "\n\r0xc0035134 = 0x%x", REG32(0xc0035134));
	hprintf(TSENV, "\n\r0xc0035118 = 0x%x", REG32(0xc0035118));
}

void parityOpen(void)
{
	hprintf(TSENV, "\n\ropen all parity enable");
	REG32(0xc0035120) = 0xffff;
	hprintf(TSENV, "\n\ropen all parity intr");
	REG32(0xc0035124) = 0xffff;
}

typedef struct {
	int intrNum;
	char *info;
	unsigned int injectReg;
	unsigned int injectVal;
	unsigned int intrClrReg;
	unsigned int intrClrVal;
} T_ERR_INJECT;
#define END_FLAG 0xffffff0f
/*care idx & eIdx must same*/
const T_ERR_INJECT sInjectInfo[] = { { 32, "noc timeout", 0, 0, 0, 0 },
				     { 33, "sf noc parity0", 0, 0, 0, 0 },
				     { 34, "sf noc parity1", 0, 0, 0, 0 },
				     { 35, "Sram parity", 0, 0, 0, 0 },
				     { 36, "Sram ECC", 0, 0, 0, 0 },
				     { 37, "R5 bus ECC", 0, 0, 0, 0 },
				     { 38, "R5 lock error", 0, 0, 0, 0 },
				     { 54, "PMU err", 0, 0, 0, 0 },
				     { 52, "top crm pwr req", 0, 0, 0, 0 },
				     { 55, "noc bist done", 0, 0, 0, 0 },
				     { 56, "pll unlock", 0, 0, 0, 0 },
				     { END_FLAG, NULL, 0, 0, 0, 0 } };

static const T_ERR_INJECT *InjectGetObj(int intrNum)
{
	int i = 0;

	while (sInjectInfo[i++].intrNum != END_FLAG) {
		if (sInjectInfo[i].intrNum == intrNum) {
			return &sInjectInfo[i];
		}
	}

	return NULL;
}

void isr_CriticalEvent(int para)
{
	const T_ERR_INJECT *pInfo;
	hprintf(TSENV, "\n\rCritical int: %d", para);

	//A55 ready
	if (para == 52) {
		hprintf(TSENV, "\n\rA55 rdy,disable now");
		SAFETY_FIQ_DISABLE(52);
		return;
	}

	parityClear();
	//*************************general handler****************************//
	if (para == 33) {
		hprintf(TSENV, "\n\rNoc parity1");
		hprintf(TSENV, "\n\r0xc0035128 = 0x%x", REG32(0xc0035128));
		//hprintf(TSENV, "\n\r0xc003512c = 0x%x",REG32(0xc003512c));

		if (REG32(0xc0035128) & 0xf0000) {
			hprintf(TSENV, "\n\rsram err,clean intr,disable");
			//clean
			REG32(0xc0035124) |= (1 << 24);
			//close
			//REG32(0xc0035124) &= ~(1<<8);
			//hprintf(TSENV, "\n\r0xc0035124 = 0x%x",REG32(0xc0035124));
		}

		if (REG32(0xc0035128) & 0x3c) {
			hprintf(TSENV, "\n\rR5 find partity err");
			//clean
			REG32(0xc0035124) |= (3 << 17);
			//close
			//REG32(0xc0035124) &= ~(3<<1);
			//hprintf(TSENV, "\n\r0xc0035124 = 0x%x",REG32(0xc0035124));
		}
	}

	//general handler,noc1
	if (para == 34) {
		hprintf(TSENV, "\n\rNoc parity2");
		//hprintf(TSENV, "\n\r0xc0035128 = 0x%x",REG32(0xc0035128));
		hprintf(TSENV, "\n\r0xc003512c = 0x%x", REG32(0xc003512c));

		if (REG32(0xc003512c) & 0xfc00) {
			hprintf(TSENV, "\n\rsec2safe noc error");
			hprintf(TSENV, "\n\rneed secure set f00010c0 = 0");
		}

		//canfd0 inject error
		if (REG32(0xc003512c) & 0xf) {
			hprintf(TSENV, "\n\rapb bus1 find partity err");
			//clean
			REG32(0x200160c4) = 0x1;
			REG32(0xc0035124) |= (1 << 29);
		}
		//canfd1 inject error
		if (REG32(0xc003512c) & 0xf0) {
			hprintf(TSENV, "\n\rapb bus2 find partity err");
			//clean
			REG32(0x200170c4) = 0x1;
			REG32(0xc0035124) |= (1 << 30);
		}
	}

	//SRAM parity
	if (para == 35) {
		hprintf(TSENV, "\n\rSarm find parity error");
		hprintf(TSENV, "\n\r0xc0035128 = 0x%x", REG32(0xc0035128));
		hprintf(TSENV, "\n\r0xc003512c = 0x%x", REG32(0xc003512c));

		hprintf(TSENV, "\n\rsram err,clean intr");
		//clean （ecc and parity is same）
		REG32(0xc0035124) |= (1 << 24);
	}

	//SRAM ecc
	if (para == 36) {
		hprintf(TSENV, "\n\rSarm ecc");
		hprintf(TSENV, "\n\rstrap pin 0xc0035100 = 0x%x",
			REG32(0xc0035100));
		hprintf(TSENV, "\n\recc cnt 0xc003511c = 0x%x",
			REG32(0xc003511c));

		hprintf(TSENV, "\n\rClose ecc intr");
		SAFETY_FIQ_DISABLE(36);

		hprintf(TSENV, "\n\rsram err,clean intr");
		//clean （ecc and parity is same）
		REG32(0xc0035124) |= (1 << 24);
		//close
		//REG32(0xc0035124) &= ~(1<<8);
		//hprintf(TSENV, "\n\r0xc0035124 = 0x%x",REG32(0xc0035124));
	}

	if (para == 38) {
		unsigned long reg_base = 0xc003705c;
		unsigned long val;

		val = REG32_READ(reg_base);
		//hprintf(TSENV, "[%s]%d.\r\n",__func__,__LINE__);
		val &= ~0x81;
		REG32_WRITE(reg_base, val); //set R5 LOCK
		hprintf(TSENV, "\n\rr5 lock clean Done.");
	}
	//********************more handle **************//
	pInfo = InjectGetObj(para);
	if (pInfo == NULL) {
		hprintf(TSENV, "\n\rcan't fine event info");
		return;
	}
	hprintf(TSENV, "\n\r%s", pInfo->info);

	if (pInfo->intrClrReg) {
		hprintf(TSENV, "\n\rclear int");
		REG32(pInfo->intrClrReg) &= ~(pInfo->intrClrVal);
	}
}

void Critical_OpenIntr(void)
{
	//ecc parity open by pin config
	//hprintf(TSENV, "\n\rbefore open critical intr");
	//getc();

	//intr enable
	//SAFETY_FIQ_ENABLE(32);
	SAFETY_FIQ_ENABLE(33);
	SAFETY_FIQ_ENABLE(34);
	SAFETY_FIQ_ENABLE(35);
	SAFETY_FIQ_ENABLE(36);
	SAFETY_FIQ_ENABLE(37);
	SAFETY_FIQ_ENABLE(38);

	SAFETY_FIQ_ENABLE(52);
	SAFETY_FIQ_ENABLE(53);
	SAFETY_FIQ_ENABLE(54);
	SAFETY_FIQ_ENABLE(55);
	SAFETY_FIQ_ENABLE(56);

	hprintf(TSENV, "\n\rCritical_OpenIntr done");
}

void parityTst(int para)
{
	unsigned long regVal;
	char c;

	parityOpen();
	dAbrtSetCb(parityClear);
	hprintf(TSENV, "\n\r1....inject config c0035130 ");
	hprintf(TSENV, "\n\r2....inject config c0035134");
	hprintf(TSENV, "\n\r3....inject ram parity");
	hprintf(TSENV, "\n\rother....not inject");
	c = getc();
	if (c == '1') {
		hprintf(TSENV, "\n\rReg 0xc0035130 = 0x");
		getHex(&regVal);
		REG32(0xc0035130) = regVal;
	} else if (c == '2') {
		hprintf(TSENV, "\n\rReg 0xc0035134 = 0x");
		getHex(&regVal);
		REG32(0xc0035134) = regVal;
	} else if (c == '3') {
		hprintf(TSENV, "\n\rsram parity inject!");
		REG32(0xc0035118) |= (1 << 16);
	} else {
		hprintf(TSENV, "\n\rno inject!");
	}
}

void trapInfo(char *info, unsigned int tstReg)
{
	char c;
	unsigned int regVal;

	while (1) {
		hprintf(TSENV, "\n\r%s ....press c to next", info);
		c = getc();
		if (c == 'c')
			return;

		if (tstReg > 0) {
			regVal = REG32(tstReg);
			hprintf(TSENV, "\n\r 0x%x = 0x%x", tstReg, regVal);
			//REG32(tstReg) = regVal;
		}
	}
}

void parityAutoTst(int para)
{
	dAbrtSetCb(parityClear);
	parityOpen();

	hprintf(TSENV, "\n\rinject ram to noc");
	REG32(0xc0035118) |= (1 << 16);
	trapInfo("inject ram to noc", 0x18000000);

	hprintf(TSENV, "\n\rinject noc to ram addr");
	REG32(0xc0035134) |= (1 << 6);
	trapInfo("inject noc to ram addr", 0x18000000);

	hprintf(TSENV, "\n\rinject noc to ram data");
	REG32(0xc0035130) |= (1 << 14);
	trapInfo("inject noc to ram data", 0x18000000);

	hprintf(TSENV, "\n\rinject apb br2 data");
	REG32(0xc0035130) |= (1 << 6);
	trapInfo("inject apb br2 data", 0x20003000);

	/*
    //parityOpen();   
    hprintf(TSENV, "\n\rinject noc to R5 data");           
    //REG32(0xc0035130) |= ((1<<26)|(1<<28));    
    REG32(0xc0035130) |= (1<<26);    
    //REG32(0xc0035130) |= (1<<28);    
    trapInfo("inject noc to R5 data",0);
    
    //parityOpen();   
    hprintf(TSENV, "\n\rinject noc to Gic data");           
    REG32(0xc0035130) |= (1<<22);    
    trapInfo("inject noc to gic data",0);
    */
}

//********************************end ofcritical intr handle****************//

void OpenDefaultIntr(void)
{
	Critical_OpenIntr();
	SafetyIpc_OpenIrq();
	hprintf(TSENV, "\n\ropen core");
	//getc();
	CoreEvent_OpenIrq();
}

void test_r5_lock_trig(void)
{
	unsigned long reg_base = 0xc003705c;
	unsigned long reg_en_base = 0xc0037010;
	unsigned long val;
	unsigned long val_en;

	val_en = REG32_READ(reg_en_base);
	val_en |= 0x100;
	REG32_WRITE(reg_en_base, val_en); //set R5 LOCK

	val = REG32_READ(reg_base);
	//hprintf(TSENV, "[%s]%d.\r\n",__func__,__LINE__);
	val |= 0x81;
	REG32_WRITE(reg_base, val); //set R5 LOCK
	hprintf(TSENV, "[%s]%d r5 lock trig Done.\r\n", __func__, __LINE__);
}

void test_r5_lock_int(int para)
{
	hprintf(TSENV, "[%s]%d Start r5 lock trig.\r\n", __func__, __LINE__);
	test_r5_lock_trig();
}

FIQ_DEFINE(32, isr_CriticalEvent, 32, "noc timeout",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(33, isr_CriticalEvent, 33, "sf noc parity0",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(34, isr_CriticalEvent, 34, "sf noc parity1",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(35, isr_CriticalEvent, 35, "Sram parity",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(36, isr_CriticalEvent, 36, "Sram ecc",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(37, isr_CriticalEvent, 37, "R5 bus ecc",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(38, isr_CriticalEvent, 38, "R5 lock err",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);

FIQ_DEFINE(52, isr_CriticalEvent, 52, "top crm pwr req",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(53, isr_CriticalEvent, 53, "reservec",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(54, isr_CriticalEvent, 54, "pmu err",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(55, isr_CriticalEvent, 55, "sec noc bist done",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(56, isr_CriticalEvent, 56, "pll unlock",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
#ifndef CONFIG_WDT_TEST
FIQ_DEFINE(57, isr_CoreEvent, 57, "Secure WDT die isr",
	   ISR_ATTR_SAFETY | ISR_ATTR_EDGE);
FIQ_DEFINE(58, isr_CoreEvent, 58, "A55 WDT die isr",
	   ISR_ATTR_SAFETY | ISR_ATTR_EDGE);
#endif
FIQ_DEFINE(66, isr_CoreEvent, 66, "pll main err",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(67, isr_CoreEvent, 67, "pll lsp err",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(68, isr_CoreEvent, 68, "pll main unlock",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(69, isr_CoreEvent, 69, "pll lsp unlock",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(70, isr_CoreEvent, 70, "pll main lock lose",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(71, isr_CoreEvent, 71, "pll lsp lock lose",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);

FIQ_DEFINE(72, isr_CoreEvent, 72, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(73, isr_CoreEvent, 73, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(74, isr_CoreEvent, 74, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(75, isr_CoreEvent, 75, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(76, isr_CoreEvent, 76, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(77, isr_CoreEvent, 77, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(78, isr_CoreEvent, 78, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(79, isr_CoreEvent, 79, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(80, isr_CoreEvent, 80, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(81, isr_CoreEvent, 81, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(82, isr_CoreEvent, 82, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(83, isr_CoreEvent, 83, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(84, isr_CoreEvent, 84, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(85, isr_CoreEvent, 85, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(86, isr_CoreEvent, 86, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(87, isr_CoreEvent, 87, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(88, isr_CoreEvent, 88, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(89, isr_CoreEvent, 89, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(90, isr_CoreEvent, 90, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(91, isr_CoreEvent, 91, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(92, isr_CoreEvent, 92, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(93, isr_CoreEvent, 93, "from A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(94, isr_CoreEvent, 94, "form A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(95, isr_CoreEvent, 95, "form A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(96, isr_CoreEvent, 96, "form A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(97, isr_CoreEvent, 97, "form A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(98, isr_CoreEvent, 98, "form A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(99, isr_CoreEvent, 99, "form A55", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(100, isr_CoreEvent, 100, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(101, isr_CoreEvent, 101, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(102, isr_CoreEvent, 102, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(103, isr_CoreEvent, 103, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(104, isr_CoreEvent, 104, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(105, isr_CoreEvent, 105, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(106, isr_CoreEvent, 106, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(107, isr_CoreEvent, 107, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(108, isr_CoreEvent, 108, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(109, isr_CoreEvent, 109, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(110, isr_CoreEvent, 110, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(111, isr_CoreEvent, 111, "from A55",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);

FIQ_DEFINE(112, isr_CoreEvent, 112, "secnoc_pty_intr0",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(113, isr_CoreEvent, 113, "secnoc_pty_intr1",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(114, isr_CoreEvent, 114, "sec_parity_intr",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(115, isr_CoreEvent, 115, "sec_ecc_intr",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);

FIQ_DEFINE(150, isr_CoreEvent, 150, "safety_r5_tx_int3",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(151, isr_CoreEvent, 151, "sec_r5_tx_int3",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(152, isr_CoreEvent, 152, "Safety_r5_tx_int1",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(153, isr_CoreEvent, 153, "Safety_r5_tx_int2",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(154, isr_CoreEvent, 154, "sec_r5_tx_int1",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
FIQ_DEFINE(155, isr_CoreEvent, 155, "sec_r5_tx_int2",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);

//CMD_DEFINE(entimer, timerIrqOpen , 0, "timer irq enabl;e", 0);
//CMD_DEFINE(distimer, timerIrqClose , 0, "timer irq disable", 0);
//IRQ_DEFINE(FIQ_IRQ_LSP0_TIMER0, irq_timer , TIMER0_PWM0_BASE_ADDR, "irq timer isr", ISR_ATTR_SAFETY|ISR_ATTR_EDGE);
//FIQ_DEFINE(FIQ_IRQ_LSP1_TIMER0, fiq_timer , TIMER1_PWM1_BASE_ADDR, "fiq timer isr", ISR_ATTR_SAFETY|ISR_ATTR_EDGE);

CMD_DEFINE(r5lock, test_r5_lock_int, 0, "r5 lock err set ", 0);
CMD_DEFINE(prauto, parityAutoTst, 0, "parity intr auto test", 0);
CMD_DEFINE(prtst, parityTst, 0, "parity intr manu test", 0);
CMD_DEFINE(swi, SwiCall, 0, "swi test", 0);
CMD_DEFINE(resec, ResetSecure, 0, "reset secure cpu", 0);
CMD_DEFINE(a55, load_a55, 0, "load_a55", 0);
CMD_DEFINE(txtst, Secure_TxTst, 0, "Safety tx tst", 0);
//CMD_DEFINE(ipctst, Secure_IpcTst, 0, "Safety to other ipc tst", 0);
