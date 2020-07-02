/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <a1000/sysreg_base.h>
#include <system.h>
#include <mmu.h>
#include <cache.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <module/a55_timer.h>
#include <printf_inc.h> //use related path
#include <utility_lite.h>
#include <test_common.h>
#include <linux/string.h>

/*
 *  With 4k page granule, a virtual address is split into 4 lookup parts
 *  spanning 9 bits each:
 *
 *    _______________________________________________
 *   |       |       |       |       |       |       |
 *   |   0   |  Lv0  |  Lv1  |  Lv2  |  Lv3  |  off  |
 *   |_______|_______|_______|_______|_______|_______|
 *     63-48   47-39   38-30   29-21   20-12   11-00
 *
 *             mask        page size
 *
 *    Lv0: FF8000000000       --
 *    Lv1:   7FC0000000       1G
 *    Lv2:     3FE00000       2M
 *    Lv3:       1FF000       4K
 *    off:          FFF
 */

#define A55_DDR_TEST_MMU_VA (0x80000000ULL)
#define A55_MMU_TEST_OFFSET (0x10 << 20)
#define A55_MMU_TEST_ADDR (A55_DDR_TEST_MMU_VA + A55_MMU_TEST_OFFSET)
#define A55_MMU_TEST_SIZE (0X400)

a55_arch_gd_t a55_arch_gd = {
	.tlb_addr = 0x80c00000ULL,
	.tlb_size = 0x100000ULL,
};

struct mm_region mem_map[] = {
	{ .virt = 0x0ULL,
	  .phys = 0x0ULL,
	  .size = 0x80000000ULL,
	  //.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE |
	  .attrs = PTE_BLOCK_PXN | PTE_BLOCK_UXN },
	{ .virt = 0x80000000ULL,
	  .phys = A55_DDR_TEST_MMU_VA,
	  .size = 0x80000000ULL,
	  .attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_INNER_SHARE },
	{
		/* List terminator */
		0,
	}
};

void mmu_enable(void);

u64 get_tcr(int el, u64 *pips, u64 *pva_bits)
{
	u64 max_addr = 0;
	u64 ips, va_bits;
	u64 tcr;
	int i;

	/* Find the largest address we need to support */
	for (i = 0; mem_map[i].size || mem_map[i].attrs; i++)
		max_addr = max(max_addr, mem_map[i].virt + mem_map[i].size);

	/* Calculate the maximum physical (and thus virtual) address */
	if (max_addr > (1ULL << 44)) {
		ips = 5;
		va_bits = 48;
	} else if (max_addr > (1ULL << 42)) {
		ips = 4;
		va_bits = 44;
	} else if (max_addr > (1ULL << 40)) {
		ips = 3;
		va_bits = 42;
	} else if (max_addr > (1ULL << 36)) {
		ips = 2;
		va_bits = 40;
	} else if (max_addr > (1ULL << 32)) {
		ips = 1;
		va_bits = 36;
	} else {
		ips = 0;
		va_bits = 32;
	}

	if (el == 1) {
		tcr = TCR_EL1_RSVD | (ips << 32) | TCR_EPD1_DISABLE;
	} else if (el == 2) {
		tcr = TCR_EL2_RSVD | (ips << 16);
	} else {
		tcr = TCR_EL3_RSVD | (ips << 16);
	}

	/* PTWs cacheable, inner/outer WBWA and inner shareable */
	tcr |= TCR_TG0_4K | TCR_SHARED_INNER | TCR_ORGN_WBWA | TCR_IRGN_WBWA;
	tcr |= TCR_T0SZ(va_bits);

	if (pips)
		*pips = ips;
	if (pva_bits)
		*pva_bits = va_bits;

	return tcr;
}

#define MAX_PTE_ENTRIES 512

static int pte_type(u64 *pte)
{
	return *pte & PTE_TYPE_MASK;
}

/* Returns the LSB number for a PTE on level <level> */
static int level2shift(int level)
{
	/* Page is 12 bits wide, every level translates 9 bits */
	return (12 + 9 * (3 - level));
}

