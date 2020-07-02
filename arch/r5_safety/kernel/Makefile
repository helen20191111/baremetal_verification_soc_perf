#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

CPPFLAGS_$(project).lds := -DHEADDIR=arch/r5_safety/kernel

obj-y := gic_basic.o safety.o

extra-y := init_f.o head.o $(project).lds

