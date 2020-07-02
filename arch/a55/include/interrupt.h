/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#define GIC_DIST_OFFSET 0X1000
#define GIC_CPU_OFFSET 0X2000
#define GICV400_MAX_INTR (512)

#define GIC_CPU_CTRL 0x00
#define GIC_CPU_PRIMASK 0x04
#define GIC_CPU_BINPOINT 0x08
#define GIC_CPU_INTACK 0x0c
#define GIC_CPU_EOI 0x10
#define GIC_CPU_RUNNINGPRI 0x14
#define GIC_CPU_HIGHPRI 0x18
#define GIC_CPU_ALIAS_BINPOINT 0x1c
#define GIC_CPU_ACTIVEPRIO 0xd0
#define GIC_CPU_IDENT 0xfc
#define GIC_CPU_DEACTIVATE 0x1000

#define GICC_ENABLE 0x1
#define GICC_INT_PRI_THRESHOLD 0xf0

#define GIC_CPU_CTRL_EOImodeNS (1 << 9)

#define GICC_IAR_INT_ID_MASK 0x3ff
#define GICC_INT_SPURIOUS 1023
#define GICC_DIS_BYPASS_MASK 0x1e0

#define GIC_DIST_CTRL 0x000
#define GIC_DIST_CTR 0x004
#define GIC_DIST_IIDR 0x008
#define GIC_DIST_IGROUP 0x080
#define GIC_DIST_ENABLE_SET 0x100
#define GIC_DIST_ENABLE_CLEAR 0x180
#define GIC_DIST_PENDING_SET 0x200
#define GIC_DIST_PENDING_CLEAR 0x280
#define GIC_DIST_ACTIVE_SET 0x300
#define GIC_DIST_ACTIVE_CLEAR 0x380
#define GIC_DIST_PRI 0x400
#define GIC_DIST_TARGET 0x800
#define GIC_DIST_CONFIG 0xc00
#define GIC_DIST_SOFTINT 0xf00
#define GIC_DIST_SGI_PENDING_CLEAR 0xf10
#define GIC_DIST_SGI_PENDING_SET 0xf20

#define GICD_ENABLE 0x1
#define GICD_DISABLE 0x0
#define GICD_INT_ACTLOW_LVLTRIG 0x0
#define GICD_INT_EN_CLR_X32 0xffffffff
#define GICD_INT_EN_SET_SGI 0x0000ffff
#define GICD_INT_EN_CLR_PPI 0xffff0000
#define GICD_INT_DEF_PRI 0xa0
#define GICD_INT_DEF_PRI_X4                                                    \
	((GICD_INT_DEF_PRI << 24) | (GICD_INT_DEF_PRI << 16) |                 \
	 (GICD_INT_DEF_PRI << 8) | GICD_INT_DEF_PRI)

#define GICH_HCR 0x0
#define GICH_VTR 0x4
#define GICH_VMCR 0x8
#define GICH_MISR 0x10
#define GICH_EISR0 0x20
#define GICH_EISR1 0x24
#define GICH_ELRSR0 0x30
#define GICH_ELRSR1 0x34
#define GICH_APR 0xf0
#define GICH_LR0 0x100

#define GICH_HCR_EN (1 << 0)
#define GICH_HCR_UIE (1 << 1)

#define GICH_LR_VIRTUALID (0x3ff << 0)
#define GICH_LR_PHYSID_CPUID_SHIFT (10)
#define GICH_LR_PHYSID_CPUID (0x3ff << GICH_LR_PHYSID_CPUID_SHIFT)
#define GICH_LR_PRIORITY_SHIFT 23
#define GICH_LR_STATE (3 << 28)
#define GICH_LR_PENDING_BIT (1 << 28)
#define GICH_LR_ACTIVE_BIT (1 << 29)
#define GICH_LR_EOI (1 << 19)
#define GICH_LR_HW (1 << 31)

#define GICH_VMCR_CTRL_SHIFT 0
#define GICH_VMCR_CTRL_MASK (0x21f << GICH_VMCR_CTRL_SHIFT)
#define GICH_VMCR_PRIMASK_SHIFT 27
#define GICH_VMCR_PRIMASK_MASK (0x1f << GICH_VMCR_PRIMASK_SHIFT)
#define GICH_VMCR_BINPOINT_SHIFT 21
#define GICH_VMCR_BINPOINT_MASK (0x7 << GICH_VMCR_BINPOINT_SHIFT)
#define GICH_VMCR_ALIAS_BINPOINT_SHIFT 18
#define GICH_VMCR_ALIAS_BINPOINT_MASK (0x7 << GICH_VMCR_ALIAS_BINPOINT_SHIFT)

#define GICH_MISR_EOI (1 << 0)
#define GICH_MISR_U (1 << 1)

#define MIN_IRQ_ID 25
#define MAX_IRQ_ID 260

