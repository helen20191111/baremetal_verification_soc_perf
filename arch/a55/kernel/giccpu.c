/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <giccpu.h>

void cpu_gicd_enable_grp0()
{
	CPU_GIC_GICD_CTLR |= 1;
}

void cpu_gicd_enable_grp1()
{
	CPU_GIC_GICD_CTLR |= 2;
}

void cpu_gicd_set_group(unsigned intr_id, unsigned group)
{
	unsigned long addr, shift, data;
	addr = CPU_GIC_GICD_IGROUPR0_ADDR + (intr_id >> 5 << 2);
	shift = intr_id & 0x1f;
	data = *((volatile unsigned *)addr);
	if (group) {
		data = data | (1 << shift);
	} else {
		data = data & ~(1 << shift);
	}
	*((volatile unsigned *)addr) = data;
}

void cpu_gicd_set_enable(unsigned intr_id)
{
	unsigned long addr, shift;
	addr = CPU_GIC_GICD_ISENABLER0_ADDR + (intr_id >> 5 << 2);
	shift = intr_id & 0x1f;
	//data = *((volatile unsigned*)addr);
	//data = data | (1 << shift);
	//*((volatile unsigned*)addr) = data;
	*((volatile unsigned *)addr) = 1 << shift;
}

void cpu_gicd_clear_enable(unsigned intr_id)
{
	unsigned long addr, shift;
	addr = CPU_GIC_GICD_ICENABLER0_ADDR + (intr_id >> 5 << 2);
	shift = intr_id & 0x1f;
	//data = *((volatile unsigned*)addr);
	//data = data | (1 << shift);
	//*((volatile unsigned*)addr) = data;
	*((volatile unsigned *)addr) = 1 << shift;
}

void cpu_gicd_set_control(unsigned intr_id, unsigned control)
{
	unsigned long addr, shift, data;
	addr = CPU_GIC_GICD_ICFGR0_ADDR + (intr_id >> 3 << 2);
	shift = (intr_id & 0xf) << 1;
	data = *((volatile unsigned *)addr);
	data = (data & ~(3 << shift)) | (control & 3);
	*((volatile unsigned *)addr) = data;
}

//void cpu_gicd_set_group(unsigned intr_id, unsigned group)
//{
//    unsigned long addr, shift, data;
//    addr = CPU_GIC_GICD_IGROUPR0_ADDR + (intr_id >> 5 << 2);
//    shift = intr_id & 0x1f;
//    data = *((volatile unsigned*)addr);
//    data = data & ~(1 << shift) | ((group != 0) << shift);
//    *((volatile unsigned*)addr) = data;
//}

void cpu_gicd_set_targets(unsigned intr_id, unsigned targets)
{
	unsigned long addr, shift, data;
	addr = CPU_GIC_GICD_ITARGETSR0_ADDR + (intr_id & 0xfc);
	shift = (intr_id & 0x3) << 3;
	data = *((volatile unsigned *)addr);
	data = (data & ~(0xff << shift)) | (targets << shift);
	*((volatile unsigned *)addr) = data;
}

void cpu_gicd_set_sgi(unsigned data)
{
	CPU_GIC_GICD_SGIR = data;
}

void cpu_gicc_enable_grp0()
{
	CPU_GIC_GICC_CTLR |= (1 << 3) | 1;
}

void cpu_gicc_enable_grp1()
{
	CPU_GIC_GICC_CTLR |= 2;
}

void cpu_gicc_set_eoimodens()
{
	CPU_GIC_GICC_CTLR |= (1 << 10);
}

void cpu_gicc_set_priority_mask(unsigned priority)
{
	CPU_GIC_GICC_PMR = priority;
}
unsigned long cpu_gicc_get_priority_mask(void)
{
	return CPU_GIC_GICC_PMR;
}

unsigned cpu_gicc_get_iar()
{
	return CPU_GIC_GICC_IAR;
}

void cpu_gicc_set_eoir(unsigned eoi)
{
	CPU_GIC_GICC_EOIR = eoi;
}

unsigned cpu_gicc_get_aiar()
{
	return CPU_GIC_GICC_AIAR;
}

void cpu_gicc_set_aeoir(unsigned aeoi)
{
	CPU_GIC_GICC_AEOIR = aeoi;
}

void cpu_gich_en()
{
	CPU_GIC_GICH_HCR = CPU_GIC_GICH_HCR | 1;
}

void cpu_gich_set_lr(unsigned lr_id, unsigned data)
{
	unsigned long addr;
	addr = CPU_GIC_GICH_LR0_ADDR + (lr_id << 2);
	*((volatile unsigned *)addr) = data;
}

void cpu_gicv_enable_grp0()
{
	CPU_GIC_GICV_CTLR |= (1 << 3) | 1;
}

void cpu_gicv_enable_grp1()
{
	CPU_GIC_GICV_CTLR |= 2;
}

void cpu_gicv_set_eoimodens()
{
	CPU_GIC_GICV_CTLR |= (1 << 10);
}

void cpu_gicv_set_priority_mask(unsigned priority)
{
	CPU_GIC_GICV_PMR = priority;
}

unsigned cpu_gicv_get_iar()
{
	return CPU_GIC_GICV_IAR;
}

void cpu_gicv_set_eoir(unsigned eoi)
{
	CPU_GIC_GICV_EOIR = eoi;
}

unsigned cpu_gicv_get_aiar()
{
	return CPU_GIC_GICV_AIAR;
}

void cpu_gicv_set_aeoir(unsigned aeoi)
{
	CPU_GIC_GICV_AEOIR = aeoi;
}
