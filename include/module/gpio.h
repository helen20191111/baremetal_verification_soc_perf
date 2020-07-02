/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _GPIO_H
#define _GPIO_H

/* 4K */
#define GPIO_GROUP_NUM 4

#define APB_DATA_WIDTH_BYTE 8
#define APB_DATA_WIDTH_SHORT 16
#define APB_DATA_WIDTH_INT 32

#define GPIO_SWPORTA_DR 0x0
#define GPIO_SWPORTA_DDR 0x4
#define GPIO_SWPORTA_CTL 0x8

#define GPIO_SWPORTB_DR 0xc
#define GPIO_SWPORTB_DDR 0x10
#define GPIO_SWPORTB_CTL 0x14

#define GPIO_SWPORTC_DR 0x18
#define GPIO_SWPORTC_DDR 0x1c
#define GPIO_SWPORTC_CTL 0x20

#define GPIO_SWPORTD_DR 0x24
#define GPIO_SWPORTD_DDR 0x28
#define GPIO_SWPORTD_CTL 0x2c

#define GPIO_INTEN 0x30
#define GPIO_INTMASK 0x34
#define GPIO_INTTYPE_LEVEL 0x38
#define GPIO_INT_POLARITY 0x3c
#define GPIO_INTSTATUS 0x40
#define GPIO_RAW_INTSTATUS 0x44
#define GPIO_DEBOUNCE 0x48
#define GPIO_PORTA_EOI 0x4c

#define GPIO_EXT_PORTA 0x50
#define GPIO_EXT_PORTB 0x54
#define GPIO_EXT_PORTC 0x58
#define GPIO_EXT_PORTD 0x5c
#define GPIO_LS_SYNC 0x60
#define GPIO_ID_CODE 0x64
#define GPIO_INT_BOTHEDGE 0x68
#define GPIO_VER_ID_CODE 0x6c

#define GPIO_CONFIG_REG2 0x70
#define GPIO_CONFIG_REG1 0x74

#define GPIO_ADDR_SIZE 0xFFF

#define GPIO_PORTA_INTR 1

#define set_bit(data, bit, val) ((data & (~(1 << bit))) | (val << bit))
#define get_bit(data, bit) ((data >> bit) & 0x1)
#define set_bits(data, bit, mask, val)                                         \
	((data & (~(mask << bit))) | (val << bit))

#define PINMUX_INFO(gpio_id, gpio_flag, pinmux_addr, pinmux_offset, pinmux_mask, attr) \
	{pinmux_addr, gpio_id, gpio_flag, pinmux_offset, pinmux_mask, attr}

typedef struct {
	u32 pinmux_addr;
	u16 gpio_id;
	u8 gpio_flag;
	u8 pinmux_offset;
	u8 pinmux_mask;
	bool_enum attr; /*Indicates whether it is a valid gpio*/
} gpio_pinmux_info;

typedef struct {
	u32 base_addr;
	u8 num_offset;
	u8 io_nums;
} gpio_group_info;

typedef struct {
	u32 gpio_base_addr;
	u8 gpio_port;
	u8 gpio_pin;
} gpio_pin_info;

enum gpio_id {
	Gpio_0,
	Gpio_1,
	Gpio_2,
	Gpio_3,
	Gpio_4,
	Gpio_5,
	Gpio_6,
	Gpio_7,
	Gpio_8,
	Gpio_9,
	Gpio_10,
	Gpio_11,
	Gpio_12,
	Gpio_13,
	Gpio_14,
	Gpio_15,
	Gpio_16,
	Gpio_17,
	Gpio_18,
	Gpio_19,
	Gpio_20,
	Gpio_21,
	Gpio_22,
	Gpio_23,
	Gpio_24,
	Gpio_25,
	Gpio_26,
	Gpio_27,
	Gpio_28,
	Gpio_29,
	Gpio_30,
	Gpio_31,
	Gpio_32,
	Gpio_33,
	Gpio_34,
	Gpio_35,
	Gpio_36,
	Gpio_37,
	Gpio_38,
	Gpio_39,
	Gpio_40,
	Gpio_41,
	Gpio_42,
	Gpio_43,
	Gpio_44,
	Gpio_45,
	Gpio_46,
	Gpio_47,
	Gpio_48,
	Gpio_49,
	Gpio_50,
	Gpio_51,
	Gpio_52,
	Gpio_53,
	Gpio_54,
	Gpio_55,
	Gpio_56,
	Gpio_57,
	Gpio_58,
	Gpio_59,
	Gpio_60,
	Gpio_61,
	Gpio_62,
	Gpio_63,
	Gpio_64,
	Gpio_65,
	Gpio_66,
	Gpio_67,
	Gpio_68,
	Gpio_69,
	Gpio_70,
	Gpio_71,
	Gpio_72,
	Gpio_73,
	Gpio_74,
	Gpio_75,
	Gpio_76,
	Gpio_77,
	Gpio_78,
	Gpio_79,
	Gpio_80,
	Gpio_81,
	Gpio_82,
	Gpio_83,
	Gpio_84,
	Gpio_85,
	Gpio_86,
	Gpio_87,
	Gpio_88,
	Gpio_89,
	Gpio_90,
	Gpio_91,
	Gpio_92,
	Gpio_93,
	Gpio_94,
	Gpio_95,
	Gpio_96,
	Gpio_97,
	Gpio_98,
	Gpio_99,
	Gpio_100,
	Gpio_101,
	Gpio_102,
	Gpio_103,
	Gpio_104,
	Gpio_105,
	Gpio_106,
	Gpio_107,
	Gpio_108,
	Gpio_109,
	Gpio_110,
	Gpio_111,
	Gpio_112,
	Gpio_113,
	Gpio_114,
	Gpio_115,
	Gpio_116,
	Gpio_117,
	Gpio_118,
	Gpio_119,
	Gpio_120,
	Gpio_121,
	Gpio_122,
	Gpio_123,
	Gpio_124,
	Gpio_125,
	Gpio_126,
	Gpio_127,
	Gpio_128,
	Gpio_129,
	Gpio_130,
	Gpio_131,
	Gpio_132,
	Gpio_133,
	Gpio_134,
	Gpio_135,
	Gpio_136,
	Gpio_137,
	Gpio_138,
	Gpio_139,
	Gpio_140,
	Gpio_141,
	Gpio_142,
	Gpio_143,
	Gpio_144,
	Gpio_145,
	Gpio_146,
	Gpio_147,
	Gpio_148,
	Gpio_149,
	Gpio_150
};

