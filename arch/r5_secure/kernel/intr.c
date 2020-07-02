/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <printf_inc.h> //use related path
#include <utility_lite.h>

/////////////////////////////////////putc  getc for printf////////////////////////////////////////
#include <a1000/sysreg_base.h>
#include <intr.h>
#include <int_num.h>
#include <RegTest.h>

typedef struct {
	volatile unsigned int IRQ_INTEN_L;
	volatile unsigned int IRQ_INTEN_H;
	volatile unsigned int IRQ_INTMASK_L;
	volatile unsigned int IRQ_INTMASK_H;
	volatile unsigned int IRQ_INTFORCE_L;
	volatile unsigned int IRQ_INTFORCE_H;
	volatile unsigned int IRQ_RAWSTATUS_L;
	volatile unsigned int IRQ_RAWSTAUTS_H;
	volatile unsigned int IRQ_STATUS_L;
	volatile unsigned int IRQ_STATUS_H;
	volatile unsigned int IRQ_MASKSTATUS_L;
	volatile unsigned int IRQ_MASKSTATUS_H;
	volatile unsigned int IRQ_FINALSTATUS_L;
	volatile unsigned int IRQ_FINALSTATUS_H;
	volatile unsigned int IRQ_VECTOR;
	volatile unsigned int reversed0;
	volatile unsigned int IRQ_VECTOR_0;
	volatile unsigned int reversed1;
	volatile unsigned int IRQ_VECTOR_1;
	volatile unsigned int reversed2;
	volatile unsigned int IRQ_VECTOR_2;
	volatile unsigned int reversed3;
	volatile unsigned int IRQ_VECTOR_3;
	volatile unsigned int reversed4;
	volatile unsigned int IRQ_VECTOR_4;
	volatile unsigned int reversed5;
	volatile unsigned int IRQ_VECTOR_5;
	volatile unsigned int reversed6;
	volatile unsigned int IRQ_VECTOR_6;
	volatile unsigned int reversed7;
	volatile unsigned int IRQ_VECTOR_7;
	volatile unsigned int reversed8;
	volatile unsigned int IRQ_VECTOR_8;
	volatile unsigned int reversed9;
	volatile unsigned int IRQ_VECTOR_9;
	volatile unsigned int reversed10;
	volatile unsigned int IRQ_VECTOR_10;
	volatile unsigned int reversed11;
	volatile unsigned int IRQ_VECTOR_11;
	volatile unsigned int reversed12;
	volatile unsigned int IRQ_VECTOR_12;
	volatile unsigned int reversed13;
	volatile unsigned int IRQ_VECTOR_13;
	volatile unsigned int reversed14;
	volatile unsigned int IRQ_VECTOR_14;
	volatile unsigned int reversed15;
	volatile unsigned int IRQ_VECTOR_15;
	volatile unsigned int reversed16;
	volatile unsigned int FIQ_INTEN;
	volatile unsigned int FIQ_INTMASK;
	volatile unsigned int FIQ_INTFORCE;
	volatile unsigned int FIQ_RAWSTATUS;
	volatile unsigned int FIQ_STATUS;
	volatile unsigned int FIQ_FINALSTATUS;
	volatile unsigned int IRQ_PLEVEL;
	volatile unsigned int IRQ_INTERNAL_PLEVEL;
	volatile unsigned int ICTRL_VERSION_ID;
	volatile unsigned int IRQ_PR_0;
	volatile unsigned int IRQ_PR_1;
	volatile unsigned int IRQ_PR_2;
	volatile unsigned int IRQ_PR_3;
	volatile unsigned int IRQ_PR_4;
	volatile unsigned int IRQ_PR_5;
	volatile unsigned int IRQ_PR_6;
	volatile unsigned int IRQ_PR_7;
	volatile unsigned int IRQ_PR_8;
	volatile unsigned int IRQ_PR_9;
	volatile unsigned int IRQ_PR_10;
	volatile unsigned int IRQ_PR_11;
	volatile unsigned int IRQ_PR_12;
	volatile unsigned int IRQ_PR_13;
	volatile unsigned int IRQ_PR_14;
	volatile unsigned int IRQ_PR_15;
	volatile unsigned int IRQ_PR_16;
	volatile unsigned int IRQ_PR_17;
	volatile unsigned int IRQ_PR_18;
	volatile unsigned int IRQ_PR_19;
	volatile unsigned int IRQ_PR_20;
	volatile unsigned int IRQ_PR_21;
	volatile unsigned int IRQ_PR_22;
	volatile unsigned int IRQ_PR_23;
	volatile unsigned int IRQ_PR_24;
	volatile unsigned int IRQ_PR_25;
	volatile unsigned int IRQ_PR_26;
	volatile unsigned int IRQ_PR_27;
	volatile unsigned int IRQ_PR_28;
	volatile unsigned int IRQ_PR_29;
	volatile unsigned int IRQ_PR_30;
	volatile unsigned int IRQ_PR_31;
} ICTL_TypeDef;

#define ICTL ((ICTL_TypeDef *)(SECURE_INTC_ADDR))

int intr_init(void)
{
	int i;

	hprintf(TSENV, "\r\nsecure irq close all");

	for (i = 0; i < IRQ_SECURE_TOTAL_CNT; i++) {
		//hprintf(TSENV, "\nirq %d",i);
		//irq_set_enable(i);
		irq_set_disable(i);
	}
	//irq_set_disable(0);
	//irq_set_disable(16);

	hprintf(TSENV, "\r\nsecure fiq close all");
	for (i = 0; i < FIQ_SECURE_TOTAL_CNT; i++) {
		fiq_set_disable(i);
	}

	return 0;
}

void irq_set_enable(unsigned intr_id)
{
	ICTL->IRQ_INTEN_L |= 1 << intr_id;
}

void irq_set_disable(unsigned intr_id)
{
	ICTL->IRQ_INTEN_L &= ~(1 << intr_id);
}

void irq_set_mask(unsigned intr_id)
{
	ICTL->IRQ_INTMASK_L |= 1 << intr_id;
}

void irq_clr_mask(unsigned intr_id)
{
	ICTL->IRQ_INTMASK_L &= ~(1 << intr_id);
}

void irq_set_sys_plevel(unsigned plevel)
{
	ICTL->IRQ_PLEVEL = plevel;
}

void fiq_set_enable(unsigned intr_id)
{
	ICTL->FIQ_INTEN |= 1 << intr_id;
}

void fiq_set_disable(unsigned intr_id)
{
	ICTL->FIQ_INTEN &= ~(1 << intr_id);
}

void fiq_set_mask(unsigned intr_id)
{
	ICTL->FIQ_INTMASK |= 1 << intr_id;
}

void fiq_clr_mask(unsigned intr_id)
{
	ICTL->FIQ_INTMASK &= ~(1 << intr_id);
}

unsigned int read_fiq_f_status()
{
	return ICTL->FIQ_FINALSTATUS;
}

void set_irq_plevel(unsigned intr_id, unsigned plevel)
{
	*(unsigned int *)(SECURE_INTC_ADDR + intr_id * 4 + 0xe8) = plevel;
}

unsigned int read_irq_f_status()
{
	return ICTL->IRQ_FINALSTATUS_L;
}
