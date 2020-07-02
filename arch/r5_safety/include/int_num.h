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

#include <hs_api.h>

#define IRQ_SAFETY_TOTAL_CNT 255
#define FIQ_SAFETY_TOTAL_CNT 255

#define FIQ_IRQ_LSP0_WDT0 41
#define FIQ_IRQ_LSP0_WDT1 42
#define FIQ_IRQ_LSP1_WDT0 43
#define FIQ_IRQ_LSP1_WDT1 44
#define FIQ_IRQ_LSP0_TIMER0 45
#define FIQ_IRQ_LSP0_TIMER1 46
#define FIQ_IRQ_LSP1_TIMER0 47
#define FIQ_IRQ_LSP1_TIMER1 48
#define FIQ_IRQ_DMA_INTR 49
#define FIQ_IRQ_DMA_CH1_INTR 50
#define FIQ_IRQ_DMA_CH2_INTR 51

#define FIQ_IRQ_SECURE_WDT_RESET 57

#define FIQ_IRQ_CRM_SOFT_INT 110

#define FIQ_IRQ_LSP0_CAN0_SATETY_ERR 116
#define FIQ_IRQ_LSP1_CAN1_SATETY_ERR 117
#define FIQ_IRQ_LSP0_PARITY_IRQ	118
#define FIQ_IRQ_LSP1_PARITY_IRQ	119

#define FIQ_IRQ_LSP0_I2C0_IRQ 132
#define FIQ_IRQ_LSP0_I2C1_IRQ 133
#define FIQ_IRQ_LSP0_I2C2_IRQ 134
#define FIQ_IRQ_LSP0_I2C3_IRQ 135
#define FIQ_IRQ_LSP0_I2C4_IRQ 136
#define FIQ_IRQ_LSP0_I2C5_IRQ 137

/*GPIO0 A组 最高8bit 每bit对应的中断*/
#define FIQ_IRQ_LSP0_GPIO0_A_24 120
#define FIQ_IRQ_LSP0_GPIO0_A_25 121
#define FIQ_IRQ_LSP0_GPIO0_A_26 122
#define FIQ_IRQ_LSP0_GPIO0_A_27 123
#define FIQ_IRQ_LSP0_GPIO0_A_28 124
#define FIQ_IRQ_LSP0_GPIO0_A_29 125
#define FIQ_IRQ_LSP0_GPIO0_A_30 126
#define FIQ_IRQ_LSP0_GPIO0_A_31 127

#define FIQ_IRQ_LSP0_CAN0 128
#define FIQ_IRQ_LSP1_CAN1 129

/*GPIO0 A组 其余24bit共用一个中断*/
#define FIQ_IRQ_LSP0_GPIO0 148
/*GPIO1 A组 共用一个中断*/
#define FIQ_IRQ_LSP1_GPIO1 149

#define FIQ_IRQ_LSP0_UART0 138
#define FIQ_IRQ_LSP0_UART1 139
#define FIQ_IRQ_LSP1_UART0 140
#define FIQ_IRQ_LSP1_UART1 141

#define FIQ_IRQ_LSP0_QSPI0 142
#define FIQ_IRQ_LSP1_QSPI1 143

#endif
