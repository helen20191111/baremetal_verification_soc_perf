///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           //
//  This file contains proprietary information that is the sole intellectual property        //
//  of Black Sesame Technologies, Inc. and its affiliates.  No portions of this material     //
//  may be reproduced in any form without the written permission of:                         //
//                                                                                           //
//  Black Sesame Technologies, Inc. and its affiliates                                       //
//  2255 Martin Ave. Suite D                                                                 //
//  Santa Clara, CA  95050                                                                   //
//  Copyright \@2018: all right reserved.                                                    //
//                                                                                           //
//  Notice:                                                                                  //
//                                                                                           //
//  You are running an EVALUATION distribution of the neural network tools provided by       //
//  Black Sesame Technologies, Inc. under NDA.                                               //
//  This copy may NOT be used in production or distributed to any third party.               //
//  For distribution or production, further Software License Agreement is required.          //
//                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////

#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <utility_lite.h>
#include <test_common.h>
#include <int_num.h>
#include <module/wdt.h>
#include <common.h>

/**
 * @brief wdt_set_sys_ctrl Set wdt system ctrl
 * @param id
 * @param on_off True to mask system ctrl, diable wdt system restart
 *               False to unmask, enable wdt system restart
 */
void wdt_set_sys_ctrl(u32 reg_base, u32 on_off)
{
	switch (reg_base) {
	case WDT0_BASE_ADDR:
		REG32_WRITE(SAFETY_WDT_CTRL,
			    (REG32_READ(SAFETY_WDT_CTRL) & (~(1 << 0))) |
				    (on_off ? 0 : 1));
		break;
	case WDT1_BASE_ADDR:
		REG32_WRITE(SAFETY_WDT_CTRL,
			    (REG32_READ(SAFETY_WDT_CTRL) & (~(1 << 1))) |
				    (on_off ? 0 : 1));
		break;
	case WDT2_BASE_ADDR:
		REG32_WRITE(SAFETY_WDT_CTRL,
			    (REG32_READ(SAFETY_WDT_CTRL) & (~(1 << 2))) |
				    (on_off ? 0 : 1));
		break;
	case WDT3_BASE_ADDR:
		REG32_WRITE(SAFETY_WDT_CTRL,
			    (REG32_READ(SAFETY_WDT_CTRL) & (~(1 << 3))) |
				    (on_off ? 0 : 1));
		break;
	default:
		break;
	}
}

/*==================================WDT DRV=====================================================*/

/*SET  Response mode.*/
int bsta1000_wdt_enable(unsigned int reg_base)
{
	unsigned int val;

	val = REG32_READ(reg_base + WDT_CR);
	val |= (1 << 0);
	REG32_WRITE(reg_base + WDT_CR, val);

	return 0;
}

/*SET  Response mode.*/
int bsta1000_wdt_rmod_set(unsigned int reg_base, WDT_RMODE_E rmode)
{
	unsigned int val;

	val = REG32_READ(reg_base + WDT_CR);
	val &= ~(1 << 1);
	val |= (rmode << 1);
	REG32_WRITE(reg_base + WDT_CR, val);

	return 0;
}

/*SET Reset Pulse length*/
int bsta1000_wdt_rpl_set(unsigned int reg_base, WDT_RPL_E rpl)
{
	unsigned int val;
	val = REG32_READ(reg_base + WDT_CR);
	val &= ~(7 << 2);
	val |= (rpl << 2);
	REG32_WRITE(reg_base + WDT_CR, val);
	return 0;
}

/*Kick the dog*/
int bsta1000_wdt_kick_dog(unsigned int reg_base)
{
	REG32_WRITE(reg_base + WDT_CRR, WDT_KICK_MAGIC);
	return 0;
}

/*Set the timeout period*/
int bsta1000_wdt_top_set(unsigned int reg_base, int top)
{
	unsigned int val;
	val = REG32_READ(reg_base + WDT_TORR);
	//val &= ~0xf;
	val = (top);
	REG32_WRITE(reg_base + WDT_TORR, val);

	bsta1000_wdt_kick_dog(reg_base);
	return 0;
}

/*Clear the intterrupt stat*/
int bsta1000_wdt_clear_intr(unsigned int reg_base)
{
	unsigned int dummy;
	dummy = REG32_READ(reg_base + WDT_EOI);
	return 0;
}

