/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _IPC_H
#define _IPC_H

//******************************ipc event handle***************************//

#define IPC_BASE 0x33100000

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
	((IPC_BASE) | (1 << 13) | (evt_mid << 8) | (1 << 7) | (evt_mid << 2))
#define REG_IPC_SRC(evt_mid, evt_d_mid)                                        \
	((IPC_BASE) | (1 << 13) | (evt_mid << 8) | (evt_d_mid << 2))

//A55 CPU0
#define REG_EN_A550 (REG_IPC_EN(EVT_MID_A55_0))
#define REG_SRC_A550 (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_A55_0))
#define REG_SRC_A550_TO_SF0 (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_SAFETY_0))
#define REG_SRC_A550_TO_SF1 (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_SAFETY_1))
#define REG_SRC_A550_TO_SC (REG_IPC_SRC(EVT_MID_A55_0, EVT_MID_SECURE))

//SAFETY CPU0
#define REG_EN_SF0 (REG_IPC_EN(EVT_MID_SAFETY_0))
#define REG_SRC_SF0 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_SAFETY_0))
#define REG_SRC_SF0_TO_SC (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_SECURE))
#define REG_SRC_SF0_TO_A550 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_0))
#define REG_SRC_SF0_TO_A551 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_1))
#define REG_SRC_SF0_TO_A552 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_2))
#define REG_SRC_SF0_TO_A553 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_3))
#define REG_SRC_SF0_TO_A554 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_4))
#define REG_SRC_SF0_TO_A555 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_5))
#define REG_SRC_SF0_TO_A556 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_6))
#define REG_SRC_SF0_TO_A557 (REG_IPC_SRC(EVT_MID_SAFETY_0, EVT_MID_A55_7))

//SECURE
#define REG_EN_SE (REG_IPC_EN(EVT_MID_SECURE))
#define REG_SRC_SC (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_SECURE))
#define REG_SRC_SC_TO_SF0 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_SAFETY_0))
#define REG_SRC_SC_TO_SF1 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_SAFETY_1))
#define REG_SRC_SC_TO_A550 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_0))
#define REG_SRC_SC_TO_A551 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_1))
#define REG_SRC_SC_TO_A552 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_2))
#define REG_SRC_SC_TO_A553 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_3))
#define REG_SRC_SC_TO_A554 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_4))
#define REG_SRC_SC_TO_A555 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_5))
#define REG_SRC_SC_TO_A556 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_6))
#define REG_SRC_SC_TO_A557 (REG_IPC_SRC(EVT_MID_SECURE, EVT_MID_A55_7))

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

#define REG_SMPH_A55_BANK0_PEND (REG_SMPH_PND(SM_MID_A55, 0))
#define REG_SMPH_A55_BANK1_PEND (REG_SMPH_PND(SM_MID_A55, 1))
#define REG_SMPH_A55_BANK2_PEND (REG_SMPH_PND(SM_MID_A55, 2))
#define REG_SMPH_A55_BANK3_PEND (REG_SMPH_PND(SM_MID_A55, 3))

#define REG_SMPH_A55_BANK0_ID0 (REG_SMPH_ACQ(SM_MID_A55, 0, 0))
#define REG_SMPH_A55_BANK1_ID0 (REG_SMPH_ACQ(SM_MID_A55, 1, 0))
#define REG_SMPH_A55_BANK2_ID0 (REG_SMPH_ACQ(SM_MID_A55, 2, 0))
#define REG_SMPH_A55_BANK3_ID0 (REG_SMPH_ACQ(SM_MID_A55, 3, 0))

#endif