static u64 *find_pte(u64 addr, int level)
{
	int start_level = 0;
	u64 *pte;
	u64 idx;
	u64 va_bits;
	int i;

	hprintf(TSENV, "addr=%llx level=%d.\n\r", addr, level);

	get_tcr(3, NULL, &va_bits);
	if (va_bits < 39)
		start_level = 1;

	if (level < start_level)
		return NULL;

	/* Walk through all page table levels to find our PTE */
	pte = (u64 *)a55_arch_gd.tlb_addr;
	for (i = start_level; i < 4; i++) {
		idx = (addr >> level2shift(i)) & 0x1FF;
		pte += idx;
		hprintf(TSENV, "idx=%llx PTE %p at level %d: %llx.\n\r", idx,
			pte, i, *pte);

		/* Found it */
		if (i == level)
			return pte;
		/* PTE is no table (either invalid or block), can't traverse */
		if (pte_type(pte) != PTE_TYPE_TABLE)
			return NULL;
		/* Off to the next level */
		pte = (u64 *)(*pte & 0x0000fffffffff000ULL);
	}

	/* Should never reach here */
	return NULL;
}

/* Returns and creates a new full table (512 entries) */
static u64 *create_table(void)
{
	u64 *new_table = (u64 *)a55_arch_gd.tlb_fillptr;
	u64 pt_len = MAX_PTE_ENTRIES * sizeof(u64);

	/* Allocate MAX_PTE_ENTRIES pte entries */
	a55_arch_gd.tlb_fillptr += pt_len;

	if (a55_arch_gd.tlb_fillptr - a55_arch_gd.tlb_addr >
	    a55_arch_gd.tlb_size)
		hprintf(ERROR,
			"Insufficient RAM for page table: 0x%lx > 0x%lx. "
			"Please increase the size in get_page_table_size()\n\r",
			a55_arch_gd.tlb_fillptr - a55_arch_gd.tlb_addr,
			a55_arch_gd.tlb_size);

	/* Mark all entries as invalid */
	memset(new_table, 0, pt_len);

	return new_table;
}

static void set_pte_table(u64 *pte, u64 *table)
{
	/* Point *pte to the new table */
	hprintf(TSENV, "Setting %p to addr=%p.\n\r", pte, table);
	*pte = PTE_TYPE_TABLE | (unsigned long)table;
}

/* Splits a block PTE into table with subpages spanning the old block */
static void split_block(u64 *pte, int level)
{
	u64 old_pte = *pte;
	u64 *new_table;
	u64 i = 0;
	/* level describes the parent level, we need the child ones */
	int levelshift = level2shift(level + 1);

	if (pte_type(pte) != PTE_TYPE_BLOCK)
		hprintf(ERROR,
			"PTE %p (%llx) is not a block. Some driver code wants to "
			"modify dcache settings for an range not covered in "
			"mem_map.\n\r",
			pte, old_pte);

	new_table = create_table();
	hprintf(TSENV, "Splitting pte %p (%llx) into %p.\n\r", pte, old_pte,
		new_table);

	for (i = 0; i < MAX_PTE_ENTRIES; i++) {
		new_table[i] = old_pte | (i << levelshift);

		/* Level 3 block PTEs have the table type */
		if ((level + 1) == 3)
			new_table[i] |= PTE_TYPE_TABLE;

		hprintf(TSENV, "Setting new_table[%lld] = %llx.\n\r", i,
			new_table[i]);
	}

	/* Set the new table into effect */
	set_pte_table(pte, new_table);
}

/* Add one mm_region map entry to the page tables */
static void add_map(struct mm_region *map)
{
	u64 *pte;
	u64 virt = map->virt;
	u64 phys = map->phys;
	u64 size = map->size;
	u64 attrs = map->attrs | PTE_TYPE_BLOCK | PTE_BLOCK_AF;
	u64 blocksize;
	int level;
	u64 *new_table = NULL;

	while (size) {
		pte = find_pte(virt, 0);
		if (pte && (pte_type(pte) == PTE_TYPE_FAULT)) {
			new_table = create_table();
			hprintf(TSENV,
				"Creating table for virt 0x%llx. new_table:%llx\n\r",
				virt, new_table);
			set_pte_table(pte, new_table);
		}

		for (level = 1; level < 4; level++) {
			pte = find_pte(virt, level);
			if (!pte)
				hprintf(ERROR, "pte not found\n");

			blocksize = 1ULL << level2shift(level);
			hprintf(TSENV,
				"Checking if pte fits for virt=%llx size=%llx blocksize=%llx.\n\r",
				virt, size, blocksize);
			if (size >= blocksize && !(virt & (blocksize - 1))) {
				/* Page fits, create block PTE */
				if (level == 3)
					*pte = phys | attrs | PTE_TYPE_PAGE;
				else
					*pte = phys | attrs;
				hprintf(TSENV,
					"Setting PTE %p(%llx) to block virt=%llx.\n\r",
					pte, *pte, virt);
				virt += blocksize;
				phys += blocksize;
				size -= blocksize;

				break;
			} else if (pte_type(pte) == PTE_TYPE_FAULT) {
				/* Page doesn't fit, create subpages */
				set_pte_table(pte, new_table);
				hprintf(TSENV,
					"Creating subtable for virt 0x%llx blksize=%llx. new_table:%llx\n\r",
					virt, blocksize, new_table);
				new_table = create_table();
			} else if (pte_type(pte) == PTE_TYPE_BLOCK) {
				hprintf(TSENV,
					"Split block into subtable for virt 0x%llx blksize=0x%llx.\n\r",
					virt, blocksize);
				split_block(pte, level);
			}
		}
	}
}

