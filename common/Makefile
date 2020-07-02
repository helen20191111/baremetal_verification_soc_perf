#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

# ISO C90 forbids variable length array [-Wvla]
CFLAGS_command.o := -Wno-vla
obj-y := cli.o command.o
obj-$(CONFIG_CMDLINE) += cli_readline.o cli_simple.o
