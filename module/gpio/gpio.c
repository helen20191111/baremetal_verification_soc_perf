/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <utility_lite.h>
#include <test_common.h>
#include <common.h>
#include <uart_sync.h>
#include <ut_test.h>
#include <int_num.h>
#include <module/gpio.h>
#include <module/a55_timer.h>

/*
 * function prototypes
 */
void dw_init_example(void);
u32 gpio_data_source_func(unsigned int base);
u32 gpio_data_direction_func(unsigned int base);
u32 gpio_data_transport_input_func(unsigned int base);
u32 gpio_data_func(unsigned int dbase);

volatile bool_enum inter_flag = HVTE_FALSE;

gpio_pinmux_info gpios_pinmux_info[151] = {
	PINMUX_INFO(Gpio_0, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 10, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_1, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 11, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_2, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 12, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_3, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 13, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_4, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 14, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_5, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 15, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_6, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 16, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_7, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 17, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_8, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 19, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_9, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 19, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_10, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 20, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_11, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 21, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_12, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 22, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_13, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 23, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_14, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 24, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_15, 0, 0, 0, 0, HVTE_FALSE),

	PINMUX_INFO(Gpio_16, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 25, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_17, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 26, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_18, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 27, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_19, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 28, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_20, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 29, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_21, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 30, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_22, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 31, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_23, 1, TOP_PMM_REG_BASE_ADDR + 0x8, 0, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_24, 0, AON_PMM_REG_BASE_ADDR, 0, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_25, 0, AON_PMM_REG_BASE_ADDR, 1, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_26, 0, AON_PMM_REG_BASE_ADDR, 2, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_27, 0, AON_PMM_REG_BASE_ADDR, 3, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_28, 0, AON_PMM_REG_BASE_ADDR, 4, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_29, 0, AON_PMM_REG_BASE_ADDR, 5, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_30, 0, AON_PMM_REG_BASE_ADDR, 6, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_31, 0, AON_PMM_REG_BASE_ADDR, 7, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_32, 1, AON_PMM_REG_BASE_ADDR, 8, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_33, 1, AON_PMM_REG_BASE_ADDR, 9, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_34, 1, AON_PMM_REG_BASE_ADDR, 10, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_35, 1, AON_PMM_REG_BASE_ADDR, 11, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_36, 1, AON_PMM_REG_BASE_ADDR, 12, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_37, 1, AON_PMM_REG_BASE_ADDR, 13, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_38, 1, AON_PMM_REG_BASE_ADDR, 14, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_39, 1, AON_PMM_REG_BASE_ADDR, 15, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_40, 1, AON_PMM_REG_BASE_ADDR, 16, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_41, 1, AON_PMM_REG_BASE_ADDR, 17, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_42, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_43, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_44, 0, 0, 0, 0, HVTE_FALSE),

	PINMUX_INFO(Gpio_45, 1, AON_PMM_REG_BASE_ADDR, 18, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_46, 1, AON_PMM_REG_BASE_ADDR, 19, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_47, 1, AON_PMM_REG_BASE_ADDR, 24, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_48, 1, AON_PMM_REG_BASE_ADDR, 25, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_49, 1, AON_PMM_REG_BASE_ADDR, 28, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_50, 1, AON_PMM_REG_BASE_ADDR, 29, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_51, 1, AON_PMM_REG_BASE_ADDR, 30, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_52, 1, AON_PMM_REG_BASE_ADDR, 31, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_53, 1, AON_PMM_REG_BASE_ADDR + 0x4, 0, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_54, 1, AON_PMM_REG_BASE_ADDR + 0x4, 1, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_55, 1, AON_PMM_REG_BASE_ADDR + 0x4, 2, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_56, 1, AON_PMM_REG_BASE_ADDR + 0x4, 3, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_57, 1, AON_PMM_REG_BASE_ADDR + 0x4, 4, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_58, 1, AON_PMM_REG_BASE_ADDR + 0x4, 5, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_59, 1, AON_PMM_REG_BASE_ADDR + 0x4, 6, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_60, 1, AON_PMM_REG_BASE_ADDR + 0x4, 7, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_61, 1, AON_PMM_REG_BASE_ADDR + 0x4, 8, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_62, 0, 0, 0, 0, HVTE_FALSE),

	PINMUX_INFO(Gpio_63, 1, AON_PMM_REG_BASE_ADDR + 0x4, 10, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_64, 1, AON_PMM_REG_BASE_ADDR, 26, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_65, 1, AON_PMM_REG_BASE_ADDR + 0x4, 12, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_66, 1, AON_PMM_REG_BASE_ADDR, 21, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_67, 1, AON_PMM_REG_BASE_ADDR + 0x4, 14, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_68, 1, AON_PMM_REG_BASE_ADDR, 27, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_69, 1, AON_PMM_REG_BASE_ADDR + 0x4, 16, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_70, 1, AON_PMM_REG_BASE_ADDR, 22, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_71, 1, AON_PMM_REG_BASE_ADDR, 23, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_72, 1, AON_PMM_REG_BASE_ADDR + 0x4, 19, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_73, 1, AON_PMM_REG_BASE_ADDR + 0x4, 20, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_74, 1, AON_PMM_REG_BASE_ADDR + 0x4, 21, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_75, 1, AON_PMM_REG_BASE_ADDR + 0x4, 22, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_76, 1, AON_PMM_REG_BASE_ADDR + 0x4, 23, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_77, 1, AON_PMM_REG_BASE_ADDR + 0x4, 24, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_78, 1, AON_PMM_REG_BASE_ADDR + 0x4, 25, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_79, 1, AON_PMM_REG_BASE_ADDR + 0x4, 26, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_80, 1, AON_PMM_REG_BASE_ADDR + 0x4, 27, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_81, 1, AON_PMM_REG_BASE_ADDR + 0x4, 28, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_82, 1, AON_PMM_REG_BASE_ADDR + 0x4, 29, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_83, 1, AON_PMM_REG_BASE_ADDR + 0x4, 30, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_84, 1, AON_PMM_REG_BASE_ADDR + 0x4, 31, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_85, 1, AON_PMM_REG_BASE_ADDR + 0x8, 0, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_86, 1, AON_PMM_REG_BASE_ADDR + 0x8, 1, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_87, 1, AON_PMM_REG_BASE_ADDR + 0x8, 2, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_88, 1, AON_PMM_REG_BASE_ADDR + 0x8, 3, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_89, 1, AON_PMM_REG_BASE_ADDR + 0x8, 4, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_90, 1, AON_PMM_REG_BASE_ADDR + 0x8, 5, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_91, 1, AON_PMM_REG_BASE_ADDR + 0x8, 6, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_92, 1, AON_PMM_REG_BASE_ADDR + 0x8, 7, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_93, 1, AON_PMM_REG_BASE_ADDR + 0x8, 8, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_94, 1, AON_PMM_REG_BASE_ADDR + 0x8, 9, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_95, 1, AON_PMM_REG_BASE_ADDR + 0x8, 10, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_96, 1, AON_PMM_REG_BASE_ADDR + 0x8, 11, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_97, 1, AON_PMM_REG_BASE_ADDR + 0x8, 12, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_98, 1, AON_PMM_REG_BASE_ADDR + 0x8, 13, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_99, 1, AON_PMM_REG_BASE_ADDR + 0x8, 14, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_100, 1, AON_PMM_REG_BASE_ADDR + 0x8, 15, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_101, 1, AON_PMM_REG_BASE_ADDR + 0x8, 16, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_102, 1, AON_PMM_REG_BASE_ADDR + 0x8, 17, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_103, 1, AON_PMM_REG_BASE_ADDR + 0x8, 18, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_104, 1, AON_PMM_REG_BASE_ADDR + 0x8, 19, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_105, 1, AON_PMM_REG_BASE_ADDR + 0x8, 20, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_106, 1, AON_PMM_REG_BASE_ADDR + 0x8, 21, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_107, 0, AON_PMM_REG_BASE_ADDR + 0x8, 22, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_108, 0, AON_PMM_REG_BASE_ADDR + 0x8, 23, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_109, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_110, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_111, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_112, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_113, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_114, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_115, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_116, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_117, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_118, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_119, 0, 0, 0, 0, HVTE_FALSE),

	PINMUX_INFO(Gpio_120, 1, TOP_PMM_REG_BASE_ADDR, 0, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_121, 1, TOP_PMM_REG_BASE_ADDR, 1, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_122, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_123, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_124, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_125, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_126, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_127, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_128, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_129, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_130, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_131, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_132, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_133, 0, 0, 0, 0, HVTE_FALSE),
	PINMUX_INFO(Gpio_134, 0, 0, 0, 0, HVTE_FALSE),

	PINMUX_INFO(Gpio_135, 1, TOP_PMM_REG_BASE_ADDR, 2, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_136, 1, TOP_PMM_REG_BASE_ADDR, 3, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_137, 1, TOP_PMM_REG_BASE_ADDR, 4, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_138, 1, TOP_PMM_REG_BASE_ADDR, 5, 0x1, HVTE_TRUE),

	PINMUX_INFO(Gpio_139, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 0, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_140, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 1, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_141, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 2, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_142, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 3, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_143, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 4, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_144, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 5, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_145, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 6, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_146, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 7, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_147, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 8, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_148, 1, TOP_PMM_REG_BASE_ADDR + 0x4, 9, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_149, 1, TOP_PMM_REG_BASE_ADDR + 0x8, 1, 0x1, HVTE_TRUE),
	PINMUX_INFO(Gpio_150, 1, TOP_PMM_REG_BASE_ADDR + 0x8, 2, 0x1, HVTE_TRUE),
};

