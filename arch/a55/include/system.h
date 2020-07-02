/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __ARMV8_SYSTEM_H__
#define __ARMV8_SYSTEM_H__

#include <asm/io.h>

typedef unsigned long long phys_addr_t;
typedef unsigned long long phys_size_t;
typedef unsigned long long u64;

void __asm_flush_dcache_all(void);
void __asm_invalidate_dcache_all(void);
void __asm_flush_dcache_range(u64 start, u64 end);

/**
 * __asm_invalidate_dcache_range() - Invalidate a range of virtual addresses
 *
 * This performance an invalidate from @start to @end - 1. Both addresses
 * should be cache-aligned, otherwise this function will align the start
 * address and may continue past the end address.
 *
 * Data in the address range is evicted from the cache and is not written back
 * to memory.
 *
 * @start: Start address to invalidate
 * @end: End address to invalidate up to (exclusive)
 */
void __asm_invalidate_dcache_range(u64 start, u64 end);
void __asm_invalidate_tlb_all(void);
void __asm_invalidate_icache_all(void);
int __asm_invalidate_l3_dcache(void);
int __asm_flush_l3_dcache(void);
int __asm_invalidate_l3_icache(void);
void __asm_switch_ttbr(u64 new_ttbr);

#define __stringify(__rname) #__rname
/*
 * Unlike read_cpuid, calls to read_sysreg are never expected to be
 * optimized away or replaced with synthetic values.
 */

#define read_sysreg(r)                                                         \
	({                                                                     \
		unsigned long __val;                                           \
		asm volatile("mrs %0, " __stringify(r) : "=r"(__val));         \
		__val;                                                         \
	})

/*
 * The "Z" constraint normally means a zero immediate, but when combined with
 * the "%x0" template means XZR.
 */
#define write_sysreg(v, r)                                                     \
	do {                                                                   \
		unsigned long __val = (unsigned long)v;                        \
		asm volatile("msr " __stringify(r) ", %x0" : : "rZ"(__val));   \
	} while (0)

/*
 * For registers without architectural names, or simply unsupported by
 * GAS.
 */
#define read_sysreg_s(r)                                                       \
	({                                                                     \
		unsigned long __val;                                           \
		asm volatile("mrs_s %0, " __stringify(r) : "=r"(__val));       \
		__val;                                                         \
	})

#define write_sysreg_s(v, r)                                                   \
	do {                                                                   \
		unsigned long __val = (unsigned long)v;                        \
		asm volatile("msr_s " __stringify(r) ", %x0" : : "rZ"(__val)); \
	} while (0)

static inline unsigned int current_el(void)
{
	unsigned int el;
	asm volatile("mrs %0, CurrentEL" : "=r"(el) : : "cc");
	return el >> 2;
}

static inline __attribute__((always_inline)) unsigned int get_sctlr(void)
{
	unsigned int el, val;

	el = current_el();
	if (el == 1)
		asm volatile("mrs %0, sctlr_el1" : "=r"(val) : : "cc");
	else if (el == 2)
		asm volatile("mrs %0, sctlr_el2" : "=r"(val) : : "cc");
	else
		asm volatile("mrs %0, sctlr_el3" : "=r"(val) : : "cc");

	return val;
}

static inline __attribute__((always_inline)) void set_sctlr(unsigned int val)
{
	unsigned int el;

	el = current_el();
	if (el == 1)
		asm volatile("msr sctlr_el1, %0" : : "r"(val) : "cc");
	else if (el == 2)
		asm volatile("msr sctlr_el2, %0" : : "r"(val) : "cc");
	else
		asm volatile("msr sctlr_el3, %0" : : "r"(val) : "cc");

	asm volatile("isb");
}

static inline unsigned long read_mpidr(void)
{
	unsigned long val;

	asm volatile("mrs %0, mpidr_el1" : "=r"(val));

	return val;
}

/*
 * SCTLR_EL1/SCTLR_EL2/SCTLR_EL3 bits definitions
 */
#define CR_M (1 << 0) /* MMU enable			*/
#define CR_A (1 << 1) /* Alignment abort enable	*/
#define CR_C (1 << 2) /* Dcache enable		*/
#define CR_SA (1 << 3) /* Stack Alignment Check Enable	*/
#define CR_I (1 << 12) /* Icache enable		*/
#define CR_WXN (1 << 19) /* Write Permision Imply XN	*/
#define CR_EE (1 << 25) /* Exception (Big) Endian	*/

#define ES_TO_AARCH64 1
#define ES_TO_AARCH32 0

/*
 * SCR_EL3 bits definitions
 */
#define SCR_EL3_RW_AARCH64 (1 << 10) /* Next lower level is AArch64     */
#define SCR_EL3_RW_AARCH32 (0 << 10) /* Lower lowers level are AArch32  */
#define SCR_EL3_HCE_EN (1 << 8) /* Hypervisor Call enable          */
#define SCR_EL3_SMD_DIS (1 << 7) /* Secure Monitor Call disable     */
#define SCR_EL3_RES1 (3 << 4) /* Reserved, RES1                  */
#define SCR_EL3_EA_EN (1 << 3) /* External aborts taken to EL3    */
#define SCR_EL3_NS_EN (1 << 0) /* EL0 and EL1 in Non-scure state  */

/*
 * SPSR_EL3/SPSR_EL2 bits definitions
 */
