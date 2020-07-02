#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

lib-y := printf.o
lib-y += vsprintf.o
lib-y += string.o
lib-y += ctype.o
lib-y += utility_lite.o
lib-y += test_common.o
lib-y += printk.o
lib-y += errno.o
lib-y += display_options.o
lib-$(CONFIG_USE_TTY_IO) += tty_io.o
lib-$(CONFIG_UART3_SYNC) += uart_sync.o
lib-$(CONFIG_ERRNO_STR) += errno_str.o