gpio_group_info gpio_group_0 = { GPIO0_BASE_ADDR, 0, 128 };
gpio_group_info gpio_group_1 = { GPIO1_BASE_ADDR, 128, 22 };

#if 0
static void delay(unsigned long times)
{
    int i;

    for (i = 0; i < times; i++);
}
#endif

/* Get pinmux information about a GPIO 
 * param   gpio_id : Gpio id number
 *         gpio_pinmux_info_one : Store GPIO pinmux information
 */
void get_gpio_pinmux_info_one(u32 gpio_id,
			      gpio_pinmux_info *gpio_pinmux_info_one)
{
	gpio_pinmux_info_one->gpio_id = gpios_pinmux_info[gpio_id].gpio_id;
	gpio_pinmux_info_one->gpio_flag = gpios_pinmux_info[gpio_id].gpio_flag;
	gpio_pinmux_info_one->pinmux_addr =
		gpios_pinmux_info[gpio_id].pinmux_addr;
	gpio_pinmux_info_one->pinmux_offset =
		gpios_pinmux_info[gpio_id].pinmux_offset;
	gpio_pinmux_info_one->pinmux_mask =
		gpios_pinmux_info[gpio_id].pinmux_mask;
	gpio_pinmux_info_one->attr = gpios_pinmux_info[gpio_id].attr;
}

/* Get pin information about a GPIO 
 * param   gpio_id : Gpio id number
 *         gpio_pinmux_info_one : Store GPIO pinmux information
 */
void get_gpio_pin_info_one(u32 gpio_id, gpio_pin_info *gpio_pin_info_one)
{
	if (gpio_id < gpio_group_1.num_offset) {
		gpio_pin_info_one->gpio_base_addr = gpio_group_0.base_addr;
		gpio_pin_info_one->gpio_port = gpio_id / 32;
		gpio_pin_info_one->gpio_pin = gpio_id % 32;

	} else {
		gpio_pin_info_one->gpio_base_addr = gpio_group_1.base_addr;
		gpio_pin_info_one->gpio_port =
			(gpio_id - gpio_group_0.io_nums) / 32;
		gpio_pin_info_one->gpio_pin =
			(gpio_id - gpio_group_0.io_nums) % 32;
	}
}

/* Configure GPIO_x pin function is GPIO
 * param   gpio_id : Gpio id number
 * return  0 : Success
 *         1 : Failed
 */