enum gpio_port {
	Gpio_port_a = 0x0,
	Gpio_port_b = 0x1,
	Gpio_port_c = 0x2,
	Gpio_port_d = 0x3
};

enum gpio_bit {
	Gpio_bit_0 = 0,
	Gpio_bit_1 = 1,
	Gpio_bit_2 = 2,
	Gpio_bit_3 = 3,
	Gpio_bit_4 = 4,
	Gpio_bit_5 = 5,
	Gpio_bit_6 = 6,
	Gpio_bit_7 = 7,
	Gpio_bit_8 = 8,
	Gpio_bit_9 = 9,
	Gpio_bit_10 = 10,
	Gpio_bit_11 = 11,
	Gpio_bit_12 = 12,
	Gpio_bit_13 = 13,
	Gpio_bit_14 = 14,
	Gpio_bit_15 = 15,
	Gpio_bit_16 = 16,
	Gpio_bit_17 = 17,
	Gpio_bit_18 = 18,
	Gpio_bit_19 = 19,
	Gpio_bit_20 = 20,
	Gpio_bit_21 = 21,
	Gpio_bit_22 = 22,
	Gpio_bit_23 = 23,
	Gpio_bit_24 = 24,
	Gpio_bit_25 = 25,
	Gpio_bit_26 = 26,
	Gpio_bit_27 = 27,
	Gpio_bit_28 = 28,
	Gpio_bit_29 = 29,
	Gpio_bit_30 = 30,
	Gpio_bit_31 = 31
};

enum gpio_data_source {
	Gpio_no_source = -1,
	Gpio_software_mode = 0x0,
	Gpio_hardware_mode = 0x1
};

enum gpio_data_direction {
	Gpio_no_direction = -1,
	Gpio_input = 0x0,
	Gpio_output = 0x1
};

enum gpio_irq_en { Gpio_irq_disable = 0x0, Gpio_irq_enable = 0x1 };

enum gpio_irq_mask { Gpio_irq_unmask = 0x0, Gpio_irq_mask = 0x1 };

enum gpio_irq_type {
	Gpio_no_type = -1,
	Gpio_low_level = 0x0,
	Gpio_falling_edge = 0x1,
	Gpio_high_level = 0x2,
	Gpio_rising_edge = 0x3
};

void get_gpio_pinmux_info_one(u32 gpio_id,
			      gpio_pinmux_info *gpio_pinmux_info_one);
void get_gpio_pin_info_one(u32 gpio_id, gpio_pin_info *gpio_pin_info_one);
int gpio_pinmux_init_one(u32 gpio_id, u32 pinmux_flag);
int gpio_get_direction(u32 gpio_id);
int gpio_set_direction(u32 gpio_id, u32 dir);
int gpio_get_value(u32 gpio_id);
int gpio_set_value(u32 gpio_id, int val);
int gpio_get_data_source(u32 gpio_id);
int gpio_set_data_source(u32 gpio_id, int data_source);
int gpio_set_irq_enable(u32 gpio_id, int enable);
int gpio_set_irq_mask(u32 gpio_id, int mask);
int gpio_set_irq_type(u32 gpio_id, int irq_type);
int gpio_get_irq_status(u32 gpio_id);
int gpio_get_irq_raw_status(u32 gpio_id);

#endif