enum pte_type {
	PTE_INVAL,
	PTE_BLOCK,
	PTE_LEVEL,
};

/*
 * This is a recursively called function to count the number of
 * page tables we need to cover a particular PTE range. If you
 * call this with level = -1 you basically get the full 48 bit
 * coverage.
 */
static int count_required_pts(u64 addr, int level, u64 maxaddr)
{
	int levelshift = level2shift(level);
	u64 levelsize = 1ULL << levelshift;
	u64 levelmask = levelsize - 1;
	u64 levelend = addr + levelsize;
	int r = 0;
	int i;
	enum pte_type pte_type = PTE_INVAL;

	for (i = 0; mem_map[i].size || mem_map[i].attrs; i++) {
		struct mm_region *map = &mem_map[i];
		u64 start = map->virt;
		u64 end = start + map->size;

		/* Check if the PTE would overlap with the map */
		if (max(addr, start) <= min(levelend, end)) {
			start = max(addr, start);
			end = min(levelend, end);

			/* We need a sub-pt for this level */
			if ((start & levelmask) || (end & levelmask)) {
				pte_type = PTE_LEVEL;
				break;
			}

			/* Lv0 can not do block PTEs, so do levels here too */
			if (level <= 0) {
				pte_type = PTE_LEVEL;
				break;
			}

			/* PTE is active, but fits into a block */
			pte_type = PTE_BLOCK;
		}
	}

	/*
	 * Block PTEs at this level are already covered by the parent page
	 * table, so we only need to count sub page tables.
	 */
	if (pte_type == PTE_LEVEL) {
		int sublevel = level + 1;
		u64 sublevelsize = 1ULL << level2shift(sublevel);

		/* Account for the new sub page table ... */
		r = 1;

		/* ... and for all child page tables that one might have */
		for (i = 0; i < MAX_PTE_ENTRIES; i++) {
			r += count_required_pts(addr, sublevel, maxaddr);
			addr += sublevelsize;

			if (addr >= maxaddr) {
				/*
				 * We reached the end of address space, no need
				 * to look any further.
				 */
				break;
			}
		}
	}

	return r;
}

/* Returns the estimated required size of all page tables */
u64 get_page_table_size(void)
{
	u64 one_pt = MAX_PTE_ENTRIES * sizeof(u64);
	u64 size = 0;
	u64 va_bits;
	int start_level = 0;
	hprintf(ERROR, "%s:%d\n", __func__, __LINE__);

	get_tcr(3, NULL, &va_bits);
	if (va_bits < 39)
		start_level = 1;

	hprintf(ERROR, "%s:%d\n", __func__, __LINE__);
	/* Account for all page tables we would need to cover our memory map */
	size = one_pt * count_required_pts(0, start_level - 1, 1ULL << va_bits);
	hprintf(ERROR, "%s:%d\n", __func__, __LINE__);

	/*
	 * We need to duplicate our page table once to have an emergency pt to
	 * resort to when splitting page tables later on
	 */
	size *= 2;

	/*
	 * We may need to split page tables later on if dcache settings change,
	 * so reserve up to 4 (random pick) page tables for that.
	 */
	size += one_pt * 4;

	return size;
}

