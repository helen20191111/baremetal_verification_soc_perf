/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <printf_inc.h>
#include <utility_lite.h>
#include <hs_api.h>
#include <a1000/sysreg_base.h>

#define CPU_GIC_BASE SAFETY_GIC_BASE_ADDR

#define CPU_GIC_DISTRIBUTOR (CPU_GIC_BASE + 0x1000)
#define CPU_GIC_CPU_INTERCACE (CPU_GIC_BASE + 0x2000)
#define CPU_GIC_VIR_INTF_CTL (CPU_GIC_BASE + 0x4000)
#define CPU_GIC_VIR_INTF_CTL_A (CPU_GIC_BASE + 0x5000)
#define CPU_GIC_VIR_CPU_INTF (CPU_GIC_BASE + 0x6000)

#define CPU_GIC_GICD_CTLR *((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x000))
#define CPU_GIC_GICD_TYPER *((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x004))
#define CPU_GIC_GICD_IIDR *((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x008))
#define CPU_GIC_GICD_IGROUPR0                                                  \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x080))
#define CPU_GIC_GICD_IGROUPR0_ADDR (CPU_GIC_DISTRIBUTOR + 0x080)
#define CPU_GIC_GICD_ISENABLER0                                                \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x100))
#define CPU_GIC_GICD_ISENABLER0_ADDR (CPU_GIC_DISTRIBUTOR + 0x100)
#define CPU_GIC_GICD_ICENABLER0                                                \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x180))
#define CPU_GIC_GICD_ICENABLER0_ADDR (CPU_GIC_DISTRIBUTOR + 0x180)
#define CPU_GIC_GICD_ISPENDR0                                                  \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x200))
#define CPU_GIC_GICD_ICPENDR0                                                  \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x280))
#define CPU_GIC_GICD_ISACTIVER0                                                \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x300))
#define CPU_GIC_GICD_ICACTIVER0                                                \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x380))
#define CPU_GIC_GICD_IPRIORITYR0                                               \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x400))
#define CPU_GIC_GICD_IPRIORITYR0_ADDR (CPU_GIC_DISTRIBUTOR + 0x400)
#define CPU_GIC_GICD_ITARGETSR0                                                \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0x800))
#define CPU_GIC_GICD_ITARGETSR0_ADDR (CPU_GIC_DISTRIBUTOR + 0x800)

//Interrupt Configruation Registers
#define CPU_GIC_LEVEL_SENSITIVE (0 << 0)
#define CPU_GIC_EDGE_TRIGGERED (1 << 0)
#define CPU_GIC_N_N_MODEL (0 << 1)
#define CPU_GIC_1_N_MODEL (1 << 1)

#define CPU_GIC_GICD_ICFGR0                                                    \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0xC00))
#define CPU_GIC_GICD_ICFGR0_ADDR (CPU_GIC_DISTRIBUTOR + 0xC00)

//
#define CPU_GIC_GICD_PPISR *((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0xD00))
#define CPU_GIC_GICD_SPISR0                                                    \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0xD04))
#define CPU_GIC_GICD_SGIR *((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0xF00))
#define CPU_GIC_GICD_CPENDSGIR0                                                \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0xF10))
#define CPU_GIC_GICD_SPENDSGIR0                                                \
	*((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0xF20))
#define CPU_GIC_GICD_PIDR4 *((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0xFD0))
#define CPU_GIC_GICD_PIDR5 *((volatile unsigned *)(CPU_GIC_DISTRIBUTOR + 0xFD4))

#define CPU_GIC_GICC_CTLR                                                      \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0000))
#define CPU_GIC_GICC_PMR                                                       \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0004))
#define CPU_GIC_GICC_BPR                                                       \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0008))
#define CPU_GIC_GICC_IAR                                                       \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x000C))
#define CPU_GIC_GICC_EOIR                                                      \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0010))
#define CPU_GIC_GICC_RPR                                                       \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0014))
#define CPU_GIC_GICC_HPPIR                                                     \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0018))
#define CPU_GIC_GICC_ABPR                                                      \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x001C))
#define CPU_GIC_GICC_AIAR                                                      \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0020))
#define CPU_GIC_GICC_AEOIR                                                     \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0024))
#define CPU_GIC_GICC_AHPPIR                                                    \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x0028))
#define CPU_GIC_GICC_APR0                                                      \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x00D0))
#define CPU_GIC_GICC_NSAPR0                                                    \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x00E0))
#define CPU_GIC_GICC_IIDR                                                      \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x00FC))
#define CPU_GIC_GICC_DIR                                                       \
	*((volatile unsigned *)(CPU_GIC_CPU_INTERCACE + 0x1000))