/*==================================WDT TEST=====================================================*/
// LSP test instance index 2
static int wdt_test_registers_dump(zebu_test_mod_t *mod, void *para)
{
	unsigned int val, ret;
	unsigned int reg_base;
	reg_base = mod->reg_base;

	val = REG32_READ(reg_base + WDT_CR);
	ret = ((val != 0x10) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n", reg_base + WDT_CR,
		"WDT_CR", val, 0x10, ret ? "Nok" : "ok");
	val = REG32_READ(reg_base + WDT_TORR);
	ret = ((val != 0x0) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n", reg_base + WDT_TORR,
		"WDT_TORR", val, 0x0, ret ? "Nok" : "ok");
	val = REG32_READ(reg_base + WDT_CCVR);
	ret = ((val != 0xffff) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n", reg_base + WDT_CCVR,
		"WDT_CCVR", val, 0xffff, ret ? "Nok" : "ok"); //only read
	//	val = REG32_READ(reg_base + WDT_CRR);
	//	hprintf(TSENV, "REG:%08X(%24s):%08X:%s\r\n",reg_base+WDT_CRR,"WDT_CRR",val,"only write");
	val = REG32_READ(reg_base + WDT_STAT);
	ret = ((val != 0x0) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n", reg_base + WDT_STAT,
		"WDT_STAT", val, 0x0, ret ? "Nok" : "ok"); //only read
	val = REG32_READ(reg_base + WDT_EOI);
	ret = ((val != 0x0) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n", reg_base + WDT_EOI,
		"WDT_EOI", val, 0x0, ret ? "Nok" : "ok"); //only read
	/*	val = REG32_READ(reg_base + WDT_PROT_LEVEL);
	hprintf(TSENV, "REG:%08X(%24s):%08X\r\n",reg_base+WDT_PROT_LEVEL,"WDT_PROT_LEVEL",val);//no 
*/
	val = REG32_READ(reg_base + WDT_COMP_PARAM_5);
	ret = ((val != 0x0) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + WDT_COMP_PARAM_5, "WDT_COMP_PARAM_5", val, 0x0,
		ret ? "Nok" : "ok"); //only read
	val = REG32_READ(reg_base + WDT_COMP_PARAM_4);
	ret = ((val != 0x0) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + WDT_COMP_PARAM_4, "WDT_COMP_PARAM_4", val, 0x0,
		ret ? "Nok" : "ok"); //only read
	val = REG32_READ(reg_base + WDT_COMP_PARAM_3);
	ret = ((val != 0x0) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + WDT_COMP_PARAM_3, "WDT_COMP_PARAM_3", val, 0x0,
		ret ? "Nok" : "ok"); //only read
	val = REG32_READ(reg_base + WDT_COMP_PARAM_2);
	ret = ((val != 0xFFFF) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + WDT_COMP_PARAM_2, "WDT_COMP_PARAM_2", val, 0xFFFF,
		ret ? "Nok" : "ok"); //only read
	val = REG32_READ(reg_base + WDT_COMP_PARAM_1);
	ret = ((val != 0x100012c0) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + WDT_COMP_PARAM_1, "WDT_COMP_PARAM_1", val,
		0x100012c0, ret ? "Nok" : "ok"); //only read
	val = REG32_READ(reg_base + WDT_COMP_VERSI);
	ret = ((val != 0x3131312a) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + WDT_COMP_VERSI, "WDT_COMP_VERSI", val, 0x3131312a,
		ret ? "Nok" : "ok"); //only read
	val = REG32_READ(reg_base + WDT_COMP_TYPE);
	ret = ((val != 0x44570120) ? 1 : 0);
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + WDT_COMP_TYPE, "WDT_COMP_TYPES", val, 0x44570120,
		ret ? "Nok" : "ok"); //only read

	return 0;
}
// LSP test instance index 3
static int wdt_test_registers_rw(zebu_test_mod_t *mod, void *para)
{
	unsigned int val;
	unsigned int reg_base;
	int result = 0;
	int ret = 0;
	reg_base = mod->reg_base;

	REG32_WRITE(reg_base + WDT_CR, 0x2E);
	val = REG32_READ(reg_base + WDT_CR);
	result = ((val == 0x2E) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s \r\n",
		reg_base + WDT_CR, 0x2E, val, 0x2E, result ? "ok" : "Nok");

	REG32_WRITE(reg_base + WDT_TORR, 0x7);
	val = REG32_READ(reg_base + WDT_TORR);
	result = ((val == 0x7) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s \r\n",
		reg_base + WDT_TORR, 0x7, val, 0x7, result ? "ok" : "Nok");

	return ret;
}

