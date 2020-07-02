/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __DMAC_H_
#define __DMAC_H_

#include <a1000/sysreg_base.h>
#include <common.h>
#include <linux/list.h>
#include <module/dmac.h>

#define DMAC_MAX_BLK_SIZE	0x200000
#define GDMA_RESETREG (0xE8)

#define DMAC_IDREG 0x0
#define DMAC_COMPVERREG 0x8
#define DMAC_CFGREG 0x10

/* DMAC_CFG */
#define DMAC_EN_POS			0
#define DMAC_EN_MASK			BIT(DMAC_EN_POS)

#define INT_EN_POS			1
#define INT_EN_MASK			BIT(INT_EN_POS)

#define DMAC_CHENREG 0x18

#define DMAC_CHAN_EN_SHIFT		0
#define DMAC_CHAN_EN_WE_SHIFT		8

#define DMAC_CHAN_SUSP_SHIFT		16
#define DMAC_CHAN_SUSP_WE_SHIFT		24

#define DMAC_INTSTATUSREG 0x30
#define DMAC_COMMONREG_INTCLEARREG 0x38
#define DMAC_COMMONREG_INTSTATUS_ENABLEREG   0x40
#define DMAC_COMMONREG_INTSIGNAL_ENABLEREG   0x48
#define DMAC_COMMONREG_INTSTATUSREG   0x50
#define DMAC_RESETREG 0x58
#define DMAC_LOWPOWER_CFGREG    0x60

#define DMAC_CHX_SAR(i)  (0x100 + (i) * 0x100)
#define DMAC_CHX_DAR(i)  (0x108 + (i) * 0x100)
#define DMAC_CHX_BLOCK_TS(i)  (0x110 + (i) * 0x100)
#define DMAC_CHX_CTL(i)  (0x118 + (i) * 0x100)
#define DMAC_CHX_CFG(i)  (0x120 + (i) * 0x100)

#define DMAC_SRC_CON 0x0
#define DMAC_SRC_REL 0x1
#define DMAC_SRC_SHA 0x2
#define DMAC_SRC_LLP 0x3

#define DMAC_DST_CON (0x0 << 2)
#define DMAC_DST_REL (0x1 << 2)
#define DMAC_DST_SHA (0x2 << 2)
#define DMAC_DST_LLP (0x3 << 2)

#define MEM_TO_MEM 0

#define DMAC_CHX_LLP(i)  (0x128 + (i) * 0x100)
#define DMAC_CHX_STATUSREG(i) (0x130 + (i) * 0x100)
#define DMAC_CHX_SWHSSRCREG(i) (0x138 + (i) * 0x100)
#define DMAC_CHX_SWHSDSTREG(i) (0x140 + (i) * 0x100)
#define DMAC_CHX_BLK_TFR_RESUMEREQREG(i)  (0x148 + (i) * 0x100)
#define DMAC_CHX_AXI_IDREG(i)  (0x150 + (i) * 0x100)
#define DMAC_CHX_AXI_QOSREG(i) (0x158 + (i) * 0x100)
#define DMAC_CHX_SSTAT(i) (0x160 + (i) * 0x100)
#define DMAC_CHX_DSTAT(i) (0x168 + (i) * 0x100)
#define DMAC_CHX_SSTATAR(i)  (0x170 + (i) * 0x100)
#define DMAC_CHX_DSTATAR(i)  (0x178 + (i) * 0x100)
#define DMAC_CHX_INTSTATUS_ENABLEREG(i)   (0x180 + (i) * 0x100)
#define DMAC_CHX_INTSTATUS(i)  (0x188 + (i) * 0x100)

/**
 * DW AXI DMA channel interrupts
 *
 * @DWAXIDMAC_IRQ_NONE: Bitmask of no one interrupt
 * @DWAXIDMAC_IRQ_BLOCK_TRF: Block transfer complete
 * @DWAXIDMAC_IRQ_DMA_TRF: Dma transfer complete
 * @DWAXIDMAC_IRQ_SRC_TRAN: Source transaction complete
 * @DWAXIDMAC_IRQ_DST_TRAN: Destination transaction complete
 * @DWAXIDMAC_IRQ_SRC_DEC_ERR: Source decode error
 * @DWAXIDMAC_IRQ_DST_DEC_ERR: Destination decode error
 * @DWAXIDMAC_IRQ_SRC_SLV_ERR: Source slave error
 * @DWAXIDMAC_IRQ_DST_SLV_ERR: Destination slave error
 * @DWAXIDMAC_IRQ_LLI_RD_DEC_ERR: LLI read decode error
 * @DWAXIDMAC_IRQ_LLI_WR_DEC_ERR: LLI write decode error
 * @DWAXIDMAC_IRQ_LLI_RD_SLV_ERR: LLI read slave error
 * @DWAXIDMAC_IRQ_LLI_WR_SLV_ERR: LLI write slave error
 * @DWAXIDMAC_IRQ_INVALID_ERR: LLI invalid error or Shadow register error
 * @DWAXIDMAC_IRQ_MULTIBLKTYPE_ERR: Slave Interface Multiblock type error
 * @DWAXIDMAC_IRQ_DEC_ERR: Slave Interface decode error
 * @DWAXIDMAC_IRQ_WR2RO_ERR: Slave Interface write to read only error
 * @DWAXIDMAC_IRQ_RD2RWO_ERR: Slave Interface read to write only error
 * @DWAXIDMAC_IRQ_WRONCHEN_ERR: Slave Interface write to channel error
 * @DWAXIDMAC_IRQ_SHADOWREG_ERR: Slave Interface shadow reg error
 * @DWAXIDMAC_IRQ_WRONHOLD_ERR: Slave Interface hold error
 * @DWAXIDMAC_IRQ_LOCK_CLEARED: Lock Cleared Status
 * @DWAXIDMAC_IRQ_SRC_SUSPENDED: Source Suspended Status
 * @DWAXIDMAC_IRQ_SUSPENDED: Channel Suspended Status
 * @DWAXIDMAC_IRQ_DISABLED: Channel Disabled Status
 * @DWAXIDMAC_IRQ_ABORTED: Channel Aborted Status
 * @DWAXIDMAC_IRQ_ALL_ERR: Bitmask of all error interrupts
 * @DWAXIDMAC_IRQ_ALL: Bitmask of all interrupts
 */
