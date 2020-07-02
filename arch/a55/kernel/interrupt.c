/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <interrupt.h>
#include <int_num.h>
#include <printf_inc.h> //use related path
#include <utility_lite.h>

#include <a1000/sysreg_base.h>
#include <RegTest.h>
#include <test_common.h>
#include <system.h>
#include <module/a55_timer.h>
#include <giccpu.h>
#include <common.h>

#define writel_relaxed(_v, _a) REG32_WRITE(_a, _v)
#define readl_relaxed(_a) REG32_READ(_a)

extern unsigned long __section_irq_table_start;
extern unsigned long __section_irq_table_end;

void GicTestMain(int para);

#define IRQ_TEST_UNUSE (-1)

int irq_handler_test_tab[MAX_IRQ_ID + 1] = { 0 };
volatile int irq_test_curr_pos = 0;
int irq_test_prio[32] = { IRQ_TEST_UNUSE };
gic_irq_t irq_handler_tab[MAX_IRQ_ID + 1] = { 0 };
int irq_test_stat = 0;

struct esr_s esr_tlb[] = {
	{ "EC_UNKNOWN", 0x0 },      { "EC_WFx", 0x1 },
	{ "EC_CP15_32", 0x3 },      { "EC_CP15_64", 0x4 },
	{ "EC_CP14_MR", 0x5 },      { "EC_CP14_LS", 0x6 },
	{ "EC_FP_ASIMD", 0x7 },     { "EC_CP10_ID", 0x8 },
	{ "EC_CP14_64", 0x0C },     { "EC_ILL", 0x0E },
	{ "EC_SVC32", 0x11 },       { "EC_HVC32", 0x12 },
	{ "EC_SMC32", 0x13 },       { "EC_SVC64", 0x15 },
	{ "EC_HVC64", 0x16 },       { "EC_SMC64", 0x17 },
	{ "EC_SYS64", 0x18 },       { "EC_IMP_DEF", 0x1f },
	{ "EC_IABT_LOW", 0x20 },    { "EC_IABT_CUR", 0x21 },
	{ "EC_PC_ALIGN", 0x22 },    { "EC_DABT_LOW", 0x24 },
	{ "EC_DABT_CUR", 0x25 },    { "EC_SP_ALIGN", 0x26 },
	{ "EC_FP_EXC32", 0x28 },    { "EC_FP_EXC64", 0x2C },
	{ "EC_SERROR", 0x2F },      { "EC_BREAKPT_LOW", 0x30 },
	{ "EC_BREAKPT_CUR", 0x31 }, { "EC_SOFTSTP_LOW", 0x32 },
	{ "EC_SOFTSTP_CUR", 0x33 }, { "EC_WATCHPT_LOW", 0x34 },
	{ "EC_WATCHPT_CUR", 0x35 }, { "EC_BKPT32", 0x38 },
	{ "EC_VECTOR32", 0x3A },    { "EC_BRK64", 0x3C },
	{ "EC_MAX", 0x3F },
};

void show_esr_str(unsigned long esr_num)
{
	unsigned long es_num = 0;
	int i;
	es_num = ESR_ELx_EC(es_num);
	printf_intr("ESR_ELx_IL:%d\n\r", es_num & ESR_ELx_IL ? 1 : 0);
	for (i = 0; i < sizeof(esr_tlb) / sizeof(struct esr_s); i++) {
		if (esr_tlb[i].esr_num == es_num) {
			printf_intr("Exception Class:%s\n\r",
				    esr_tlb[i].esr_str);
			break;
		}
	}
}

void show_regs(struct pt_regs *regs, unsigned int esr_num)
{
	int i;

	printf_intr("elr: %016lx lr : %016lx\n", regs->elr, regs->regs[30]);
	show_esr_str(esr_num);
	for (i = 0; i < 29; i += 2)
		printf_intr("x%-2d: %016lx x%-2d: %016lx\n", i, regs->regs[i],
			    i + 1, regs->regs[i + 1]);
	printf_intr("\n");
	while (1)
		;
}

void disable_irq(void)
{
	__asm("msr daifset, #2");
}

void enable_irq(void)
{
	__asm("msr daifclr, #0xf");
}

u32 hvte_register_irq(u32 irq, void (*handler)(int), u32 flags, u64 para,
		      s8 *name)
{
	if (irq >= MAX_IRQ_ID || irq < MIN_IRQ_ID) {
		hprintf(ERROR, "%s-> irq id is not right.\r\n", __FUNCTION__);
		return 1;
	}

	hprintf(TSENV, "%s-> irq:%d reged.\r\n", __FUNCTION__, irq);
	irq_handler_tab[irq].irqflags = flags;
	irq_handler_tab[irq].irq_handler = handler;
	irq_handler_tab[irq].irqid = irq;
	irq_handler_tab[irq].irqname = name;
	irq_handler_tab[irq].para = para;

	cpu_gicd_set_control(irq, IS_EDGE(flags));
	return 0;
}

