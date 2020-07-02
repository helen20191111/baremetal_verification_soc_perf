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
#include <RegTest.h>

u32 core_id;
extern unsigned get_core(void);
//************event********//
#define EVT_MID_A55_0 (0)
#define EVT_MID_A55_1 (1)
#define EVT_MID_A55_2 (2)
#define EVT_MID_A55_3 (3)
#define EVT_MID_A55_4 (4)
#define EVT_MID_A55_5 (5)
#define EVT_MID_A55_6 (6)
#define EVT_MID_A55_7 (7)

#define EVT_MID_SAFETY_0 (8)
#define EVT_MID_SAFETY_1 (9)
#define EVT_MID_SECURE (17)

#define REG_IPC_EN(evt_mid)                                                    \
	((IPC_BASE) | (1 << 13) | (evt_mid << 8) | (1 << 7) |                  \
	 (evt_mid << 2)) //1<<13:event offset@ipc
#define REG_IPC_SRC(evt_mid, evt_d_mid)                                        \
	((IPC_BASE) | (1 << 13) | (evt_mid << 8) | (evt_d_mid << 2))

//A55 CPU0
#define REG_EN_A550 (REG_IPC_EN(EVT_MID_A55_0))
#define REG_SRC_A550 (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_A55_0))
#define REG_SRC_A550_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_SAFETY_0))
#define REG_SRC_A550_TO_SC (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_SECURE))

#define REG_EN_A551 (REG_IPC_EN(EVT_MID_A55_1))
#define REG_SRC_A551 (REG_IPC_SRC(EVT_MID_A55_1, EVT_MID_A55_1))
#define REG_SRC_A551_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_1, EVT_MID_SAFETY_0))
#define REG_SRC_A551_TO_SC (REG_IPC_SRC(EVT_MID_A55_1, EVT_MID_SECURE))

#define REG_EN_A552 (REG_IPC_EN(EVT_MID_A55_2))
#define REG_SRC_A552 (REG_IPC_SRC(EVT_MID_A55_2, EVT_MID_A55_2))
#define REG_SRC_A552_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_2, EVT_MID_SAFETY_0))
#define REG_SRC_A552_TO_SC (REG_IPC_SRC(EVT_MID_A55_2, EVT_MID_SECURE))

#define REG_EN_A553 (REG_IPC_EN(EVT_MID_A55_3))
#define REG_SRC_A553 (REG_IPC_SRC(EVT_MID_A55_3, EVT_MID_A55_3))
#define REG_SRC_A553_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_3, EVT_MID_SAFETY_0))
#define REG_SRC_A553_TO_SC (REG_IPC_SRC(EVT_MID_A55_3, EVT_MID_SECURE))

#define REG_EN_A554 (REG_IPC_EN(EVT_MID_A55_4))
#define REG_SRC_A554 (REG_IPC_SRC(EVT_MID_A55_4, EVT_MID_A55_4))
#define REG_SRC_A554_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_4, EVT_MID_SAFETY_0))
#define REG_SRC_A554_TO_SC (REG_IPC_SRC(EVT_MID_A55_4, EVT_MID_SECURE))

#define REG_EN_A555 (REG_IPC_EN(EVT_MID_A55_5))
#define REG_SRC_A555 (REG_IPC_SRC(EVT_MID_A55_5, EVT_MID_A55_5))
#define REG_SRC_A555_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_5, EVT_MID_SAFETY_0))
#define REG_SRC_A555_TO_SC (REG_IPC_SRC(EVT_MID_A55_5, EVT_MID_SECURE))

#define REG_EN_A556 (REG_IPC_EN(EVT_MID_A55_6))
#define REG_SRC_A556 (REG_IPC_SRC(EVT_MID_A55_6, EVT_MID_A55_6))
#define REG_SRC_A556_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_6, EVT_MID_SAFETY_0))
#define REG_SRC_A556_TO_SC (REG_IPC_SRC(EVT_MID_A55_6, EVT_MID_SECURE))

#define REG_EN_A557 (REG_IPC_EN(EVT_MID_A55_7))
#define REG_SRC_A557 (REG_IPC_SRC(EVT_MID_A55_7, EVT_MID_A55_7))
#define REG_SRC_A557_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_7, EVT_MID_SAFETY_0))
#define REG_SRC_A557_TO_SC (REG_IPC_SRC(EVT_MID_A55_7, EVT_MID_SECURE))

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
	((IPC_BASE) | (sm_mid << 9) | (bank << 7) |                            \
	 (sm_id << 2)) //sm_mid<<9:offset+0x200, sm_id<<2:4bytes align
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