int gpio_pinmux_init_one(u32 gpio_id, u32 pinmux_flag)
{
	gpio_pinmux_info gpio_pinmux_info_one;
	u32 reg_val;

	if (gpio_id < gpio_group_0.num_offset ||
	    (gpio_id > gpio_group_1.num_offset + gpio_group_1.io_nums)) {
		hprintf(ERROR, "GPIO_%d is a invalid GPIO pin.\r\n", gpio_id);
		return -1;
	}

	get_gpio_pinmux_info_one(gpio_id, &gpio_pinmux_info_one);
	if (gpio_pinmux_info_one.attr == HVTE_FALSE) {
		hprintf(ERROR, "GPIO_%d is a invalid GPIO pin.\r\n",
			gpio_pinmux_info_one.gpio_id);
		return -1;
	}

	reg_val = REG32_READ(gpio_pinmux_info_one.pinmux_addr);
	//    hprintf(DEBUG, "pinmux val : 0x%08x.\r\n", reg_val);
	reg_val = set_bits(reg_val, gpio_pinmux_info_one.pinmux_offset,
			   gpio_pinmux_info_one.pinmux_mask, pinmux_flag);
	REG32_WRITE(gpio_pinmux_info_one.pinmux_addr, reg_val);

	//    reg_val = REG32_READ(gpio_pinmux_info_one.pinmux_addr);
	//    hprintf(DEBUG, "pinmux val : 0x%08x.\r\n", reg_val);

	return 0;
}

/* Get the data direction of GPIO_x 
 * param   gpio_id : Gpio id number
 * return  Gpio_x direction
 */
int gpio_get_direction(u32 gpio_id)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;
	u32 dir = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);

	switch (gpio_pin_info_one.gpio_port) {
	case Gpio_port_a:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTA_DDR);
		break;
	case Gpio_port_b:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTB_DDR);
		break;
	case Gpio_port_c:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTC_DDR);
		break;
	case Gpio_port_d:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTD_DDR);
		break;
	}

	dir = get_bit(reg_val, gpio_pin_info_one.gpio_pin);

	return dir;
}

/* Configure the data direction of GPIO_x 
 * param   gpio_id : Gpio id number
 *         dir : Gpio_x direction
 * return  0 : Success
 *         1 : Failed
 */
int gpio_set_direction(u32 gpio_id, u32 dir)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);
	gpio_pinmux_init_one(gpio_id, gpios_pinmux_info[gpio_id].gpio_flag);

	switch (gpio_pin_info_one.gpio_port) {
	case Gpio_port_a:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTA_DDR);
		reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, dir);
		REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_SWPORTA_DDR,
			    reg_val);
		break;
	case Gpio_port_b:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTB_DDR);
		reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, dir);
		REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_SWPORTB_DDR,
			    reg_val);
		break;
	case Gpio_port_c:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTC_DDR);
		reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, dir);
		REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_SWPORTC_DDR,
			    reg_val);
		break;
	case Gpio_port_d:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTD_DDR);
		reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, dir);
		REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_SWPORTD_DDR,
			    reg_val);
		break;
	}

	return 0;
}

/* Get GPIO_x value
 * param   gpio_id : Gpio id number
 * return  GPIO_x value
 */
int gpio_get_value(u32 gpio_id)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;
	u32 pin_val = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);

	switch (gpio_pin_info_one.gpio_port) {
	case Gpio_port_a:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_EXT_PORTA);
		break;
	case Gpio_port_b:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_EXT_PORTB);
		break;
	case Gpio_port_c:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_EXT_PORTC);
		break;
	case Gpio_port_d:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_EXT_PORTD);
		break;
	}

	pin_val = get_bit(reg_val, gpio_pin_info_one.gpio_pin);

	return pin_val;
}

/* Set GPIO_x value
 * param   gpio_id : Gpio id number
 *         val : output value
 * return  0 : Success
 *         1 : Failed
 */
int gpio_set_value(u32 gpio_id, int val)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;
	u32 pin_ddr_val = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);

	pin_ddr_val = gpio_get_direction(gpio_id);
	if (pin_ddr_val != Gpio_output) {
		hprintf(ERROR, "GPIO_%d is not a output GPIO pin.\r\n",
			gpio_id);
		return -1;
	}

	switch (gpio_pin_info_one.gpio_port) {
	case Gpio_port_a:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTA_DR);
		reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, val);
		REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_SWPORTA_DR,
			    reg_val);
		break;
	case Gpio_port_b:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTB_DR);
		reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, val);
		REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_SWPORTB_DR,
			    reg_val);
		break;
	case Gpio_port_c:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTC_DR);
		reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, val);
		REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_SWPORTC_DR,
			    reg_val);
		break;
	case Gpio_port_d:
		reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
				     GPIO_SWPORTD_DR);
		reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, val);
		REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_SWPORTD_DR,
			    reg_val);
		break;
	}

	return 0;
}

/* Enable or disable GPIO_x irq, only port a support interrupt
 * param   gpio_id : Gpio id number
 *         enable : Whether to enable interrupts
 * return  0 : Success
 *         1 : Failed
 */
int gpio_set_irq_enable(u32 gpio_id, int enable)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;
	u32 pin_ddr_val = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);

	pin_ddr_val = gpio_get_direction(gpio_id);
	if (pin_ddr_val != Gpio_input) {
		hprintf(ERROR, "GPIO_%d is not a input GPIO pin.\r\n", gpio_id);
		return -1;
	}
	if (gpio_pin_info_one.gpio_port != Gpio_port_a) {
		hprintf(ERROR, "GPIO_%d is not a interrupt GPIO pin.\r\n",
			gpio_id);
	}

	reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr + GPIO_INTEN);
	reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, enable);
	REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_INTEN, reg_val);

	return 0;
}

/* Mask or unmask GPIO_x irq
 * param   gpio_id : Gpio id number
 *         enable : Whether to mask interrupts
 * return  0 : Success
 *         1 : Failed
 */
