/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __CACHE_H__
#define __CACHE_H__

extern void arch_disable_cache(unsigned int flags);
extern void set_ttbr_el3(void);

typedef unsigned long uintptr_t;
typedef struct a55_arch_gd_s {
	unsigned long tlb_addr;
	unsigned long tlb_size;
	unsigned long tlb_fillptr;
	unsigned long tlb_emerg;
} a55_arch_gd_t;

#endif
