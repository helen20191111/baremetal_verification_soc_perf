/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _INTNUM_H
#define _INTNUM_H
#include <interrupt.h>

#define A55_NS_PHY_TIMER_INT 30
#define A55_S_PHY_TIMER_INT 29
#define A55_V_TIMER_INT 27
#define A55_HYP_TIMER_INT 26

#define FIQ_IRQ_A55_WDT0 99
#define FIQ_IRQ_A55_WDT1 100
#define FIQ_IRQ_A55_WDT2 101
#define FIQ_IRQ_A55_WDT3 102
#define FIQ_IRQ_A55_WDT4 103
#define FIQ_IRQ_A55_WDT5 104
#define FIQ_IRQ_A55_WDT6 105
#define FIQ_IRQ_A55_WDT7 106

#define FIQ_IRQ_A55_TIMER0 107
#define FIQ_IRQ_A55_TIMER1 108
#define FIQ_IRQ_A55_TIMER2 109
#define FIQ_IRQ_A55_TIMER3 110
#define FIQ_IRQ_A55_TIMER4 111
#define FIQ_IRQ_A55_TIMER5 112
#define FIQ_IRQ_A55_TIMER6 113
#define FIQ_IRQ_A55_TIMER7 114

#define FIQ_IRQ_AXI_SRAM_PTY_INTR 121
#define FIQ_IRQ_AXI_SRAM_ECC_INTR 122
#define FIQ_IRQ_R5_BUS_ECC_ERR 123
#define FIQ_IRQ_R5_LOCK_ERR 124

#define FIQ_IRQ_LSP0_WDT0 125
#define FIQ_IRQ_LSP0_WDT1 126
#define FIQ_IRQ_LSP1_WDT0 127
#define FIQ_IRQ_LSP1_WDT1 128

#define FIQ_IRQ_LSP0_TIMER0 129
#define FIQ_IRQ_LSP0_TIMER1 130
#define FIQ_IRQ_LSP1_TIMER0 131
#define FIQ_IRQ_LSP1_TIMER1 132

#define FIQ_IRQ_CRM_SOFT_INT 137

#define FIQ_IRQ_DMA_CMN_INTR 176
#define FIQ_IRQ_DMA_CH1_INTR 177
#define FIQ_IRQ_DMA_CH2_INTR 178
#define FIQ_IRQ_DMA_CH3_INTR 179
#define FIQ_IRQ_DMA_CH4_INTR 180
#define FIQ_IRQ_DMA_CH5_INTR 181
#define FIQ_IRQ_DMA_CH6_INTR 182
#define FIQ_IRQ_DMA_CH7_INTR 183
#define FIQ_IRQ_DMA_CH8_INTR 184

#define A55_IRQ_PCIE_X4_INT 223
#define A55_IRQ_PCIE_X4_CFG_SAFETY_CORR 224
#define A55_IRQ_PCIE_X4_CFG_SAFETY_UNCORR 225
#define A55_IRQ_PCIE_X2_INT 226
#define A55_IRQ_PCIE_X2_CFG_SAFETY_CORR 227
#define A55_IRQ_PCIE_X2_CFG_SAFETY_UNCORR 228
#define A55_IRQ_PCIE_X4_MSI_CTRL_INT 229
#define A55_IRQ_PCIE_X4_OTHER_INT 230
#define A55_IRQ_PCIE_X2_OTHER_INT 231

#define FIQ_IRQ_HSP_USB3 232

#define FIQ_IRQ_LSP0_CAN0 235
#define FIQ_IRQ_LSP1_CAN1 236

#define FIQ_IRQ_LSP0_I2C0_IRQ 239
#define FIQ_IRQ_LSP0_I2C1_IRQ 240
#define FIQ_IRQ_LSP0_I2C2_IRQ 241
#define FIQ_IRQ_LSP0_I2C3_IRQ 242
#define FIQ_IRQ_LSP0_I2C4_IRQ 243
#define FIQ_IRQ_LSP0_I2C5_IRQ 244

#define FIQ_IRQ_LSP0_UART0 245
#define FIQ_IRQ_LSP0_UART1 246
#define FIQ_IRQ_LSP1_UART0 247
#define FIQ_IRQ_LSP1_UART1 248

#define FIQ_IRQ_LSP0_QSPI0 249
#define FIQ_IRQ_LSP1_QSPI1 250

#define FIQ_IRQ_LSP0_GPIO0 255
#define FIQ_IRQ_LSP1_GPIO1 256

#endif