int gpio_set_irq_mask(u32 gpio_id, int mask)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;
	u32 pin_ddr_val = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);

	pin_ddr_val = gpio_get_direction(gpio_id);
	if (pin_ddr_val != Gpio_input) {
		hprintf(ERROR, "GPIO_%d is not a input GPIO pin.\r\n", gpio_id);
		return -1;
	}
	if (gpio_pin_info_one.gpio_port != Gpio_port_a) {
		hprintf(ERROR, "GPIO_%d is not a interrupt GPIO pin.\r\n",
			gpio_id);
	}

	reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr + GPIO_INTMASK);
	reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, mask);
	REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_INTMASK, reg_val);

	return 0;
}

/* Set GPIO_x irq type
 * param   gpio_id : Gpio id number
 *         irq_type : GPIO_x irq type
 * return  0 : Success
 *         1 : Failed
 */
int gpio_set_irq_type(u32 gpio_id, int irq_type)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;
	u32 pin_ddr_val = 0;
	u32 polarity = 0;
	u32 type_level = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);

	pin_ddr_val = gpio_get_direction(gpio_id);
	if (pin_ddr_val != Gpio_input) {
		hprintf(ERROR, "GPIO_%d is not a input GPIO pin.\r\n", gpio_id);
		return -1;
	}
	if (gpio_pin_info_one.gpio_port != Gpio_port_a) {
		hprintf(ERROR, "GPIO_%d is not a interrupt GPIO pin.\r\n",
			gpio_id);
	}

	polarity = (irq_type & 0x2) >> 1;
	type_level = irq_type & 0x1;

	reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
			     GPIO_INT_POLARITY);
	reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, polarity);
	REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_INT_POLARITY,
		    reg_val);

	reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
			     GPIO_INTTYPE_LEVEL);
	reg_val = set_bit(reg_val, gpio_pin_info_one.gpio_pin, type_level);
	REG32_WRITE(gpio_pin_info_one.gpio_base_addr + GPIO_INTTYPE_LEVEL,
		    reg_val);

	return 0;
}

/* Get GPIO_x irq status
 * param   gpio_id : Gpio id number
 * return  irq status
 */
int gpio_get_irq_status(u32 gpio_id)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;
	u32 pin_ddr_val = 0;
	u32 irq_status = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);

	pin_ddr_val = gpio_get_direction(gpio_id);
	if (pin_ddr_val != Gpio_input) {
		hprintf(ERROR, "GPIO_%d is not a input GPIO pin.\r\n", gpio_id);
		return -1;
	}
	if (gpio_pin_info_one.gpio_port != Gpio_port_a) {
		hprintf(ERROR, "GPIO_%d is not a interrupt GPIO pin.\r\n",
			gpio_id);
	}

	reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr + GPIO_INTSTATUS);
	irq_status = get_bit(reg_val, gpio_pin_info_one.gpio_pin);

	return irq_status;
}

/* Get GPIO_x raw irq status
 * param   gpio_id : Gpio id number
 * return  raw irq status
 */
int gpio_get_irq_raw_status(u32 gpio_id)
{
	gpio_pin_info gpio_pin_info_one;
	u32 reg_val = 0;
	u32 pin_ddr_val = 0;
	u32 irq_raw_status = 0;

	get_gpio_pin_info_one(gpio_id, &gpio_pin_info_one);

	pin_ddr_val = gpio_get_direction(gpio_id);
	if (pin_ddr_val != Gpio_input) {
		hprintf(ERROR, "GPIO_%d is not a input GPIO pin.\r\n", gpio_id);
		return -1;
	}
	if (gpio_pin_info_one.gpio_port != Gpio_port_a) {
		hprintf(ERROR, "GPIO_%d is not a interrupt GPIO pin.\r\n",
			gpio_id);
	}

	reg_val = REG32_READ(gpio_pin_info_one.gpio_base_addr +
			     GPIO_RAW_INTSTATUS);
	irq_raw_status = get_bit(reg_val, gpio_pin_info_one.gpio_pin);

	return irq_raw_status;
}

void gpio_gic_irq_enable(u32 base, u32 pin)
{
#if defined(CONFIG_A55)
	if (GPIO0_BASE_ADDR == base)
		A55_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0);
	else
		A55_IRQ_ENABLE(FIQ_IRQ_LSP1_GPIO1);
#endif

#if defined(CONFIG_R5_SAFETY)
	if (GPIO0_BASE_ADDR == base)
		switch (pin) {
		case Gpio_24:
			SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0_A_24);
			break;
		case Gpio_25:
			SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0_A_25);
			break;
		case Gpio_26:
			SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0_A_26);
			break;
		case Gpio_27:
			SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0_A_27);
			break;
		case Gpio_28:
			SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0_A_28);
			break;
		case Gpio_29:
			SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0_A_29);
			break;
		case Gpio_30:
			SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0_A_30);
			break;
		default:
			SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_GPIO0);
		}
	else
		SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP1_GPIO1);

#endif
}

void gpio_gic_irq_disable(u32 base, u32 pin)
{
#if defined(CONFIG_A55)
	if (GPIO0_BASE_ADDR == base)
		A55_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0);
	else
		A55_IRQ_DISABLE(FIQ_IRQ_LSP1_GPIO1);
#endif

#if defined(CONFIG_R5_SAFETY)
	if (GPIO0_BASE_ADDR == base)
		switch (pin) {
		case Gpio_24:
			SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0_A_24);
			break;
		case Gpio_25:
			SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0_A_25);
			break;
		case Gpio_26:
			SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0_A_26);
			break;
		case Gpio_27:
			SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0_A_27);
			break;
		case Gpio_28:
			SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0_A_28);
			break;
		case Gpio_29:
			SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0_A_29);
			break;
		case Gpio_30:
			SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0_A_30);
			break;
		default:
			SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_GPIO0);
		}
	else
		SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP1_GPIO1);

#endif
}