#define ESR_ELx_EC_UNKNOWN (0x00)
#define ESR_ELx_EC_WFx (0x01)
/* Unallocated EC: 0x02 */
#define ESR_ELx_EC_CP15_32 (0x03)
#define ESR_ELx_EC_CP15_64 (0x04)
#define ESR_ELx_EC_CP14_MR (0x05)
#define ESR_ELx_EC_CP14_LS (0x06)
#define ESR_ELx_EC_FP_ASIMD (0x07)
#define ESR_ELx_EC_CP10_ID (0x08)
/* Unallocated EC: 0x09 - 0x0B */
#define ESR_ELx_EC_CP14_64 (0x0C)
/* Unallocated EC: 0x0d */
#define ESR_ELx_EC_ILL (0x0E)
/* Unallocated EC: 0x0F - 0x10 */
#define ESR_ELx_EC_SVC32 (0x11)
#define ESR_ELx_EC_HVC32 (0x12)
#define ESR_ELx_EC_SMC32 (0x13)
/* Unallocated EC: 0x14 */
#define ESR_ELx_EC_SVC64 (0x15)
#define ESR_ELx_EC_HVC64 (0x16)
#define ESR_ELx_EC_SMC64 (0x17)
#define ESR_ELx_EC_SYS64 (0x18)
/* Unallocated EC: 0x19 - 0x1E */
#define ESR_ELx_EC_IMP_DEF (0x1f)
#define ESR_ELx_EC_IABT_LOW (0x20)
#define ESR_ELx_EC_IABT_CUR (0x21)
#define ESR_ELx_EC_PC_ALIGN (0x22)
/* Unallocated EC: 0x23 */
#define ESR_ELx_EC_DABT_LOW (0x24)
#define ESR_ELx_EC_DABT_CUR (0x25)
#define ESR_ELx_EC_SP_ALIGN (0x26)
/* Unallocated EC: 0x27 */
#define ESR_ELx_EC_FP_EXC32 (0x28)
/* Unallocated EC: 0x29 - 0x2B */
#define ESR_ELx_EC_FP_EXC64 (0x2C)
/* Unallocated EC: 0x2D - 0x2E */
#define ESR_ELx_EC_SERROR (0x2F)
#define ESR_ELx_EC_BREAKPT_LOW (0x30)
#define ESR_ELx_EC_BREAKPT_CUR (0x31)
#define ESR_ELx_EC_SOFTSTP_LOW (0x32)
#define ESR_ELx_EC_SOFTSTP_CUR (0x33)
#define ESR_ELx_EC_WATCHPT_LOW (0x34)
#define ESR_ELx_EC_WATCHPT_CUR (0x35)
/* Unallocated EC: 0x36 - 0x37 */
#define ESR_ELx_EC_BKPT32 (0x38)
/* Unallocated EC: 0x39 */
#define ESR_ELx_EC_VECTOR32 (0x3A)
/* Unallocted EC: 0x3B */
#define ESR_ELx_EC_BRK64 (0x3C)
/* Unallocated EC: 0x3D - 0x3F */
#define ESR_ELx_EC_MAX (0x3F)

#define ESR_ELx_EC_SHIFT (26)
#define ESR_ELx_EC_MASK ((0x3FUL) << ESR_ELx_EC_SHIFT)
#define ESR_ELx_EC(esr) (((esr)&ESR_ELx_EC_MASK) >> ESR_ELx_EC_SHIFT)

#define ESR_ELx_IL ((1UL) << 25)
#define ESR_ELx_ISS_MASK (ESR_ELx_IL - 1)

struct pt_regs {
	unsigned long elr;
	unsigned long regs[31];
};

struct esr_s {
	char *esr_str;
	unsigned long esr_num;
};

typedef struct gic_irq_struct {
	void (*irq_handler)(int irqid);
	int irqid;
	unsigned int irqflags;
	char *irqname;
	unsigned long para;
} gic_irq_t;

int gic_init(void);

extern void switch_to_el2(void);
extern void switch_to_el1(void);
extern void switch_to_el0(void);

void gic_unmask_irq(int irqid);
void gic_mask_irq(int irqid);

#define SECURE_IRQ_ENABLE(...)
#define SECURE_IRQ_DISABLE(...)
#define SECURE_FIQ_ENABLE(...)
#define SECURE_FIQ_DISABLE(...)

#define SAFETY_IRQ_ENABLE(...)
#define SAFETY_IRQ_DISABLE(...)
#define SAFETY_FIQ_ENABLE(...)
#define SAFETY_FIQ_DISABLE(...)

#define A55_IRQ_ENABLE(__irq) gic_unmask_irq(__irq)
#define A55_IRQ_DISABLE(__irq) gic_mask_irq(__irq)
#define A55_FIQ_ENABLE(__irq) gic_unmask_irq(__irq)
#define A55_FIQ_DISABLE(__irq) gic_mask_irq(__irq)

/*which cpu*/
#define ISR_ATTR_SECURE 0x1
#define ISR_ATTR_SAFETY 0x2
#define ISR_ATTR_A55 0x4
/*level or edge( bit 16)*/
#define ISR_ATTR_LEVEL 0X00000
#define ISR_ATTR_EDGE 0X20000
#define IS_EDGE(a) ((a & 0x30000) >> 16)

#define FIQ_DEFINE(__isrName, __isrFn, __para, __description, __attr)

#define IRQ_DEFINE(__IrqId, __IrqHandle, __para, __IrqName, __IrqFlags)        \
	__attribute__((section(".section_irq_table"))) struct gic_irq_struct   \
		_section_item_##__IrqId##_tlb = {                              \
			.irq_handler = __IrqHandle,                            \
			.irqid = __IrqId,                                      \
			.irqflags = __IrqFlags,                                \
			.irqname = __IrqName,                                  \
			.para = __para,                                        \
		}

#endif
