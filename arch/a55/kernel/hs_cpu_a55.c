/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <hs_api.h>

void cpu_a55_enable(char cpu, unsigned rvbaraddr)
{
	if (cpu & (1 << 1)) {
		CPU_CSR_RVBARADDR1_REG = rvbaraddr;
		CPU_CSR_CORE1_CLKEN_REG = 1;
		CPU_CSR_CORE1_RESET_REG = 3;
	}
	if (cpu & (1 << 2)) {
		CPU_CSR_RVBARADDR2_REG = rvbaraddr;
		CPU_CSR_CORE2_CLKEN_REG = 1;
		CPU_CSR_CORE2_RESET_REG = 3;
	}
	if (cpu & (1 << 3)) {
		CPU_CSR_RVBARADDR3_REG = rvbaraddr;
		CPU_CSR_CORE3_CLKEN_REG = 1;
		CPU_CSR_CORE3_RESET_REG = 3;
	}
	if (cpu & (1 << 4)) {
		CPU_CSR_RVBARADDR4_REG = rvbaraddr;
		CPU_CSR_CORE4_CLKEN_REG = 1;
		CPU_CSR_CORE4_RESET_REG = 3;
	}
	if (cpu & (1 << 5)) {
		CPU_CSR_RVBARADDR5_REG = rvbaraddr;
		CPU_CSR_CORE5_CLKEN_REG = 1;
		CPU_CSR_CORE5_RESET_REG = 3;
	}
	if (cpu & (1 << 6)) {
		CPU_CSR_RVBARADDR6_REG = rvbaraddr;
		CPU_CSR_CORE6_CLKEN_REG = 1;
		CPU_CSR_CORE6_RESET_REG = 3;
	}
	if (cpu & (1 << 7)) {
		CPU_CSR_RVBARADDR7_REG = rvbaraddr;
		CPU_CSR_CORE7_CLKEN_REG = 1;
		CPU_CSR_CORE7_RESET_REG = 3;
	}
}

void arch_local_irq_enable(void)
{
	asm volatile("msr    daifclr, #2     // arch_local_irq_enable"
		     :
		     :
		     : "memory");
}

void arch_local_irq_disable(void)
{
	asm volatile("msr    daifset, #2     // arch_local_irq_disable"
		     :
		     :
		     : "memory");
}

void arch_local_fiq_enable(void)
{
	asm volatile("msr    daifclr, #0xf     // arch_local_fiq_enable"
		     :
		     :
		     : "memory");
}

void arch_local_fiq_disable(void)
{
	asm volatile("msr    daifset, #1     // arch_local_fiq_disable"
		     :
		     :
		     : "memory");
}

void el3_intr_en(unsigned char intr_id, unsigned char target)
{
	unsigned long data;
	cpu_gicd_set_enable(intr_id);
	cpu_gicd_set_control(intr_id,
			     CPU_GIC_LEVEL_SENSITIVE | CPU_GIC_1_N_MODEL);
	cpu_gicc_set_priority_mask(255);
	cpu_gicd_set_targets(intr_id, target);
	cpu_gicd_enable_grp0();
	cpu_gicc_enable_grp0();

	arch_local_fiq_enable();
	asm volatile("mrs  %0, scr_el3" : "=r"(data) : : "memory");

	data = data | (1 << 2);
	asm volatile("msr  scr_el3, %0" : : "r"(data) : "memory");
}