// LSP test instance index 4
static int wdt_test_timer_cnt(zebu_test_mod_t *mod, void *para)
{
	int val1, val2;
	int result = 0;
	bsta1000_wdt_top_set(mod->reg_base, 7);
	bsta1000_wdt_enable(mod->reg_base);
	val1 = REG32_READ(mod->reg_base + WDT_CCVR);
	val2 = REG32_READ(mod->reg_base + WDT_CCVR);
	result = ((val2 >= val1) ? 1 : 0);
	hprintf(TSENV, "WDT timer Cnt 8M :%x -> %x :%s\r\n", val1, val2,
		result ? "Nok" : "ok");

	bsta1000_wdt_top_set(mod->reg_base, 0xf);
	val1 = REG32_READ(mod->reg_base + WDT_CCVR);
	bsta1000_wdt_enable(mod->reg_base);
	udelay(5);
	val2 = REG32_READ(mod->reg_base + WDT_CCVR);
	result = ((val2 >= val1) ? 1 : 0);
	hprintf(TSENV, "WDT timer Cnt 2G :%x -> %x :%s\r\n", val1, val2,
		result ? "Nok" : "ok");
	return result;
}
// LSP test instance index 5
int wdt_test_pause_signal(zebu_test_mod_t *mod, void *para)
{
	int result = 0;
	int val, val1, val2;
	bsta1000_wdt_top_set(mod->reg_base, 7);
	bsta1000_wdt_enable(mod->reg_base);
	val1 = REG32_READ(mod->reg_base + WDT_CCVR);
	val = REG32_READ(0x20020004);
	val |= (1 << 15);
	REG32_WRITE(0x20020004, val);
	val2 = REG32_READ(mod->reg_base + WDT_CCVR);
	result = ((val2 >= val1) ? 1 : 0);
	hprintf(TSENV, "WDT timer Cnt 8M :%x -> %x :%s\r\n", val1, val2,
		result ? "Nok" : "ok");
	val = REG32_READ(0x20020004);
	val &= ~(1 << 15);
	REG32_WRITE(0x20020004, val);
	val1 = REG32_READ(mod->reg_base + WDT_CCVR);
	val2 = REG32_READ(mod->reg_base + WDT_CCVR);
	result = ((val2 >= val1) ? 1 : 0);
	hprintf(TSENV,
		"disable pause signal WDT timer Cnt 8M :%x -> %x :%s\r\n", val1,
		val2, result ? "Nok" : "ok");
	return result;
}

// // LSP test instance index 6
static int wdt_test_kick_dog_disable(zebu_test_mod_t *mod, void *para)
{
	int val, val1, val2;
	int i;
	int ret = 0;
	bsta1000_wdt_top_set(mod->reg_base, 7);
	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_RESET);
	for (i = 0; i < 2; i++) {
		bsta1000_wdt_enable(mod->reg_base);
		udelay(30);
		val1 = REG32_READ(mod->reg_base + WDT_CCVR);
		udelay(30);
		val2 = REG32_READ(mod->reg_base + WDT_CCVR);
		ret = ((val2 >= val1) ? 1 : 0);
		bsta1000_wdt_kick_dog(mod->reg_base);
		val = REG32_READ(mod->reg_base + WDT_CCVR);
		hprintf(TSENV, "WDT timer Cnt:%x -> %x :%s\r\n", val1, val2,
			ret ? "Nok" : "ok");
		hprintf(TSENV, "after kick dog ,val -------:%08X \r\n", val);
		bsta1000_wdt_top_set(mod->reg_base, 0xf);
	}
	bsta1000_wdt_top_set(mod->reg_base, 0x0);
	return ret;
}
// LSP test instance index 7
static int wdt_test_kick_dog(zebu_test_mod_t *mod, void *para)
{
	int val1, val2;
	int i;
	int ret = 0;
	bsta1000_wdt_top_set(mod->reg_base, 7);
	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_RESET);
	bsta1000_wdt_enable(mod->reg_base);
	for (i = 0; i < WDT_TEST_KICK_DOG_CNT; i++) {
		udelay(5);
		val1 = REG32_READ(mod->reg_base + WDT_CCVR);
		udelay(1);

		bsta1000_wdt_kick_dog(mod->reg_base);
		val2 = REG32_READ(mod->reg_base + WDT_CCVR);
		ret += val1 >= val2;
		if (!(i & 0xf)) {
			hprintf(TSENV, "*");
		}
	}
	hprintf(TSENV, "WDT Kick teset %d times: error %d times \r\n",
		WDT_TEST_KICK_DOG_CNT, ret);
	return ret;
}