void setup_pgtables(void)
{
	int i;

	if (!a55_arch_gd.tlb_fillptr || !a55_arch_gd.tlb_addr)
		hprintf(ERROR, "Page table pointer not setup.\n");

	/*
	 * Allocate the first level we're on with invalidate entries.
	 * If the starting level is 0 (va_bits >= 39), then this is our
	 * Lv0 page table, otherwise it's the entry Lv1 page table.
	 */
	create_table();

	/* Now add all MMU table entries one after another to the table */
	for (i = 0; mem_map[i].size || mem_map[i].attrs; i++)
		add_map(&mem_map[i]);
}

//static void setup_all_pgtables(void)
//{
//	u64 tlb_addr = a55_arch_gd.tlb_addr;
//	u64 tlb_size = a55_arch_gd.tlb_size;

/* Reset the fill ptr */
//	a55_arch_gd.tlb_fillptr = tlb_addr;

/* Create normal system page tables */
//	setup_pgtables();

/* Create emergency page tables */
//	a55_arch_gd.tlb_size -= (uintptr_t)a55_arch_gd.tlb_fillptr -
//			     (uintptr_t)a55_arch_gd.tlb_addr;
//	a55_arch_gd.tlb_addr = a55_arch_gd.tlb_fillptr;
//	setup_pgtables();
//	a55_arch_gd.tlb_emerg = a55_arch_gd.tlb_addr;
//	a55_arch_gd.tlb_addr = tlb_addr;
//	a55_arch_gd.tlb_size = tlb_size;
//}

/* to activate the MMU we need to set up virtual memory */
void mmu_setup(void)
{
	/* enable the mmu */
	mmu_enable();
	return;
}

/*
 * Performs a invalidation of the entire data cache at all levels
 */
void invalidate_dcache_all(void)
{
	__asm_invalidate_dcache_all();
	__asm_invalidate_l3_dcache();
}

/*
 * Performs a clean & invalidation of the entire data cache at all levels.
 * This function needs to be inline to avoid using stack.
 * __asm_flush_l3_dcache return status of timeout
 */
static __attribute__((always_inline)) inline void flush_dcache_all(void)
{
	//	int ret;

	__asm_flush_dcache_all();
	__asm_flush_l3_dcache();
	//if (ret)
	//hprintf(TSENV,"flushing dcache returns 0x%x.\n\r", ret);
	//else
	//hprintf(TSENV,"flushing dcache successfully.\n");
}

/*
 * Invalidates range in all levels of D-cache/unified cache
 */
void invalidate_dcache_range(unsigned long start, unsigned long stop)
{
	__asm_invalidate_dcache_range(start, stop);
}

/*
 * Flush range(clean & invalidate) from all levels of D-cache/unified cache
 */
void flush_dcache_range(unsigned long start, unsigned long stop)
{
	__asm_flush_dcache_range(start, stop);
}

int dcache_status(void)
{
	return (get_sctlr() & CR_C) != 0;
}

void mmu_enable(void)
{
	unsigned int sctlr;

	sctlr = get_sctlr();

	/* if cache isn't enabled no need to disable */
	if ((sctlr & CR_M))
		return;

	set_sctlr(sctlr | (CR_M));
}

void dcache_enable(void)
{
	/* The data cache is not active unless the mmu is enabled */
	int ret;
	//	set_ttbr_el3();
	if (!(get_sctlr() & CR_M)) {
		invalidate_dcache_all();
		__asm_invalidate_tlb_all();
		mmu_setup();
	}
	set_sctlr(get_sctlr() | CR_C);
	dsb();
	ret = dcache_status();
	hprintf(TSENV, "Dcache %s.\n\r", ret ? "Enabled" : "Disabled");
}

void dcache_disable(void)
{
	unsigned int sctlr;
	int ret;

	sctlr = get_sctlr();

	/* if cache isn't enabled no need to disable */
	if (!(sctlr & CR_C))
		return;

	set_sctlr(sctlr & ~(CR_C | CR_M));
	flush_dcache_all();
	__asm_invalidate_tlb_all();
	//hprintf(ERROR,"%s:%d\n",__func__,__LINE__);
	ret = dcache_status();
	hprintf(TSENV, "Dcache %s.\n\r", ret ? "Enabled" : "Disabled");
}

int dcache_save(unsigned int *flag)
{
	unsigned int sctlr;
	sctlr = get_sctlr();

	/* if cache isn't enabled no need to disable */
	if (!(sctlr & CR_C)) {
		*flag = 0;
	} else {
		dcache_disable();
	}
	*flag = 1;
	return 0;
}

