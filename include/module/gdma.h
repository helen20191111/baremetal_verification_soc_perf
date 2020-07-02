/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _GDMA_H
#define _GDMA_H

/* DMA register */
#define GDMA_BASE_ADDR (0x33200000)
#define GRAM_SAFETY_MAX_OFFSET (0x3320efff)
#define GDMA_CHANNEL_CNT 8

#define TOP_CRM_BASE (0x33002000)
#define GDMA_RESETREG (0xE8)
#define SRAM_A55_BASE ((u64 *)0x18000000)
#define SRAM_A55_SENCOND ((u64 *)0x180ff000)

#define DDRC0_A55_BASE ((u64 *)0x80000000)
#define DDRC0_A55_SECOND ((u64 *)0x8000f000)
#define DDRC1_A55_BASE ((u64 *)0x180000000)
#define DDRC1_A55_SECOND ((u64 *)0x18ffffff0)

#define SRAM_A55_OUTSIDE ((u64 *)0x180ffff0)
#define DDR1_A55_OUTSIDE ((u64 *)0x27ffffff0)
#define ERROR_BASE_ADDRESS ((u64 *)0x19000000)

#define DMAC_IDREG 0x0 // GDMA: value=0x0, mask = 0x0
#define DMAC_COMPVERREG 0x8 // SDMA: value=0x3130332a, mask = 0x0 // GDMA
#define DMAC_CFGREG 0x10 // SDMA: value=0x0, mask = 0x3		 // GDMA
#define DMAC_CHENREG 0x18 // GDMA: value=0x0, mask = 0x00000003 00ff00ff
#define DMAC_INTSTATUSREG 0x30 // SDMA: value=0x0, mask = 0x0		 // GDMA
#define DMAC_COMMONREG_INTCLEARREG                                             \
	0x38 // SDMA: value=0x0, mask = 0x0		 // GDMA
#define DMAC_COMMONREG_INTSTATUS_ENABLEREG                                     \
	0x40 // SDMA: value=0xffffffffffffffff, mask = 0x10f  //GDMA
#define DMAC_COMMONREG_INTSIGNAL_ENABLEREG                                     \
	0x48 // SDMA: value=0xffffffffffffffff, mask = 0x10f  //GDMA
#define DMAC_COMMONREG_INTSTATUSREG                                            \
	0x50 // SDMA: value=0x0, mask=0x0		 // GDMA
#define DMAC_RESETREG 0x58 // SDMA: value=0x0, mask=0x1		 // GDMA
#define DMAC_LOWPOWER_CFGREG                                                   \
	0x60 // SDMA: value=0x000404040000000b, mask=0x00ffffff0000000f

#define DMAC_CHX_SAR(i)                                                        \
	(0x100 +                                                               \
	 (i - 1) *                                                             \
		 0x100) // SDMA: value=0x0, mask=0xffffffffffffffff	//GDMA
#define DMAC_CHX_DAR(i)                                                        \
	(0x108 +                                                               \
	 (i - 1) *                                                             \
		 0x100) // SDMA: value=0x0, mask=0xffffffffffffffff	//GDMA
#define DMAC_CHX_BLOCK_TS(i)                                                   \
	(0x110 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0x3fffff			//GDMA
#define DMAC_CHX_CTL(i)                                                        \
	(0x118 + (i - 1) * 0x100) // GDMA: value=0x0, mask=0xc7ffffff 7fffff50
#define DMAC_CHX_CFG(i)                                                        \
	(0x120 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x1b00000000, mask=0x7f8e739f00000000  // block here
#define DMAC_CHX_LLP(i)                                                        \
	(0x128 +                                                               \
	 (i - 1) *                                                             \
		 0x100) // SDMA: value=0x0, mask=0xffffffff ffffffc0	//GDMA
#define DMAC_CHX_STATUSREG(i)                                                  \
	(0x130 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0x0				//GDMA
#define DMAC_CHX_SWHSSRCREG(i)                                                 \
	(0x138 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0x15				//GDMA
#define DMAC_CHX_SWHSDSTREG(i)                                                 \
	(0x140 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0x15				//GDMA
#define DMAC_CHX_BLK_TFR_RESUMEREQREG(i)                                       \
	(0x148 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0x0				//GDMA
#define DMAC_CHX_AXI_IDREG(i)                                                  \
	(0x150 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0xffffffff			//GDMA
#define DMAC_CHX_AXI_QOSREG(i)                                                 \
	(0x158 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0xff				//GDMA
#define DMAC_CHX_SSTAT(i) (0x160 + (i - 1) * 0x100) // GDMA: value=0x0, mask=0x0
#define DMAC_CHX_DSTAT(i) (0x168 + (i - 1) * 0x100) // GDMA: value=0x0, mask=0x0
#define DMAC_CHX_SSTATAR(i)                                                    \
	(0x170 + (i - 1) * 0x100) // GDMA: value=0x0, mask=0xffffffff ffffffff
#define DMAC_CHX_DSTATAR(i)                                                    \
	(0x178 + (i - 1) * 0x100) // GDMA: value=0x0, mask=0xffffffff ffffffff
#define DMAC_CHX_INTSTATUS_ENABLEREG(i)                                        \
	(0x180 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0xffffffffffffffff, mask=0xf83f7ffb //GDMA
#define DMAC_CHX_INTSTATUS(i)                                                  \
	(0x188 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0x0				//GDMA
#define DMAC_CHX_INTSIGNAL_ENABLEREG(i)                                        \
	(0x190 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0xffffffffffffffff, mask=0xf83f7ffb//GDMA
#define DMAC_CHX_INTCLEARREG(i)                                                \
	(0x198 +                                                               \
	 (i -                                                                  \
	  1) * 0x100) // SDMA: value=0x0, mask=0x0				//GDMA


#define DMA_BLOCK_MAX_SIZE 0x400000 //4MB

struct lli_struct {
	u32 chx_sar_l;
	u32 chx_sar_h;
	u32 chx_dar_l;
	u32 chx_dar_h;
	u32 chx_block_ts;
	u32 reserved_1;
	u32 chx_llp_l;
	u32 chx_llp_h;
	/* optional */
	u32 chx_ctl_l;
	u32 chx_ctl_h;
	u32 write_back_for_chx_sstat;
	u32 write_back_for_chx_dstat;
	u32 chx_llp_status_l;
	u32 chx_llp_status_h;
};

#endif