// LSP test instance index 9
static int wdt_interrupt_clean_no_reset(zebu_test_mod_t *mod, void *para)
{
	int intr_stat;
	u32 i = 0;
	int ret = 0;

	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_INTR);
	bsta1000_wdt_top_set(mod->reg_base, 3);
	bsta1000_wdt_enable(mod->reg_base);
	while (1) {
		intr_stat = REG32_READ(mod->reg_base + WDT_STAT);
		if (intr_stat & 1) {
			hprintf(TSENV, "delay times %d\r\n", i);
			break;
		} else {
			i++;
		}
	}
	udelay(1);
	bsta1000_wdt_clear_intr(mod->reg_base);
	intr_stat = REG32_READ(mod->reg_base + WDT_STAT);
	if (intr_stat & 1) {
		hprintf(TSENV, "failed to clean interrupt flag.\r\n");
		return ret++;
	} else {
		hprintf(TSENV, "succeed to clean interrupt flag.\r\n");
	}
	udelay(1);

	hprintf(TSENV, "after T/2 system don't reset \r\n");
	udelay(1);

	hprintf(TSENV, "after T system don't reset \r\n");
	bsta1000_wdt_clear_intr(mod->reg_base);
	hprintf(TSENV, "test -----ok \r\n");

	return ret;
}

// LSP test instance index 11
static int wdt_interrupt_clean_and_reset(zebu_test_mod_t *mod, void *para)
{
	u32 val1 = 0, val2 = 0;
	int intr_stat;
	int i;
	int ret = 0;

	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_INTR);
	bsta1000_wdt_top_set(mod->reg_base, 1);
	bsta1000_wdt_enable(mod->reg_base);

	hprintf(TSENV, "wait for interrupt.\r\n");
	for (i = 0; i < 100; i++) {
		val1 = REG32_READ(mod->reg_base + WDT_CCVR);
		while(1){	
			val2 = REG32_READ(mod->reg_base + WDT_CCVR);
			if(val2 > val1){
				hprintf(TSENV, "wdt already reload.\r\n");
				break;
			}
			val1 = val2;
		}

		intr_stat = REG32_READ(mod->reg_base + WDT_STAT);
		if (intr_stat & 1) {
			bsta1000_wdt_clear_intr(mod->reg_base);
			hprintf(TSENV, "WDT  Clear num...%d\r\n", i);
			
			intr_stat = REG32_READ(mod->reg_base + WDT_STAT);
			if (intr_stat & 1) {
				hprintf(TSENV, "WDT  Clear intr Faild. Inits...\r\n");
				ret++;
				//如果清中断失败，后续就没必要再进行测试了
				return ret;
			}
		}
	}
	udelay(10);
	return ret;
}

static int wdt_interrupt_reset(zebu_test_mod_t *mod, void *para, int i)
{
	u32 val1 = 0, val2 = 0;
	int ret = 0;
	int intr_stat;
	
	hprintf(TSENV, "wait for interrupt.\r\n");
	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_INTR);
	bsta1000_wdt_top_set(mod->reg_base, i);
	bsta1000_wdt_enable(mod->reg_base);

	//delaySoft(1);
	while (1) {
		val1 = REG32_READ(mod->reg_base + WDT_CCVR);
		udelay(5);
		val2 = REG32_READ(mod->reg_base + WDT_CCVR);

		ret = ((val2 >= val1) ? 1 : 0);
		if(!ret){
			intr_stat = REG32_READ(mod->reg_base + WDT_STAT);
			if (intr_stat & 1) {
				bsta1000_wdt_clear_intr(mod->reg_base);
				hprintf(TSENV, "WDT INTERRUPT generated!!!\r\n");
				break;
			}
		}else{
			hprintf(TSENV, "wdt counter error!\r\n");
		}
		
	}
	hprintf(TSENV, "Wait for reset generated!!!\r\n");

	return ret;
}

