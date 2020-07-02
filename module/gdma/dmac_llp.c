/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */
#define pr_fmt(fmt) KBUILD_BASENAME ": " fmt

#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <utility_lite.h>
#include <test_common.h>
#include <uart_sync.h>
#include <ut_test.h>
#include <int_num.h>
#include <module/gdma.h>
#include <module/dmac.h>
#include <linux/types.h>
#include <common.h>

extern struct lli_cpy lli_head0;
extern struct lli_cpy lli_next0;

extern struct lli_cpy lli_head1;
extern struct lli_cpy lli_next1;

extern struct lli_cpy lli_head2;
extern struct lli_cpy lli_next2;

extern struct lli_cpy lli_head3;
extern struct lli_cpy lli_next3;

extern struct lli_cpy lli_head4;
extern struct lli_cpy lli_next4;

extern struct lli_cpy lli_head5;
extern struct lli_cpy lli_next5;

extern struct lli_cpy lli_head6;
extern struct lli_cpy lli_next6;

extern struct lli_cpy lli_head7;
extern struct lli_cpy lli_next7;

struct lli_struct lli0 __attribute__((aligned(64)));
struct lli_struct lli0_next __attribute__((aligned(64)));

struct lli_struct lli1 __attribute__((aligned(64)));
struct lli_struct lli1_next __attribute__((aligned(64)));

struct lli_struct lli2 __attribute__((aligned(64)));
struct lli_struct lli2_next __attribute__((aligned(64)));

struct lli_struct lli3 __attribute__((aligned(64)));
struct lli_struct lli3_next __attribute__((aligned(64)));

struct lli_struct lli4 __attribute__((aligned(64)));
struct lli_struct lli4_next __attribute__((aligned(64)));

struct lli_struct lli5 __attribute__((aligned(64)));
struct lli_struct lli5_next __attribute__((aligned(64)));

struct lli_struct lli6 __attribute__((aligned(64)));
struct lli_struct lli6_next __attribute__((aligned(64)));

struct lli_struct lli7 __attribute__((aligned(64)));
struct lli_struct lli7_next __attribute__((aligned(64)));

struct lli_cpy {
    void *base;
    struct lli_struct *lli;
    struct lli_cpy *next;
    u64 *src;
    u64 *dst;
    u32 length;
#define DMAC_LLP_READY 0
#define DMAC_LLP_RUN 1
#define DMAC_LLP_LAST 2
#define DMAC_LLP_FINISH 3
    int status;
};

struct lli_cpy lli_head0 = {
    .lli = &lli0,
    .next = &lli_next0,
    .status = DMAC_LLP_READY,
};
struct lli_cpy lli_next0 = {
    .lli = &lli0_next,
    .next = &lli_head0,
    .status = DMAC_LLP_READY,
};

struct lli_cpy lli_head1 = {
    .lli = &lli1,
    .next = &lli_next1,
    .status = DMAC_LLP_READY,
};
struct lli_cpy lli_next1 = {
    .lli = &lli1_next,
    .next = &lli_head1,
    .status = DMAC_LLP_READY,
};

struct lli_cpy lli_head2 = {
    .lli = &lli2,
    .next = &lli_next2,
    .status = DMAC_LLP_READY,
};
struct lli_cpy lli_next2 = {
    .lli = &lli2_next,
    .next = &lli_head2,
    .status = DMAC_LLP_READY,
};

struct lli_cpy lli_head3 = {
    .lli = &lli3,
    .next = &lli_next3,
    .status = DMAC_LLP_READY,
};
struct lli_cpy lli_next3 = {
    .lli = &lli3_next,
    .next = &lli_head3,
    .status = DMAC_LLP_READY,
};

struct lli_cpy lli_head4 = {
    .lli = &lli4,
    .next = &lli_next4,
    .status = DMAC_LLP_READY,
};
struct lli_cpy lli_next4 = {
    .lli = &lli4_next,
    .next = &lli_head4,
    .status = DMAC_LLP_READY,
};

struct lli_cpy lli_head5 = {
    .lli = &lli5,
    .next = &lli_next5,
    .status = DMAC_LLP_READY,
};
struct lli_cpy lli_next5 = {
    .lli = &lli5_next,
    .next = &lli_head5,
};

struct lli_cpy lli_head6 = {
    .lli = &lli6,
    .next = &lli_next6,
    .status = DMAC_LLP_READY,
};
struct lli_cpy lli_next6 = {
    .lli = &lli6_next,
    .next = &lli_head6,
    .status = DMAC_LLP_READY,
};