int dcache_restore(unsigned int *flag)
{
	unsigned int sctlr;
	sctlr = get_sctlr();

	/* if cache isn't enabled no need to disable */
	if (*flag) {
		dcache_enable();
	} else {
		dcache_disable();
	}
	return 0;
}

u64 *arch_get_page_table(void)
{
	puts("No page table offset defined\n\r");
	return NULL;
}

static int is_aligned(u64 addr, u64 size, u64 align)
{
	return !(addr & (align - 1)) && !(size & (align - 1));
}

/* Use flag to indicate if attrs has more than d-cache attributes */
static u64 set_one_region(u64 start, u64 size, u64 attrs, int flag, int level)
{
	int levelshift = level2shift(level);
	u64 levelsize = 1ULL << levelshift;
	u64 *pte = find_pte(start, level);

	/* Can we can just modify the current level block PTE? */
	if (is_aligned(start, size, levelsize)) {
		if (flag) {
			*pte &= ~PMD_ATTRMASK;
			*pte |= attrs & PMD_ATTRMASK;
		} else {
			*pte &= ~PMD_ATTRINDX_MASK;
			*pte |= attrs & PMD_ATTRINDX_MASK;
		}
		hprintf(TSENV, "Set attrs=%llx pte=%p level=%d.\n\r", attrs,
			pte, level);

		return levelsize;
	}

	/* Unaligned or doesn't fit, maybe split block into table */
	hprintf(TSENV, "addr=%llx level=%d pte=%p (%llx).\n\r", start, level,
		pte, *pte);

	/* Maybe we need to split the block into a table */
	if (pte_type(pte) == PTE_TYPE_BLOCK)
		split_block(pte, level);

	/* And then double-check it became a table or already is one */
	if (pte_type(pte) != PTE_TYPE_TABLE)
		hprintf(ERROR,
			"PTE %p (%llx) for addr=%llx should be a table\n\r",
			pte, *pte, start);

	/* Roll on to the next page table level */
	return 0;
}

void mmu_set_region_dcache_behaviour(phys_addr_t start, size_t size,
				     enum dcache_option option)
{
	u64 attrs = PMD_ATTRINDX(option);
	u64 real_start = start;
	u64 real_size = size;

	hprintf(TSENV, "start=%lx size=%lx.\n\r", (unsigned long)start,
		(unsigned long)size);

	if (!a55_arch_gd.tlb_emerg)
		hprintf(ERROR, "Emergency page table not setup.\n\r");

	/*
	 * We can not modify page tables that we're currently running on,
	 * so we first need to switch to the "emergency" page tables where
	 * we can safely modify our primary page tables and then switch back
	 */
	__asm_switch_ttbr(a55_arch_gd.tlb_emerg);

	/*
	 * Loop through the address range until we find a page granule that fits
	 * our alignment constraints, then set it to the new cache attributes
	 */
	while (size > 0) {
		int level;
		u64 r;

		for (level = 1; level < 4; level++) {
			/* Set d-cache attributes only */
			r = set_one_region(start, size, attrs, 0, level);
			if (r) {
				/* PTE successfully replaced */
				size -= r;
				start += r;
				break;
			}
		}
	}

	/* We're done modifying page tables, switch back to our primary ones */
	__asm_switch_ttbr(a55_arch_gd.tlb_addr);

	/*
	 * Make sure there's nothing stale in dcache for a region that might
	 * have caches off now
	 */
	flush_dcache_range(real_start, real_start + real_size);
}

/*
 * Modify MMU table for a region with updated PXN/UXN/Memory type/valid bits.
 * The procecess is break-before-make. The target region will be marked as
 * invalid during the process of changing.
 */
void mmu_change_region_attr(phys_addr_t addr, size_t siz, u64 attrs)
{
	int level;
	u64 r, size, start;

	start = addr;
	size = siz;
	/*
	 * Loop through the address range until we find a page granule that fits
	 * our alignment constraints, then set it to "invalid".
	 */
	while (size > 0) {
		for (level = 1; level < 4; level++) {
			/* Set PTE to fault */
			r = set_one_region(start, size, PTE_TYPE_FAULT, 1,
					   level);
			if (r) {
				/* PTE successfully invalidated */
				size -= r;
				start += r;
				break;
			}
		}
	}

	flush_dcache_range(a55_arch_gd.tlb_addr,
			   a55_arch_gd.tlb_addr + a55_arch_gd.tlb_size);
	__asm_invalidate_tlb_all();

	/*
	 * Loop through the address range until we find a page granule that fits
	 * our alignment constraints, then set it to the new cache attributes
	 */
	start = addr;
	size = siz;
	while (size > 0) {
		for (level = 1; level < 4; level++) {
			/* Set PTE to new attributes */
			r = set_one_region(start, size, attrs, 1, level);
			if (r) {
				/* PTE successfully updated */
				size -= r;
				start += r;
				break;
			}
		}
	}
	flush_dcache_range(a55_arch_gd.tlb_addr,
			   a55_arch_gd.tlb_addr + a55_arch_gd.tlb_size);
	__asm_invalidate_tlb_all();
}

