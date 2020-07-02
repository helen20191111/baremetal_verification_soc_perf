/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */
#define pr_fmt(fmt) KBUILD_BASENAME ": " fmt

#include <common.h>
#include <int_num.h>

#include "dmac.h"

/*
 * stupid
 */
struct bst_dmac_chip dmac = {
    .name = "bst gdma controller",
    .base = (void *)GDMA_BASE_ADDR,
    .chan_num_max = CONFIG_DMAC_CHA_MAX,
};

#if 0
static char *irq_sources[] = {
	[DMAC_BLOCK_DONE] =
		"dmac block transfer done",
	[DMAC_DMA_DONE] =
		"dmac dma transfer done",
	[DMAC_SRC_DONE] =
		"src transfer done",
	[DMAC_DST_DONE] =
		"dst transfer done",
	[DMAC_SRC_ERR] =
		"src transfer err",
	[DMAC_DST_ERR] =
		"dst transfer err",
	[DMAC_SHA_INV] =
		"lli or shadow invaild detected",
};
#endif

static inline u32 dmac_readl(struct bst_dmac_chip *dmac, int offset)
{
    return readl(dmac->base + offset);
}

static inline void dmac_writel(struct bst_dmac_chip *dmac, u32 v, int offset)
{
    writel(v, dmac->base + offset);
}

static inline void dmac_writeq(struct bst_dmac_chip *dmac, u64 v, int offset)
{
    writel(lower_32_bits(v), dmac->base + offset);
    writel(upper_32_bits(v), dmac->base + offset + 4);
}

static int dmac_reset(struct bst_dmac_chip *dmac)
{
    u32 val, count = 20;

    dmac_writel(dmac, 0x1, DMAC_RESETREG);
    while (1) {
        udelay(20);
        val = dmac_readl(dmac, DMAC_RESETREG);
        if ((val & 0x1) == 0)
            return 0;

        if (count-- == 0)
            break;
    }

    return -1;
}

static void dmac_dma_disable(struct bst_dmac_chip *dmac)
{
    u32 val;

    val = dmac_readl(dmac, DMAC_CFGREG);
    val &= ~DMAC_EN_MASK;
    dmac_writel(dmac, val, DMAC_CFGREG);
}

static void dmac_dma_enable(struct bst_dmac_chip *dmac)
{
    u32 val;

    val = dmac_readl(dmac, DMAC_CFGREG);
    val |= DMAC_EN_MASK;
    dmac_writel(dmac, val, DMAC_CFGREG);
}

static void dmac_irq_disable(struct bst_dmac_chip *dmac)
{
    u32 val;

    val = dmac_readl(dmac, DMAC_CFGREG);
    val &= ~INT_EN_MASK;
    dmac_writel(dmac, val, DMAC_CFGREG);
}

static void dmac_irq_enable(struct bst_dmac_chip *dmac)
{
    u32 val;

    val = dmac_readl(dmac, DMAC_CFGREG);
    val |= INT_EN_MASK;
    dmac_writel(dmac, val, DMAC_CFGREG);
}

static void dmac_dump_chan(struct bst_dmac_cha *cha)
{
    pr_debug("id: %d\n", cha->id);
    pr_debug("tran_type: %d\n", cha->tran_type);
    pr_debug("chan_lock: %d\n", cha->chan_lock);
    pr_debug("chan_prior: %d\n", cha->chan_prior);
    pr_debug("src addr: %08x\n", cha->src_addr);
    pr_debug("dst addr: %08x\n", cha->dst_addr);
    pr_debug("length: %08x\n", cha->length);
    pr_debug("flag: %d\n", cha->flag);
}

static void dmac_chan_disable(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHENREG);
    val &= ~(BIT(cha->id) << DMAC_CHAN_EN_SHIFT);
    val |= BIT(cha->id) << DMAC_CHAN_EN_WE_SHIFT;
    dmac_writel(cha->dmac, val, DMAC_CHENREG);
}

static int dmac_wait_cha_en_clear(struct bst_dmac_cha *cha)
{
    int timeout = 100;
    while (dmac_readl(cha->dmac, DMAC_CHENREG) & (BIT(cha->id) << DMAC_CHAN_EN_SHIFT)) {
        if (timeout-- == 0)
            return -1;
        udelay(10);
    }

    return 0;
}

static void dmac_chan_enable(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHENREG);
    val |= BIT(cha->id) << DMAC_CHAN_EN_SHIFT |
                BIT(cha->id) << DMAC_CHAN_EN_WE_SHIFT;
    dmac_writel(cha->dmac, val, DMAC_CHENREG);
}