static void clock_menu(void)
{
	hprintf(DEBUG, "0......WDT_TORR  64k Clocks\r\n");
	hprintf(DEBUG, "1......WDT_TORR  128k Clocks\r\n");
	hprintf(DEBUG, "2......WDT_TORR  256k Clocks\r\n");
	hprintf(DEBUG, "3......WDT_TORR  512k Clocks\r\n");
	hprintf(DEBUG, "4......WDT_TORR  1M Clocks\r\n");
	hprintf(DEBUG, "5......WDT_TORR  2M Clocks\r\n");
	hprintf(DEBUG, "6......WDT_TORR  4M Clocks\r\n");
	hprintf(DEBUG, "7......WDT_TORR  8M Clocks\r\n");
	hprintf(DEBUG, "8......WDT_TORR  16M Clocks\r\n");
	hprintf(DEBUG, "9......WDT_TORR  32M Clocks\r\n");
	hprintf(DEBUG, "a......WDT_TORR  64M Clocks\r\n");
	hprintf(DEBUG, "b......WDT_TORR  128M Clocks\r\n");
	hprintf(DEBUG, "c......WDT_TORR  256M Clocks\r\n");
	hprintf(DEBUG, "d......WDT_TORR  512M Clocks\r\n");
	hprintf(DEBUG, "e......WDT_TORR  1G Clocks\r\n");
	hprintf(DEBUG, "f......WDT_TORR  2G Clocks\r\n");
}

// LSP test instance index 10
static int wdt_test_to_interrupt_mode(zebu_test_mod_t *mod, void *para)
{
	char c = 0;
	int ret = 0;
	clock_menu();
	c = getc();
	putc(c);

	if ((c >= '0') && (c <= '9')) {
		ret = wdt_interrupt_reset(mod, para, c - '0');
	} else if ((c >= 'a') && (c <= 'f')) {
		ret = wdt_interrupt_reset(mod, para, 10 + c - 'a');
	}

	return ret;
}

static int wdt_set_timeout_period(zebu_test_mod_t *mod, void *para, int i)
{
	u32 j = 0;
	u32 val1 = 0, val2 = 0;
	int ret = 0;

	//	bsta1000_wdt_kick_dog(mod->reg_base);
	bsta1000_wdt_top_set(mod->reg_base, i);
	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_RESET);
	bsta1000_wdt_enable(mod->reg_base);
	hprintf(TSENV, "\n");
	while (1) {
		val1 = REG32_READ(mod->reg_base + WDT_CCVR);
		udelay(5);
		val2 = REG32_READ(mod->reg_base + WDT_CCVR);

		ret = ((val2 >= val1) ? 1 : 0);
		if(!ret){
			j++;
			if (!(j & 0xffff))
				hprintf(TSENV, "j--- %d\r", j);
		}else{
			hprintf(DEBUG, "wdt counter error!\r\n");
		}
	}

	return ret;
}
// LSP test instance index 8
static int wdt_reset_mode_timeout(zebu_test_mod_t *mod, void *para)
{
	char c = 0;
	int ret = 0;
	clock_menu();

	c = getc();
	putc(c);

	if ((c >= '0') && (c <= '9')) {
		ret = wdt_set_timeout_period(mod, para, c - '0');
	} else if ((c >= 'a') && (c <= 'f')) {
		ret = wdt_set_timeout_period(mod, para, 10 + c - 'a');
	}

	return ret;
}