static void gic_poke_irq(int irqid, u32 offset)
{
	unsigned long dist_base = GIC_BASE_ADDR + GIC_DIST_OFFSET;
	u32 mask = 1 << (irqid % 32);
	writel_relaxed(mask, dist_base + offset + (irqid / 32) * 4);
}

//static int gic_peek_irq(int irqid, u32 offset)
//{
//	unsigned long dist_base = GIC_BASE_ADDR + GIC_DIST_OFFSET;
//	u32 mask = 1 << (irqid % 32);
//	return !!(readl_relaxed(dist_base + offset + (irqid / 32) * 4) & mask);
//}

void gic_mask_irq(int irqid)
{
	gic_poke_irq(irqid, GIC_DIST_ENABLE_CLEAR);
}

//static void gic_eoimode1_mask_irq(int irqid)
//{
//	gic_mask_irq(irqid);
/*
	 * When masking a forwarded interrupt, make sure it is
	 * deactivated as well.
	 *
	 * This ensures that an interrupt that is getting
	 * disabled/masked will not get "stuck", because there is
	 * noone to deactivate it (guest is being terminated).
	 */
//	gic_poke_irq(irqid, GIC_DIST_ACTIVE_CLEAR);
//}

void gic_clean_active_irq(int irqid)
{
	gic_poke_irq(irqid, GIC_DIST_ACTIVE_CLEAR);
}

void gic_unmask_irq(int irqid)
{
	gic_poke_irq(irqid, GIC_DIST_ENABLE_SET);
	//hprintf(TSENV, "[%s]%d irqid:%x!\r\n",__func__,__LINE__,irqid);
}
void gic_send_sgi(unsigned int cpu_id, unsigned int irq)
{
	cpu_id = 1 << cpu_id;
	/* this always happens on GIC0 */
	writel_relaxed((cpu_id << 16) | irq,
		       GIC_BASE_ADDR + GIC_DIST_OFFSET + GIC_DIST_SOFTINT);
}

static void gic_cpu_config(unsigned long dist_base, void (*sync_access)(void))
{
	int i;

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 * Make sure everything is deactivated.
	 */
	writel_relaxed(GICD_INT_EN_CLR_X32, dist_base + GIC_DIST_ACTIVE_CLEAR);
	//	writel_relaxed(GICD_INT_EN_CLR_PPI, dist_base + GIC_DIST_ENABLE_CLEAR);
	//	writel_relaxed(GICD_INT_EN_SET_SGI | GICD_INT_EN_CLR_PPI, dist_base + GIC_DIST_ENABLE_SET);
	writel_relaxed(GICD_INT_EN_SET_SGI | GICD_INT_EN_CLR_PPI,
		       dist_base + GIC_DIST_ENABLE_CLEAR);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for (i = 0; i < 32; i += 4)
		writel_relaxed(GICD_INT_DEF_PRI_X4,
			       dist_base + GIC_DIST_PRI + i * 4 / 4);

	if (sync_access)
		sync_access();
}

static void gic_cpu_if_up(unsigned long cpu_base)
{
	//unsigned long cpu_base = gic_data_cpu_base(gic);
	u32 bypass = 0;
	//u32 mode = 0;

	//if (gic == &gic_data[0] && static_key_true(&supports_deactivate))
	//	mode = GIC_CPU_CTRL_EOImodeNS;

	/*
	* Preserve bypass disable bits to be written back later
	*/
	//bypass = readl_relaxed(cpu_base + GIC_CPU_CTRL);
	//bypass &= ~GICC_DIS_BYPASS_MASK;
	bypass = 0;

	//writel_relaxed(bypass | mode | GICC_ENABLE, cpu_base + GIC_CPU_CTRL);
	writel_relaxed(0x3, cpu_base + GIC_CPU_CTRL);
}

//static int gic_cpu_set_priority_mask(unsigned int priority_mask)
//{
//	unsigned long cpu_base = GIC_BASE_ADDR + GIC_CPU_OFFSET;
//	writel_relaxed(priority_mask, cpu_base + GIC_DIST_CTR);
//	return 0;
//}

