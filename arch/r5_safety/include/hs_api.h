/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _HS_CPU_GIC_
#define _HS_CPU_GIC_

void cpu_gicd_enable_grp0(void);
void cpu_gicd_enable_grp1(void);
void cpu_gicd_set_group(unsigned intr_id, unsigned group);
void cpu_gicd_set_enable(unsigned intr_id);
void cpu_gicd_clear_enable(unsigned intr_id);
void cpu_gicd_set_control(unsigned intr_id, unsigned control);
void cpu_gicd_set_targets(unsigned intr_id, unsigned targets);
void cpu_gicc_enable_grp0(void);
void cpu_gicc_enable_grp1(void);
void cpu_gicc_set_eoimodens(void);
void cpu_gicc_set_priority_mask(unsigned priority);
unsigned cpu_gicc_get_iar(void);
void cpu_gicc_set_eoir(unsigned eoi);
unsigned cpu_gicc_get_aiar(void);
void cpu_gicc_set_aeoir(unsigned eoi);
void cpu_gich_en(void);
void cpu_gich_set_lr(unsigned lr_id, unsigned data);
void cpu_gicv_enable_grp0(void);
void cpu_gicv_enable_grp1(void);
void cpu_gicv_set_eoimodens(void);
void cpu_gicv_set_priority_mask(unsigned priority);
unsigned cpu_gicv_get_iar(void);
void cpu_gicv_set_eoir(unsigned eoi);
unsigned cpu_gicv_get_aiar(void);
void cpu_gicv_set_aeoir(unsigned eoi);
void cpu_gicd_set_disable(unsigned intr_id);

//********new api for R5 Safety**************//
#define CPU0_IDX 1
#define CPU1_IDX 2

void cpu_gicd_set_priority(unsigned intr_id, unsigned char priority);
void cpu_gic_config_nonsecure(void);
void cpu_gic_config_fiq(unsigned int intr_id, unsigned int edgeCfg,
			unsigned int cpuIdx);
void cpu_gic_config_irq(unsigned int intr_id, unsigned int edgeCfg,
			unsigned int cpuIdx);

typedef void (*F_isrFn)(int para);

typedef struct isr_struct {
	F_isrFn isrFn;
	int para;
	unsigned int attr;
	unsigned int isrName;
	const char *description;
} T_isr_list;

/*which cpu*/
#define ISR_ATTR_SECURE 0x1
#define ISR_ATTR_SAFETY 0x2
#define ISR_ATTR_A55 0x4
/*level or edge( bit 16)*/
#define ISR_ATTR_LEVEL 0X00000
#define ISR_ATTR_EDGE 0X20000
#define IS_EDGE(a) ((a & 0x30000) >> 16)

#define IRQ_DEFINE(__isrName, __isrFn, __para, __description, __attr)          \
	__attribute__((section(".section_irq_table"))) struct isr_struct       \
		_section_item_##__isrName##_isr = {                            \
			.isrFn = __isrFn,                                      \
			.para = __para,                                        \
			.attr = __attr,                                        \
			.isrName = __isrName,                                  \
			.description = __description,                          \
		}

#define FIQ_DEFINE(__isrName, __isrFn, __para, __description, __attr)          \
	__attribute__((section(".section_fiq_table"))) struct isr_struct       \
		_section_item_##__isrName##_isr = {                            \
			.isrFn = __isrFn,                                      \
			.para = __para,                                        \
			.attr = __attr,                                        \
			.isrName = __isrName,                                  \
			.description = __description,                          \
		}

#define SAFETY_IRQ_ENABLE cpu_gicd_set_enable
#define SAFETY_IRQ_DISABLE cpu_gicd_clear_enable
#define SAFETY_FIQ_ENABLE cpu_gicd_set_enable
#define SAFETY_FIQ_DISABLE cpu_gicd_clear_enable

#define A55_IRQ_ENABLE(...)
#define A55_IRQ_DISABLE(...)
#define A55_FIQ_ENABLE(...)
#define A55_FIQ_DISABLE(...)

#endif