int wdt_test_to_reset_mode(zebu_test_mod_t *mod, void *para)
{
	int val1, val2;
	int intr_stat;
	int ret = 0;

	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_RESET);
	bsta1000_wdt_top_set(mod->reg_base, 7);
	bsta1000_wdt_enable(mod->reg_base);

	intr_stat = REG32_READ(mod->reg_base + WDT_STAT);
	if (intr_stat & 1) {
		bsta1000_wdt_clear_intr(mod->reg_base);
		udelay(1);
		intr_stat = REG32_READ(mod->reg_base + WDT_STAT);
		if (intr_stat & 1) {
			hprintf(TSENV, "WDT  Clear intr Faild. Inits...\r\n");
			ret++;
			/*如果清中断失败，后续就没必要再进行测试了*/
			return ret;
		}
	}

	hprintf(TSENV, "WDT Now Test For Reset.\r\n");
	val1 = REG32_READ(mod->reg_base + WDT_CCVR);
	val2 = REG32_READ(mod->reg_base + WDT_CCVR);
	while (1) {
		if (val2 > val1) {
			break;
		}
		val1 = val2;
		udelay(1);
		val2 = REG32_READ(mod->reg_base + WDT_CCVR);
	}

	intr_stat = REG32_READ(mod->reg_base + WDT_STAT);
	if ((intr_stat & 1) == 0) {
		ret++;
		hprintf(TSENV, "WDT INTERRUPT not generated!!!\r\n");
	}

	hprintf(TSENV, "WDT Now Waiting For Reset.\r\n");
	val1 = REG32_READ(mod->reg_base + WDT_CCVR);
	val2 = REG32_READ(mod->reg_base + WDT_CCVR);
	while (1) {
		if (val2 > val1) {
			break;
		}
		val1 = val2;
		udelay(1);
		val2 = REG32_READ(mod->reg_base + WDT_CCVR);
	}

	hprintf(TSENV, "WDT Kick teset %d times: error %d times \r\n",
		WDT_TEST_KICK_DOG_CNT, ret);
	return ret;
}

static void pulse_menu(void)
{
	hprintf(DEBUG, "0.......WDT_CR  2 pclk cycles\r\n");
	hprintf(DEBUG, "1.......WDT_CR  4 pclk cycles\r\n");
	hprintf(DEBUG, "2.......WDT_CR  8 pclk cycles\r\n");
	hprintf(DEBUG, "3.......WDT_CR  16 pclk cycles\r\n");
	hprintf(DEBUG, "4.......WDT_CR  32 pclk cycles\r\n");
	hprintf(DEBUG, "5.......WDT_CR  64 pclk cycles\r\n");
	hprintf(DEBUG, "6.......WDT_CR  128 pclk cycles\r\n");
	hprintf(DEBUG, "7.......WDT_CR  256 pclk cycles\r\n");
}

static int wdt_set_pulse(zebu_test_mod_t *mod, WDT_RPL_E i)
{
	int ret = 0;
	bsta1000_wdt_top_set(mod->reg_base, 0);
	bsta1000_wdt_rpl_set(mod->reg_base, i);
	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_RESET);
	bsta1000_wdt_enable(mod->reg_base);

	return ret;
}

// LSP test instance index 12
static int wdt_set_pulse_reset_mode(zebu_test_mod_t *mod, void *para)
{
	char c = 0;
	int ret = 0;
	pulse_menu();

	c = getc();
	putc(c);

	switch (c) {
	case '0':
		ret = wdt_set_pulse(mod, WDT_RPL_2_CYCLES);
		break;
	case '1':
		ret = wdt_set_pulse(mod, WDT_RPL_4_CYCLES);
		break;
	case '2':
		ret = wdt_set_pulse(mod, WDT_RPL_8_CYCLES);
		break;
	case '3':
		ret = wdt_set_pulse(mod, WDT_RPL_16_CYCLES);
		break;
	case '4':
		ret = wdt_set_pulse(mod, WDT_RPL_32_CYCLES);
		break;
	case '5':
		ret = wdt_set_pulse(mod, WDT_RPL_64_CYCLES);
		break;
	case '6':
		ret = wdt_set_pulse(mod, WDT_RPL_128_CYCLES);
		break;
	case '7':
		ret = wdt_set_pulse(mod, WDT_RPL_256_CYCLES);
		break;
	}
	return ret;
}

//test interrupt connect line
volatile bool_enum wdt_flag = HVTE_FALSE;