static int gic_cpu_init(unsigned long base)
{
	unsigned long dist_base = base + GIC_DIST_OFFSET;
	unsigned long cpu_base = base + GIC_CPU_OFFSET;
	//unsigned long base = gic_data_cpu_base(gic);

	//gic_cpu_set_priority_mask(0xff);
	gic_cpu_config(dist_base, NULL);

	//writel_relaxed(GICC_INT_PRI_THRESHOLD, base + GIC_CPU_PRIMASK);
	writel_relaxed(0xff, cpu_base + GIC_CPU_PRIMASK);
	gic_cpu_if_up(cpu_base);

	return 0;
}
/*
static u8 gic_get_cpumask(unsigned long dist_base)
{
	u32 mask, i;

	for (i = mask = 0; i < 32; i += 4) {
		mask = readl_relaxed(dist_base + GIC_DIST_TARGET + i);
		mask |= mask >> 16;
		mask |= mask >> 8;
		if (mask)
			break;
	}
	return mask;
}
*/
void gic_dist_config(unsigned long dist_base, int gic_irqs,
		     void (*sync_access)(void))
{
	unsigned int i;

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for (i = 32; i < gic_irqs; i += 16)
		writel_relaxed(0x55555555, dist_base + GIC_DIST_CONFIG + i / 4);

	/*
	 * Set priority on all global interrupts.
	 */
	for (i = 32; i < gic_irqs; i += 4)
		writel_relaxed(GICD_INT_DEF_PRI_X4,
			       dist_base + GIC_DIST_PRI + i);

	/*
	 * Deactivate and disable all SPIs. Leave the PPI and SGIs
	 * alone as they are in the redistributor registers on GICv3.
	 */
	for (i = 32; i < gic_irqs; i += 32) {
		writel_relaxed(GICD_INT_EN_CLR_X32,
			       dist_base + GIC_DIST_ACTIVE_CLEAR + i / 8);
		writel_relaxed(GICD_INT_EN_CLR_X32,
			       dist_base + GIC_DIST_ENABLE_CLEAR + i / 8);
	}

	if (sync_access)
		sync_access();
}

static void gic_dist_init(unsigned long base, int gic_irqs)
{
	unsigned int i;
	u32 cpumask;
	unsigned long dist_base = base + GIC_DIST_OFFSET;

	writel_relaxed(GICD_DISABLE, dist_base + GIC_DIST_CTRL);

	/*
	 * Set all global interrupts to this CPU only.
	 */
	//cpumask = gic_get_cpumask(dist_base);
	//cpumask |= cpumask << 8;
	//cpumask |= cpumask << 16;
	cpumask = 0x01010101;
	//for (i = 32; i < gic_irqs; i += 4)
	for (i = 0; i < gic_irqs; i += 4)
		writel_relaxed(cpumask,
			       dist_base + GIC_DIST_TARGET + i * 4 / 4);

	gic_dist_config(dist_base, gic_irqs, NULL);

	writel_relaxed(GICD_ENABLE, dist_base + GIC_DIST_CTRL);
	//writel_relaxed(0x2, dist_base + GIC_DIST_CTRL);
}

void curr_el_spx_irq_test_default(int irqnr)
{
	unsigned int regdata;
	regdata = current_el();
	irq_handler_test_tab[irq_test_curr_pos++] = irqnr;
	if (irq_test_curr_pos >= 200) {
		irq_test_curr_pos = 0;
	}

	printf_intr("IRQ:%d Catched. EL%d.\r\n", irqnr, regdata);
	gic_poke_irq(irqnr, GIC_DIST_PENDING_CLEAR);
	gic_mask_irq(irqnr);
}

void curr_el_spx_irq_vector(void)
{
	u32 irqstat, irqnr;
	unsigned long cpu_base = GIC_BASE_ADDR + GIC_CPU_OFFSET;

	do {
		irqnr = 0;
		irqstat = readl_relaxed(cpu_base + GIC_CPU_INTACK);
		irqnr = irqstat & GICC_IAR_INT_ID_MASK;
		if ((irqnr >= 0) && (irqnr < 1020)) {
			writel_relaxed(irqstat, cpu_base + GIC_CPU_EOI);
			pr_debug("IRQ:%d Catched.\r\n", irqnr);
			if (irq_test_stat) {
				curr_el_spx_irq_test_default(irqnr);
			} else {
				if (irq_handler_tab[irqnr].irq_handler) {
					irq_handler_tab[irqnr].irq_handler(
						irq_handler_tab[irqnr].para);
				} else {
					printf_intr(
						"Not Found handler.please Check IRQ_DEFINE.\r\n",
						irqnr);
				}
			}
			gic_clean_active_irq(irqstat);
			//gic_eoimode1_mask_irq(irqstat);
			//continue;
		}
		break;
	} while (0);
}