#define CPU_GIC_GICH_HCR *((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x000))
#define CPU_GIC_GICH_VTR *((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x004))
#define CPU_GIC_GICH_VMCR *((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x008))
#define CPU_GIC_GICH_MISR *((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x010))
#define CPU_GIC_GICH_EISR0                                                     \
	*((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x020))
#define CPU_GIC_GICH_EISR1                                                     \
	*((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x024))
#define CPU_GIC_GICH_ELSR0                                                     \
	*((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x030))
#define CPU_GIC_GICH_ELSR1                                                     \
	*((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x034))
#define CPU_GIC_GICH_APR *((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x0F0))
#define CPU_GIC_GICH_LR0 *((volatile unsigned *)(CPU_GIC_VIR_INTF_CTL + 0x100))
#define CPU_GIC_GICH_LR0_ADDR (CPU_GIC_VIR_INTF_CTL + 0x100)

#define CPU_GIC_GICV_CTLR                                                      \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0000))
#define CPU_GIC_GICV_PMR *((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0004))
#define CPU_GIC_GICV_BPR *((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0008))
#define CPU_GIC_GICV_IAR *((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x000C))
#define CPU_GIC_GICV_EOIR                                                      \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0010))
#define CPU_GIC_GICV_RPR *((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0014))
#define CPU_GIC_GICV_HPPIR                                                     \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0018))
#define CPU_GIC_GICV_ABPR                                                      \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x001C))
#define CPU_GIC_GICV_AIAR                                                      \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0020))
#define CPU_GIC_GICV_AEOIR                                                     \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0024))
#define CPU_GIC_GICV_AHPPIR                                                    \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x0028))
#define CPU_GIC_GICV_APR0                                                      \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x00D0))
#define CPU_GIC_GICV_NSAPR0                                                    \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x00E0))
#define CPU_GIC_GICV_IIDR                                                      \
	*((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x00FC))
#define CPU_GIC_GICV_DIR *((volatile unsigned *)(CPU_GIC_VIR_CPU_INTF + 0x1000))

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

/*
void cpu_gicd_set_disable(unsigned intr_id)
{
    unsigned long addr, shift, data;
    addr = CPU_GIC_GICD_ICENABLER0_ADDR + (intr_id >> 5 << 2);
    shift = intr_id & 0x1f;
    //data = *((volatile unsigned*)addr);
    //data = data | (1 << shift);
    //((volatile unsigned*)addr) = data;
    *((volatile unsigned*)addr) &= ~(1 << shift);
}
*/

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
	//old from zijian
	//addr = CPU_GIC_GICD_ICFGR0_ADDR + (intr_id >> 3 << 2);
	//shift = (intr_id & 0xf) << 1;
	// data = *((volatile unsigned*)addr);
	//data = data & ~(3 << shift) | (control & 3);
	//*((volatile unsigned*)addr) = data;

	//dengjian
	addr = CPU_GIC_GICD_ICFGR0_ADDR + (intr_id >> 4 << 2);
	shift = (intr_id & 0xf) << 1;

	data = *((volatile unsigned *)addr);
	data = (data & ~(3 << shift)) | ((control & 3) << shift);
	*((volatile unsigned *)addr) = data;
}

