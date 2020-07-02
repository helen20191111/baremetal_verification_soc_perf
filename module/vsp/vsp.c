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
//  You are running an EVALUATION distribution of the neural network tools provided by       //
//  Black Sesame Technologies, Inc. under NDA.                                               //
//  This copy may NOT be used in production or distributed to any third party.               //
//  For distribution or production, further Software License Agreement is required.          //
//                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////
#include <common.h>
#include "vsp.h"

#define REG_CHECK(ADDR, W_VAL, GOLDEN_VAL)   \
	writel(W_VAL, ADDR);    \
	regval = readl(ADDR);   \
	result = (regval == GOLDEN_VAL) ? 1 : 0;  \
	if(result == 0) { \
		printk(KERN_ERR, "vsp verify error: reg-0x%08x, w-0x%08x, r-0x%08x, g-0x%08x", ADDR, W_VAL, regval, GOLDEN_VAL); \
	}
#define REG_CHECK_RO(ADDR, GOLDEN_VAL)   \
	regval = readl(ADDR);   \
	result = (regval == GOLDEN_VAL) ? 1 : 0;  \
	if(result == 0) { \
		test_result = 0;  \
		printk(KERN_ERR, "vsp verify error: reg-0x%08x, r-0x%08x, g-0x%08x", ADDR, regval, GOLDEN_VAL); \
	}	
extern void a55Ipc_OpenIrq(void);

int vsp_dev(void)
{
	u32 regval;
	u32 result = 0;	
	u32 test_result = 1;

	// Register R/W verification
	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 4), 0xef, 0xef);    // reset

	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 0x10), 0x0, 0x0);    //PC me
	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 0x14), 0x0, 0x0);    //PC md
	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 0x18), 0x0, 0x0);    //PC code

	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 0x18), 0x0, 0x0);    //PC code
	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 0x18), 0x0, 0x0);    //PC code

	REG_CHECK((volatile u32 *)(VSP_BASE_DBGBUS_BSMEM + 0), 0x00ffffff, 0x00ffffff);
	//REG_CHECK((volatile u32 *)(VSP_BASE_DBGBUS_XFORM + 0)  //RO
	REG_CHECK((volatile u32 *)(VSP_BASE_DBGBUS_ME + 8), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(VSP_BASE_DBGBUS_MD + 8), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(VSP_BASE_DBGBUS_CODE + 8), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(VSP_BASE_DBGBUS_CODE_CMEM + 0), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(VSP_BASE_DBGBUS_BSRAM + 0), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(VSP_BASE_INT2SYNC + 0x500), 0xffffffff, 0xffffffff);


#ifdef CONFIG_VRISCV_BOOT
	{
	u32* entry = (u32 *)0x03a00000;
	u32 vsp_riscv_text[24] = {  // pay attention to DDR base for VSP  core
		0x0480006f, 0x00000013, 0x0380006f, 0x00000013,
		0x0300006f, 0x00000013, 0x0280006f, 0x00000013,
		0x0200006f, 0x00000013, 0x0180006f, 0x00000013,
		0x0100006f, 0x00000013, 0x0080006f, 0x00000013,
		0x24001fd1, 0x00000013, 0x331032b7, 0xf0028293,
		0x00008337, 0x00030313, 0x000062d5, 0xfe5ff06f,
	};
	int i;

	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 0x10), 0x03e00000, 0x03e00000);    //PC me
	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 0x14), 0x03c00000, 0x03c00000);    //PC md
	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 0x18), 0x03a00000, 0x03a00000);    //PC code
	REG_CHECK((volatile u32 *)(VSP_IPC_CODE2A55CORE0_EN_ADDR), 0x4003fffe, 0x8003fffe);    //enable IPC interrupt
	a55Ipc_OpenIrq();

	/*****************************************************
	 * Test VSP RISCV core and IPC(RISCV->ARM A55 core0)
	 * 1. reset vsp riscv core;
	 * 2. load vsp riscv binary code to DDR;
	 * 3. release vsp riscv core(code);
	 * 4. check PC by read register: 0x54160020(thread-0 PC),
	 *    if PC == 0x03a0005c, then succeed;
	 * 5. check ARM A55(core-0) IPC INT status, 
	 *    if reg_val(0x33102000) == 0x00008000, then succeed;
	 */
	for(i=0; i<(sizeof(vsp_riscv_text) / 4); i++) {
		REG_CHECK((entry + i * 4), vsp_riscv_text[i], vsp_riscv_text[i]);
	}

	REG_CHECK((volatile u32 *)(VSP_BASE_TOP + 4), 0xff, 0xff);    //release vsp code core

	udelay(1000000);
	REG_CHECK_RO((volatile u32 *)(VSP_BASE_DBGBUS_CODE + 0x20), 0x03a0005c);   //check PC
	
	REG_CHECK_RO((volatile u32 *)(VSP_IPC_CODE2A55CORE0_STAT_ADDR), 0x00008000);   //check IPC INT status	
	writel(0x00008000, (volatile u32 *)VSP_IPC_CODE2A55CORE0_STAT_ADDR);    //clear interrupt
	}
#endif

	if(test_result == 1) {
		printf("vsp test-0 pass.\n");
	} else {
		printf("vsp test-0 fail.\n");
	}
	return 0;
}