void curr_el_spx_FIQ_vector(void)
{
	//	u32 irqnr;
	//	unsigned long cpu_base = GIC_BASE_ADDR + GIC_CPU_OFFSET;
	printf_intr("Catch FIQ.\r\n");
#if 1
	curr_el_spx_irq_vector();
#else
	do {
		irqstat = readl_relaxed(cpu_base + GIC_CPU_INTACK);
		irqnr = irqstat & GICC_IAR_INT_ID_MASK;
		hprintf(TSENV, "%s->:%d Fiq:%d\r\n", __FUNCTION__, __LINE__,
			irqnr);
		if (irqnr < 32) {
			if (irq_test_stat) {
				curr_el_spx_irq_test_default(irqnr);
			} //if (static_key_true(&supports_deactivate))
			gic_clean_active_irq(irqnr);
			writel_relaxed(irqnr, cpu_base + GIC_CPU_EOI);
			//writel_relaxed(irqstat, cpu_base + GIC_CPU_DEACTIVATE);
			//gic_clean_active_irq(irqnr);
			continue;
		} else {
			hprintf(TSENV, " * ");
		}
		break;
	} while (1);
#endif
}

void curr_el_sp0_IRQ_vector(void)
{
	printf_intr("Catch EL0 IRQ.\r\n");
	curr_el_spx_irq_vector();
	return;
}
void curr_el_sp0_FIQ_vector(void)
{
	printf_intr("Catch EL0 FIQ.\r\n");
	curr_el_spx_irq_vector();
	return;
}
void curr_el_sp0_sync_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}

void curr_el_sp0_irq_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void curr_el_sp0_fiq_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void curr_el_sp0_serror_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void curr_el_spx_sync_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void curr_el_spx_serror_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void lower_el_aarch64_sync_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void lower_el_aarch64_irq_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void lower_el_aarch64_fiq_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void lower_el_aarch64_serror_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void lower_el_aarch32_sync_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void lower_el_aarch32_irq_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void lower_el_aarch32_fiq_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}
void lower_el_aarch32_serror_vector(struct pt_regs *pt_regs, unsigned long esr)
{
	printf_intr("Now IN[%s] ESR:%lx.\r\n", __func__, esr);
	show_regs(pt_regs, esr);
	return;
}

void init_irq(void)
{
	gic_irq_t *irq_tab = (gic_irq_t *)&__section_irq_table_start;
	unsigned int irqid;
	for (; irq_tab < (gic_irq_t *)&__section_irq_table_end; irq_tab++) {
		irqid = irq_tab->irqid;
#if 0
		if(irqid >= MAX_IRQ_ID || irqid < MIN_IRQ_ID) {
			hprintf(ERROR, "%s-> irq id is not right.\r\n", __FUNCTION__);
			continue;
		}
		irq_handler_tab[irqid].irqflags = irq_tab->irqflags;
		irq_handler_tab[irqid].irqid = irq_tab->irqid;
		irq_handler_tab[irqid].irqname = irq_tab->irqname;
		irq_handler_tab[irqid].irq_handler = irq_tab->irq_handler;
#endif
		if (ISR_ATTR_A55 & irq_tab->irqflags) {
			hvte_register_irq(irq_tab->irqid, irq_tab->irq_handler,
					  irq_tab->irqflags, irq_tab->para,
					  irq_tab->irqname);
			gic_unmask_irq(irqid);
		}
	}
}

int gic_init(void)
{
	unsigned long data;

	gic_dist_init(GIC_BASE_ADDR, MAX_IRQ_ID);
	gic_cpu_init(GIC_BASE_ADDR);

	data = cpu_gicc_get_priority_mask();
	hprintf(TSENV, "CPU_GIC_GICC_PMR=%x.\r\n", data);
	if (!(data & 0xf)) {
		hprintf(TSENV, "Support prio Num=%d.\r\n", 16);
	} else if (!(data & 0x7)) {
		hprintf(TSENV, "Support prio Num=%d.\r\n", 32);
	} else if (!(data & 0x3)) {
		hprintf(TSENV, "Support prio Num=%d.\r\n", 64);
	} else if (!(data & 0x1)) {
		hprintf(TSENV, "Support prio Num=%d.\r\n", 128);
	} else {
		hprintf(TSENV, "Support prio Num=?.\r\n");
	}
	init_irq();
	enable_irq();
	return 0;
}

// *********************Test entry*************************