u32 wdt_get_int_num_by_reg_base(u32 reg_base)
{
	switch (reg_base) {
	case WDT0_BASE_ADDR:
		return FIQ_IRQ_LSP0_WDT0;
	case WDT1_BASE_ADDR:
		return FIQ_IRQ_LSP0_WDT1;
	case WDT2_BASE_ADDR:
		return FIQ_IRQ_LSP1_WDT0;
	case WDT3_BASE_ADDR:
		return FIQ_IRQ_LSP1_WDT1;
#if defined(CONFIG_A55)
	case A55_WDT0_BASE_ADDR:
		return FIQ_IRQ_A55_WDT0;
	case A55_WDT1_BASE_ADDR:
		return FIQ_IRQ_A55_WDT1;
	case A55_WDT2_BASE_ADDR:
		return FIQ_IRQ_A55_WDT2;
	case A55_WDT3_BASE_ADDR:
		return FIQ_IRQ_A55_WDT3;
	case A55_WDT4_BASE_ADDR:
		return FIQ_IRQ_A55_WDT4;
	case A55_WDT5_BASE_ADDR:
		return FIQ_IRQ_A55_WDT5;
	case A55_WDT6_BASE_ADDR:
		return FIQ_IRQ_A55_WDT6;
	case A55_WDT7_BASE_ADDR:
		return FIQ_IRQ_A55_WDT7;
#endif
	default:
		break;
	}
	return 0;
}

void irq_wdt(int reg_base)
{
	unsigned int regval;
	u32 int_num;
	printf_intr("\r\n");
	printf_intr("wdt irq triggered, reg_base: 0x%08x \r\n", reg_base);
	int_num = wdt_get_int_num_by_reg_base(reg_base);
#ifdef CONFIG_R5_SAFETY
	SAFETY_IRQ_DISABLE(int_num);
#endif
#ifdef CONFIG_A55
	A55_IRQ_DISABLE(int_num);
#endif
	regval = REG32_READ(reg_base + WDT_STAT);
	wdt_flag = HVTE_TRUE;
	return;
}

static int wdt_irq_test(zebu_test_mod_t *mod, void *para)
{
	int ret = 0;
	u32 int_num;
	int_num = wdt_get_int_num_by_reg_base(mod->reg_base);
	hprintf(DEBUG, "interrupt number : %d \r\n", int_num);
#ifdef CONFIG_R5_SAFETY
	SAFETY_IRQ_ENABLE(int_num);
#endif
#ifdef CONFIG_A55
	A55_IRQ_ENABLE(int_num);
#endif
	wdt_flag = HVTE_FALSE;
	bsta1000_wdt_rmod_set(mod->reg_base, WDT_RMODE_INTR);
	bsta1000_wdt_top_set(mod->reg_base, 3);
	bsta1000_wdt_enable(mod->reg_base);
	while (1) {
		if (wdt_flag) {
			break;
		}
	}

	hprintf(TSENV, "wdt interrupt connect line test -----ok \r\n");
	return ret;
}

#if defined(CONFIG_R5_SAFETY)
static int wdt_top_irq_enable(zebu_test_mod_t *p_mod, void *p_para)
{
	SAFETY_IRQ_ENABLE(58);
	return HVTE_MODULE_TEST_PASS;
}

void wdt_top_irq_handler(int para)
{
	// printf_intr("\r\n");
	// printf_intr("wdt top irq triggered \r\n");
	putc('c');
	SAFETY_IRQ_DISABLE(58);
}
#endif

/*==================================WDT ZEBU TEST=====================================================*/

zebu_test_mod_t zebu_test_wdt_mods[] = {
	{ "WDT0", WDT0_BASE_ADDR },	{ "WDT1", WDT1_BASE_ADDR },
	{ "WDT2", WDT2_BASE_ADDR },	{ "WDT3", WDT3_BASE_ADDR },
#if defined(CONFIG_A55)
	{ "A55WDT0", A55_WDT0_BASE_ADDR }, { "A55WDT1", A55_WDT1_BASE_ADDR },
	{ "A55WDT2", A55_WDT2_BASE_ADDR }, { "A55WDT3", A55_WDT3_BASE_ADDR },
	{ "A55WDT4", A55_WDT4_BASE_ADDR }, { "A55WDT5", A55_WDT5_BASE_ADDR },
	{ "A55WDT6", A55_WDT6_BASE_ADDR }, { "A55WDT7", A55_WDT7_BASE_ADDR },
#endif
#if defined(CONFIG_R5_SAFETY)
//    {"SEC_WDT", 0xf0004000}
#endif
};

