#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

OBJCOPYFLAGS := -O binary -R .note.gnu.build-id -R .comment -S

targets := Ecc_Safety.raw Safety.lst Safety.raw r5_safety_mkRawEcc $(project) System.map

tools := arch/r5_safety/tools
mkEcc_tools := $(tools)/mkRawEcc

# Debug maybe need lst file
OBJDUMPFLAGS_Safety.lst := -D -x -f

# mkEcc tools use
quiet_cmd_mkEcc = MKECC   $@
	  cmd_mkEcc  = $(CONFIG_SHELL) $(srctree)/arch/$(ARCH)/tools/mkRawEcc.sh $< $@

$(obj)/Safety.raw:  $(project) $(obj)/Safety.lst FORCE
	$(call if_changed,objcopy)

$(obj)/Safety.lst: $(project) FORCE
	$(call if_changed,objdump)
	@cp $(project) arch/r5_safety/boot/
	@cp System.map arch/r5_safety/boot/

$(obj)/Ecc_Safety.raw: $(obj)/Safety.raw $(mkEcc_tools) FORCE
	$(call if_changed,mkEcc)