static inline void dmac_chan_irq_clear(struct bst_dmac_cha *cha, u32 irq_mask)
{
	dmac_writel(cha->dmac, irq_mask, DMAC_CHX_INTCLEARREG(cha->id));
}

static inline u32 dmac_chan_irq_read(struct bst_dmac_cha *cha)
{
	return dmac_readl(cha->dmac, DMAC_CHX_INTSTATUS(cha->id));
}

static void dmac_chan_cfg(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHX_CFG(cha->id));
    val &= ~GENMASK(3, 0);
    switch (cha->tran_type) {
    case CON_TO_CON:
        val |= DMAC_SRC_CON | DMAC_DST_CON;
        pr_debug("CON_TO_CON\n");
        break;
    case SHA_TO_CON:
        val |= DMAC_SRC_SHA | DMAC_DST_CON;
        pr_debug("SHA_TO_CON\n");
        break;
    case REL_TO_SHA:
        val |= DMAC_SRC_REL | DMAC_DST_SHA;
        pr_debug("REL_TO_SHA\n");
        break;
    case SHA_TO_SHA:
        val |= DMAC_SRC_SHA | DMAC_DST_SHA;
        pr_debug("SHA_TO_SHA\n");
        break;
    case LLP_TO_LLP:
        val |= DMAC_SRC_LLP | DMAC_DST_LLP;
        pr_debug("LLP_TO_LLP\n");
        break;
    default:
        pr_debug("DEFAULT\n");
        break;
    }
    dmac_writel(cha->dmac, val, DMAC_CHX_CFG(cha->id));

    val = dmac_readl(cha->dmac, DMAC_CHX_CFG(cha->id) + 0x4);
    val &= ~GENMASK(2, 0);
    val |= MEM_TO_MEM; /* memory to memory */

    val &= ~GENMASK(4, 3); /* src dst hardware handshaking */

    val &= ~GENMASK(30, 23); /* src dst osr lmt*/
    
    if (cha->chan_lock) {
        val &= ~GENMASK(19, 17);
        val |= (cha->chan_prior & 0x7) << 17; /* set prior */
        val |= BIT(20); /* enable lock chan */

        val &= ~GENMASK(22, 21);
        if ((cha->chan_lock & 0x3) == BLOCK_LOCK)
            val |= BIT(21);
    }

    dmac_writel(cha->dmac, val, DMAC_CHX_CFG(cha->id) + 0x4);
}

static void dmac_write_sar(struct bst_dmac_cha *cha)
{
    if (cha->tran_type != LLP_TO_LLP)
        dmac_writeq(cha->dmac, (u64)cha->src_addr, DMAC_CHX_SAR(cha->id));
    else
        cha->desc->lli.chx_sar = (u64)cha->src_addr;
}

static void dmac_write_dar(struct bst_dmac_cha *cha)
{
    if (cha->tran_type != LLP_TO_LLP)
        dmac_writeq(cha->dmac, (u64)cha->dst_addr, DMAC_CHX_DAR(cha->id));
    else
        cha->desc->lli.chx_dar = (u64)cha->dst_addr;
}

static void dmac_ctl_l_conf(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHX_CTL(cha->id));
    val &= ~BIT(4); /* src address increment */
    val &= ~BIT(6); /* dst address increment */
    val &= ~GENMASK(13,  8);
    val |= 0x2 << 8; /* src width 32 bit */
    val |= 0x2 << 11; /* src width 32 bit */
    val &= ~GENMASK(21, 14); /* src dst msize 8 byte */
    dmac_writel(cha->dmac, val, DMAC_CHX_CTL(cha->id));
}

static void dmac_ctl_h_conf(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHX_CTL(cha->id) + 0x4);
    val |= BIT(6); /* src burst len enable */
    val &= ~GENMASK(14, 7);
    val |= BIT(7);
    val |= BIT(15); /* dst burst len enable */
    val &= ~GENMASK(23, 16);
    val |= BIT(16);
    val |= BIT(24);
    val |= BIT(25);
    val |= BIT(26); /* enable shadow blk done interrup */
    dmac_writel(cha->dmac, val, DMAC_CHX_CTL(cha->id) + 0x4);
}

static inline void dmac_last_block_set(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHX_CTL(cha->id) + 0x4);
    val |= BIT(30);
    dmac_writel(cha->dmac, val, DMAC_CHX_CTL(cha->id) + 0x4);
}

static inline void dmac_not_last_block_set(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHX_CTL(cha->id) + 0x4);
    val &= ~BIT(30);
    dmac_writel(cha->dmac, val, DMAC_CHX_CTL(cha->id) + 0x4);
}

