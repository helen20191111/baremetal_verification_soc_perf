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
#include <vsprintf.h>

//static int test_opt_find_do(char opt,zebu_test_ctrl_t* test_ctrl);

int zebu_mod_test_one_entry(zebu_test_mod_t *mod, int entry_num, void *para,
			    zebu_test_ctrl_t *test_ctrl)
{
	zebu_test_entry_t *test_entry;
	int ret;
	if (entry_num >= test_ctrl->test_num) {
		hprintf(TSENV, "ERROR test Entry:%d \r\n", entry_num);
		return -1;
	}
	test_entry = test_ctrl->test_entry_arr + entry_num;
	hprintf(TSENV, "\r\n++++++++Mod:(%s): Runing test Entry:%s++++++++\r\n",
		mod->alias, test_entry->test_alias);
	ret = test_entry->test_fun(mod, NULL);
	hprintf(TSENV, "+++++++++Test Entry:%s Result:%s(%d)++++++++\r\n\r\n",
		test_entry->test_alias, ret ? "Failed" : "OK", ret);
	return 0;
}

int zebu_mod_test_all_entry(zebu_test_mod_t *mod, void *para,
			    zebu_test_ctrl_t *test_ctrl)
{
	zebu_test_entry_t *test_entry;
	int i;
	hprintf(TSENV,
		"-------------Now Test Mod:[%s] All Entry ---------------\r\n",
		mod->alias);
	for (i = 0; i < test_ctrl->test_num; i++) {
		test_entry = test_ctrl->test_entry_arr + i;
		if (!test_entry->auto_run) {
			continue;
		}
		zebu_mod_test_one_entry(mod, i, para, test_ctrl);
	}
	hprintf(TSENV,
		"----------------End Test Mod:[%s]--------------------\r\n\r\n",
		mod->alias);
	return 0;
}

int zebu_mod_test_all_mod(zebu_test_ctrl_t *test_ctrl)
{
	int i;
	zebu_test_mod_t *test_mod;
	hprintf(TSENV, "Now Test Auto. %d Mods X %d Test Entrys.\r\n",
		test_ctrl->mod_num, test_ctrl->test_num);
	hprintf(TSENV,
		"=====================================================\r\n");
	for (i = 0; i < test_ctrl->mod_num; i++) {
		test_mod = test_ctrl->mod_arr + i;
		zebu_mod_test_all_entry(test_mod, NULL, test_ctrl);
	}
	hprintf(TSENV,
		"=====================================================\r\n");
	return 0;
}

int zebu_mod_run_current_mod(zebu_test_ctrl_t *test_ctrl)
{
	zebu_test_mod_t *test_mod;
	//zebu_test_entry_t *test_entry;
	test_mod = test_ctrl->mod_arr + test_ctrl->current_mod;
	hprintf(TSENV, "Now Test  Mod(%s) .Run %d Test Entrys.\r\n",
		test_mod->alias);
	hprintf(TSENV,
		"=====================================================\r\n");
	zebu_mod_test_all_entry(test_mod, NULL, test_ctrl);
	hprintf(TSENV,
		"=====================================================\r\n");
	return 0;
}

int mod_logo_msg(zebu_test_ctrl_t *test_ctrl)
{
	hprintf(TSENV, "\r\n%s>", test_ctrl->alias);
	return 0;
}

int test_opt_find_do(char opt, zebu_test_ctrl_t *test_ctrl)
{
	int i;
	zebu_test_mod_t *test_mod;
	zebu_test_entry_t *test_entry;
	if (test_ctrl->mod_num == 0) {
		return -2;
	}
	if (test_ctrl->current_mod >= test_ctrl->mod_num) {
		test_ctrl->current_mod = 0;
	}
	test_mod = test_ctrl->mod_arr + test_ctrl->current_mod;
	for (i = 0; i < test_ctrl->test_num; i++) {
		test_entry = test_ctrl->test_entry_arr + i;
		if (opt == test_entry->opt_c) {
			zebu_mod_test_one_entry(test_mod, i, NULL, test_ctrl);
			return 0;
		}
	}
	return -1;
}

void mod_menu(zebu_test_ctrl_t *test_ctrl)
{
	int i;
	zebu_test_entry_t *test_entry;

	putc('\r');
	putc('\n');
	for (i = 0; i < test_ctrl->test_num; i++) {
		test_entry = test_ctrl->test_entry_arr + i;
		hprintf(DEBUG, "%c......%s\r\n", test_entry->opt_c,
			test_entry->test_alias);
	}
	if (test_ctrl->mod_num > 1) {
		hprintf(DEBUG, "c......Chage test MOD(0-%d),current Mod:%d\r\n",
			test_ctrl->mod_num - 1, test_ctrl->current_mod);
	}
	hprintf(DEBUG, "a......Auto test all Mod\r\n");
	hprintf(DEBUG, "r......Run Current Mod,All Test\r\n");
	hprintf(DEBUG, "d......Dump registert\r\n");
	hprintf(DEBUG, "s......Set32 registert\r\n");
	hprintf(DEBUG, "q......Quit This test\r\n");
	hprintf(DEBUG, "?......dump this HELP message\r\n");
}

#if 0
void changeBase(zebu_test_ctrl_t *test_ctrl)
{
	char c = 0;
	hprintf(DEBUG, "input timer base N?(0~%d)\r\n", test_ctrl->mod_num - 1);
	c = getc();
	putc(c);

	if (c < '0' || c >= '0' + test_ctrl->mod_num) {
		test_ctrl->current_mod = 0;
	} else {
		test_ctrl->current_mod = c - '0';
	}
	//test_opt_find_do('1',test_ctrl);
	//timer_test_registers_dump(test_ctrl->mod_arr+test_ctrl->current_mod,NULL);
}
#else
void changeBase(zebu_test_ctrl_t *test_ctrl)
{
	char c_buf[3] = {0};
	int num, i;
	hprintf(DEBUG, "input timer base N?(0~%d)\r\n", test_ctrl->mod_num - 1);
	getString(c_buf, sizeof(c_buf));

	num = 0;
	i = 0;
	while(c_buf[i] != '\0'){
		num = num * 10 + (c_buf[i] - '0');
		i++;
	}

	if (num < 0 || num >= test_ctrl->mod_num) {
		test_ctrl->current_mod = 0;
	} else {
		test_ctrl->current_mod = num;
	}
}
#endif
void TestLoop(zebu_test_ctrl_t *test_ctrl)
{
	char c = 0;
	int ret;
	mod_menu(test_ctrl);
	while (1) {
		mod_logo_msg(test_ctrl);
		c = getc();
		putc(c);
		if ('\r' == c || '\n' == c) {
			continue;
		}
		putc('\r');
		putc('\n');

		switch (c) {
		case 'c':
			if (test_ctrl->mod_num > 1) {
				changeBase(test_ctrl);
			} else {
				test_ctrl->current_mod = 0;
			}
			break;
		case 'a':
			zebu_mod_test_all_mod(test_ctrl);
			break;
		case 'r':
			zebu_mod_run_current_mod(test_ctrl);
			break;
		case 'd':
			dumpMem(0);
			break;
		case 's':
			setMem32(0);
			break;
		case 'q':
			return;
		case '?':
			mod_menu(test_ctrl);
			break;
		default:
			ret = test_opt_find_do(c, test_ctrl);
			if (ret == -1) {
				hprintf(TSENV, "wrong case\r\n");
			} else if (ret == -2) {
				hprintf(TSENV, "NO Mod To test\r\n");
			}
			break;
		}
	}
}
