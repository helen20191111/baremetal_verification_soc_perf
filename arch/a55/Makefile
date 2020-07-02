#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

LDFLAGS_$(project) := --no-undefined -X

KBUILD_DEFCONFIG := defconfig

KBUILD_CFLAGS	+= -mgeneral-regs-only $(lseinstr) $(brokengasinst)
KBUILD_CFLAGS	+= -fno-asynchronous-unwind-tables

KBUILD_CFLAGS	+= $(call cc-option,-mabi=lp64)
KBUILD_AFLAGS	+= $(call cc-option,-mabi=lp64)

ifeq ($(CONFIG_CPU_BIG_ENDIAN), y)
KBUILD_CPPFLAGS	+= -mbig-endian
CHECKFLAGS	+= -D__AARCH64EB__
AS		+= -EB
# Prefer the baremetal ELF build target, but not all toolchains include
# # it so fall back to the standard linux version if needed.
KBUILD_LDFLAGS	+= -EB $(call ld-option, -maarch64elfb, -maarch64linuxb)
UTS_MACHINE	:= aarch64_be
else
KBUILD_CPPFLAGS	+= -mlittle-endian
CHECKFLAGS	+= -D__AARCH64EL__
AS		+= -EL
# # Same as above, prefer ELF but fall back to linux target if needed.
KBUILD_LDFLAGS	+= -EL $(call ld-option, -maarch64elf, -maarch64linux)
UTS_MACHINE	:= aarch64
endif

# cancel built-in func
KBUILD_CFLAGS   += -fno-builtin

CHECKFLAGS	+= -D__aarch64__
ARCH_CFLAGS += -DAARCH64

head-y :=
ifdef CONFIG_APP_FLASH_SRAM
head-y += arch/a55/kernel/bootcodecpy.o
head-y += arch/a55/kernel/pagetables.o
endif
head-y := arch/a55/kernel/bootcode.o
head-y += arch/a55/kernel/vectors.o

core-y += arch/a55/kernel/

boot := arch/a55/boot

PHONY += a55_a.raw a55_b.raw

all: a55_a.raw a55_b.raw

KBUILD_IMAGE := $(boot)/a55_a.raw $(boot)/a55_b.raw

a55_a.raw a55_b.raw: $(project)
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@

archscripts: scripts_basic

archheaders:

archprepare:

archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

define archhelp
	echo  '* a55_a/b.raw   - kernel image (arch/$(ARCH)/boot/a55_a/b.raw)'
endef