static void enable_core_1_to_7_ipc_event(void)
{
	hprintf(TSENV, "REG_EN_A551 0x%x = 0x%x\r\n", REG_EN_A551,
		REG32(REG_EN_A551));
	REG32(REG_EN_A551) = 0x7fffffff;
	A55_IRQ_ENABLE(146);

	hprintf(TSENV, "REG_EN_A552 0x%x = 0x%x\r\n", REG_EN_A552,
		REG32(REG_EN_A552));
	REG32(REG_EN_A552) = 0x7fffffff;
	A55_IRQ_ENABLE(147);

	hprintf(TSENV, "REG_EN_A553 0x%x = 0x%x\r\n", REG_EN_A553,
		REG32(REG_EN_A553));
	REG32(REG_EN_A553) = 0x7fffffff;
	A55_IRQ_ENABLE(148);

	hprintf(TSENV, "REG_EN_A554 0x%x = 0x%x\r\n", REG_EN_A554,
		REG32(REG_EN_A554));
	REG32(REG_EN_A554) = 0x7fffffff;
	A55_IRQ_ENABLE(149);

	hprintf(TSENV, "REG_EN_A555 0x%x = 0x%x\r\n", REG_EN_A555,
		REG32(REG_EN_A555));
	REG32(REG_EN_A555) = 0x7fffffff;
	A55_IRQ_ENABLE(150);

	hprintf(TSENV, "REG_EN_A556 0x%x = 0x%x\r\n", REG_EN_A556,
		REG32(REG_EN_A556));
	REG32(REG_EN_A556) = 0x7fffffff;
	A55_IRQ_ENABLE(151);

	hprintf(TSENV, "REG_EN_A557 0x%x = 0x%x\r\n", REG_EN_A557,
		REG32(REG_EN_A557));
	REG32(REG_EN_A557) = 0x7fffffff;
	A55_IRQ_ENABLE(152);
}

void a55Ipc_OpenIrq(void)
{
	u32 cpu_id, bank, sm_id;

	core_id = get_core(); //A55 has 8 cores:0~7

	switch (core_id) { //enable GIC level irq and ipc to core event irq
	case 0:
		hprintf(TSENV, "REG_EN_A550 0x%x = 0x%x\r\n", REG_EN_A550,
			REG32(REG_EN_A550));
		REG32(REG_EN_A550) = 0x7fffffff;
		A55_IRQ_ENABLE(145);
		enable_core_1_to_7_ipc_event();
		break;
	case 1:
		hprintf(TSENV, "REG_EN_A551 0x%x = 0x%x\r\n", REG_EN_A551,
			REG32(REG_EN_A551));
		REG32(REG_EN_A551) = 0x7fffffff;
		A55_IRQ_ENABLE(146);
		break;
	case 2:
		hprintf(TSENV, "REG_EN_A552 0x%x = 0x%x\r\n", REG_EN_A552,
			REG32(REG_EN_A552));
		REG32(REG_EN_A552) = 0x7fffffff;
		A55_IRQ_ENABLE(147);
		break;
	case 3:
		hprintf(TSENV, "REG_EN_A553 0x%x = 0x%x\r\n", REG_EN_A553,
			REG32(REG_EN_A553));
		REG32(REG_EN_A553) = 0x7fffffff;
		A55_IRQ_ENABLE(148);
		break;
	case 4:
		hprintf(TSENV, "REG_EN_A554 0x%x = 0x%x\r\n", REG_EN_A554,
			REG32(REG_EN_A554));
		REG32(REG_EN_A554) = 0x7fffffff;
		A55_IRQ_ENABLE(149);
		break;
	case 5:
		hprintf(TSENV, "REG_EN_A555 0x%x = 0x%x\r\n", REG_EN_A555,
			REG32(REG_EN_A555));
		REG32(REG_EN_A555) = 0x7fffffff;
		A55_IRQ_ENABLE(150);
		break;
	case 6:
		hprintf(TSENV, "REG_EN_A556 0x%x = 0x%x\r\n", REG_EN_A556,
			REG32(REG_EN_A556));
		REG32(REG_EN_A556) = 0x7fffffff;
		A55_IRQ_ENABLE(151);
		break;
	case 7:
		hprintf(TSENV, "REG_EN_A557 0x%x = 0x%x\r\n", REG_EN_A557,
			REG32(REG_EN_A557));
		REG32(REG_EN_A557) = 0x7fffffff;
		A55_IRQ_ENABLE(152);
		break;
	default:
		break;
	}
	A55_IRQ_ENABLE(141); //bank0 semaphore
	A55_IRQ_ENABLE(142); //bank1 semaphore
	A55_IRQ_ENABLE(143); //bank2 semaphore
	A55_IRQ_ENABLE(144); //bank3 semaphore

	cpu_id = 0; //A55 core0~core7 in the same group
	for (bank = 0; bank < 4; bank++) {
		for (sm_id = 0; sm_id < 16; sm_id++) {
			REG32(REG_SMPH_ACQ(cpu_id, bank, sm_id)) =
				(1 << 31); //enable semaphore irq
		}
	}

	hprintf(TSENV, "a55(core%d) ipc irq enable\r\n", core_id);
}