int gpio_input_output_test_one(u32 output_pin, u32 input_pin)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 val = 0;

	gpio_set_direction(input_pin, Gpio_input);
	gpio_set_direction(output_pin, Gpio_output);

	hprintf(DEBUG, "Gpio_%d direction:  %d.(0: input, 1: output)\r\n",
		input_pin, gpio_get_direction(input_pin));
	hprintf(DEBUG, "Gpio_%d direction:  %d.(0: input, 1: output)\r\n",
		output_pin, gpio_get_direction(output_pin));

	gpio_set_value(output_pin, 0x0);
	val = gpio_get_value(input_pin);
	hprintf(DEBUG, "Gpio_%d val : 0x%x.\r\n", input_pin, val);
	HCOMP(0x0, val);

	gpio_set_value(output_pin, 0x1);
	val = gpio_get_value(input_pin);
	hprintf(DEBUG, "Gpio_%d val : 0x%x.\r\n", input_pin, val);
	HCOMP(0x1, val);

	return result;
}

int gpio_input_output_test_zebu(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 base = mod->reg_base;

	if (base == GPIO0_BASE_ADDR) {
		result = gpio_input_output_test_one(Gpio_24, Gpio_26);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_input_output_test_one(Gpio_26, Gpio_24);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_input_output_test_one(Gpio_25, Gpio_27);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_input_output_test_one(Gpio_27, Gpio_25);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
	}

	if (base == GPIO1_BASE_ADDR) {
		result = gpio_input_output_test_one(
			Gpio_141, Gpio_142); //GPIO1_13, GPIO1_14
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_input_output_test_one(Gpio_142, Gpio_141);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_input_output_test_one(
			Gpio_143, Gpio_144); //GPIO1_15, GPIO1_16
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_input_output_test_one(Gpio_144, Gpio_143);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_input_output_test_one(
			Gpio_145, Gpio_146); //GPIO1_17, GPIO1_18
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_input_output_test_one(Gpio_146, Gpio_145);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_input_output_test_one(
			Gpio_147, Gpio_148); //GPIO1_19, GPIO1_20
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_input_output_test_one(Gpio_148, Gpio_147);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
	}

	return result;
}

int gpio_input_output_test_haps(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 base = mod->reg_base;

	if (base == GPIO0_BASE_ADDR) {
		result = gpio_input_output_test_one(Gpio_29, Gpio_30);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_input_output_test_one(Gpio_30, Gpio_29);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
	}

	if (base == GPIO1_BASE_ADDR) {
		//TODO
	}
	return result;
}

int gpio_irq_test_one(u32 output_pin, u32 input_pin, u32 irq_type)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i = 10;
	gpio_pin_info input_pin_info;

	get_gpio_pin_info_one(input_pin, &input_pin_info);

	gpio_set_direction(input_pin, Gpio_input);
	gpio_set_direction(output_pin, Gpio_output);

	hprintf(DEBUG, "Gpio_%d direction:  %d.(0: input, 1: output)\r\n",
		input_pin, gpio_get_direction(input_pin));
	hprintf(DEBUG, "Gpio_%d direction:  %d.(0: input, 1: output)\r\n",
		output_pin, gpio_get_direction(output_pin));

	if ((irq_type == Gpio_high_level) || (irq_type == Gpio_rising_edge)) {
		gpio_set_value(output_pin, 0x0);
	} else {
		gpio_set_value(output_pin, 0x1);
	}

	gpio_set_irq_type(input_pin, irq_type);
	gpio_set_irq_mask(input_pin, Gpio_irq_unmask);
	gpio_set_irq_enable(input_pin, Gpio_irq_enable);
	gpio_gic_irq_enable(input_pin_info.gpio_base_addr,
			    input_pin_info.gpio_pin);

	if ((irq_type == Gpio_high_level) || (irq_type == Gpio_rising_edge)) {
		gpio_set_value(output_pin, 0x1);
	} else {
		gpio_set_value(output_pin, 0x0);
	}

	//    hprintf(DEBUG, "Gpio_%d irq status : %d.\r\n", input_pin, gpio_get_irq_status(input_pin));

	while (inter_flag == HVTE_FALSE) {
		i--;
		udelay(100);
		if (i <= 0) {
			hprintf(ERROR, "Gpio_%d irq timeout.\r\n", input_pin);
			break;
		}
	}
	HCOMP(HVTE_TRUE, inter_flag);

	if ((irq_type == Gpio_rising_edge) || (irq_type == Gpio_falling_edge))
		gpio_gic_irq_disable(input_pin_info.gpio_base_addr,
				     input_pin_info.gpio_pin);

	inter_flag = HVTE_FALSE;

	return result;
}

