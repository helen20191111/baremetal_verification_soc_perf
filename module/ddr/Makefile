#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

ifeq ($(CONFIG_EVB),y)
obj-y := a1000_clk.o a1000_ddr.o ddr.o memtester.o
obj-$(CONFIG_DDR_1066) += config_1066.o lpddr4_training_init_sequence_1066.o

ifeq ($(CONFIG_DDR_3200), y)
obj-y += config_3200.o
ifeq ($(CONFIG_DDR_QOS_3200), y)
obj-y += lpddr4_training_init_sequence_qos_3200.o
else
obj-y += lpddr4_training_init_sequence_3200.o
endif
endif

ifeq ($(CONFIG_DDR_3200_2D), y)
obj-y += config_2d_3200.o
ifeq ($(CONFIG_DDR_QOS_3200), y)
obj-y += lpddr4_training_init_sequence_qos_3200.o
else
obj-y += lpddr4_training_init_sequence_3200.o
endif
endif
else
obj-y := ddr.o memtester.o
endif