int icache_status(void)
{
	return (get_sctlr() & CR_I) != 0;
}

void invalidate_icache_all(void)
{
	__asm_invalidate_icache_all();
	__asm_invalidate_l3_icache();
}

void icache_enable(void)
{
	int ret;
	if (!(get_sctlr() & CR_M)) {
		invalidate_dcache_all();
		__asm_invalidate_tlb_all();
		mmu_setup();
	}
	invalidate_icache_all();
	set_sctlr(get_sctlr() | CR_I);
	ret = icache_status();
	hprintf(TSENV, "Icache %s.\n\r", ret ? "Enabled" : "Disabled");
}

void icache_disable(void)
{
	int ret;
	invalidate_icache_all();
	set_sctlr(get_sctlr() & ~CR_I);
	ret = icache_status();
	hprintf(TSENV, "Icache %s.\n\r", ret ? "Enabled" : "Disabled");
}

static int a55_icache_enable_test(zebu_test_mod_t *mod, void *para)
{
	icache_enable();
	return 0;
}
static int a55_icache_disable_test(zebu_test_mod_t *mod, void *para)
{
	icache_disable();
	return 0;
}

static int a55_dcache_enable_test(zebu_test_mod_t *mod, void *para)
{
	dcache_enable();
	return 0;
}

static int a55_dcache_disable_test(zebu_test_mod_t *mod, void *para)
{
	dcache_disable();
	return 0;
}
static int cget_percent(int val, int max)
{
	if (max > 0xffff) {
		val >>= 4;
		max >>= 4;
	}
	return val / (max / 100);
}

/*
 * jump test
 */
static void __attribute__((section(".jump1"))) a55_icache_jump1(void)
{
	asm("nop");
}

static void __attribute__((section(".jump2"))) a55_icache_jump2(void)
{
	asm("nop");
}

static void __attribute__((section(".jump3"))) a55_icache_jump3(void)
{
	asm("nop");
}

static void a55_icache_dot_(void)
{
	//asm("nop");
	//asm("nop");
	//asm("nop");
	a55_icache_jump1();
	a55_icache_jump3();
	a55_icache_jump2();
	return;
}

static int a55_icache_do_test(void)
{
#define ICACHE_TEST_CNT 40000
	int percent = 0, temp_percent;
	int i;
	hprintf(TSENV, "\n\r");
	for (i = 0; i < ICACHE_TEST_CNT; i++) {
		if (!(i % (ICACHE_TEST_CNT / 20))) {
			temp_percent = cget_percent(i, ICACHE_TEST_CNT);
			if (temp_percent != percent) {
				percent = temp_percent;
				hprintf(TSENV, "\r %d%%   ", percent);
			}
		}
		a55_icache_dot_();
	}
	hprintf(TSENV, "\n\r");
	return 0;
}

static int a55_dcache_do_test(void)
{
#define DCACHE_TEST_CNT 0x1000
	char temp_b[DCACHE_TEST_CNT] = { 0 };
	char temp_b1[DCACHE_TEST_CNT] = { 0 };
	unsigned long long l;
	for (l = 0; l < DCACHE_TEST_CNT; l++) {
		temp_b[l] = (l & 1) ? (l & 0xff) : (~l & 0xff);
	}
	for (l = 0; l < DCACHE_TEST_CNT; l++) {
		temp_b1[l] = temp_b[l];
	}

	for (l = 0; l < DCACHE_TEST_CNT; l++) {
		temp_b[l] = (l & 1) ? (~l & 0xff) : (l & 0xff);
	}
	for (l = 0; l < DCACHE_TEST_CNT; l++) {
		temp_b1[l] = temp_b[l];
	}
	return 1;
}