static inline int dmac_ctl_status_get(struct bst_dmac_cha *cha)
{
    return (dmac_readl(cha->dmac, DMAC_CHX_CTL(cha->id) + 0x4) >> 31) & 0x1;
}

static inline void dmac_ctl_enable(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHX_CTL(cha->id) + 0x4);
    val |= BIT(31);
    dmac_writel(cha->dmac, val, DMAC_CHX_CTL(cha->id) + 0x4);
}

static void dmac_blk_ts(struct bst_dmac_cha *cha)
{
    u32 blk_ts;

    if (cha->length > DMAC_MAX_BLK_SIZE) {
        blk_ts = DMAC_MAX_BLK_SIZE;
        cha->length -= DMAC_MAX_BLK_SIZE;
        dmac_not_last_block_set(cha);
        pr_debug("not last length is %08x\n", cha->length);
    } else {
        blk_ts = cha->length;
        cha->length = 0;
        dmac_last_block_set(cha);
        pr_debug("last length is %08x\n", cha->length);
    }
    cha->src_addr += blk_ts;
    cha->dst_addr += blk_ts;
    dmac_writel(cha->dmac, blk_ts - 1, DMAC_CHX_BLOCK_TS(cha->id));
    dmac_writel(cha->dmac, 0, DMAC_CHX_BLOCK_TS(cha->id) + 0x4);
}

static void dmac_chan_irq_enable(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHX_INTSTATUS_ENABLEREG(cha->id));
    val |= GENMASK(6, 0);
    val |= BIT(13);
    dmac_writel(cha->dmac, val, DMAC_CHX_INTSTATUS_ENABLEREG(cha->id));
}

static void dmac_chan_irq_disable(struct bst_dmac_cha *cha)
{
    u32 val;

    val = dmac_readl(cha->dmac, DMAC_CHX_INTSTATUS_ENABLEREG(cha->id));
    val &= ~DWAXIDMAC_IRQ_ALL;
    dmac_writel(cha->dmac, val, DMAC_CHX_INTSTATUS_ENABLEREG(cha->id));
}

static void dmac_topcrm_reset(void)
{
    u32 val;

    val = readl(TOP_CRM_BASE_ADDR + GDMA_RESETREG);
    val |= BIT(12);
    writel(val, TOP_CRM_BASE_ADDR + GDMA_RESETREG);
}

static void dmac_hw_init(struct bst_dmac_chip *dmac)
{
    int i;

    dmac_topcrm_reset();

    if (dmac_reset(dmac)) {
        printf("dmac hardware reset failed!!!\n");
        return;
    }

    gic_dmac_irq_enable();
    dmac_irq_disable(dmac);
    dmac_dma_disable(dmac);
    for (i = 0; i < dmac->chan_num_max; i++) {
        dmac->cha[i].dmac = dmac;
        dmac->cha[i].id = i;
        dmac->cha[i].tran_type = CON_TO_CON;
        dmac->cha[i].chan_lock = 0;
        dmac->cha[i].chan_prior = 0;
        dmac->cha[i].flag = 0;
        dmac_chan_irq_disable(&dmac->cha[i]);
        dmac_chan_disable(&dmac->cha[i]);
    }
}

int dmac_xfer_start(struct bst_dmac_cha *cha)
{
    dmac_dump_chan(cha);
    dmac_irq_enable(cha->dmac);
    dmac_dma_enable(cha->dmac);
    dmac_chan_cfg(cha);
    dmac_write_sar(cha);
    dmac_write_dar(cha);
    dmac_blk_ts(cha);
    dmac_ctl_l_conf(cha);
    dmac_ctl_h_conf(cha);
    dmac_ctl_enable(cha);

    cha->flag = 0;
    dmac_chan_irq_enable(cha);
    dmac_chan_enable(cha);
    while (1) {
        if (!dmac_ctl_status_get(cha)) {
            dmac_write_sar(cha);
            dmac_write_dar(cha);
            dmac_blk_ts(cha);
            dmac_ctl_l_conf(cha);
            dmac_ctl_h_conf(cha);
            dmac_ctl_enable(cha);
            if (cha->flag & BIT(DMAC_SHA_INV)) {
                cha->flag &= ~BIT(DMAC_SHA_INV);
                dmac_writeq(cha->dmac, 0x1, DMAC_CHX_BLK_TFR_RESUMEREQREG(cha->id));
            }
        }
        if ((cha->flag & BIT(DMAC_SRC_ERR)) ||
                (cha->flag & BIT(DMAC_SRC_ERR))) {
            cha->flag = 0;
            dmac_chan_irq_disable(cha);
            dmac_chan_disable(cha);
            return 1;
        }
        if (cha->flag & BIT(DMAC_DMA_DONE))
            break;
    }
    if (dmac_wait_cha_en_clear(cha)) {
        printf("dmac xfer chan not auto clear...\n");
        return 1;
    }

    cha->flag = 0;
    return 0;
}

