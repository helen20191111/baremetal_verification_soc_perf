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

#include "isp.h"

#define REG_CHECK(ADDR, W_VAL, GOLDEN_VAL)   \
	writel(W_VAL, ADDR);    \
	regval = readl(ADDR);   \
	result = (regval == GOLDEN_VAL) ? 1 : 0;  \
	if(result == 0) { \
		printk(KERN_ERR, "isp verify error: reg-0x%08x, w-0x%08x, r-0x%08x, g-0x%08x", ADDR, W_VAL, regval, GOLDEN_VAL); \
	}
#define REG_CHECK_RO(ADDR, GOLDEN_VAL)   \
	regval = readl(ADDR);   \
	result = (regval == GOLDEN_VAL) ? 1 : 0;  \
	if(result == 0) { \
		test_result = 0;  \
		printk(KERN_ERR, "isp verify error: reg-0x%08x, r-0x%08x, g-0x%08x", ADDR, regval, GOLDEN_VAL); \
	}	

extern void a55Ipc_OpenIrq(void);

int isp_dev(void)
{
	u32 regval;
	u32 result = 0;
	u32 test_result = 1;

	// Register R/W verification
	REG_CHECK((volatile u32 *)(ISP_BASE_TOP + 0), 0xffffffff, 0xffffffff); 
	writel(0xffffff9f, (volatile u32 *)(ISP_BASE_TOP + 4));    //reset

	REG_CHECK((volatile u32 *)(ISP_BASE_ENG0 + OFFSET_SNR1_ISP_NON + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG0 + OFFSET_SNR1_ISP_RISC_RW + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG0 + OFFSET_SNR2_ISP_NON + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG0 + OFFSET_SNR2_ISP_RISC_RW + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG0 + OFFSET_ISP_BIST_RW + 0), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(ISP_BASE_DMA0 + 0), 0xffffffff, 0xffffffff);

	REG_CHECK((volatile u32 *)(ISP_BASE_ENG1 + OFFSET_SNR1_ISP_NON + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG1 + OFFSET_SNR1_ISP_RISC_RW + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG1 + OFFSET_SNR2_ISP_NON + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG1 + OFFSET_SNR2_ISP_RISC_RW + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG1 + OFFSET_ISP_BIST_RW + 0), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(ISP_BASE_DMA1 + 0), 0xffffffff, 0xffffffff);

	REG_CHECK((volatile u32 *)(ISP_BASE_ENG2 + OFFSET_SNR1_ISP_NON + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG2 + OFFSET_SNR1_ISP_RISC_RW + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG2 + OFFSET_SNR2_ISP_NON + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG2 + OFFSET_SNR2_ISP_RISC_RW + 0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_ENG2 + OFFSET_ISP_BIST_RW + 0), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(ISP_BASE_DMA2 + 0), 0xffffffff, 0xffffffff);

	//REG_CHECK(ISP_BASE_RISCV + 0x420, 0xffffffff, 0xffffffff);  //RO
	REG_CHECK((volatile u32 *)(ISP_BASE_SRAM_DATA + 0x0), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(ISP_BASE_SRAM_CODE + 0x0), 0xffffffff, 0xffffffff); 
	REG_CHECK((volatile u32 *)(ISP_BASE_LTM + 0x10), 0xffffffff, 0xffffffff);
	REG_CHECK((volatile u32 *)(ISP_BASE_PDNS + 0x0), 0x202800fc, 0x202800fc);

#ifdef CONFIG_IRISCV_BOOT
	{
	u32* entry = (u32 *)0x52040000;
	u32 isp_riscv_text[14] = {
		0x0200006f, 0x0000006f, 0x0000006f, 0x0000006f,
		0x0000006f, 0x0000006f,	0x0000006f, 0x0000006f,
		0x000042b7, 0x00028293, 0x33103337, 0xe0030313,
		0x00532023, 0x0000006f,
	};
	int i;

	REG_CHECK_RO((volatile u32 *)ISP_BASE_TOP_RISCV_PC, 0x52040000);    //PC me
	REG_CHECK((volatile u32 *)ISP_IPC_ISP2A55CORE0_EN_ADDR, 0x4003fffe, 0x8003fffe);    //enable IPC interrupt
	a55Ipc_OpenIrq();

	/*****************************************************
	 * Test ISP RISCV core and IPC(RISCV->ARM A55 core0)
	 * 1. reset isp riscv core;
	 * 2. load isp riscv binary code to program sram;
	 * 3. release isp riscv core;
	 * 4. check PC by read register: 0x0x520d0020(thread-0 PC),
	 *    if PC == 0x52040034, then succeed;
	 * 5. check ARM A55(core-0) IPC INT status, 
	 *    if reg_val(0x33102000) == 0x00004000, then succeed;
	 */

	for(i=0; i<(sizeof(isp_riscv_text) / 4); i++) {
		REG_CHECK((entry + i * 4), isp_riscv_text[i], isp_riscv_text[i]);
	}

	REG32_WRITE((volatile u32 *)(ISP_BASE_TOP + 4), 0xffffffff);    //release riscv
	udelay(1000000);
	REG_CHECK_RO((volatile u32 *)(ISP_BASE_RISCV + 0x20), 0x52040034);   //check PC
	
	REG_CHECK_RO((volatile u32 *)(ISP_IPC_ISP2A55CORE0_STAT_ADDR), 0x00004000);   //check IPC status
	writel(0x00004000, (volatile u32 *)ISP_IPC_ISP2A55CORE0_STAT_ADDR);    //clear interrupt	
	}
#endif
	if(test_result == 1) {
		printf("isp test-0 pass.\n");
	} else {
		printf("isp test-0 fail.\n");
	}
	return 0;

}