static int a55_icache_test(zebu_test_mod_t *mod, void *para)
{
	unsigned long long dttime_disable;
	unsigned long long dttime_enable;
	unsigned long long time_start;
	unsigned long long time_end;
	int dcache_stat;
	dcache_save(&dcache_stat);
	hprintf(TSENV, "[%s]: Start DO Test: Icache Disable\n\r", __func__);
	icache_disable();
	time_start = get_ticks();
	a55_icache_do_test();
	time_end = get_ticks();
	dttime_disable = time_end - time_start;

	hprintf(TSENV, "[%s]: Start DO Test: Icache Enable\n\r", __func__);
	icache_enable();
	time_start = get_ticks();
	a55_icache_do_test();
	time_end = get_ticks();
	dttime_enable = time_end - time_start;

	//icache_disable();
	hprintf(TSENV, "[%s]: Time Dis:%u,en:%u\n\r", __func__, dttime_disable,
		dttime_enable);
	if (dttime_enable >= dttime_disable) {
		dcache_restore(&dcache_stat);
		hprintf(TSENV, "[%s]: Test Failed. %u >= %u\n\r", __func__,
			dttime_enable, dttime_disable);
		return 1;
	}
	dcache_restore(&dcache_stat);
	hprintf(TSENV, "[%s]: Test OK. %u < %u\n\r", __func__, dttime_enable,
		dttime_disable);
	return 0;
}

static int a55_dcache_test(zebu_test_mod_t *mod, void *para)
{
	unsigned long long dttime_disable;
	unsigned long long dttime_enable;
	unsigned long long time_start;
	unsigned long long time_end;

	hprintf(TSENV, "[%s]: Start DO Dcache Disable\n\r", __func__);
	dcache_disable();
	//mmu_enable();
	time_start = get_ticks();
	a55_dcache_do_test();
	time_end = get_ticks();
	dttime_disable = time_end - time_start;

	hprintf(TSENV, "[%s]: Start DO Dcache Enable\n\r", __func__);
	dcache_enable();
	//mmu_enable();
	time_start = get_ticks();
	a55_dcache_do_test();
	time_end = get_ticks();
	dttime_enable = time_end - time_start;
	//dcache_disable();
	//mmu_enable();
	hprintf(TSENV, "[%s]: Time Dis:%u,en:%u\n\r", __func__, dttime_disable,
		dttime_enable);
	if (dttime_enable >= dttime_disable) {
		hprintf(TSENV, "[%s]: Test Failed. %u < %u\n\r", __func__,
			dttime_disable, dttime_enable);
		return 1;
	}
	hprintf(TSENV, "[%s]: Test OK. %u >= %u\n\r", __func__, dttime_disable,
		dttime_enable);
	return 0;
}

static int a55_mmu_do_test(unsigned long long vir_addr, unsigned long long size)
{
	unsigned long temp_b[A55_MMU_TEST_SIZE];
	unsigned char *test_8p, *test_8vp;
	unsigned short *test_16p, *test_16vp;
	unsigned int *test_32p, *test_32vp;
	unsigned long *test_64p, *test_64vp;
	unsigned long long l;

	//RW 8Bit Test.
	//start Write a Value.after read the value and compare.
	test_8p = (char *)temp_b;
	test_8vp = (char *)vir_addr;
	for (l = 0; l < A55_MMU_TEST_SIZE; l++) {
		test_8p[l] = (l & 1) ? (l & 0xff) : (~l & 0xff);
		test_8vp[l] = (l & 1) ? (l & 0xff) : (~l & 0xff);
	}
	test_8p = (char *)vir_addr;
	for (l = 0; l < A55_MMU_TEST_SIZE; l++) {
		if (test_8p[l] != test_8vp[l]) {
			hprintf(ERROR, "[%s]Test Failed.In 8bit RW.\n\r",
				__func__);
			return 1;
		}
	}

	//RW 16Bit Test.
	//start Write a Value.after read the value and compare.
	test_16p = (unsigned short *)temp_b;
	test_16vp = (unsigned short *)vir_addr;
	for (l = 0; l < A55_MMU_TEST_SIZE / 2; l++) {
		test_16p[l] = (l & 1) ? (l & 0xffFF) : (~l & 0xfFFf);
		test_16vp[l] = (l & 1) ? (l & 0xffFF) : (~l & 0xffFF);
	}
	for (l = 0; l < A55_MMU_TEST_SIZE / 2; l++) {
		if (test_16p[l] != test_16vp[l]) {
			hprintf(ERROR, "[%s]Test Failed.In 16bit RW.\n\r",
				__func__);
			return 1;
		}
	}

	//RW 32Bit Test.
	//start Write a Value.after read the value and compare.
	test_32p = (unsigned int *)temp_b;
	test_32vp = (unsigned int *)vir_addr;
	for (l = 0; l < A55_MMU_TEST_SIZE / 4; l++) {
		test_32p[l] = (l & 1) ? (l & 0xffFFffff) : (~l & 0xfFFfffff);
		test_32vp[l] = (l & 1) ? (l & 0xffFFffff) : (~l & 0xffFFffff);
	}
	for (l = 0; l < A55_MMU_TEST_SIZE / 4; l++) {
		if (test_32p[l] != test_32vp[l]) {
			hprintf(ERROR, "[%s]Test Failed.In 32bit RW.\n\r",
				__func__);
			return 1;
		}
	}

	//RW 64Bit Test.
	//start Write a Value.after read the value and compare.
	test_64p = (unsigned long *)temp_b;
	test_64vp = (unsigned long *)vir_addr;
	for (l = 0; l < A55_MMU_TEST_SIZE / 8; l++) {
		test_64p[l] = (l & 1) ? (l) : (~l);
		test_64vp[l] = (l & 1) ? (l) : (~l);
	}
	for (l = 0; l < A55_MMU_TEST_SIZE / 8; l++) {
		if (test_64p[l] != test_64vp[l]) {
			hprintf(ERROR, "[%s]Test Failed.In 64bit RW.\n\r",
				__func__);
			return 1;
		}
	}
	hprintf(TSENV, "[%s]Test ok.In 8&16&32&64bit RW.\n\r", __func__);
	return 0;
}