/*
core info:such as a55 wdt,
*/
void isr_IpcEvent(int para)
{
	volatile unsigned int val;
	hprintf(TSENV, "para is %d\r\n", para);

	switch (para) { //enable ipc to core event irq
	case 141:
		val = REG32(REG_SMPH_PND(1, 0));
		hprintf(TSENV, "pending reg is %x\n\r", val);
		REG32(REG_SMPH_PND(1, 0)) = val;
		break;
	case 142:
		val = REG32(REG_SMPH_PND(1, 1));
		hprintf(TSENV, "pending reg is %x\n\r", val);
		REG32(REG_SMPH_PND(1, 1)) = val;
		break;
	case 143:
		val = REG32(REG_SMPH_PND(1, 2));
		hprintf(TSENV, "pending reg is %x\n\r", val);
		REG32(REG_SMPH_PND(1, 2)) = val;
		break;
	case 144:
		val = REG32(REG_SMPH_PND(1, 3));
		hprintf(TSENV, "pending reg is %x\n\r", val);
		REG32(REG_SMPH_PND(1, 3)) = val;
		break;
	case 145:
		val = REG32(REG_IPC_SRC(
			0, 0)); //para1: cpu(core) id, para2:event id
		hprintf(TSENV, "event reg is %x\n\r", val);
		REG32(REG_IPC_SRC(0, 0)) = val;
		break;
	case 146:
		val = REG32(REG_IPC_SRC(1, 1));
		hprintf(TSENV, "event reg is %x\n\r", val);
		REG32(REG_IPC_SRC(1, 1)) = val;
		break;
	case 147:
		val = REG32(REG_IPC_SRC(2, 2));
		hprintf(TSENV, "event reg is %x\n\r", val);
		REG32(REG_IPC_SRC(2, 2)) = val;
		break;
	case 148:
		val = REG32(REG_IPC_SRC(3, 3));
		hprintf(TSENV, "event reg is %x\n\r", val);
		REG32(REG_IPC_SRC(3, 3)) = val;
		break;
	case 149:
		val = REG32(REG_IPC_SRC(4, 4));
		hprintf(TSENV, "event reg is %x\n\r", val);
		REG32(REG_IPC_SRC(4, 4)) = val;
		break;
	case 150:
		val = REG32(REG_IPC_SRC(5, 5));
		hprintf(TSENV, "event reg is %x\n\r", val);
		REG32(REG_IPC_SRC(5, 5)) = val;
		break;
	case 151:
		val = REG32(REG_IPC_SRC(6, 6));
		hprintf(TSENV, "event reg is %x\n\r", val);
		REG32(REG_IPC_SRC(6, 6)) = val;
		break;
	case 152:
		val = REG32(REG_IPC_SRC(7, 7));
		hprintf(TSENV, "event reg is %x\n\r", val);
		REG32(REG_IPC_SRC(7, 7)) = val;
		break;
	default:
		break;
	}
}

IRQ_DEFINE(141, isr_IpcEvent, 141, "a55 IPC BANK0",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(142, isr_IpcEvent, 142, "a55 IPC BANK1",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(143, isr_IpcEvent, 143, "a55 IPC BANK2",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(144, isr_IpcEvent, 144, "a55 IPC BANK3",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(145, isr_IpcEvent, 145, "a55 core0 IPC0 EVENT",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(146, isr_IpcEvent, 146, "a55 core1 IPC0 EVENT",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(147, isr_IpcEvent, 147, "a55 core2 IPC0 EVENT",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(148, isr_IpcEvent, 148, "a55 core3 IPC0 EVENT",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(149, isr_IpcEvent, 149, "a55 core4 IPC0 EVENT",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(150, isr_IpcEvent, 150, "a55 core5 IPC0 EVENT",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(151, isr_IpcEvent, 151, "a55 core6 IPC0 EVENT",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(152, isr_IpcEvent, 152, "a55 core7 IPC0 EVENT",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);

//******************************end of ipc event handle*******************//
