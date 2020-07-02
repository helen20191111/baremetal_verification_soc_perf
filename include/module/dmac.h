/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __CMD_DMAC_H
#define __CMD_DMAC_H
#include <common.h>

#define CON_TO_CON 0 /* single */
#define SHA_TO_CON 1
#define REL_TO_SHA 2
#define SHA_TO_SHA 3
#define LLP_TO_LLP 4

struct dma_memcpy_struct {
    dma_addr_t src;
    dma_addr_t dst;
    u32 length;
    int chan;
    int tran_type;
    int chan_lock;
    int chan_prior;
};

struct dmac_xfer_struct {
    int (*start)(struct dma_memcpy_struct *mem);
    void (*init)(void);
    int (*run)(struct dma_memcpy_struct *mem[]);
    const int chan_num_max;
};

#ifdef CONFIG_DMAC
extern struct dmac_xfer_struct dmac_xfer;
static inline void dma_init(void)
{
    dmac_xfer.init();
}

static inline int dma_start(struct dma_memcpy_struct *mem)
{
    return dmac_xfer.start(mem);
}

static inline int dma_run(struct dma_memcpy_struct *mem[])
{
    return dmac_xfer.run(mem);
}

#else
static inline void dma_init(void) {}
static inline int dma_start(struct dma_memcpy_struct *mem) {return -1;}
static inline int dma_start(struct dma_memcpy_struct *mem[]) {return -1};
#endif

struct dma_test_struct {
    void (*contiguous)(int channel, int reg_base, u64 *src, u64 *dst, u32 is_32);
    void (*single)(int channel, int reg_base, u64 *src, u64 *dst, u32 is_32);
    void (*auto_reload)(int channel, int reg_base, u64 *src, u64 *dst, u32 is_32);
    void (*shadow)(int channel, int reg_base, u64 *src, u64 *dst, u32 is_32);
    void (*llp)(int channel, int reg_base, u64 *src, u64 *dst, u32 is_32);
    void (*dma_memcpy)(u8 channel, void *reg_base, u64 *src, u64 *dst, u32 length);
};

extern u32 *dma_transfer_finished(void);
extern void dma_transfer_finished_flag_clear(u8 channel);
#endif