int gpio_irq_test_zebu(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 base = mod->reg_base;

#if defined(CONFIG_R5_SAFETY) /* These interrupts are only sent to safety */
	if (base == GPIO0_BASE_ADDR) {
		result = gpio_irq_test_one(Gpio_24, Gpio_26,
					   Gpio_low_level); //GPIO0_24, GPIO0_26
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_24, Gpio_26, Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_24, Gpio_26, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_24, Gpio_26, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_26, Gpio_24, Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_26, Gpio_24, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_25, Gpio_27, Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_25, Gpio_27, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_27, Gpio_25, Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_27, Gpio_25, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
	}
#endif

	if (base == GPIO0_BASE_ADDR) {
		//TODO
		hprintf(DEBUG, "No testable interrupt I/O pins\r\n");
	}

	if (base == GPIO1_BASE_ADDR) {
		result = gpio_irq_test_one(Gpio_141, Gpio_142,
					   Gpio_low_level); //GPIO1_13, GPIO1_14
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_141, Gpio_142,
					   Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_141, Gpio_142, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result =
			gpio_irq_test_one(Gpio_141, Gpio_142, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_142, Gpio_141, Gpio_low_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_142, Gpio_141,
					   Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_142, Gpio_141, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result =
			gpio_irq_test_one(Gpio_142, Gpio_141, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_143, Gpio_144,
					   Gpio_low_level); //GPIO1_15, GPIO1_16
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_143, Gpio_144,
					   Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_143, Gpio_144, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result =
			gpio_irq_test_one(Gpio_143, Gpio_144, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_144, Gpio_143, Gpio_low_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_144, Gpio_143,
					   Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_144, Gpio_143, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result =
			gpio_irq_test_one(Gpio_144, Gpio_143, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_145, Gpio_146,
					   Gpio_low_level); //GPIO1_17, GPIO1_18
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_145, Gpio_146,
					   Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_145, Gpio_146, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result =
			gpio_irq_test_one(Gpio_145, Gpio_146, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_146, Gpio_145, Gpio_low_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_146, Gpio_145,
					   Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_146, Gpio_145, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result =
			gpio_irq_test_one(Gpio_146, Gpio_145, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_147, Gpio_148,
					   Gpio_low_level); //GPIO1_19, GPIO1_20
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_147, Gpio_148,
					   Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_147, Gpio_148, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result =
			gpio_irq_test_one(Gpio_147, Gpio_148, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);

		result = gpio_irq_test_one(Gpio_148, Gpio_147, Gpio_low_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_148, Gpio_147,
					   Gpio_falling_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result = gpio_irq_test_one(Gpio_148, Gpio_147, Gpio_high_level);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
		result =
			gpio_irq_test_one(Gpio_148, Gpio_147, Gpio_rising_edge);
		HCOMP(HVTE_MODULE_TEST_PASS, result);
	}

	return result;
}

void gpio_irq_handle(int base)
{
	u32 status = 0;
	u32 irq_pin = 0;
	u32 irq_type = 0;
	int i = 0;

	hprintf(DEBUG, "gpio irq handle\r\n");

	status = REG32_READ(base + GPIO_INTSTATUS);
	hprintf(DEBUG, "gpio irq status : 0x%08x.\r\n", status);
	for (i = 0; i < 32; i++) {
		if ((status >> i) & 0x1) {
			irq_pin = i;
			break;
		}
	}
	hprintf(DEBUG, "irq pin : %d.\r\n", irq_pin);

	REG32_WRITE(base + GPIO_PORTA_EOI, (0x1 << irq_pin)); //Clear Interrupt

	irq_type = get_bit(REG32_READ(base + GPIO_INTTYPE_LEVEL), irq_pin);
	hprintf(DEBUG, "irq type : %d.\r\n", irq_type);
	if (irq_type == 0x0) { //level type
		gpio_gic_irq_disable(base, irq_pin);
	}

	inter_flag = HVTE_TRUE;
}

void dw_init_example(void)
{
	unsigned int reg_offset;
	volatile unsigned reg_addr, reg_val;

	hprintf(DEBUG, "=====> Initializing port ...\r\n");

	for (reg_offset = 0; reg_offset <= GPIO_CONFIG_REG1; reg_offset += 4) {
		reg_addr = ((GPIO0_BASE_ADDR) + (reg_offset));
		reg_val = REG32_READ(reg_addr);
		if (reg_val != 0) {
			if (reg_addr == (GPIO0_BASE_ADDR + GPIO_VER_ID_CODE)) {
				hprintf(DEBUG,
					"==> GPIO0 Version code is 0x%x\r\n",
					reg_val);
			} else if (reg_addr ==
				   (GPIO0_BASE_ADDR + GPIO_CONFIG_REG2)) {
				hprintf(DEBUG,
					"==> GPIO0 config reg2 is 0x%x\r\n",
					reg_val);
			} else if (reg_addr ==
				   (GPIO0_BASE_ADDR + GPIO_CONFIG_REG1)) {
				hprintf(DEBUG,
					"==> GPIO0 config reg1 is 0x%x\r\n",
					reg_val);
			} else {
				hprintf(DEBUG,
					"==> GPIO0 Reg 0x%x Defalut Val Error is 0x%x\r\n",
					reg_addr, reg_val);
			}
		}
	}

	hprintf(DEBUG, "GPIO0 Defalut Read FINISHED\r\n");

	for (reg_offset = 0; reg_offset <= GPIO_CONFIG_REG1; reg_offset += 4) {
		reg_addr = ((GPIO1_BASE_ADDR) + (reg_offset));
		reg_val = REG32_READ(reg_addr);
		if (reg_val != 0) {
			if (reg_addr == (GPIO1_BASE_ADDR + GPIO_VER_ID_CODE)) {
				hprintf(DEBUG,
					"==> GPIO1 Version code is 0x%x\r\n",
					reg_val);
			} else if (reg_addr ==
				   (GPIO1_BASE_ADDR + GPIO_CONFIG_REG2)) {
				hprintf(DEBUG,
					"==> GPIO1 config reg2 is 0x%x\r\n",
					reg_val);
			} else if (reg_addr ==
				   (GPIO1_BASE_ADDR + GPIO_CONFIG_REG1)) {
				hprintf(DEBUG,
					"==> GPIO1 config reg1 is 0x%x\r\n",
					reg_val);
			} else {
				hprintf(DEBUG,
					"==> GPIO1 Reg 0x%x Defalut Val Error is 0x%x\r\n",
					reg_addr, reg_val);
			}
		}
	}

	hprintf(DEBUG, "GPIO1 Defalut Read FINISHED\r\n");
}

void dw_basic_example(void)
{
	u32 result;

	//    hprintf(DEBUG, "=====> GPIO 0 Data Source testing ...\r\n");
	//    result = gpio_data_source_func(GPIO0_BASE_ADDR);

	hprintf(DEBUG, "=====> GPIO 0 Data Direction testing ...\r\n");
	result = gpio_data_direction_func(GPIO0_BASE_ADDR);

	hprintf(DEBUG, "=====> GPIO 0 Data testing ...\r\n");
	result = gpio_data_func(GPIO0_BASE_ADDR);

	//    hprintf(DEBUG, "=====> GPIO 1 Data Source testing ...\r\n");
	//    result = gpio_data_source_func(GPIO1_BASE_ADDR);

	hprintf(DEBUG, "=====> GPIO 1 Data Direction testing ...\r\n");
	result = gpio_data_direction_func(GPIO1_BASE_ADDR);

	hprintf(DEBUG, "=====> GPIO 1 Data testing ...\r\n");
	result = gpio_data_func(GPIO1_BASE_ADDR);
}

//2019.8.16 : tag6.2中GPIO不支持hardware模式
u32 gpio_data_source_func(unsigned int base)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	volatile unsigned int val;
	volatile unsigned int reg_addr;

	/* SW GPIO A */
	reg_addr = base + GPIO_SWPORTA_CTL;
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "default GPIO A CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "default GPIO A CTRL is hardware mode \r\n");
	}

	REG32_WRITE(reg_addr, 0x1);
	hprintf(DEBUG, "GPIO A cfg to hardware mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "GPIO A CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO A CTRL is hardware mode \r\n");
	}
	HCOMP(0x1, val);

	REG32_WRITE(reg_addr, 0x0);
	hprintf(DEBUG, "GPIO A cfg to software mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "GPIO A CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO A CTRL is hardware mode \r\n");
	}
	HCOMP(0x0, val);

	/* SW GPIO B */
	reg_addr = base + GPIO_SWPORTB_CTL;
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "default GPIO B CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "default GPIO B CTRL is hardware mode \r\n");
	}

	REG32_WRITE(reg_addr, 0x1);
	hprintf(DEBUG, "GPIO B cfg to hardware mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "GPIO B CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO B CTRL is hardware mode \r\n");
	}
	HCOMP(0x1, val);

	REG32_WRITE(reg_addr, 0x0);
	hprintf(DEBUG, "GPIO B cfg to software mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "GPIO B CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO B CTRL is hardware mode \r\n");
	}
	HCOMP(0x0, val);

	/* SW GPIO C */
	reg_addr = base + GPIO_SWPORTC_CTL;
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "default GPIO C CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "default GPIO C CTRL is hardware mode \r\n");
	}

	REG32_WRITE(reg_addr, 0x1);
	hprintf(DEBUG, "GPIO C cfg to hardware mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "GPIO C CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO C CTRL is hardware mode \r\n");
	}
	HCOMP(0x1, val);

	REG32_WRITE(reg_addr, 0x0);
	hprintf(DEBUG, "GPIO C cfg to software mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "GPIO C CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO C CTRL is hardware mode \r\n");
	}
	HCOMP(0x0, val);

	/* SW GPIO D */
	reg_addr = base + GPIO_SWPORTD_CTL;
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "default GPIO D CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "default GPIO D CTRL is hardware mode \r\n");
	}

	REG32_WRITE(reg_addr, 0x1);
	hprintf(DEBUG, "GPIO D cfg to hardware mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "GPIO D CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO D CTRL is hardware mode \r\n");
	}
	HCOMP(0x1, val);

	REG32_WRITE(reg_addr, 0x0);
	hprintf(DEBUG, "GPIO D cfg to software mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_software_mode) {
		hprintf(DEBUG, "GPIO D CTRL is software mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO D CTRL is hardware mode \r\n");
	}
	HCOMP(0x0, val);
	return result;
}

