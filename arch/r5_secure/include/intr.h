/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __INTR_H_
#define __INTR_H_

typedef void (*F_isrFn)(int para);

typedef struct isr_struct {
	F_isrFn isrFn;
	int para;
	unsigned int attr;
	unsigned int isrName;
	const char *description;
} T_isr_list;

/* which cpu */
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

#define SECURE_IRQ_ENABLE irq_set_enable
#define SECURE_IRQ_DISABLE irq_set_disable
#define SECURE_FIQ_ENABLE fiq_set_enable
#define SECURE_FIQ_DISABLE fiq_set_disable

#define SAFETY_IRQ_ENABLE(...)
#define SAFETY_IRQ_DISABLE(...)
#define SAFETY_FIQ_ENABLE(...)
#define SAFETY_FIQ_DISABLE(...)

#define A55_IRQ_ENABLE(...)
#define A55_IRQ_DISABLE(...)
#define A55_FIQ_ENABLE(...)
#define A55_FIQ_DISABLE(...)

int intr_init(void);
void irq_set_enable(unsigned intr_id);
void irq_set_disable(unsigned intr_id);
void irq_set_mask(unsigned intr_id);
void irq_clr_mask(unsigned intr_id);
void irq_set_sys_plevel(unsigned plevel);
void set_irq_plevel(unsigned intr_id, unsigned plevel);
unsigned int read_irq_f_status(void);
void fiq_set_enable(unsigned intr_id);
void fiq_set_disable(unsigned intr_id);
void fiq_set_mask(unsigned intr_id);
void fiq_clr_mask(unsigned intr_id);
unsigned int read_fiq_f_status(void);

#endif
