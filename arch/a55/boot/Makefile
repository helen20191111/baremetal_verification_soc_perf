#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

OBJCOPYFLAGS_a55.raw := -O binary -R .comment -S
OBJCOPYFLAGS_a55_a.raw := -b 0 -i 4 --interleave-width 2 -I binary
OBJCOPYFLAGS_a55_b.raw := -b 2 -i 4 --interleave-width 2 -I binary

targets := a55_a.raw a55_b.raw a55.raw a55.lst $(project) System.map

# Debug maybe need lst file
OBJDUMPFLAGS_a55.lst := -D -x -f

$(obj)/a55.raw:  $(project) $(obj)/a55.lst FORCE
	$(call if_changed,objcopy)

$(obj)/a55_a.raw: $(obj)/a55.raw FORCE
	$(call if_changed,objcopy)

$(obj)/a55_b.raw: $(obj)/a55.raw FORCE
	$(call if_changed,objcopy)

$(obj)/a55.lst: $(project) FORCE
	$(call if_changed,objdump)
	@cp $(project) arch/a55/boot/
	@cp System.map arch/a55/boot/