u32 gpio_data_direction_func(unsigned int base)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	volatile unsigned int val;
	volatile unsigned int reg_addr;

	/* SW GPIO A DDR */
	reg_addr = base + GPIO_SWPORTA_DDR;
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "default GPIO A direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "default GPIO A direction is output mode \r\n");
	}

	REG32_WRITE(reg_addr, 0x1);
	hprintf(DEBUG, "GPIO A direction to output mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "GPIO A direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO A direction is output mode \r\n");
	}
	HCOMP(0x1, val);

	REG32_WRITE(reg_addr, 0x0);
	hprintf(DEBUG, "GPIO A direction to input mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "GPIO A direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO A direction is output mode \r\n");
	}
	HCOMP(0x0, val);

	/* SW GPIO B DDR */
	reg_addr = base + GPIO_SWPORTB_DDR;
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "default GPIO B direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "default GPIO B direction is output mode \r\n");
	}

	REG32_WRITE(reg_addr, 0x1);
	hprintf(DEBUG, "GPIO B direction to output mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "GPIO B direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO B direction is output mode \r\n");
	}
	HCOMP(0x1, val);

	REG32_WRITE(reg_addr, 0x0);
	hprintf(DEBUG, "GPIO B direction to input mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "GPIO B direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO B direction is output mode \r\n");
	}
	HCOMP(0x0, val);

	/* SW GPIO C DDR */
	reg_addr = base + GPIO_SWPORTC_DDR;
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "default GPIO C direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "default GPIO C direction is output mode \r\n");
	}

	REG32_WRITE(reg_addr, 0x1);
	hprintf(DEBUG, "GPIO C direction to output mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "GPIO C direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO C direction is output mode \r\n");
	}
	HCOMP(0x1, val);

	REG32_WRITE(reg_addr, 0x0);
	hprintf(DEBUG, "GPIO C direction to input mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "GPIO C direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO C direction is output mode \r\n");
	}
	HCOMP(0x0, val);

	/* SW GPIO D DDR */
	reg_addr = base + GPIO_SWPORTD_DDR;
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "default GPIO D direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "default GPIO D direction is output mode \r\n");
	}

	REG32_WRITE(reg_addr, 0x1);
	hprintf(DEBUG, "GPIO D direction to output mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "GPIO D direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO D direction is output mode \r\n");
	}
	HCOMP(0x1, val);

	REG32_WRITE(reg_addr, 0x0);
	hprintf(DEBUG, "GPIO D direction to input mode\r\n");
	val = REG32_READ(reg_addr);
	if (val == Gpio_input) {
		hprintf(DEBUG, "GPIO D direction is input mode \r\n");
	} else {
		hprintf(DEBUG, "GPIO D direction is output mode \r\n");
	}
	HCOMP(0x0, val);

	return result;
}