static int gic_test_registers_dump(zebu_test_mod_t *mod, void *para)
{
	unsigned long dist_base = GIC_BASE_ADDR + GIC_DIST_OFFSET;
	unsigned long cpu_base = GIC_BASE_ADDR + GIC_CPU_OFFSET;
	int i;
	hprintf(TSENV, "Dump GIC CPU registers.\r\n");
	dump_one_reg(cpu_base, GIC_CPU_CTRL);
	dump_one_reg(cpu_base, GIC_CPU_PRIMASK);
	dump_one_reg(cpu_base, GIC_CPU_BINPOINT);
	dump_one_reg(cpu_base, GIC_CPU_INTACK);
	dump_one_reg(cpu_base, GIC_CPU_EOI);
	dump_one_reg(cpu_base, GIC_CPU_RUNNINGPRI);
	dump_one_reg(cpu_base, GIC_CPU_HIGHPRI);
	dump_one_reg(cpu_base, GIC_CPU_ALIAS_BINPOINT);
	dump_one_reg(cpu_base, GIC_CPU_ACTIVEPRIO);
	dump_one_reg(cpu_base, GIC_CPU_IDENT);

	hprintf(TSENV, "Dump GIC Distributor registers.\r\n");
	dump_one_reg(dist_base, GIC_DIST_CTRL);
	dump_one_reg(dist_base, GIC_DIST_CTR);
	dump_one_reg(dist_base, GIC_DIST_IIDR);
	for (i = 0; i < GICV400_MAX_INTR / 8; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_IGROUP, i);
	}

	for (i = 0; i < GICV400_MAX_INTR / 8; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_ENABLE_SET, i);
	}

	for (i = 0; i < GICV400_MAX_INTR / 8; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_ENABLE_CLEAR, i);
	}

	for (i = 0; i < GICV400_MAX_INTR / 8; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_PENDING_SET, i);
	}

	for (i = 0; i < GICV400_MAX_INTR / 8; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_PENDING_CLEAR, i);
	}

	for (i = 0; i < GICV400_MAX_INTR / 8; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_ACTIVE_SET, i);
	}

	for (i = 0; i < GICV400_MAX_INTR / 8; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_ACTIVE_CLEAR, i);
	}

	for (i = 0; i < GICV400_MAX_INTR; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_PRI, i);
	}

	for (i = 0; i < GICV400_MAX_INTR / 2; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_TARGET, i);
	}

	for (i = 0; i < GICV400_MAX_INTR / 4; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_CONFIG, i);
	}

	dump_one_reg(dist_base, GIC_DIST_SOFTINT);

	for (i = 0; i < 0x10; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_SGI_PENDING_CLEAR, i);
	}

	for (i = 0; i < 0x10; i += 4) {
		dump_one_reg_offset2(dist_base, GIC_DIST_SGI_PENDING_SET, i);
	}
	return 0;
}

static int gic_test_trig_sgi_pending(int irqnr)
{
	writel_relaxed(0xff << 16 | irqnr,
		       GIC_BASE_ADDR + GIC_DIST_OFFSET + GIC_DIST_SOFTINT);
	return 0;
}

//static int gic_test_trig_ppi_pending(int irqnr)
//{

//}

//static int gic_test_trig_irq_pending(int irqnr)
//{

//}

static int gic_test_trig_irqs_pending(unsigned long offset,
				      unsigned int irqmask)
{
	unsigned long dist_base = GIC_BASE_ADDR + GIC_DIST_OFFSET;
	writel_relaxed(irqmask, dist_base + GIC_DIST_PENDING_SET + offset);
	//hprintf(TSENV, "[%s]%d! %x set:%x \r\n",__func__,__LINE__,dist_base + GIC_DIST_PENDING_SET + offset,irqmask);
	return 0;
}

//static int gic_test_clean_irqs_pending(unsigned long offset,unsigned int irqmask)
//{
//	unsigned long dist_base = GIC_BASE_ADDR + GIC_DIST_OFFSET;
//	writel_relaxed(irqmask, dist_base + GIC_DIST_PENDING_CLEAR + offset);
//	hprintf(TSENV, "[%s]%d! %x set:%x \r\n",__func__,__LINE__,dist_base + GIC_DIST_PENDING_CLEAR + offset,irqmask);
//	return 0;
//}

//static int gic_test_prios_set(int offset,int prios)
//{
//	unsigned long dist_base = GIC_BASE_ADDR + GIC_DIST_OFFSET;
//	hprintf(TSENV, "[%s]%d ADDR:%x Set:%x\r\n",__func__,__LINE__,dist_base + GIC_DIST_PRI + offset,prios);
//	writel_relaxed(prios, dist_base + GIC_DIST_PRI + offset);
//	return 0;
//}

