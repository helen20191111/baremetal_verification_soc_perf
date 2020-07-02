/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

#ifndef SIZE_ARRAY
#define SIZE_ARRAY(_a) (sizeof(_a) / sizeof(_a[0]))
#endif

typedef struct zebu_test_mod_s {
	char *alias;
	unsigned int reg_base;
} zebu_test_mod_t;

typedef struct zebu_test_entry_s {
	char opt_c;
	char auto_run;
	char *test_alias;
	int (*test_fun)(zebu_test_mod_t *, void *);
} zebu_test_entry_t;

typedef struct zebu_test_ctrl_s {
	char *alias;
	int mod_num;
	zebu_test_mod_t *mod_arr;
	int test_num;
	zebu_test_entry_t *test_entry_arr;
	int current_mod;
} zebu_test_ctrl_t;

int zebu_mod_test_one_entry(zebu_test_mod_t *mod, int entry_num, void *para,
			    zebu_test_ctrl_t *test_ctrl);

int zebu_mod_test_all_entry(zebu_test_mod_t *mod, void *para,
			    zebu_test_ctrl_t *test_ctrl);

int zebu_mod_test_all_mod(zebu_test_ctrl_t *test_ctrl);

int zebu_mod_run_current_mod(zebu_test_ctrl_t *test_ctrl);

int mod_logo_msg(zebu_test_ctrl_t *test_ctrl);

int test_opt_find_do(char opt, zebu_test_ctrl_t *test_ctrl);

void mod_menu(zebu_test_ctrl_t *test_ctrl);

void changeBase(zebu_test_ctrl_t *test_ctrl);

void TestLoop(zebu_test_ctrl_t *test_ctrl);

#define dump_one_reg(__base, __offset)                                         \
	do {                                                                   \
		hprintf(TSENV, "REG:%08X(%-20s):%08X\r\n", __base + __offset,  \
			#__offset, REG32_READ(__base + __offset));             \
	} while (0)

#define dump_one_reg_offset2(__base, __offset, __offset2)                      \
	do {                                                                   \
		hprintf(TSENV, "REG:%08X(%-17s+%x):%08X\r\n",                  \
			__base + __offset + __offset2, #__offset, __offset2,   \
			REG32_READ(__base + __offset + __offset2));            \
	} while (0)

#endif