zebu_test_entry_t zebu_test_wdt_entrys[] = {
	// not must
	{ '0', 1, "Reg Dump ", wdt_test_registers_dump }, //test2
	{ '1', 1, "Reg W/R test", wdt_test_registers_rw }, //test3
	// must
	{ '2', 0, "wdt_kick_dog_then_reset",
	  wdt_test_kick_dog_disable }, //test6
	{ '3', 0, "Kick Dog test", wdt_test_kick_dog }, //test7
	{ '4', 0, "reset mode timeout-reset", wdt_reset_mode_timeout }, //test8
	{ '5', 0, "Timeout Interrupt mode firest interrupt clean",
	  wdt_interrupt_clean_no_reset }, //tset9
	{ '6', 0, "Interrupt mode timeout-reset",
	  wdt_test_to_interrupt_mode }, //test10
	{ '7', 0, "Timeout mult-Interrupt and reset",
	  wdt_interrupt_clean_and_reset }, //test11
	{ '8', 0, "reset mode set pulse width",
	  wdt_set_pulse_reset_mode }, //test12
	{ '9', 0, "interrupt connect line with cpu",
	  wdt_irq_test }, // test wdt interrupt connect
	// not must
	{ 'b', 0, "Timer cnt test", wdt_test_timer_cnt }, //test4
#if defined(CONFIG_R5_SAFETY)
	{ 'e', 0, "top wdt interrupt enable", wdt_top_irq_enable }
#endif
//	{'p' , 0, "pause signal" , wdt_test_pause_signal},//test5
};

static zebu_test_ctrl_t zebu_mod_test_ctrl = { "wdt",
					       SIZE_ARRAY(zebu_test_wdt_mods),
					       zebu_test_wdt_mods,
					       SIZE_ARRAY(zebu_test_wdt_entrys),
					       zebu_test_wdt_entrys,
					       0 };

void wdtMain(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;
	wdt_set_sys_ctrl(WDT0_BASE_ADDR, 1);
	wdt_set_sys_ctrl(WDT1_BASE_ADDR, 1);
	wdt_set_sys_ctrl(WDT2_BASE_ADDR, 1);
	wdt_set_sys_ctrl(WDT3_BASE_ADDR, 1);
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "auto test err %d!!!!\r\n", ret);
	} else {
		hprintf(TSENV, "enter LSP WDT test!\r\n");
		TestLoop(&zebu_mod_test_ctrl);
	}
}

CMD_DEFINE(wdt, wdtMain, 0, "wdt test", 0);

#if defined(CONFIG_R5_SAFETY)
IRQ_DEFINE(57 , irq_wdt , 0xf0004000, "irq sec wdt isr", ISR_ATTR_SAFETY|ISR_ATTR_LEVEL);
IRQ_DEFINE(58 , wdt_top_irq_handler , 58, "irq top wdt isr", ISR_ATTR_SAFETY|ISR_ATTR_LEVEL);
#endif

IRQ_DEFINE(FIQ_IRQ_LSP0_WDT0, irq_wdt, WDT0_BASE_ADDR, "irq wdt0 isr",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_WDT1, irq_wdt, WDT1_BASE_ADDR, "irq wdt1 isr",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP1_WDT0, irq_wdt, WDT2_BASE_ADDR, "irq wdt2 isr",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP1_WDT1, irq_wdt, WDT3_BASE_ADDR, "irq wdt3 isr",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);

#if defined(CONFIG_A55)
IRQ_DEFINE(FIQ_IRQ_A55_WDT0, irq_wdt, A55_WDT0_BASE_ADDR, "irq wdt0 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_WDT1, irq_wdt, A55_WDT1_BASE_ADDR, "irq wdt1 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_WDT2, irq_wdt, A55_WDT2_BASE_ADDR, "irq wdt2 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_WDT3, irq_wdt, A55_WDT3_BASE_ADDR, "irq wdt3 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_WDT4, irq_wdt, A55_WDT4_BASE_ADDR, "irq wdt0 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_WDT5, irq_wdt, A55_WDT5_BASE_ADDR, "irq wdt1 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_WDT6, irq_wdt, A55_WDT6_BASE_ADDR, "irq wdt2 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_WDT7, irq_wdt, A55_WDT7_BASE_ADDR, "irq wdt3 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
#endif