static int gic_test_prio_set(int inr, int prio)
{
	unsigned int data;
	unsigned int sprio = prio & 0xff;
	//unsigned int sprio = (prio << 3 )& 0xff;
	unsigned long dist_base = GIC_BASE_ADDR + GIC_DIST_OFFSET;
	int offset;
	offset = ((inr >> 2) << 2);
	data = readl_relaxed(dist_base + GIC_DIST_PRI + offset);
	//data &= ~(0xff << ((inr & 0x3) ));
	//data |= sprio << ((inr & 0x3) );
	data &= ~(0xff << ((inr & 0x3) << 3));
	data |= sprio << ((inr & 0x3) << 3);
	writel_relaxed(data, dist_base + GIC_DIST_PRI + offset);
	return 0;
}

static int gic_test_mask_all_intrs(void)
{
	int offset;
	int i;
	unsigned long dist_base = GIC_BASE_ADDR + GIC_DIST_OFFSET;
	//	unsigned long cpu_base = GIC_BASE_ADDR + GIC_CPU_OFFSET;
	for (offset = 0; offset < 0x40; offset += 4) {
		writel_relaxed(0xffffffff,
			       dist_base + GIC_DIST_ENABLE_CLEAR + offset);
	}
	for (i = 0; i <= MAX_IRQ_ID; i++) {
		irq_handler_test_tab[i] = IRQ_TEST_UNUSE;
	}

	for (i = 0; i < 32; i++) {
		irq_test_prio[i] = IRQ_TEST_UNUSE;
	}
	irq_test_curr_pos = 0;

	hprintf(TSENV, "[%s]%d!\r\n", __func__, __LINE__);
	return 0;
}

static int gic_test_find_prio(int first, int irq)
{
	int i;
	for (i = first; i < 32; i++) {
		if (irq_test_prio[i] == irq) {
			return i;
		}
	}
	return -1;
}

static int gic_test_wait_for_irqfiqs(int start, int cnt)
{
	int i;
	int first_prio = 0;
	int prio_find;
	int err_cnt = 0;
	unsigned long start_time;
	unsigned long cur_time;

	start_time = timer_get_boot_us();
	hprintf(TSENV, "Wait For interrupt Respon!\r\n");
	do {
		if (irq_test_curr_pos >= cnt) {
			//����ж���Ӧ�����ֶ�����pending���жϣ��������һ��������
			break;
		}
		cur_time = timer_get_boot_us();
	} while (cur_time - 3000000 < start_time);

	hprintf(TSENV, "\n\n");
	if (0 == irq_test_curr_pos) {
		hprintf(TSENV, "None Intr has been responed!\r\n");
		//���û���жϱ���Ӧ��ֱ���˳���
		return -1;
	}
	for (i = 0; i < 32; i++) {
		if (irq_test_prio[i] != IRQ_TEST_UNUSE) {
			//����������ȼ���
			first_prio = i;
			break;
		}
	}
	for (i = 0; i < cnt; i++) {
		if (irq_handler_test_tab[i] != irq_test_prio[first_prio]) {
			//����Ƚ��յ����жϣ��������ȼ�����ߵ��жϣ�������
			prio_find = gic_test_find_prio(first_prio,
						       irq_handler_test_tab[i]);
			hprintf(TSENV,
				"Error intr HighestPrio:%d C:%d FoundPrio:%d\r\n",
				irq_handler_test_tab[i],
				irq_test_prio[first_prio], prio_find);
			err_cnt++;
		} else {
			first_prio++;
		}
	}
	if (err_cnt == 0) {
		hprintf(TSENV, "Test Ok!\r\n");
	}
	irq_test_curr_pos = 0;
	return err_cnt;
}