#define SPSR_EL_END_LE (0 << 9) /* Exception Little-endian          */
#define SPSR_EL_DEBUG_MASK (1 << 9) /* Debug exception masked           */
#define SPSR_EL_ASYN_MASK (1 << 8) /* Asynchronous data abort masked   */
#define SPSR_EL_SERR_MASK (1 << 8) /* System Error exception masked    */
#define SPSR_EL_IRQ_MASK (1 << 7) /* IRQ exception masked             */
#define SPSR_EL_FIQ_MASK (1 << 6) /* FIQ exception masked             */
#define SPSR_EL_T_A32 (0 << 5) /* AArch32 instruction set A32      */
#define SPSR_EL_M_AARCH64 (0 << 4) /* Exception taken from AArch64     */
#define SPSR_EL_M_AARCH32 (1 << 4) /* Exception taken from AArch32     */
#define SPSR_EL_M_SVC (0x3) /* Exception taken from SVC mode    */
#define SPSR_EL_M_HYP (0xa) /* Exception taken from HYP mode    */
#define SPSR_EL_M_EL1H (5) /* Exception taken from EL1h mode   */
#define SPSR_EL_M_EL2H (9) /* Exception taken from EL2h mode   */

/*
 * CPTR_EL2 bits definitions
 */
#define CPTR_EL2_RES1 (3 << 12 | 0x3ff) /* Reserved, RES1 */

/*
 * SCTLR_EL2 bits definitions
 */
#define SCTLR_EL2_RES1                                                         \
	(3 << 28 | 3 << 22 | 1 << 18 | 1 << 16 | 1 << 11 |                     \
	 3 << 4) /* Reserved, RES1 */
#define SCTLR_EL2_EE_LE (0 << 25) /* Exception Little-endian          */
#define SCTLR_EL2_WXN_DIS (0 << 19) /* Write permission is not XN       */
#define SCTLR_EL2_ICACHE_DIS (0 << 12) /* Instruction cache disabled       */
#define SCTLR_EL2_SA_DIS (0 << 3) /* Stack Alignment Check disabled   */
#define SCTLR_EL2_DCACHE_DIS (0 << 2) /* Data cache disabled              */
#define SCTLR_EL2_ALIGN_DIS (0 << 1) /* Alignment check disabled         */
#define SCTLR_EL2_MMU_DIS (0) /* MMU disabled                     */

/*
 * CNTHCTL_EL2 bits definitions
 */
#define CNTHCTL_EL2_EL1PCEN_EN (1 << 1) /* Physical timer regs accessible   */
#define CNTHCTL_EL2_EL1PCTEN_EN (1 << 0) /* Physical counter accessible      */

/*
 * HCR_EL2 bits definitions
 */
#define HCR_EL2_RW_AARCH64 (1 << 31) /* EL1 is AArch64                   */
#define HCR_EL2_RW_AARCH32 (0 << 31) /* Lower levels are AArch32         */
#define HCR_EL2_HCD_DIS (1 << 29) /* Hypervisor Call disabled         */

/*
 * CPACR_EL1 bits definitions
 */
#define CPACR_EL1_FPEN_EN (3 << 20) /* SIMD and FP instruction enabled  */

/*
 * SCTLR_EL1 bits definitions
 */
#define SCTLR_EL1_RES1                                                         \
	(3 << 28 | 3 << 22 | 1 << 20 |                                         \
	 1 << 11) /* Reserved, RES1                   */
#define SCTLR_EL1_UCI_DIS (0 << 26) /* Cache instruction disabled       */
#define SCTLR_EL1_EE_LE (0 << 25) /* Exception Little-endian          */
#define SCTLR_EL1_WXN_DIS (0 << 19) /* Write permission is not XN       */
#define SCTLR_EL1_NTWE_DIS (0 << 18) /* WFE instruction disabled         */
#define SCTLR_EL1_NTWI_DIS (0 << 16) /* WFI instruction disabled         */
#define SCTLR_EL1_UCT_DIS (0 << 15) /* CTR_EL0 access disabled          */
#define SCTLR_EL1_DZE_DIS (0 << 14) /* DC ZVA instruction disabled      */
#define SCTLR_EL1_ICACHE_DIS (0 << 12) /* Instruction cache disabled       */
#define SCTLR_EL1_UMA_DIS (0 << 9) /* User Mask Access disabled        */
#define SCTLR_EL1_SED_EN (0 << 8) /* SETEND instruction enabled       */
#define SCTLR_EL1_ITD_EN (0 << 7) /* IT instruction enabled           */
#define SCTLR_EL1_CP15BEN_DIS (0 << 5) /* CP15 barrier operation disabled  */
#define SCTLR_EL1_SA0_DIS (0 << 4) /* Stack Alignment EL0 disabled     */
#define SCTLR_EL1_SA_DIS (0 << 3) /* Stack Alignment EL1 disabled     */
#define SCTLR_EL1_DCACHE_DIS (0 << 2) /* Data cache disabled              */
#define SCTLR_EL1_ALIGN_DIS (0 << 1) /* Alignment check disabled         */
#define SCTLR_EL1_MMU_DIS (0) /* MMU disabled                     */

u64 get_page_table_size(void);
#define PGTABLE_SIZE get_page_table_size()

/* 2MB granularity */
#define MMU_SECTION_SHIFT 21
#define MMU_SECTION_SIZE (1 << MMU_SECTION_SHIFT)

/* These constants need to be synced to the MT_ types in asm/armv8/mmu.h */
enum dcache_option {
	DCACHE_OFF = 0 << 2,
	DCACHE_WRITETHROUGH = 3 << 2,
	DCACHE_WRITEBACK = 4 << 2,
	DCACHE_WRITEALLOC = 4 << 2,
};

#endif
