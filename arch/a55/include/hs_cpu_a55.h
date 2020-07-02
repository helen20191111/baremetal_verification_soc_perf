/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _HS_CPU_A55_H_
#define _HS_CPU_A55_H_

#include <a1000/sysreg_base.h>

#define CPU_CSR_BASE CPU_CSR_BASE_ADDR

#define CPU_CSR_CORE_CONFIG_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x00)
#define CPU_CSR_RVBARADDR1_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x04)
#define CPU_CSR_RVBARADDR2_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x08)
#define CPU_CSR_RVBARADDR3_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x0C)
#define CPU_CSR_RVBARADDR4_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x10)
#define CPU_CSR_RVBARADDR5_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x14)
#define CPU_CSR_RVBARADDR6_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x18)
#define CPU_CSR_RVBARADDR7_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x1C)
#define CPU_CSR_CORE0_RESET_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x20)
#define CPU_CSR_CORE1_RESET_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x24)
#define CPU_CSR_CORE2_RESET_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x28)
#define CPU_CSR_CORE3_RESET_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x2C)
#define CPU_CSR_CORE4_RESET_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x30)
#define CPU_CSR_CORE5_RESET_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x34)
#define CPU_CSR_CORE6_RESET_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x38)
#define CPU_CSR_CORE7_RESET_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x3C)
#define CPU_CSR_CORE0_CLKEN_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x40)
#define CPU_CSR_CORE1_CLKEN_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x44)
#define CPU_CSR_CORE2_CLKEN_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x48)
#define CPU_CSR_CORE3_CLKEN_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x4C)
#define CPU_CSR_CORE4_CLKEN_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x50)
#define CPU_CSR_CORE5_CLKEN_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x54)
#define CPU_CSR_CORE6_CLKEN_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x58)
#define CPU_CSR_CORE7_CLKEN_REG *(volatile unsigned *)(CPU_CSR_BASE + 0x5C)
#define CPU_CSR_PERIPH_DEBG_RESET_REG                                          \
	*(volatile unsigned *)(CPU_CSR_BASE + 0x60)

void cpu_a55_enable(char cpu, unsigned rvbaraddr);
void arch_local_irq_enable(void);
void arch_local_irq_disable(void);
void arch_local_fiq_enable(void);
void arch_local_fiq_disable(void);
void el3_intr_en(unsigned char intr_id, unsigned char target);

#endif