/*
����SGI&PRIO
���裺1.���������ж��쳣
	  2.��������SGI�����ȼ�
	  3.ʹ��SGI�ж�
	  4.�ֶ�pending����SGI�ж�
	  5.�鿴�ж��Ƿ���Ӧ���ҶԱ��ж���Ӧ���Ⱥ�˳��
*/
static int gic_test_sgi_and_pri(zebu_test_mod_t *mod, void *para)
{
	int k = 0;
	int i = 0;
	int prio = 0;
	int ret;
	unsigned int flags = 0;
	int test_cnt = 0;

	gic_test_mask_all_intrs();
	for (i = 0; i < 16; i++) {
		if (i == 0) {
			//gic_test_clean_irqs_pending((i / 32)<< 2 , 0xffffffff);
			flags = readl_relaxed(GIC_BASE_ADDR + GIC_DIST_OFFSET +
					      GIC_DIST_PENDING_SET +
					      ((i / 32) << 2));
			hprintf(TSENV, "[%s]%d flags= %08x\r\n", __func__,
				__LINE__, flags);
		}
		if (!((1 << (i & 0x1f)) & flags)) {
			prio &= 0x1f;
			//4���жϺţ�����һ�����ȼ�
			gic_test_prio_set(i, prio);
			//gic_test_prios_set((i>>2)<<2 ,(prio) | ((prio-1) << 8)| ((prio-2) << 16) | ((prio-3) << 24));
			irq_test_prio[prio] = i;
			prio++;
			test_cnt++;
			//cpu_gicd_set_group(i,1);
			gic_unmask_irq(i);
			gic_test_trig_sgi_pending(i);
		} else {
			hprintf(TSENV,
				"[%s]%d IRQ:%d Default Stat Is Pending.\r\n",
				__func__, __LINE__, i);
		}

		if ((i & 0xf) == 0xf) {
			//�ж�����pending�󣬵ȴ��ж���ɡ�
			ret = gic_test_wait_for_irqfiqs(1, test_cnt);
			if (ret != 0) {
				hprintf(TSENV,
					"Inter Test,irq:%d + %d.Error cnt:%d\r\n",
					0, test_cnt, ret);
			}
			for (k = 0; k < 32; k++) {
				irq_test_prio[k] = IRQ_TEST_UNUSE;
			}
			test_cnt = 0;
		}
	}
	return 0;
}

/*
����PPI&PRIO
���裺1.���������ж��쳣
	  2.��������PPI�����ȼ�
	  3.ʹ��PPI�ж�
	  4.�ֶ�pending����PPI�ж�
	  5.�鿴�ж��Ƿ���Ӧ���ҶԱ��ж���Ӧ���Ⱥ�˳��
*/
static int gic_test_ppi_and_pri(zebu_test_mod_t *mod, void *para)
{
	int k = 0;
	int i = 0;
	int prio = 0;
	int ret;
	int test_cnt = 0;
	unsigned int flags = 0;

	gic_test_mask_all_intrs();
	for (i = 25; i < 32; i++) {
		if (i == 25) {
			//gic_test_clean_irqs_pending((i / 32)<< 2 , 0xffffffff);
			flags = readl_relaxed(GIC_BASE_ADDR + GIC_DIST_OFFSET +
					      GIC_DIST_PENDING_SET +
					      ((i / 32) << 2));
			hprintf(TSENV, "[%s]%d flags= %08x\r\n", __func__,
				__LINE__, flags);
		}
		if (!((1 << (i & 0x1f)) & flags)) {
			prio &= 0x1f;
			//4���жϺţ�����һ�����ȼ�
			gic_test_prio_set(i, prio);
			//gic_test_prios_set((i>>2)<<2 ,(prio) | ((prio-1) << 8)| ((prio-2) << 16) | ((prio-3) << 24));
			irq_test_prio[prio] = i;
			prio++;
			test_cnt++;
			//cpu_gicd_set_group(i,1);
			gic_unmask_irq(i);
		} else {
			hprintf(TSENV,
				"[%s]%d IRQ:%d Default Stat Is Pending.\r\n",
				__func__, __LINE__, i);
		}

		if ((i & 0x1f) == 0x1f) {
			//PPIֻ��8���жϺ�
			gic_test_trig_irqs_pending((i / 32) << 2,
						   0xffff << ((i & 0x1f) - 15));
			ret = gic_test_wait_for_irqfiqs(i - 0xf, test_cnt);
			if (ret != 0) {
				hprintf(TSENV,
					"Inter Test,irq:%d + %d.Error cnt:%d\r\n",
					i - 0xf, test_cnt, ret);
			}
			for (k = 0; k < 32; k++) {
				irq_test_prio[k] = IRQ_TEST_UNUSE;
			}
			test_cnt = 0;
		}
	}
	return 0;
}

