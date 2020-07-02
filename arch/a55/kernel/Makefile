#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

CPPFLAGS_$(project).lds := -DHEADDIR=arch/a55/kernel

obj-y += hs_cpu_a55.o interrupt.o giccpu.o cpu.o cachev8.o cache.o a55.o
ifndef CONFIG_APP_FLASH_SRAM
obj-y += pagetables.o
endif

extra-y := bootcode.o vectors.o $(project).lds
extra-$(CONFIG_APP_FLASH_SRAM) += bootcodecpy.o pagetables.o