#if 0
static void dmac_cmn_interrupt(int irq)
{
    u32 status;

    pr_info("enter dmac cmn interrup <not implement>\n");

    status = dmac_readl(&dmac, DMAC_COMMONREG_INTSTATUSREG);
    pr_info("dmac cmn IRQ status: 0x%08x\n", status);
    dmac_writel(&dmac, status, DMAC_COMMONREG_INTCLEARREG);
}

static void dmac_interrupt(int id)
{
    unsigned long status, i;
    struct bst_dmac_cha *cha = &dmac.cha[id];

    status = dmac_chan_irq_read(cha);
    
    pr_debug("%s chan %u IRQ status: 0x%08x\n", dmac.name, id, status);

    for_each_set_bit(i, &status, ARRAY_SIZE(irq_sources))
        printf("dmac interrup %s\n", irq_sources[i]);
    cha->flag |= status;
    dmac_chan_irq_clear(cha, status);
}

IRQ_DEFINE(FIQ_IRQ_DMA_CMN_INTR, dmac_cmn_interrupt, 0, "dmac cmn intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH1_INTR, dmac_interrupt, 0, "dmac ch0 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH2_INTR, dmac_interrupt, 1, "dmac ch1 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH3_INTR, dmac_interrupt, 2, "dmac ch2 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH4_INTR, dmac_interrupt, 3, "dmac ch3 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH5_INTR, dmac_interrupt, 4, "dmac ch4 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH6_INTR, dmac_interrupt, 5, "dmac ch5 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH7_INTR, dmac_interrupt, 6, "dmac ch6 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH8_INTR, dmac_interrupt, 7, "dmac ch7 intr", ISR_ATTR_A55);
#endif

static int dmac_start(struct dma_memcpy_struct *mem)
{
    int id = mem->chan;
    int type = mem->tran_type;

    if (id >= dmac.chan_num_max)
        return 1;

    if (id != dmac.cha[id].id)
        return 1;
#if 0
    dmac.cha[id].tran_type = mem->tran_type;
    dmac.cha[id].chan_lock = mem->chan_lock;
    dmac.cha[id].chan_prior = mem->chan_prior;
    dmac.cha[id].src_addr = mem->src;
    dmac.cha[id].dst_addr = mem->dst;
    dmac.cha[id].length = mem->length;
    return dmac_xfer_start(&dmac.cha[id]);
#else
    dmac_irq_enable(&dmac);
    dmac_dma_enable(&dmac);
    dma_test.dma_memcpy(id + 1, dmac.base, (void *)mem->src, (void *)mem->dst, mem->length);
    return 0;
    switch (type) {
    case SHA_TO_CON:
        dma_test.contiguous(id + 1, (size_t)dmac.base, (void *)mem->src, (void *)mem->dst, 0);
        break;
    case REL_TO_SHA:
        dma_test.auto_reload(id + 1, (size_t)dmac.base, (void *)mem->src, (void *)mem->dst, 0);
        break;
    case SHA_TO_SHA:
        dma_test.shadow(id + 1, (size_t)dmac.base, (void *)mem->src, (void *)mem->dst, 0);
        break;
    case LLP_TO_LLP:
        dma_test.llp(id + 1, (size_t)dmac.base, (void *)mem->src, (void *)mem->dst, 0);
        break;
    default:
        dma_test.single(id + 1, (size_t)dmac.base, (void *)mem->src, (void *)mem->dst, 0);
        break;
    }
#endif
    return 0;
}

static int dmac_run(struct dma_memcpy_struct *mem[])
{
    int i;

    dmac_irq_enable(&dmac);
    dmac_dma_enable(&dmac);

    for (i = 0; i < dmac.chan_num_max; i++) {
        if (i != dmac.cha[i].id) {
            printf("Chan %d and damc channel %d initialization do not match. quit\n", 
                        i, dmac.cha[i].id);
            continue;
        }
        if (mem[i]->length == 0)
            continue;
        dma_test.dma_memcpy(i + 1, dmac.base, (void *)mem[i]->src, (void *)mem[i]->dst, mem[i]->length);
    }
    
    return 0;
}

static inline void dmac_init(void)
{
    dmac_hw_init(&dmac);
}

struct dmac_xfer_struct dmac_xfer = {
    .start = dmac_start,
    .init = dmac_init,
    .run = dmac_run,
    .chan_num_max = CONFIG_DMAC_CHA_MAX,
};