u32 gpio_data_func(unsigned int dbase)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	volatile unsigned int val;
	volatile unsigned int daddr0;
	volatile unsigned int daddr1;
	volatile unsigned int daddr2;
	volatile unsigned int daddr3;
	/* SW GPIO A DR */
	daddr0 = dbase + GPIO_SWPORTA_DR;
	REG32_WRITE(daddr0, 0x5a5a5a5a);
	hprintf(DEBUG, "write 0x5a5a5a5a to GPIO A DR \r\n");
	val = REG32_READ(daddr0);
	if (val == 0x5a5a5a5a) {
		hprintf(DEBUG, "GPIO A data is correct \r\n");
	} else {
		hprintf(DEBUG, "GPIO A data is error 0x%x\r\n", val);
	}

	hprintf(DEBUG, "GPIO_EXT_PORTA : 0x%x \r\n",
		REG32_READ(dbase + GPIO_EXT_PORTA));

	HCOMP(0x5a5a5a5a, val);

	REG32_WRITE(daddr0, 0x25a5a5a5);
	hprintf(DEBUG, "write 0x25a5a5a5 to GPIO A DR \r\n");
	val = REG32_READ(daddr0);
	if (val == 0x25a5a5a5) {
		hprintf(DEBUG, "GPIO A data is correct \r\n");
	} else {
		hprintf(DEBUG, "GPIO A data is error 0x%x\r\n", val);
	}
	HCOMP(0x25a5a5a5, val);

	/* SW GPIO B DR */
	daddr1 = dbase + GPIO_SWPORTB_DR;
	REG32_WRITE(daddr1, 0x5a5a5a5a);
	hprintf(DEBUG, "write 0x5a5a5a5a to GPIO B DR \r\n");
	val = REG32_READ(daddr1);
	if (val == 0x5a5a5a5a) {
		hprintf(DEBUG, "GPIO B data is correct \r\n");
	} else {
		hprintf(DEBUG, "GPIO B data is error 0x%x\r\n", val);
	}
	HCOMP(0x5a5a5a5a, val);

	REG32_WRITE(daddr1, 0x25a5a5a5);
	hprintf(DEBUG, "write 0x25a5a5a5 to GPIO B DR \r\n");
	val = REG32_READ(daddr1);
	if (val == 0x25a5a5a5) {
		hprintf(DEBUG, "GPIO B data is correct \r\n");
	} else {
		hprintf(DEBUG, "GPIO B data is ected rror 0x%x\r\n", val);
	}
	HCOMP(0x25a5a5a5, val);

	/* SW GPIO C DR */
	daddr2 = dbase + GPIO_SWPORTC_DR;
	REG32_WRITE(daddr2, 0x5a5a5a5a);
	hprintf(DEBUG, "write 0x5a5a5a5a to GPIO C DR \r\n");
	val = REG32_READ(daddr2);
	if (val == 0x5a5a5a5a) {
		hprintf(DEBUG, "GPIO C data is correct \r\n");
	} else {
		hprintf(DEBUG, "GPIO C data is error 0x%x\r\n", val);
	}
	HCOMP(0x5a5a5a5a, val);

	REG32_WRITE(daddr2, 0x25a5a5a5);
	hprintf(DEBUG, "write 0x25a5a5a5 to GPIO C DR \r\n");
	val = REG32_READ(daddr2);
	if (val == 0x25a5a5a5) {
		hprintf(DEBUG, "GPIO C data is correct \r\n");
	} else {
		hprintf(DEBUG, "GPIO C data is error 0x%x\r\n", val);
	}
	HCOMP(0x25a5a5a5, val);

	/* SW GPIO D DR */
	daddr3 = dbase + GPIO_SWPORTD_DR;
	REG32_WRITE(daddr3, 0x5a5a5a5a);
	hprintf(DEBUG, "write 0x5a5a5a5a to GPIO D DR \r\n");
	val = REG32_READ(daddr3);
	if (val == 0x5a5a5a5a) {
		hprintf(DEBUG, "GPIO D data is correct \r\n");
	} else {
		hprintf(DEBUG, "GPIO D data is error 0x%x\r\n", val);
	}
	HCOMP(0x5a5a5a5a, val);

	REG32_WRITE(daddr3, 0x25a5a5a5);
	hprintf(DEBUG, "write 0x25a5a5a5 to GPIO D DR \r\n");
	val = REG32_READ(daddr3);
	if (val == 0x25a5a5a5) {
		hprintf(DEBUG, "GPIO D data is correct \r\n");
	} else {
		hprintf(DEBUG, "GPIO D data is error 0x%x\r\n", val);
	}
	HCOMP(0x25a5a5a5, val);

	return result;
}

/*==================================GPIO ZEBU TEST=====================================================*/

zebu_test_mod_t zebu_test_gpio_mods[] = {
	{ "gpio0", GPIO0_BASE_ADDR },
	{ "gpio1", GPIO1_BASE_ADDR },
};

zebu_test_entry_t zebu_test_gpio_entrys[] = {
//must
#if defined(CONFIG_ZEBU)
	{ '1', 0, "gpio input and output test on zebu",
	  gpio_input_output_test_zebu },
	{ '2', 0, "gpio irq test on zebu", gpio_irq_test_zebu },
#endif
#if defined(CONFIG_HAPS)
	{ '1', 0, "gpio input and output test on haps",
	  gpio_input_output_test_haps },
#endif
	// not must
};

static zebu_test_ctrl_t zebu_mod_test_ctrl = { "gpio",
					       SIZE_ARRAY(zebu_test_gpio_mods),
					       zebu_test_gpio_mods,
					       SIZE_ARRAY(
						       zebu_test_gpio_entrys),
					       zebu_test_gpio_entrys,
					       0 };

/*
 * gpiost entry function, main() will call gpioMain base user input CMD
 * para:
 *    0->enter gpio test while loop, rx test cmd and run related test case then go back to rx test cmd
 *    1->run all the gpio test case and return
 */
void gpioMain(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "！！auto test err %d!!!!\r\n", ret);
	} else {
		//TestLoop();
		TestLoop(&zebu_mod_test_ctrl);
	}
}

CMD_DEFINE(gpio, gpioMain, 0, "gpio test", 0);

#if defined(CONFIG_R5_SAFETY) || defined(CONFIG_A55)
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP1_GPIO1, gpio_irq_handle, GPIO1_BASE_ADDR,
	   "irq timer isr gpio1",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
#endif

#if defined(CONFIG_R5_SAFETY)
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0_A_24, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0_A_25, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0_A_26, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0_A_27, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0_A_28, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0_A_29, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0_A_30, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_GPIO0_A_31, gpio_irq_handle, GPIO0_BASE_ADDR,
	   "irq timer isr gpio0", ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
#endif