enum {
	DWAXIDMAC_IRQ_NONE		= 0,
	DWAXIDMAC_IRQ_BLOCK_TRF		= BIT(0),
	DWAXIDMAC_IRQ_DMA_TRF		= BIT(1),
	DWAXIDMAC_IRQ_SRC_TRAN		= BIT(3),
	DWAXIDMAC_IRQ_DST_TRAN		= BIT(4),
	DWAXIDMAC_IRQ_SRC_DEC_ERR	= BIT(5),
	DWAXIDMAC_IRQ_DST_DEC_ERR	= BIT(6),
	DWAXIDMAC_IRQ_SRC_SLV_ERR	= BIT(7),
	DWAXIDMAC_IRQ_DST_SLV_ERR	= BIT(8),
	DWAXIDMAC_IRQ_LLI_RD_DEC_ERR	= BIT(9),
	DWAXIDMAC_IRQ_LLI_WR_DEC_ERR	= BIT(10),
	DWAXIDMAC_IRQ_LLI_RD_SLV_ERR	= BIT(11),
	DWAXIDMAC_IRQ_LLI_WR_SLV_ERR	= BIT(12),
	DWAXIDMAC_IRQ_INVALID_ERR	= BIT(13),
	DWAXIDMAC_IRQ_MULTIBLKTYPE_ERR	= BIT(14),
	DWAXIDMAC_IRQ_DEC_ERR		= BIT(16),
	DWAXIDMAC_IRQ_WR2RO_ERR		= BIT(17),
	DWAXIDMAC_IRQ_RD2RWO_ERR	= BIT(18),
	DWAXIDMAC_IRQ_WRONCHEN_ERR	= BIT(19),
	DWAXIDMAC_IRQ_SHADOWREG_ERR	= BIT(20),
	DWAXIDMAC_IRQ_WRONHOLD_ERR	= BIT(21),
	DWAXIDMAC_IRQ_LOCK_CLEARED	= BIT(27),
	DWAXIDMAC_IRQ_SRC_SUSPENDED	= BIT(28),
	DWAXIDMAC_IRQ_SUSPENDED		= BIT(29),
	DWAXIDMAC_IRQ_DISABLED		= BIT(30),
	DWAXIDMAC_IRQ_ABORTED		= BIT(31),
	DWAXIDMAC_IRQ_ALL_ERR		= (GENMASK(21, 16) | GENMASK(14, 5)),
	DWAXIDMAC_IRQ_ALL		= GENMASK(31, 0)
};

#define DMAC_CHX_INTSIGNAL_ENABLEREG(i) (0x190 + (i) * 0x100)
#define DMAC_CHX_INTCLEARREG(i) (0x198 + (i) * 0x100)

struct __packed lli_struct {
	u64 chx_sar;
	u64 chx_dar;
	u32 chx_block_ts;
	u32 reserved_1;
	u64 chx_llp;
	/* optional */
	u64 chx_ctl;
	u32 write_back_for_chx_sstat;
	u32 write_back_for_chx_dstat;
	u32 chx_llp_status;
};

struct dmac_desc {
	struct lli_struct lli;
	struct list_head xfer_list;
};

struct bst_dmac_chip;

struct bst_dmac_cha {
    struct bst_dmac_chip *dmac;
    u8 id;
	int tran_type;

	u8 chan_lock;
#define DMA_LOCK 0x1
#define BLOCK_LOCK 0x2
	u8 chan_prior;

	dma_addr_t src_addr;
	dma_addr_t dst_addr;
	u64 length;
	struct dmac_desc *desc;
	u32 flag;

};

struct bst_dmac_chip {
    char name[20];
    void __iomem *base;
    struct bst_dmac_cha cha[CONFIG_DMAC_CHA_MAX];
	int chan_num_max;
};

#define CH_CTL_H_LLI_LAST		BIT(30)
#define CH_CTL_H_LLI_VALID		BIT(31)

static inline void gic_dmac_irq_enable(void)
{
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CMN_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH1_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH2_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH3_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH4_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH5_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH6_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH7_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH8_INTR);
}

static inline void gic_dmac_irq_disable(void)
{
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CMN_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH1_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH2_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH3_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH4_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH5_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH6_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH7_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH8_INTR);
}

#define DMAC_BLOCK_DONE 0
#define DMAC_DMA_DONE 1
#define DMAC_SRC_DONE 3
#define DMAC_DST_DONE 4
#define DMAC_SRC_ERR 5
#define DMAC_DST_ERR 6
#define DMAC_SHA_INV 13

extern struct dma_test_struct dma_test;

#endif