/*
����SPI&PRIO
���裺1.���������ж��쳣
	  2.��������SGI�����ȼ�
	  3.ʹ��SPI�ж�
	  4.�ֶ�pending����SPI�ж�
	  5.�鿴�ж��Ƿ���Ӧ���ҶԱ��ж���Ӧ���Ⱥ�˳��
*/
static int gic_test_spi_and_pri(zebu_test_mod_t *mod, void *para)
{
	int k = 0;
	int i = 0;
	int prio = 0;
	int ret;
	int test_cnt = 0;
	unsigned int flags = 0;

	gic_test_mask_all_intrs();
	for (i = 32; i < 256; i++) {
		if ((i & 0x1f) == 0) {
			//gic_test_clean_irqs_pending((i / 32)<< 2 , 0xffffffff);
			flags = readl_relaxed(GIC_BASE_ADDR + GIC_DIST_OFFSET +
					      GIC_DIST_PENDING_SET +
					      ((i / 32) << 2));
			hprintf(TSENV, "[%s]%d Init flags = %08x\r\n", __func__,
				__LINE__, flags);
		}
		if (!((1 << (i & 0x1f)) & flags)) {
			prio &= 0x1f;
			//4���жϺţ�����һ�����ȼ�
			gic_test_prio_set(i, prio);
			irq_test_prio[prio] = i;
			prio++;
			test_cnt++;
			//cpu_gicd_set_group(i,1);
			gic_unmask_irq(i);
		} else {
			hprintf(TSENV,
				"[%s]%d IRQ:%d Default Stat Is Pending.\r\n",
				__func__, __LINE__, i);
		}
		if ((i & 0x1f) == 0x1f) {
			//32���жϺţ�����һ���ж�
			//hprintf(TSENV, "[%s]%d!\r\n",__func__,__LINE__);
			gic_test_trig_irqs_pending((i >> 5) << 2, 0xffffffff);
			ret = gic_test_wait_for_irqfiqs(i - 31, test_cnt);
			if (ret != 0) {
				hprintf(TSENV,
					"Inter Test,irq:%d + %d.Error cnt:%d\r\n",
					i - 31, test_cnt, ret);
			} else {
				hprintf(TSENV, "Inter Test,irq:%d + %d.OK\r\n",
					i - 31, test_cnt);
			}
			for (k = 0; k < 32; k++) {
				irq_test_prio[k] = IRQ_TEST_UNUSE;
			}
			test_cnt = 0;
			prio = 0;
		}
	}
	return 0;
}

static int gic_test_switch_el2secure(zebu_test_mod_t *mod, void *para)
{
	//SWITCH CPU RUN IN EL2
	unsigned int el_stat;
	//	unsigned long reg_data;

	//reg_data = read_sysreg(SCR_EL3);
	//reg_data |= 1 <<��
	switch_to_el2();

	el_stat = current_el();
	hprintf(TSENV, "Now In EL%d\n\r", el_stat);
	GicTestMain(0);
	return 0;
}

static int gic_test_switch_el1(zebu_test_mod_t *mod, void *para)
{
	//SWITCH CPU RUN IN EL1
	unsigned int el_stat;
	switch_to_el1();
	el_stat = current_el();
	hprintf(TSENV, "Now In EL%d\n\r", el_stat);
	GicTestMain(0);
	return 0;
}

static zebu_test_mod_t zebu_test_gic_mods[] = {
	{ "GIC", 0 },
};

static zebu_test_entry_t zebu_test_gic_entrys[] = {
	{ '1', 1, "Reg Dump", gic_test_registers_dump },
	{ '2', 1, "SGI&PRI Test", gic_test_sgi_and_pri },
	{ '3', 1, "PPI&PRI Test", gic_test_ppi_and_pri },
	{ '4', 1, "SPI&PRI Test", gic_test_spi_and_pri },
	{ '5', 0, "Switch To EL2&secure", gic_test_switch_el2secure },
	{ '6', 0, "Switch To EL1", gic_test_switch_el1 },
};

static zebu_test_ctrl_t zebu_gic_mod_test_ctrl = {
	"A55-GIC",
	SIZE_ARRAY(zebu_test_gic_mods),
	zebu_test_gic_mods,
	SIZE_ARRAY(zebu_test_gic_entrys),
	zebu_test_gic_entrys,
	0
};

void GicTestMain(int para)
{
	int ret;
	int regdata = 0;
	unsigned int el_stat;
	zebu_gic_mod_test_ctrl.current_mod = 0;
	irq_test_stat = 1;
	regdata = current_el();
	hprintf(TSENV, "Current Run EL:%d!!!!\r\n", regdata);
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_gic_mod_test_ctrl);
		hprintf(TSENV, "auto test err %d!!!!\r\n", ret);
	} else {
		hprintf(TSENV, "enter GIC test!\r\n");
		TestLoop(&zebu_gic_mod_test_ctrl);
	}
	irq_test_stat = 0;
	el_stat = current_el();
	if (el_stat <= 2) {
		cmd_main();
	}
}

CMD_DEFINE(gic, GicTestMain, 0, "gic test", CMD_ATTR_AUTO_TEST);