struct lli_cpy lli_head7 = {
    .lli = &lli7,
    .next = &lli_next7,
    .status = DMAC_LLP_READY,
};
struct lli_cpy lli_next7 = {
    .lli = &lli7_next,
    .next = &lli_head7,
    .status = DMAC_LLP_READY,
};

struct lli_cpy *lli_list[] = {
    &lli_head0,
    &lli_head1,
    &lli_head2,
    &lli_head3,
    &lli_head4,
    &lli_head5,
    &lli_head6,
    &lli_head7,
};

u32 bDma_finished[8] = {false};
static void dmac_ll_memcpy(u8 channel, void *reg_base, u64 *src, u64 *dst, u32 length)
{
    u32 value, len;
    struct lli_struct *lli = lli_list[channel - 1]->lli;
    struct lli_struct *lli_next = lli_list[channel - 1]->next->lli; 
    if (channel - 1 >= CONFIG_DMAC_CHA_MAX) {
        printf("chan over max channel. quit\n");
        return;
    }

    if (!(((lli_list[channel - 1]->status == DMAC_LLP_READY) || 
        (lli_list[channel - 1]->status == DMAC_LLP_FINISH)) &&
            ((lli_list[channel - 1]->next->status == DMAC_LLP_READY) ||
            (lli_list[channel - 1]->next->status == DMAC_LLP_FINISH)))) {
        printf("chan %d llp descriptor not ready. quit\n", channel - 1);
        return;
    }

    memset(lli, 0, sizeof (struct lli_struct));
    memset(lli_next, 0, sizeof (struct lli_struct));

    value = readl(reg_base + DMAC_CHX_CFG(channel));
    value |= GENMASK(3, 0); /* src/dst set ll */
    writel(value, reg_base + DMAC_CHX_CFG(channel));
    
    value = readl(reg_base + DMAC_CHX_CFG(channel) + 0x4);
    value &= ~GENMASK(2, 0); /* mem to mem dmac flow */
    value &= ~BIT(3); /* src hardware handshaking */
    value &= ~BIT(4); /* dst hardware handshaking */
    value &= ~GENMASK(26, 23); /* src osr lmt */
    value &= ~GENMASK(30, 27); /* dst osr lmt */
    writel(value, reg_base + DMAC_CHX_CFG(channel) + 0x4);

    lli->chx_sar_l = ((u64)src & 0xffffffff);
    lli->chx_sar_h = ((u64)src >> 32) & 0xffffffff;
    lli->chx_dar_l = ((u64)dst & 0xffffffff);
    lli->chx_dar_h = ((u64)dst >> 32) & 0xffffffff;

    if (length > DMA_BLOCK_MAX_SIZE) {
        len = DMA_BLOCK_MAX_SIZE;
        length -= DMA_BLOCK_MAX_SIZE;
        lli_list[channel - 1]->status = DMAC_LLP_RUN;
    } else {
        len = length;
        length = 0;
        lli_list[channel - 1]->status = DMAC_LLP_LAST;
    }

    lli->chx_block_ts = len - 1;
    
    value = readl(reg_base + DMAC_CHX_CTL(channel));
    value &= ~BIT(4); /* src address increment */
    value &= ~BIT(6); /* dst address increment  */
    value &= ~GENMASK(10, 8);
    value |= 0x2 << 8; /* src width 32 bit */
    value &= ~GENMASK(13, 11);
    value |= 0x2 << 11; /* dst width 32 bit */
    value &= ~GENMASK(17, 14); /* src msize 8 byte */
    value &= ~GENMASK(21, 18); /* dst msize 8 byte */
    lli->chx_ctl_l = value;

    value = readl(reg_base + DMAC_CHX_CTL(channel) + 0x4);
    value |= BIT(6); /* src burst len enable */
    
    /*
     * TODO src/dst burst length AXI
     */
    value = (value & (~(0xff << 7))) | BIT(7);
	value = (value & (~(0x1 << 15))) | BIT(15); /* dst burst len enable */
    value = (value & (~(0xff << 16))) | BIT(16); /* src burst len enable */

    value |= BIT(26); /* enable shadow blk done interrupt */

    if (lli_list[channel - 1]->status == DMAC_LLP_RUN)
        value &= ~BIT(30); /* not last block */
    else
        value |= BIT(30); /* last block */
    
    value |= BIT(31); /* shadow register enable */
    lli->chx_ctl_h = value;

    lli->chx_llp_l = ((u64)lli_list[channel - 1]->next->lli & 0xffffffff);
    lli->chx_llp_h = ((u64)(lli_list[channel - 1]->next->lli) >> 32) & 0xffffffff;
    writel((u64)lli & 0xffffffff, reg_base + DMAC_CHX_LLP(channel));
    writel(((u64)lli >> 32) & 0xffffffff, reg_base + DMAC_CHX_LLP(channel) + 0x4);

    value = readl(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(channel));
    value |= BIT(0) | BIT(1) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(13);
    writel(value, reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(channel));

    lli_list[channel - 1] = lli_list[channel - 1]->next;
    lli_list[channel - 1]->base = reg_base;
    lli_list[channel - 1]->src = src + len;
    lli_list[channel - 1]->dst = dst + len;
    lli_list[channel - 1]->length = length;    

    pr_debug("chan: %d start transfer.\n", channel - 1);
    value = readl(reg_base + DMAC_CHENREG);
    value = (value & (~(1 << (channel - 1)))) | (0x1 << (channel - 1));
	value = (value & (~(1 << (channel + 7)))) | (0x1 << (channel + 7));
    writel(value, reg_base + DMAC_CHENREG);
}