void cpu_gicd_set_priority(unsigned intr_id, unsigned char priority)
{
	unsigned long addr, shift, data;

	addr = CPU_GIC_GICD_IPRIORITYR0_ADDR + (intr_id >> 2 << 2);
	shift = (intr_id & 0x3) << 3;

	data = *((volatile unsigned *)addr);
	data = (data & ~(0xff << shift)) | ((priority & 0xff) << shift);
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

void cpu_gicc_enable_grp0(void)
{
	CPU_GIC_GICC_CTLR |= (1 << 3) | 1;
}

void cpu_gicc_enable_grp1(void)
{
	CPU_GIC_GICC_CTLR |= 2;
}

void cpu_gicc_enable_ackctl(void)
{
	CPU_GIC_GICC_CTLR |= 4;
}

void cpu_gicc_disable_ackctl(void)
{
	CPU_GIC_GICC_CTLR &= ~(1 << 2);
}

void cpu_gicc_set_eoimodens(void)
{
	CPU_GIC_GICC_CTLR |= (1 << 10);
}

void cpu_gicc_set_priority_mask(unsigned priority)
{
	CPU_GIC_GICC_PMR = priority;
}

unsigned cpu_gicc_get_iar(void)
{
	return CPU_GIC_GICC_IAR;
}

void cpu_gicc_set_eoir(unsigned eoi)
{
	CPU_GIC_GICC_EOIR = eoi;
}

unsigned cpu_gicc_get_aiar(void)
{
	return CPU_GIC_GICC_AIAR;
}

void cpu_gicc_set_aeoir(unsigned aeoi)
{
	CPU_GIC_GICC_AEOIR = aeoi;
}

void cpu_gich_en(void)
{
	CPU_GIC_GICH_HCR = CPU_GIC_GICH_HCR | 1;
}

void cpu_gich_set_lr(unsigned lr_id, unsigned data)
{
	unsigned long addr;
	addr = CPU_GIC_GICH_LR0_ADDR + (lr_id << 2);
	*((volatile unsigned *)addr) = data;
}

void cpu_gicv_enable_grp0(void)
{
	CPU_GIC_GICV_CTLR |= (1 << 3) | 1;
}

void cpu_gicv_enable_grp1(void)
{
	CPU_GIC_GICV_CTLR |= 2;
}

void cpu_gicv_set_eoimodens(void)
{
	CPU_GIC_GICV_CTLR |= (1 << 10);
}

void cpu_gicv_set_priority_mask(unsigned priority)
{
	CPU_GIC_GICV_PMR = priority;
}

unsigned cpu_gicv_get_iar(void)
{
	return CPU_GIC_GICV_IAR;
}

void cpu_gicv_set_eoir(unsigned eoi)
{
	CPU_GIC_GICV_EOIR = eoi;
}

unsigned cpu_gicv_get_aiar(void)
{
	return CPU_GIC_GICV_AIAR;
}

void cpu_gicv_set_aeoir(unsigned aeoi)
{
	CPU_GIC_GICV_AEOIR = aeoi;
}

//********new api for R5 Safety**************//
void cpu_gic_config_nonsecure(void)
{
	//int mask level and mode
	cpu_gicc_set_priority_mask(255);
	cpu_gicc_set_eoimodens();

	//irq enable
	cpu_gicd_enable_grp1();
	cpu_gicc_enable_grp1();

	//fiq enable
	cpu_gicd_enable_grp0();
	cpu_gicc_enable_grp0();
	cpu_gicc_disable_ackctl();
}

void cpu_gic_config_irq(unsigned int intr_id, unsigned int edgeCfg,
			unsigned int cpuIdx)
{
	unsigned int priorityCfg;

	//group 1 irq
	cpu_gicd_set_group(intr_id, 1);
	//level trigger
	cpu_gicd_set_control(intr_id, edgeCfg);
	//hprintf(TSENV, "\nis edge %d",IS_EDGE(pIsrList->attr));
	//send to cpu1
	cpu_gicd_set_targets(intr_id, cpuIdx);

	//set priority
	priorityCfg = (intr_id & 0xff) | 0x80;
	if (priorityCfg >= 0xf0) {
		priorityCfg -= 0x10;
	}
	cpu_gicd_set_priority(intr_id, priorityCfg);
}

void cpu_gic_config_fiq(unsigned int intr_id, unsigned int edgeCfg,
			unsigned int cpuIdx)
{
	unsigned int priorityCfg;

	//group 0 fiq
	cpu_gicd_set_group(intr_id, 0);
	//level trigger
	cpu_gicd_set_control(intr_id, edgeCfg);
	//hprintf(TSENV, "\nis edge %d",IS_EDGE(pIsrList->attr));
	//send to cpu1
	cpu_gicd_set_targets(intr_id, cpuIdx);

	//set priority
	cpu_gicd_set_priority(intr_id, intr_id & (~0x80));
	//set priority
	priorityCfg = (intr_id & 0xff) & 0x80;
	if (priorityCfg <= 0x10) {
		priorityCfg += 0x10;
	}
	cpu_gicd_set_priority(intr_id, priorityCfg);
}