static int a55_mmu_test(zebu_test_mod_t *mod, void *para)
{
	dcache_enable();
	return a55_mmu_do_test(A55_MMU_TEST_ADDR, A55_MMU_TEST_SIZE);
}

static int a55_dcache_flush_test(zebu_test_mod_t *mod, void *para)
{
	char *addr = (char *)0xa0000000;
	int loop;
	static int cnt = 0;
	int i;
	for (loop = 0; loop < 16; loop++) {
		if (cnt & 1) {
			for (i = 0; i < 0x10004; i++) {
				*(addr + i) = (i & 1) ? (unsigned char)(i) :
							(unsigned char)(~i);
			}
		} else {
			for (i = 0; i < 0x10004; i++) {
				*(addr + i) = (i & 1) ? (unsigned char)(~i) :
							(unsigned char)(i);
			}
		}
		flush_dcache_all();
		//asm volatile("dsb sy");
		hprintf(TSENV, "loop:%d.\n\r", loop);
	}
	return 0;
}

zebu_test_mod_t zebu_test_a55_cache_mods[] = {
	{ "A55CACHE", 0 },
};

zebu_test_entry_t zebu_test_a55_cache_entrys[] = {
	{ '1', 1, "a55 I-cache test", a55_icache_test },
	{ '2', 1, "a55 D-cache test", a55_dcache_test },
	{ '3', 0, "a55 I-cache enable ", a55_icache_enable_test },
	{ '4', 0, "a55 D-cache enable ", a55_dcache_enable_test },
	{ '5', 0, "a55 I-cache disable ", a55_icache_disable_test },
	{ '6', 0, "a55 D-cache disable ", a55_dcache_disable_test },
	{ '7', 1, "a55 MMU test& 8/16/32/64Bit RW", a55_mmu_test },
	{ '8', 1, "a55_dcache_flush_test", a55_dcache_flush_test },
};

static zebu_test_ctrl_t zebu_mod_test_ctrl = {
	"A55 CACHE",
	SIZE_ARRAY(zebu_test_a55_cache_mods),
	zebu_test_a55_cache_mods,
	SIZE_ARRAY(zebu_test_a55_cache_entrys),
	zebu_test_a55_cache_entrys,
	0
};

void a55_cache_test_main(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;

	//dcache_disable();
	//icache_disable();
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "auto test err %d!!!!.\n\r", ret);

	} else {
		hprintf(TSENV, "enter A55 cache test!\r\n");
		TestLoop(&zebu_mod_test_ctrl);
	}
}

CMD_DEFINE(cache, a55_cache_test_main, 0, "A55 Cache test", CMD_ATTR_AUTO_TEST);