static void dmac_irq_enable(void)
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

static void dmac_irq_close(void)
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

static void irq_dma(int channel)
{
	u32 status = 0;
	pr_debug("enter dma interrupt \n");

	status = REG32_READ(GDMA_BASE_ADDR + DMAC_COMMONREG_INTSTATUSREG);
	pr_debug("\n int status: %08X \n", status);
	REG32_WRITE(GDMA_BASE_ADDR + DMAC_COMMONREG_INTCLEARREG,
		    status); // clear interrupt
	pr_debug("\n clear status: ok!...");
}

static void irq_dma_chx(int channel)
{
	u32 reg_data, status, value, len;
    u32 length = lli_list[channel - 1]->length;
    struct lli_struct *lli = lli_list[channel - 1]->lli;
    struct lli_cpy *lli_cpy = lli_list[channel - 1];

    dmac_irq_close();
	status = readl(lli_cpy->base + DMAC_CHX_INTSTATUS(channel));
	reg_data = readl(lli_cpy->base + DMAC_CHX_INTSTATUS_ENABLEREG(channel));

	pr_debug("\nchannel %d, int status: %08X \n", channel - 1, status);
	if ((status & 0x1) == 0x1) {// check  block transfer done
		pr_debug("chan %d block trans done.\n", channel - 1);
		reg_data = reg_data | 0x1;
	}
	if ((status & 0x2) == 0x2) {// check dma transfer done  interrupt
		// printf("chan %d dma trans done.\n", channel - 1);
		reg_data = reg_data | 0x2;
        lli_cpy->status = DMAC_LLP_FINISH;
        lli_cpy->status = DMAC_LLP_FINISH;
        lli_cpy->next->status = DMAC_LLP_FINISH;
        lli_cpy->next->status = DMAC_LLP_FINISH;
        bDma_finished[channel - 1] = true;
        goto end;
	}
	if ((status & (0x1 << 3)) == (0x1 << 3)) {// src trans done interrupt
		pr_debug("src trans done  irq detected\n");
		reg_data = reg_data | (0x1 << 3);
	}
	if ((status & (0x1 << 4)) == (0x1 << 4)) {// dst trans done interrupt
		pr_debug("dst trans done  irq detected \n");
		reg_data = reg_data | (0x1 << 4);
	}
	if ((status & (0x1 << 5)) == (0x1 << 5)) {// src trans err interrupt
		pr_emerg("src_dec_err detected \n");
		reg_data = reg_data | (0x1 << 5);
        lli_cpy->status = DMAC_LLP_READY;
        lli_cpy->status = DMAC_LLP_READY;
        lli_cpy->next->status = DMAC_LLP_READY;
        lli_cpy->next->status = DMAC_LLP_READY;
        goto end;
	}
	if ((status & (0x1 << 6)) == (0x1 << 6)) {// destination decode error interrupt
		pr_emerg("dst_dec_err detected \n");
		reg_data = reg_data | (0x1 << 6);
        lli_cpy->status = DMAC_LLP_READY;
        lli_cpy->status = DMAC_LLP_READY;
        lli_cpy->next->status = DMAC_LLP_READY;
        lli_cpy->next->status = DMAC_LLP_READY;
        goto end;
	}
	if ((status & (0x1 << 13)) == (0x1 << 13)) { // check lli or shadow invaild err
		pr_debug("chan %d lli or shadow invaild detected\n", channel - 1);
		reg_data = reg_data | (0x1 << 13);
        memset(lli, 0, sizeof (struct lli_struct));
        lli->chx_sar_l = ((u64)lli_cpy->src & 0xffffffff);
        lli->chx_sar_h = ((u64)lli_cpy->src >> 32) & 0xffffffff;
        lli->chx_dar_l = ((u64)lli_cpy->dst & 0xffffffff);
        lli->chx_dar_h = ((u64)lli_cpy->dst >> 32) & 0xffffffff;

        if (length > DMA_BLOCK_MAX_SIZE) {
            len = DMA_BLOCK_MAX_SIZE;
            length -= DMA_BLOCK_MAX_SIZE;
            lli_cpy->status = DMAC_LLP_RUN;
        } else {
            len = length;
            length = 0;
            lli_cpy->status = DMAC_LLP_LAST;
        }
        lli->chx_block_ts = len - 1;

        lli->chx_llp_l = ((u64)lli_cpy->next->lli & 0xffffffff);
        lli->chx_llp_h = ((u64)lli_cpy->next->lli >> 32) & 0xffffffff;

        value = readl(lli_cpy->base + DMAC_CHX_CTL(channel));
        value &= ~BIT(4); /* src address increment */
        value &= ~BIT(6); /* dst address increment  */
        value &= ~GENMASK(10, 8);
        value |= 0x2 << 8; /* src width 32 bit */
        value &= ~GENMASK(13, 11);
        value |= 0x2 << 11; /* dst width 32 bit */
        value &= ~GENMASK(17, 14); /* src msize 8 byte */
        value &= ~GENMASK(21, 18); /* dst msize 8 byte */
        lli->chx_ctl_l = value;

        value = readl(lli_cpy->base + DMAC_CHX_CTL(channel) + 0x4);
        value |= BIT(6); /* src burst len enable */
        
        /*
        * TODO src/dst burst length AXI
        */
        value = (value & (~(0xff << 7))) | BIT(7);
        value = (value & (~(0x1 << 15))) | BIT(15); /* dst burst len enable */
        value = (value & (~(0xff << 16))) | BIT(16); /* src burst len enable */

        value |= BIT(26); /* enable shadow blk done interrupt */

        if (lli_cpy->status == DMAC_LLP_RUN)
            value &= ~BIT(30); /* not last block */
        else
            value |= BIT(30); /* last block */
        
        value |= BIT(31); /* shadow register enable */
        lli->chx_ctl_h = value;

        lli_cpy->next->src = lli_cpy->src + len;
        lli_cpy->next->dst = lli_cpy->dst + len;
        lli_cpy->next->length = length;
        lli_cpy->next->base = lli_cpy->base;
        lli_list[channel - 1] = lli_cpy->next;
        memset(lli_cpy->next->lli, 0, sizeof (struct lli_struct));
        pr_debug("blk resumereq...\n");
        goto resume;
	}

    writel(reg_data, lli_cpy->base + DMAC_CHX_INTCLEARREG(channel));
    dmac_irq_enable();
    return;

end:
    writel(reg_data, lli_cpy->base + DMAC_CHX_INTCLEARREG(channel));
    pr_debug("\nwait for ch_%d_en auto clear....\n", channel);
	while ((readl(lli_cpy->base + DMAC_CHENREG) & 
            (0x1 << (channel - 1))) != 0x0);
	pr_debug(" ok!\n", channel);
    dmac_irq_enable();
    return;

resume:
    writel(reg_data, lli_cpy->base + DMAC_CHX_INTCLEARREG(channel));
    writel(0x1, lli_cpy->base + DMAC_CHX_BLK_TFR_RESUMEREQREG(channel));
    writel(0x0, lli_cpy->base + DMAC_CHX_BLK_TFR_RESUMEREQREG(channel) + 0x4);
    dmac_irq_enable();
    return;
}

IRQ_DEFINE(FIQ_IRQ_DMA_CMN_INTR, irq_dma, GDMA_BASE_ADDR, "irq dma intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH1_INTR, irq_dma_chx, 1, "irq dma ch1 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH2_INTR, irq_dma_chx, 2, "irq dma ch2 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH3_INTR, irq_dma_chx, 3, "irq dma ch3 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH4_INTR, irq_dma_chx, 4, "irq dma ch4 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH5_INTR, irq_dma_chx, 5, "irq dma ch5 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH6_INTR, irq_dma_chx, 6, "irq dma ch6 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH7_INTR, irq_dma_chx, 7, "irq dma ch7 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH8_INTR, irq_dma_chx, 8, "irq dma ch8 intr",
	   ISR_ATTR_A55);

struct dma_test_struct dma_test = {
    .dma_memcpy = dmac_ll_memcpy,
};

/**
 * test if dma tranfer finished
 */
u32 *dma_transfer_finished(void)
{
    return bDma_finished;
}

/**
 * clear dma channel transfer finished flags
 */
void dma_transfer_finished_flag_clear(u8 channel)
{
    bDma_finished[channel] = false;
}
