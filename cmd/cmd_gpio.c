/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <common.h>
#include <command.h>
#include <a1000/sysreg_base.h>
#include <RegTest.h>
#include <module/gpio.h>
#include <vsprintf.h>

static bool_enum gpio_is_vaild(u32 gpio_id){
	gpio_pinmux_info pinmux_info;

	if(gpio_id > Gpio_0 || gpio_id < Gpio_150){
		get_gpio_pinmux_info_one(gpio_id, &pinmux_info);
		return pinmux_info.attr;
	}else{
		return HVTE_FALSE;
	}
}

static int do_gpio_dir(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]){	
	u32 gpio_id = 0;
	u8 dir = 0;
	
	if(argc != 3)
		return CMD_RET_USAGE;

	gpio_id = simple_strtoul(argv[1], NULL, 10);
	if(gpio_is_vaild(gpio_id)){
		//whether the direction is correct
		if(0 == strcmp(argv[2], "in")){
			dir = Gpio_input;
		}else if(0 == strcmp(argv[2], "out")){
			dir = Gpio_output;
		}else{
			printf("Incorrect data direction.\r\n");
			return CMD_RET_USAGE;
		}
		gpio_set_direction(gpio_id, dir);
		printf("Set Gpio_%d direction : %d.\r\n", gpio_id, dir);
	}else {
		printf("%d is not a valid GPIO pin.\r\n", gpio_id);
		return CMD_RET_USAGE;
	}
	
	return CMD_RET_SUCCESS;
}

static int do_gpio_val(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]){	
	u32 gpio_id = 0;
	u8 val = 0;
	
	if(argc != 3)
		return CMD_RET_USAGE;

	gpio_id = simple_strtoul(argv[1], NULL, 10);
	if(gpio_is_vaild(gpio_id)){
		//whether the value is correct
		if(0 == strcmp(argv[2], "0")){
			val = 0;
		}else if(0 == strcmp(argv[2], "1")){
			val = 1;
		}else{
			printf("Incorrect data value.\r\n");
			return CMD_RET_USAGE;
		}
		gpio_set_value(gpio_id, val);
		printf("Set Gpio_%d val : %d.\r\n", gpio_id, val);
	}else {
		printf("%d is not a valid GPIO pin.\r\n", gpio_id);	
		return CMD_RET_USAGE;
	}
	return CMD_RET_SUCCESS;
}

static int do_gpio_sta(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]){	
	
	u32 gpio_id = 0;
	u8 val = 0;
	u8 dir = 0;
	
	if(argc != 2)
		return CMD_RET_USAGE;

	gpio_id = simple_strtoul(argv[1], NULL, 10);
	if(gpio_is_vaild(gpio_id)){
		dir = gpio_get_direction(gpio_id);
		val = gpio_get_value(gpio_id);
		printf("Gpio_%d direction : %s.\r\n", gpio_id, dir ? "out" : "in");	
		printf("Gpio_%d val : %d.\r\n", gpio_id, val);	
	}else {
		printf("%d is not a valid GPIO pin.\r\n", gpio_id);	
		return CMD_RET_USAGE;
	}
	return CMD_RET_SUCCESS;
}


static cmd_tbl_t cmd_gpio_sub[] = {
	BST_CMD_MKENT(dir, 1, 1, do_gpio_dir, "", ""),
	BST_CMD_MKENT(val, 1, 1, do_gpio_val, "", ""),
	BST_CMD_MKENT(sta, 1, 1, do_gpio_sta, "", ""),
};


static int do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{	
	cmd_tbl_t *c;

	if(argc < 3)
		return CMD_RET_USAGE;

	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_gpio_sub[0], ARRAY_SIZE(cmd_gpio_sub));

	if(c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}


BST_CMD(gpio, CONFIG_SYS_MAXARGS, 1, do_gpio,
	   "gpio module test", 
	   "\tgpio dir gpio_id in/out \r\n"
	   "\tgpio val gpio_id 0/1 \r\n"
	   "\tgpio sta gpio_id \r\n");

