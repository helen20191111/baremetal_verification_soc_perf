#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

LDFLAGS_$(project) := -p --no-undefined -X --pic-veneer
KBUILD_CFLAGS	+= $(call cc-option,-fno-dwarf2-cfi-asm)

KBUILD_DEFCONFIG := defconfig

ifeq ($(CONFIG_FRAME_POINTER),y)
KBUILD_CFLAGS	+=-fno-omit-frame-pointer -mapcs -mno-sched-prolog
endif

ifeq ($(CONFIG_CPU_BIG_ENDIAN),y)
KBUILD_CPPFLAGS	+= -mbig-endian
CHECKFLAGS	+= -D__ARMEB__
AS		+= -EB
KBUILD_LDFLAGS	+= -EB
else
KBUILD_CPPFLAGS	+= -mlittle-endian
CHECKFLAGS	+= -D__ARMEL__
AS		+= -EL
KBUILD_LDFLAGS	+= -EL
endif


#
## The Scalar Replacement of Aggregates (SRA) optimization pass in GCC 4.9 and
# later may result in code being generated that handles signed short and signed
# # char struct members incorrectly. So disable it.
# # (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65932)
# #
KBUILD_CFLAGS	+= $(call cc-option,-fno-ipa-sra)
KBUILD_CFLAGS   += -march=armv7-r

# Accept old syntax despite ".syntax unified"
AFLAGS_NOWARN	:=$(call as-option,-Wa$(comma)-mno-warn-deprecated,-Wa$(comma)-W)

CFLAGS_ISA	:=$(call cc-option,-marm,) $(AFLAGS_NOWARN)
AFLAGS_ISA	:=$(CFLAGS_ISA)

# Need -Uarm for gcc < 3.x
KBUILD_CFLAGS	+=$(CFLAGS_ISA) $(call cc-option,-mshort-load-bytes,$(call cc-option,-malignment-traps,)) -msoft-float -Uarm
KBUILD_AFLAGS	+=$(AFLAGS_ISA) $(arch-y) -msoft-float

# cancel built-in func
KBUILD_CFLAGS   += -fno-builtin

# arm-none-eabi- Cross-platform problem
ifeq ($(CROSS_COMPILE), arm-none-eabi-)
ARCH_CFLAGS += -fno-short-wchar 
endif

CHECKFLAGS	+= -D__arm__


head-y := arch/r5_safety/kernel/head.o
head-y += arch/r5_safety/kernel/init_f.o

core-y += arch/r5_safety/kernel/

boot := arch/r5_safety/boot

PHONY += Ecc_Safety.raw

all: Ecc_Safety.raw

KBUILD_IMAGE := $(boot)/Ecc_Safety.raw

Ecc_Safety.raw: $(project)
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@

archscripts: scripts_basic

archheaders:
	
archprepare:

archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

define archhelp
	echo  '* Ecc_Safety.raw   - kernel image (arch/$(ARCH)/boot/Ecc_Safety.raw)'
endef

