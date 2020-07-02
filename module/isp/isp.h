/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _ISP_H
#define _ISP_H

#include <module/isp.h>
#include <a1000/sysreg_base.h>
#include <int_num.h>

#define ISP_BASE_TOP  (ISP_BASE + 0x30000)
#define ISP_BASE_ENG0 (ISP_BASE)
#define ISP_BASE_ENG1 (ISP_BASE+0x10000)
#define ISP_BASE_ENG2 (ISP_BASE+0x20000)

#define ISP_BASE_DMA0 (ISP_BASE_ENG0+0xFF00)
#define ISP_BASE_DMA1 (ISP_BASE_ENG1+0xFF00)
#define ISP_BASE_DMA2 (ISP_BASE_ENG2+0xFF00)

#define ISP_BASE_RISCV           (ISP_BASE + 0xd0000)
#define ISP_BASE_SRAM_DATA       (ISP_BASE + 0x80000)
#define ISP_BASE_SRAM_CODE       (ISP_BASE + 0x40000)
#define ISP_BASE_LTM             (ISP_BASE + 0x31000)
#define ISP_BASE_PDNS            (ISP_BASE + 0x33000)

#define ISP_BASE_INT2SYNC        (ISP_BASE + 0x34000)

#define ISP_BASE_TOP_RISCV_PC    (ISP_BASE_TOP + 0x88)

#define OFFSET_SNR1_ISP_NON       0x0
#define OFFSET_SNR1_ISP_RISC_RW   0xb00
#define OFFSET_SNR1_ISP_RISC_RO   0xd00
#define OFFSET_SNR2_ISP_NON       0x1000
#define OFFSET_SNR2_ISP_RISC_RW   0x1b00
#define OFFSET_SNR2_ISP_RISC_RO   0x1d00
#define OFFSET_ISP_BIST_RW        0x2000
#define OFFSET_ISP_BIST_RO        0x4000

#define ISP_IPC_ISP2A55CORE0_EN_ADDR     IPC_BASE + 0x2080
#define ISP_IPC_ISP2A55CORE0_EN_VAL      0x4003fffe
#define ISP_IPC_ISP2A55CORE0_TRIG_ADDR   IPC_BASE + 0x2e00
#define ISP_IPC_ISP2A55CORE0_TRIG_VAL    0x00004000
#define ISP_IPC_ISP2A55CORE0_STAT_ADDR   IPC_BASE + 0x2000
#define ISP_IPC_ISP2A55CORE0_STAT_VAL    0x00004000

#endif
