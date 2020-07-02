///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           //
//  This file contains proprietary information that is the sole intellectual
//  property        //
//  of Black Sesame Technologies, Inc. and its affiliates.  No portions of this
//  material     //
//  may be reproduced in any form without the written permission of: //
//                                                                                           //
//  Black Sesame Technologies, Inc. and its affiliates //
//  2255 Martin Ave. Suite D //
//  Santa Clara, CA  95050 //
//  Copyright \@2018: all right reserved. //
//                                                                                           //
//  Notice: //
//                                                                                           //
//  You are running an EVALUATION distribution of the neural network tools
//  provided by       //
//  Black Sesame Technologies, Inc. under NDA. //
//  This copy may NOT be used in production or distributed to any third party.
//  //
//  For distribution or production, further Software License Agreement is
//  required.          //
//                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////

#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <int_num.h>
#include <module/pwm.h>
#include <test_common.h>
#include <utility_lite.h>
#include <common.h>

volatile unsigned int iqr_flag = 0;

static void pwm_pinmux_init(unsigned int reg_base)
{
	unsigned int val = 0;

	if (reg_base == TIMER0_PWM0_BASE_ADDR) {
		val = REG32_READ(AON_PMM_REG_BASE_ADDR + 0x4);
		val = val & (~(PINMUX_PWM0));
		REG32_WRITE(AON_PMM_REG_BASE_ADDR + 0x4, val);
	} else if (reg_base == TIMER1_PWM1_BASE_ADDR) {
		val = REG32_READ(AON_PMM_REG_BASE_ADDR + 0x4);
		val = val & (~(PINMUX_PWM1));
		REG32_WRITE(AON_PMM_REG_BASE_ADDR + 0x4, val);
	} else if (reg_base == TIMER2_PWM2_BASE_ADDR) {
		val = REG32_READ(AON_PMM_REG_BASE_ADDR + 0x4);
		val = val & (~(PINMUX_PWM2));
		REG32_WRITE(AON_PMM_REG_BASE_ADDR + 0x4, val);
	} else if (reg_base == TIMER3_PWM3_BASE_ADDR) {
		val = REG32_READ(AON_PMM_REG_BASE_ADDR + 0x4);
		val = val & (~(PINMUX_PWM3));
		REG32_WRITE(AON_PMM_REG_BASE_ADDR + 0x4, val);
	}
}

static int bst_a1000_timer_ch_max_mod(zebu_test_mod_t *mod)
{
	u32 regbase = mod->reg_base;

	switch (regbase) {
	case A55_TIMER_BASE_ADDR:
		return CFG_TIMER_A55_CNT;
	case TIMER0_PWM0_BASE_ADDR:
	case TIMER1_PWM1_BASE_ADDR:
		return CFG_TIMER_LSP_CNT;
	default:
		break;
	}
	return 0;
}

void bsta1000_timer_enable(unsigned int base, int timer_channel)
{
	unsigned int val;
	val = REG32_READ(base + TIMERNCTROLREG(timer_channel));
	val |= 1;
	REG32_WRITE(base + TIMERNCTROLREG(timer_channel), val);
}

void bsta1000_timer_disable(unsigned int base, int timer_channel)
{
	unsigned int val;
	val = REG32_READ(base + TIMERNCTROLREG(timer_channel));
	val &= ~1;
	REG32_WRITE(base + TIMERNCTROLREG(timer_channel), val);
}

/*
 * mode 1:user_defined mode
 *      0:free_running mode
 */
void bsta1000_timer_mode_set(unsigned int base, int timer_channel, int mode)
{
	unsigned int val;
	val = REG32_READ(base + TIMERNCTROLREG(timer_channel));
	if (mode) {
		val |= (1 << 1);
	} else {
		val &= ~(1 << 1);
	}
	REG32_WRITE(base + TIMERNCTROLREG(timer_channel), val);
}

/*
 * mask 1:masked
 *      0:unmasked
 */
void bsta1000_timer_intr_mask_set(unsigned int base, int timer_channel,
				  int mask)
{
	unsigned int val;
	val = REG32_READ(base + TIMERNCTROLREG(timer_channel));
	if (mask) {
		val |= (1 << 2);
	} else {
		val &= ~(1 << 2);
	}
	REG32_WRITE(base + TIMERNCTROLREG(timer_channel), val);
}

void bsta1000_timer_pwm_enable(unsigned int base, int timer_channel, int enable)
{
	unsigned int val;
	val = REG32_READ(base + TIMERNCTROLREG(timer_channel));
	if (enable) {
		val |= (1 << 3);
	} else {
		val &= ~(1 << 3);
	}
	REG32_WRITE(base + TIMERNCTROLREG(timer_channel), val);
}

void bsta1000_timer_0n100_pwm_enable(unsigned int base, int timer_channel,
				     int enable)
{
	unsigned int val;
	val = REG32_READ(base + TIMERNCTROLREG(timer_channel));
	if (enable) {
		val |= 1 << 4;
	} else {
		val &= ~(1 << 4);
	}
	REG32_WRITE(base + TIMERNCTROLREG(timer_channel), val);
}

void bsta1000_timer_load_val(unsigned int base, int timer_channel,
			     unsigned int load_val)
{
	REG32_WRITE(base + TIMERNLOADCOUNT(timer_channel), load_val);
}

unsigned int bsta1000_timer_current_val(unsigned int base, int timer_channel)
{
	return REG32_READ(base + TIMERNCURRENTVALUE(timer_channel));
}

void bsta1000_timer_load_val2(unsigned int base, int timer_channel,
			      unsigned int load_val)
{
	REG32_WRITE(base + TIMERNLOADCOUNT2(timer_channel), load_val);
}

int bsta1000_timer_intr_status(unsigned int base, int timer_channel)
{
	unsigned int val;
	val = REG32_READ(base + TIMERNINTSTATUS(timer_channel));
	return val;
}

int bsta1000_timer_clear_intr(unsigned int base, int timer_channel)
{
	unsigned int val;
	val = REG32_READ(base + TIMERNEOI(timer_channel));
	return val;
}

int bsta1000_timer_all_intr_status(unsigned int base)
{
	unsigned int val;
	val = REG32_READ(base + TIMERSINTSTATUS);
	return val;
}

int bsta1000_timer_clear_all_intr(unsigned int base)
{
	unsigned int val;
	val = REG32_READ(base + TIMERSEOI);
	return val;
}

int bsta1000_timer_all_raw_intr(unsigned int base)
{
	unsigned int val;
	val = REG32_READ(base + TIMERSRAWINTSTATUS);
	return val;
}

static void a55_timer_interrupt_enable(u32 enable)
{
	if (enable) {
		A55_IRQ_ENABLE(FIQ_IRQ_A55_TIMER0);
		A55_IRQ_ENABLE(FIQ_IRQ_A55_TIMER1);
		A55_IRQ_ENABLE(FIQ_IRQ_A55_TIMER2);
		A55_IRQ_ENABLE(FIQ_IRQ_A55_TIMER3);
		A55_IRQ_ENABLE(FIQ_IRQ_A55_TIMER4);
		A55_IRQ_ENABLE(FIQ_IRQ_A55_TIMER5);
		A55_IRQ_ENABLE(FIQ_IRQ_A55_TIMER6);
		A55_IRQ_ENABLE(FIQ_IRQ_A55_TIMER7);
	} else {
		A55_IRQ_DISABLE(FIQ_IRQ_A55_TIMER0);
		A55_IRQ_DISABLE(FIQ_IRQ_A55_TIMER1);
		A55_IRQ_DISABLE(FIQ_IRQ_A55_TIMER2);
		A55_IRQ_DISABLE(FIQ_IRQ_A55_TIMER3);
		A55_IRQ_DISABLE(FIQ_IRQ_A55_TIMER4);
		A55_IRQ_DISABLE(FIQ_IRQ_A55_TIMER5);
		A55_IRQ_DISABLE(FIQ_IRQ_A55_TIMER6);
		A55_IRQ_DISABLE(FIQ_IRQ_A55_TIMER7);
	}
}

void timer_interrupt_enable_by_addr(u32 addr, u32 enable)
{
	u32 int_num0, int_num1;
	switch (addr) {
	case TIMER0_PWM0_BASE_ADDR:
		int_num0 = FIQ_IRQ_LSP0_TIMER0;
		int_num1 = FIQ_IRQ_LSP0_TIMER1;
		break;
	case TIMER1_PWM1_BASE_ADDR:
		int_num0 = FIQ_IRQ_LSP1_TIMER0;
		int_num1 = FIQ_IRQ_LSP1_TIMER1;
		break;
	case A55_TIMER_BASE_ADDR:
		a55_timer_interrupt_enable(enable);
		return;
	default:
		int_num0 = FIQ_IRQ_LSP0_TIMER0;
		int_num1 = FIQ_IRQ_LSP0_TIMER1;
		break;
	}
	if (enable) {
		SAFETY_IRQ_ENABLE(int_num0);
		SAFETY_IRQ_ENABLE(int_num1);
		A55_IRQ_ENABLE(int_num0);
		A55_IRQ_ENABLE(int_num1);
	} else {
		SAFETY_IRQ_DISABLE(int_num0);
		SAFETY_IRQ_DISABLE(int_num1);
		A55_IRQ_DISABLE(int_num0);
		A55_IRQ_DISABLE(int_num1);
	}
}

static int timer_test_registers_dump(zebu_test_mod_t *mod, void *para)
{
	unsigned int val, result;
	int i;
	int ret = 0;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);

	for (i = 1; i <= timer_ch_cnt; i++) {
		val = REG32_READ(reg_base + TIMERNLOADCOUNT(i));
		result = ((val != 0x0) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X:(%08X):%s \r\n",
			reg_base + TIMERNLOADCOUNT(i), "TIMERNLOADCOUNT", i,
			val, 0x0, result ? "Nok" : "ok");
		val = REG32_READ(reg_base + TIMERNCURRENTVALUE(i));
		result = ((val != 0xFFFFFFFF) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X:(%08X):%s\r\n",
			reg_base + TIMERNCURRENTVALUE(i), "TIMERNCURRENTVALUE",
			i, val, 0xFFFFFFFF, result ? "Nok" : "ok");
		val = REG32_READ(reg_base + TIMERNCTROLREG(i));
		result = ((val != 0x0) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X:(%08X):%s\r\n",
			reg_base + TIMERNCTROLREG(i), "TIMERNCTROLREG", i, val,
			0x0, result ? "Nok" : "ok");
		val = REG32_READ(reg_base + TIMERNEOI(i));
		result = ((val != 0x0) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X:(%08X):%s \r\n",
			reg_base + TIMERNEOI(i), "TIMERNEOI", i, val, 0x0,
			result ? "Nok" : "ok");
		val = REG32_READ(reg_base + TIMERNINTSTATUS(i));
		result = ((val != 0x0) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X:(%08X):%s \r\n",
			reg_base + TIMERNINTSTATUS(i), "TIMERNINTSTATUS", i,
			val, 0x0, result ? "Nok" : "ok");
		val = REG32_READ(reg_base + TIMERNLOADCOUNT2(i));
		result = ((val != 0x0) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X:(%08X):%s \r\n",
			reg_base + TIMERNLOADCOUNT2(i), "TIMERNLOADCOUNT2", i,
			val, 0x0, result ? "Nok" : "ok");
	}
	val = REG32_READ(reg_base + TIMERSINTSTATUS);
	result = ((val != 0x0) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + TIMERSINTSTATUS, "TIMERSINTSTATUS", val, 0x0,
		result ? "Nok" : "ok");
	val = REG32_READ(reg_base + TIMERSEOI);
	result = ((val != 0x0) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + TIMERSEOI, "TIMERSEOI", val, 0x0,
		result ? "Nok" : "ok");
	val = REG32_READ(reg_base + TIMERSRAWINTSTATUS);
	result = ((val != 0x0) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + TIMERSRAWINTSTATUS, "TIMERSRAWINTSTATUS", val, 0x0,
		result ? "Nok" : "ok");
	val = REG32_READ(reg_base + TIMERS_COMP_VERSI);
	result = ((val != 0x3231322A) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X(%24s):%08X:(%08X):%s\r\n",
		reg_base + TIMERS_COMP_VERSI, "TIMERS_COMP_VERSI", val,
		0x3231322A, result ? "Nok" : "ok");
	return 0;
}

static int timer_test_registers_wr(zebu_test_mod_t *mod, void *para)
{
	unsigned int val;
	int i;
	unsigned int result = 0;
	int ret = 0;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);

	for (i = 1; i <= timer_ch_cnt; i++) {
		REG32_WRITE(reg_base + TIMERNLOADCOUNT(i), 0x7FFFFFFF);
		val = REG32_READ(reg_base + TIMERNLOADCOUNT(i));
		result = ((val != 0x7FFFFFFF) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s \r\n",
			reg_base + TIMERNLOADCOUNT(i), 0x7FFFFFFF, val,
			0x7FFFFFFF, result ? "Nok" : "ok");

		REG32_WRITE(reg_base + TIMERNCURRENTVALUE(i), 0x8234);
		val = REG32_READ(reg_base + TIMERNCURRENTVALUE(i));
		result = ((val != 0xFFFFFFFF) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s:%s \r\n",
			reg_base + TIMERNCURRENTVALUE(i), 0x8234, val, 0x8234,
			result ? "Nok" : "ok", "only read");

		REG32_WRITE(reg_base + TIMERNCTROLREG(i), 0x0F);
		val = REG32_READ(reg_base + TIMERNCTROLREG(i));
		result = ((val != 0x0F) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s \r\n",
			reg_base + TIMERNCTROLREG(i), 0x0F, val, 0x0F,
			result ? "Nok" : "ok");

		REG32_WRITE(reg_base + TIMERNEOI(i), 0xFFFFFFFF);
		val = REG32_READ(reg_base + TIMERNEOI(i));
		result = ((val != 0) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s:%s \r\n",
			reg_base + TIMERNEOI(i), 0xFFFFFFFF, val, 0xFFFFFFFF,
			result ? "Nok" : "ok", "only read");

		REG32_WRITE(reg_base + TIMERNINTSTATUS(i), 0xFFFFFFFF);
		val = REG32_READ(reg_base + TIMERNINTSTATUS(i));
		result = ((val != 0) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s:%s \r\n",
			reg_base + TIMERNINTSTATUS(i), 0xFFFFFFFF, val,
			0xFFFFFFFF, result ? "Nok" : "ok", "only read");

		REG32_WRITE(reg_base + TIMERNLOADCOUNT2(i), 0xFFFFFFFF);
		val = REG32_READ(reg_base + TIMERNLOADCOUNT2(i));
		result = ((val != 0xFFFFFFFF) ? 1 : 0);
		ret += result;
		hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s \r\n",
			reg_base + TIMERNLOADCOUNT2(i), 0xFFFFFFFF, val,
			0xFFFFFFFF, result ? "Nok" : "ok");
	}

	REG32_WRITE(reg_base + TIMERSINTSTATUS, 0xFFFFFFFF);
	val = REG32_READ(reg_base + TIMERSINTSTATUS);
	result = ((val != 0) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s:%s \r\n",
		reg_base + TIMERSINTSTATUS, 0xFFFFFFFF, val, 0xFFFFFFFF,
		result ? "Nok" : "ok", "only read");

	REG32_WRITE(reg_base + TIMERSEOI, 0xFFFFFFFF);
	val = REG32_READ(reg_base + TIMERSEOI);
	result = ((val != 0) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s:%s \r\n",
		reg_base + TIMERSEOI, 0xFFFFFFFF, val, 0xFFFFFFFF,
		result ? "Nok" : "ok", "only read");

	REG32_WRITE(reg_base + TIMERSRAWINTSTATUS, 0xFFFFFFFF);
	val = REG32_READ(reg_base + TIMERSRAWINTSTATUS);
	result = ((val != 0) ? 1 : 0);
	ret += result;
	hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s:%s \r\n",
		reg_base + TIMERSRAWINTSTATUS, 0xFFFFFFFF, val, 0xFFFFFFFF,
		result ? "Nok" : "ok", "only read");

	REG32_WRITE(reg_base + TIMERS_COMP_VERSI, 0xFFFFFFFF);
	val = REG32_READ(reg_base + TIMERS_COMP_VERSI);
	result = 0;
	ret += result;
	hprintf(TSENV, "REG:%08X W/R %08X - %08X(%08X):%s:%s \r\n",
		reg_base + TIMERS_COMP_VERSI, 0xFFFFFFFF, val, 0xFFFFFFFF,
		result ? "Nok" : "ok", "only read");
	return ret;
}

static int get_percent(int val, int max)
{
	if (max > 0xffff) {
		val >>= 4;
		max >>= 4;
	}
	return val / (max / 100);
}

static int timerRun_usrmode_mask_intr(zebu_test_mod_t *mod, void *para)
{
	u32 ret = 0, i, j;
	unsigned int val1, val2;
	int status[CFG_TIMER_CNT_MAX], allstatus_raw, allstatus;
	int percent = 0, temp_percent = 0;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);
	timer_interrupt_enable_by_addr(mod->reg_base, 0);

	bsta1000_timer_clear_all_intr(reg_base);
	for (i = 1; i <= timer_ch_cnt; i++) {
		// disable timer
		bsta1000_timer_disable(reg_base, i);
		bsta1000_timer_load_val(reg_base, i, TIMER_LOAD_VALUE);
		bsta1000_timer_mode_set(reg_base, i, 1);
		bsta1000_timer_pwm_enable(reg_base, i, 0);
		bsta1000_timer_0n100_pwm_enable(reg_base, i, 0);
		bsta1000_timer_intr_mask_set(reg_base, i, 1);
		bsta1000_timer_clear_intr(reg_base, i);

		// start config
		bsta1000_timer_enable(reg_base, i);

		val1 = REG32_READ(reg_base + TIMERNCTROLREG(i));
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X\r\n",
			reg_base + TIMERNCTROLREG(i), "TIMERNCTROLREG", i,
			val1);

		val1 = bsta1000_timer_current_val(reg_base, i);
		// hprintf(TSENV, "val1 is 0x%x\r\n",val1);
		val2 = bsta1000_timer_current_val(reg_base, i);
		// hprintf(TSENV, "val2 is 0x%x\r\n",val2);

		if (val1 == val2) {
			hprintf(TSENV,
				"timer chn:%d don't run(%x-%x)Nok!!!\r\n", i,
				val1, val2);
			ret += 1;
			goto out;
		} else {
			hprintf(TSENV, "timer chn:%d is running(%x-%x)\r\n", i,
				val1, val2);
		}

		val1 = bsta1000_timer_current_val(reg_base, i);
		val2 = bsta1000_timer_current_val(reg_base, i);
		hprintf(TSENV, "Wait For Timeout to reload...... \r\n");
		while (1) {
			if (val2 > val1) {
				break;
			}
			val1 = val2;
			val2 = bsta1000_timer_current_val(reg_base, i);
			temp_percent =
				100 - get_percent(val2, TIMER_LOAD_VALUE);
			if (temp_percent != percent) {
				percent = temp_percent;
				hprintf(TSENV, "\r %d%%   ", percent);
			}
		}

		hprintf(TSENV, "\r %d%%   \r\n", 100);
		if (val2 > TIMER_LOAD_VALUE) {
			ret += 1;
			hprintf(TSENV,
				"timer chn:%d User Mode Failed.(%x - %x)Nok!!!\r\n",
				i, val2, TIMER_LOAD_VALUE);
		} else {
			hprintf(TSENV,
				"timer chn:%d User Mode OK.(%x - %x)\r\n", i,
				val2, TIMER_LOAD_VALUE);
		}
		allstatus_raw = bsta1000_timer_all_raw_intr(reg_base);
		allstatus = bsta1000_timer_all_intr_status(reg_base);
		hprintf(TSENV,
			"timer chn:%d TIMERSINTSTATUS:%x  TIMERSRAWINTSTATUS:%x\r\n",
			i, allstatus, allstatus_raw);

		for (j = 1; j <= timer_ch_cnt; j++) {
			status[j - 1] = bsta1000_timer_intr_status(reg_base, j);
		}

		if (allstatus_raw & (0x1 << (i - 1))) {
			hprintf(TSENV,
				"Intrrupt IN REG[TIMERSRAWINTSTATUS] Catched.\r\n");
		} else {
			ret++;
			hprintf(TSENV,
				"Error Intrrupt IN REG[TIMERSRAWINTSTATUS] No "
				"Catched.(status:%x)Nok!!!\r\n",
				allstatus_raw);
		}

		if (allstatus_raw & ~(0x1 << (i - 1))) {
			ret++;
			hprintf(TSENV,
				"Errors Intrrupt IN REG[TIMERSRAWINTSTATUS] Catched(%x).Nok\r\n",
				allstatus_raw);
		}

		if (allstatus) {
			ret++;
			hprintf(TSENV,
				"Errors Intrrupt IN REG[TIMERSINTSTATUS] Catched(%x).Nok\r\n",
				allstatus);
		}

		for (j = 1; j <= timer_ch_cnt; j++) {
			if (status[j - 1]) {
				ret++;
				hprintf(TSENV,
					"Error Intrrupt IN REG[%x] Catched.Nok!!!\r\n",
					i, reg_base + TIMERNINTSTATUS(j));
			}
		}

		bsta1000_timer_clear_intr(reg_base, i);
		bsta1000_timer_disable(reg_base, i);
	}
out:

	for (i = 1; i <= timer_ch_cnt; i++) {
		bsta1000_timer_disable(reg_base, i);
	}
	bsta1000_timer_clear_all_intr(reg_base);
	return ret;
}

static int timerRun_freemode_mask_intr(zebu_test_mod_t *mod, void *para)
{
	u32 ret, i, j;
	unsigned int val1, val2;
	int status[CFG_TIMER_CNT_MAX], allstatus_raw, allstatus;
	int percent = 0, temp_percent = 0;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);
	timer_interrupt_enable_by_addr(mod->reg_base, 0);

	ret = 0;
	bsta1000_timer_clear_all_intr(reg_base);
	for (i = 1; i <= timer_ch_cnt; i++) {
		// disable timer
		bsta1000_timer_disable(reg_base, i);
		bsta1000_timer_load_val(reg_base, i, TIMER_LOAD_VALUE);
		bsta1000_timer_mode_set(reg_base, i, 0);
		bsta1000_timer_pwm_enable(reg_base, i, 0);
		bsta1000_timer_0n100_pwm_enable(reg_base, i, 0);
		bsta1000_timer_intr_mask_set(reg_base, i, 1);
		bsta1000_timer_clear_intr(reg_base, i);
		// start config
		bsta1000_timer_enable(reg_base, i);
		val1 = REG32_READ(reg_base + TIMERNCTROLREG(i));
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X\r\n",
			reg_base + TIMERNCTROLREG(i), "TIMERNCTROLREG", i,
			val1);

		val1 = bsta1000_timer_current_val(reg_base, i);
		val2 = bsta1000_timer_current_val(reg_base, i);

		if (val1 == val2) {
			hprintf(TSENV, "timer chn:%d don't run(%x-%x)\r\n", i,
				val1, val2);
			ret += 1;
			goto out;
		} else {
			hprintf(TSENV, "timer chn:%d is running(%x-%x)\r\n", i,
				val1, val2);
		}
		val1 = bsta1000_timer_current_val(reg_base, i);
		val2 = bsta1000_timer_current_val(reg_base, i);
		hprintf(TSENV, "Wait For Timeout to reload...... \r\n");
		while (1) {
			if (val2 > val1) {
				break;
			}
			val1 = val2;
			val2 = bsta1000_timer_current_val(reg_base, i);
			temp_percent =
				100 - get_percent(val2, TIMER_LOAD_VALUE);
			if (temp_percent != percent) {
				percent = temp_percent;
				hprintf(TSENV, "\r %d%%   ", percent);
			}
		}
		hprintf(TSENV, "\r %d%%   \r\n", 100);
		if (val2 <= TIMER_LOAD_VALUE) {
			ret += 1;
			hprintf(TSENV,
				"timer chn:%d FREE Mode Failed.(%x - %x)\r\n",
				i, val2, 0xffffffff);
		} else {
			hprintf(TSENV,
				"timer chn:%d FREE Mode OK.(%x - %x)\r\n", i,
				val2, TIMER_LOAD_VALUE);
		}
		// allstatus = bsta1000_timer_all_raw_intr( reg_base );
		for (j = 1; j <= timer_ch_cnt; j++) {
			status[j - 1] = bsta1000_timer_intr_status(reg_base, j);
		}

		allstatus_raw = bsta1000_timer_all_raw_intr(reg_base);
		allstatus = bsta1000_timer_all_intr_status(reg_base);
		hprintf(TSENV,
			"timer chn:%d TIMERSINTSTATUS:%x  TIMERSRAWINTSTATUS:%x\r\n",
			i, allstatus, allstatus_raw);
		for (j = 1; j <= timer_ch_cnt; j++) {
			status[j - 1] = bsta1000_timer_intr_status(reg_base, j);
		}

		if (allstatus_raw & (0x1 << (i - 1))) {
			hprintf(TSENV,
				"Intrrupt IN REG[TIMERSRAWINTSTATUS] Catched.\r\n");
		} else {
			ret++;
			hprintf(TSENV,
				"Error Intrrupt IN REG[TIMERSRAWINTSTATUS] No "
				"Catched.(status:%x)Nok!!!\r\n",
				allstatus_raw);
		}

		if (allstatus_raw & ~(0x1 << (i - 1))) {
			ret++;
			hprintf(TSENV,
				"Errors Intrrupt IN REG[TIMERSRAWINTSTATUS] Catched(%x).Nok\r\n",
				allstatus_raw);
		}

		if (allstatus) {
			ret++;
			hprintf(TSENV,
				"Errors Intrrupt IN REG[TIMERSINTSTATUS] Catched(%x).Nok\r\n",
				allstatus);
		}

		for (j = 1; j <= timer_ch_cnt; j++) {
			if (status[j - 1]) {
				ret++;
				hprintf(TSENV,
					"Error Intrrupt Chn:%d IN REG[%x] Catched.\r\n",
					i, reg_base + TIMERNINTSTATUS(j));
			}
		}

		bsta1000_timer_clear_intr(reg_base, i);
		bsta1000_timer_disable(reg_base, i);
	}
out:
	for (i = 1; i <= timer_ch_cnt; i++) {
		bsta1000_timer_disable(reg_base, i);
	}
	bsta1000_timer_clear_all_intr(reg_base);
	return ret;
}

static int timerRun_usrmode_unmask_intr(zebu_test_mod_t *mod, void *para)
{
	u32 ret, i, j;
	unsigned int val1, val2;
	int status[CFG_TIMER_CNT_MAX], allstatus_raw, allstatus;
	int percent = 0, temp_percent = 0;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);
	timer_interrupt_enable_by_addr(mod->reg_base, 0);

	ret = 0;
	bsta1000_timer_clear_all_intr(reg_base);
	for (i = 1; i <= timer_ch_cnt; i++) {
		// disable timer
		bsta1000_timer_disable(reg_base, i);
		bsta1000_timer_load_val(reg_base, i, TIMER_LOAD_VALUE);
		bsta1000_timer_mode_set(reg_base, i, 1);
		bsta1000_timer_pwm_enable(reg_base, i, 0);
		bsta1000_timer_0n100_pwm_enable(reg_base, i, 0);
		bsta1000_timer_intr_mask_set(reg_base, i, 0);
		bsta1000_timer_clear_intr(reg_base, i);
		// start config
		bsta1000_timer_enable(reg_base, i);

		val1 = REG32_READ(reg_base + TIMERNCTROLREG(i));
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X\r\n",
			reg_base + TIMERNCTROLREG(i), "TIMERNCTROLREG", i,
			val1);

		val1 = bsta1000_timer_current_val(reg_base, i);
		hprintf(TSENV, "val1 is 0x%x\r\n", val1);
		val2 = bsta1000_timer_current_val(reg_base, i);
		hprintf(TSENV, "val2 is 0x%x\r\n", val2);

		if (val1 == val2) {
			hprintf(TSENV,
				"timer chn:%d don't run(%x-%x)Nok!!!\r\n", i,
				val1, val2);
			ret += 1;
			goto out;
		} else {
			hprintf(TSENV, "timer chn:%d is running(%x-%x)\r\n", i,
				val1, val2);
		}

		val1 = bsta1000_timer_current_val(reg_base, i);
		val2 = bsta1000_timer_current_val(reg_base, i);
		hprintf(TSENV, "Wait For Timeout to reload...... \r\n");
		while (1) {
			if (val2 > val1) {
				break;
			}
			val1 = val2;
			val2 = bsta1000_timer_current_val(reg_base, i);
			temp_percent =
				100 - get_percent(val2, TIMER_LOAD_VALUE);
			if (temp_percent != percent) {
				percent = temp_percent;
				hprintf(TSENV, "\r %d%%   ", percent);
			}
		}
		hprintf(TSENV, "\r %d%%   \r\n", 100);
		if (val2 > TIMER_LOAD_VALUE) {
			ret += 1;
			hprintf(TSENV,
				"timer chn:%d User Mode Failed.(%x - %x)Nok!!!\r\n",
				i, val2, TIMER_LOAD_VALUE);
		} else {
			hprintf(TSENV,
				"timer chn:%d User Mode OK.(%x - %x)\r\n", i,
				val2, TIMER_LOAD_VALUE);
		}

		allstatus_raw = bsta1000_timer_all_raw_intr(reg_base);
		allstatus = bsta1000_timer_all_intr_status(reg_base);
		hprintf(TSENV,
			"timer chn:%d TIMERSINTSTATUS:%x  TIMERSRAWINTSTATUS:%x\r\n",
			i, allstatus, allstatus_raw);
		for (j = 1; j <= timer_ch_cnt; j++) {
			status[j - 1] = bsta1000_timer_intr_status(reg_base, j);
		}

		if (allstatus & (0x1 << (i - 1))) {
			hprintf(TSENV,
				"timer chn:%d Intrrupt IN REG[TIMERSINTSTATUS] Catched.\r\n",
				i);
		} else {
			ret++;
			hprintf(TSENV,
				"timer chn:%d Intrrupt IN REG[TIMERSINTSTATUS] NO Catched.Nok!!!\r\n",
				i);
		}

		if (allstatus & ~(0x1 << (i - 1))) {
			ret++;
			hprintf(TSENV,
				"Errors Intrrupt IN REG[TIMERSINTSTATUS] Catched(%x)Nok!!!.\r\n",
				allstatus);
		}

		if (allstatus_raw & (0x1 << (i - 1))) {
			hprintf(TSENV,
				"timer chn:%d Intrrupt IN REG[TIMERSRAWINTSTATUS] Catched.\r\n",
				i);
		} else {
			ret++;
			hprintf(TSENV,
				"timer chn:%d Intrrupt IN REG[TIMERSRAWINTSTATUS] NO "
				"Catched.Nok!!!\r\n",
				i);
		}

		if (allstatus_raw & ~(0x1 << (i - 1))) {
			ret++;
			hprintf(TSENV,
				"Errors Intrrupt IN REG[TIMERSRAWINTSTATUS] Catched.\r\n",
				i);
		}

		for (j = 1; j <= timer_ch_cnt; j++) {
			if (i == j) {
				if (status[j - 1]) {
					bsta1000_timer_clear_intr(reg_base, j);
					hprintf(TSENV,
						"timer chn:%d Intrrupt IN REG[%X] Catched.\r\n",
						i,
						reg_base + TIMERNINTSTATUS(j));
					if (bsta1000_timer_intr_status(reg_base,
								       j)) {
						ret++;
						hprintf(TSENV,
							"timer chn:%d Intrrupt Clear Nok!!!.\r\n",
							i,
							reg_base +
								TIMERNINTSTATUS(
									j));
					}
				} else {
					ret++;
					hprintf(TSENV,
						"timer chn:%d Intrrupt IN REG[%X] NO Catched. Nok!!!\r\n",
						i,
						reg_base + TIMERNINTSTATUS(j));
				}
			} else if (status[j - 1]) {
				ret++;
				hprintf(TSENV,
					"Error Intrrupt IN REG[%x] Catched.Nok!!!\r\n",
					i, reg_base + TIMERNINTSTATUS(j));
			}
		}

		val1 = bsta1000_timer_current_val(reg_base, i);
		val2 = bsta1000_timer_current_val(reg_base, i);
		percent = 0;
		hprintf(TSENV,
			"Wait For Timeout to reload ...Test Clear ALL intr... \r\n");
		while (1) {
			if (val2 > val1) {
				break;
			}
			val1 = val2;
			val2 = bsta1000_timer_current_val(reg_base, i);
			temp_percent =
				100 - get_percent(val2, TIMER_LOAD_VALUE);
			if (temp_percent != percent) {
				percent = temp_percent;
				hprintf(TSENV, "\r %d%%   ", percent);
			}
		}
		hprintf(TSENV, "\r %d%%   \r\n", 100);
		bsta1000_timer_clear_all_intr(reg_base);
		allstatus = bsta1000_timer_all_intr_status(reg_base);
		if (allstatus) {
			ret++;
			hprintf(TSENV,
				"Error Clear All intr Failed(%x).Nok!!!\r\n",
				allstatus);
		}

		bsta1000_timer_clear_intr(reg_base, i);
		bsta1000_timer_disable(reg_base, i);
	}
out:
	for (i = 1; i <= timer_ch_cnt; i++) {
		bsta1000_timer_disable(reg_base, i);
	}
	bsta1000_timer_clear_all_intr(reg_base);
	return ret;
}

static int timerRun_freemode_unmask_intr(zebu_test_mod_t *mod, void *para)
{
	u32 ret, i, j;
	unsigned int val1, val2;
	int status[CFG_TIMER_CNT_MAX], allstatus_raw, allstatus;
	int percent = 0, temp_percent = 0;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);
	timer_interrupt_enable_by_addr(mod->reg_base, 0);

	ret = 0;
	bsta1000_timer_clear_all_intr(reg_base);
	for (i = 1; i <= timer_ch_cnt; i++) {
		// disable timer
		bsta1000_timer_disable(reg_base, i);
		bsta1000_timer_load_val(reg_base, i, TIMER_LOAD_VALUE);
		bsta1000_timer_mode_set(reg_base, i, 0);
		bsta1000_timer_pwm_enable(reg_base, i, 0);
		bsta1000_timer_0n100_pwm_enable(reg_base, i, 0);
		bsta1000_timer_intr_mask_set(reg_base, i, 0);
		bsta1000_timer_clear_intr(reg_base, i);
		// start configs
		bsta1000_timer_enable(reg_base, i);

		val1 = REG32_READ(reg_base + TIMERNCTROLREG(i));
		hprintf(TSENV, "REG:%08X(%22s:%d):%08X\r\n",
			reg_base + TIMERNCTROLREG(i), "TIMERNCTROLREG", i,
			val1);

		val1 = bsta1000_timer_current_val(reg_base, i);
		// hprintf(TSENV, "val1 is 0x%x\r\n",val1);
		val2 = bsta1000_timer_current_val(reg_base, i);
		// hprintf(TSENV, "val2 is 0x%x\r\n",val2);

		if (val1 == val2) {
			hprintf(TSENV,
				"timer chn:%d don't run(%x-%x)Nok!!!\r\n", i,
				val1, val2);
			ret += 1;
			goto out;
		} else {
			hprintf(TSENV, "timer chn:%d is running(%x-%x)\r\n", i,
				val1, val2);
		}

		val1 = bsta1000_timer_current_val(reg_base, i);
		val2 = bsta1000_timer_current_val(reg_base, i);
		hprintf(TSENV, "Wait For Timeout to reload...... \r\n");
		while (1) {
			if (val2 > val1) {
				break;
			}
			val1 = val2;
			val2 = bsta1000_timer_current_val(reg_base, i);
			temp_percent =
				100 - get_percent(val2, TIMER_LOAD_VALUE);
			if (temp_percent != percent) {
				percent = temp_percent;
				hprintf(TSENV, "\r %d%%   ", percent);
			}
		}
		hprintf(TSENV, "\r %d%%   \r\n", 100);
		if (val2 <= TIMER_LOAD_VALUE) {
			ret += 1;
			hprintf(TSENV,
				"timer chn:%d User Mode Failed.(%x - %x)Nok!!!\r\n",
				i, val2, TIMER_LOAD_VALUE);
		} else {
			hprintf(TSENV,
				"timer chn:%d User Mode OK.(%x - %x)\r\n", i,
				val2, TIMER_LOAD_VALUE);
		}
		allstatus_raw = bsta1000_timer_all_raw_intr(reg_base);
		allstatus = bsta1000_timer_all_intr_status(reg_base);
		hprintf(TSENV,
			"timer chn:%d TIMERSINTSTATUS:%x  TIMERSRAWINTSTATUS:%x\r\n",
			i, allstatus, allstatus_raw);

		for (j = 1; j <= timer_ch_cnt; j++) {
			status[j - 1] = bsta1000_timer_intr_status(reg_base, j);
		}

		if (allstatus & (0x1 << (i - 1))) {
			hprintf(TSENV,
				"timer chn:%d Intrrupt IN REG[TIMERSINTSTATUS] Catched.\r\n",
				i);
		} else {
			ret++;
			hprintf(TSENV,
				"timer chn:%d Intrrupt IN REG[TIMERSINTSTATUS] NO Catched.Nok!!!\r\n",
				i);
		}

		if (allstatus & ~(0x1 << (i - 1))) {
			ret++;
			hprintf(TSENV,
				"Errors Intrrupt IN REG[TIMERSINTSTATUS] Catched(%x)Nok!!!.\r\n",
				allstatus);
		}

		if (allstatus_raw & (0x1 << (i - 1))) {
			hprintf(TSENV,
				"timer chn:%d Intrrupt IN REG[TIMERSRAWINTSTATUS] Catched.\r\n",
				i);
		} else {
			ret++;
			hprintf(TSENV,
				"timer chn:%d Intrrupt IN REG[TIMERSRAWINTSTATUS] NO "
				"Catched.Nok!!!\r\n",
				i);
		}

		if (allstatus_raw & ~(0x1 << (i - 1))) {
			ret++;
			hprintf(TSENV,
				"Errors Intrrupt IN REG[TIMERSRAWINTSTATUS] Catched.\r\n",
				i);
		}

		for (j = 1; j <= timer_ch_cnt; j++) {
			if (i == j) {
				if (status[j - 1]) {
					bsta1000_timer_clear_intr(reg_base, j);
					hprintf(TSENV,
						"timer chn:%d Intrrupt IN REG[%X] Catched.\r\n",
						i,
						reg_base + TIMERNINTSTATUS(j));
					if (bsta1000_timer_intr_status(reg_base,
								       j)) {
						ret++;
						hprintf(TSENV,
							"timer chn:%d Intrrupt Clear Nok!!!.\r\n",
							i,
							reg_base +
								TIMERNINTSTATUS(
									j));
					}
				} else {
					ret++;
					hprintf(TSENV,
						"timer chn:%d Intrrupt IN REG[%X] NO Catched. Nok!!!\r\n",
						i,
						reg_base + TIMERNINTSTATUS(j));
				}
			} else if (status[j - 1]) {
				ret++;
				hprintf(TSENV,
					"Error Intrrupt IN REG[%x] Catched.Nok!!!\r\n",
					i, reg_base + TIMERNINTSTATUS(j));
			}
		}

		bsta1000_timer_clear_intr(reg_base, i);
		bsta1000_timer_disable(reg_base, i);
	}
out:
	for (i = 1; i <= timer_ch_cnt; i++) {
		bsta1000_timer_disable(reg_base, i);
	}
	bsta1000_timer_clear_all_intr(reg_base);
	return ret;
}

// test9
static int pwmMode_assign_valid(zebu_test_mod_t *mod, void *para)
{
	u32 ret, i;
	unsigned int val1, val2;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	if(reg_base == A55_TIMER_BASE_ADDR){
		hprintf(ERROR, "A55 Timer module does not have PWM function.\r\n");
		return HVTE_TEST_FAIL;
	}

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);

	pwm_pinmux_init(mod->reg_base);

	timer_interrupt_enable_by_addr(mod->reg_base, 0);

	ret = 0;
	for (i = 1; i <= timer_ch_cnt; i++) {
		for (val1 = 1; val1 <= 99; val1++) {
			val2 = 100 - val1;
			// disable timer
			bsta1000_timer_disable(reg_base, i);

			bsta1000_timer_mode_set(reg_base, i,
						1); // TIMER_MODE = 1 for
				// user_running
			bsta1000_timer_pwm_enable(reg_base, i,
						  1); // TIMER_PWM =1
			// bsta1000_timer_0n100_pwm_enable(reg_base,i,0);
			// bsta1000_timer_intr_mask_set(reg_base,i,0);
			if (i & 0x01) {
				bsta1000_timer_load_val(reg_base, i, val2);
				bsta1000_timer_load_val2(reg_base, i, val1);
			} else {
				bsta1000_timer_load_val(reg_base, i, val1);
				bsta1000_timer_load_val2(reg_base, i, val2);
			}
			// start config
			bsta1000_timer_enable(reg_base, i);
			// start config
			udelay(5);

		}
		for (val1 = 99; val1 <= 1; val1--) {
			val2 = 100 - val1;
			// disable timer
			bsta1000_timer_disable(reg_base, i);

			bsta1000_timer_mode_set(reg_base, i,
						1); // TIMER_MODE = 1 for
				// user_running
			bsta1000_timer_pwm_enable(reg_base, i,
						  1); // TIMER_PWM =1
			if (i & 0x01) {
				bsta1000_timer_load_val(reg_base, i, val2);
				bsta1000_timer_load_val2(reg_base, i, val1);
			} else {
				bsta1000_timer_load_val(reg_base, i, val1);
				bsta1000_timer_load_val2(reg_base, i, val2);
			}
			// start config
			bsta1000_timer_enable(reg_base, i);
			// start config
			udelay(5);
		}
	}
	return ret;
}

static int pwmMode_common(zebu_test_mod_t *mod, int mode, int pwm)
{
	u32 val1, val2, i;
	unsigned int ret = 0;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);
	timer_interrupt_enable_by_addr(mod->reg_base, 0);
	for (i = 1; i <= timer_ch_cnt; i++) {
		for (val1 = 1; val1 <= 99; val1++) {
			val2 = 100 - val1;
			// disable timer
			bsta1000_timer_disable(reg_base, i);
			bsta1000_timer_load_val(reg_base, i, TIMER_LOAD_VALUE);
			if (!mode) {
				bsta1000_timer_mode_set(
					reg_base, i,
					0); // TIMER_MODE = 0 for free_running
			} else {
				bsta1000_timer_mode_set(
					reg_base, i,
					1); // TIMER_MODE = 1 for usr_running
			}
			if (!pwm) {
				bsta1000_timer_pwm_enable(reg_base, i,
							  0); // TIMER_PWM =0
			} else {
				bsta1000_timer_pwm_enable(reg_base, i,
							  1); // TIMER_PWM =1
			}

			if (i & 0x01) {
				bsta1000_timer_load_val(reg_base, i, val2);
				bsta1000_timer_load_val2(reg_base, i, val1);
			} else {
				bsta1000_timer_load_val(reg_base, i, val1);
				bsta1000_timer_load_val2(reg_base, i, val2);
			}
			// start config
			bsta1000_timer_enable(reg_base, i);
			// start config

			udelay(5);

		}
		for (val1 = 99; val1 <= 1; val1--) {
			val2 = 100 - val1;
			// disable timer
			bsta1000_timer_disable(reg_base, i);
			bsta1000_timer_load_val(reg_base, i, TIMER_LOAD_VALUE);
			if (!mode) {
				bsta1000_timer_mode_set(
					reg_base, i,
					0); // TIMER_MODE = 0 for free_running
			} else {
				bsta1000_timer_mode_set(
					reg_base, i,
					1); // TIMER_MODE = 1 for usr_running
			}
			if (!pwm) {
				bsta1000_timer_pwm_enable(reg_base, i,
							  0); // TIMER_PWM =0
			} else {
				bsta1000_timer_pwm_enable(reg_base, i,
							  1); // TIMER_PWM =1
			}
			if (i & 0x01) {
				bsta1000_timer_load_val(reg_base, i, val2);
				bsta1000_timer_load_val2(reg_base, i, val1);
			} else {
				bsta1000_timer_load_val(reg_base, i, val1);
				bsta1000_timer_load_val2(reg_base, i, val2);
			}
			// start config
			bsta1000_timer_enable(reg_base, i);
			// start config
			udelay(5);

		}
	}
	return ret;
}

// test10
static int pwmMode_assign_no_valid(zebu_test_mod_t *mod, void *para)
{
	int ret = 0;

	pwm_pinmux_init(mod->reg_base);

	// TIMER_MODE=0 TIMER_PWM =1
	pwmMode_common(mod, 0, 1);
	// TIMER_MODE=1 TIMER_PWM =0
	pwmMode_common(mod, 1, 0);
	// TIMER_MODE=0 TIMER_PWM =0
	pwmMode_common(mod, 0, 0);

	return ret;
}
// test4,5
static int timer_reload(zebu_test_mod_t *mod, void *para)
{
	u32 ret, i, val;
	u32 reg_base = mod->reg_base;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);
	timer_interrupt_enable_by_addr(mod->reg_base, 0);

	bsta1000_timer_clear_all_intr(reg_base);
	for (i = 1; i <= timer_ch_cnt; i++) {
		bsta1000_timer_disable(reg_base, i);
		// val= 0x8fffff;
		bsta1000_timer_load_val(reg_base, i, 0x8fffff);
		// hprintf(DEBUG, "write---timerNloadcount:%x\r\n",val);
		bsta1000_timer_enable(reg_base, i);
		ret = bsta1000_timer_current_val(reg_base, i);
		val = ((ret < 0x8fffff) ? 1 : 0);
		hprintf(DEBUG, "counter decrease progressively:%s\r\n",
			val ? "ok" : "nok");
		bsta1000_timer_disable(reg_base, i);
		ret = bsta1000_timer_current_val(reg_base, i);
		val = ((ret == 0xffffffff) ? 1 : 0);

		hprintf(DEBUG, "counter is 0xffffffff:%s\r\n",
			val ? "ok" : "nok");
		val = 0x2fff;
		bsta1000_timer_load_val(reg_base, i, val);
		ret = bsta1000_timer_current_val(reg_base, i);
		val = ((ret == 0xffffffff) ? 1 : 0);
		hprintf(DEBUG, "reload counter is always 0xffffffff :%s\r\n",
			val ? "ok" : "nok");
		bsta1000_timer_enable(reg_base, i);
		ret = bsta1000_timer_current_val(reg_base, i);
		val = ((ret < 0x2fff) ? 1 : 0);
		hprintf(DEBUG,
			"reload and enbale counter decrease progressively:%s\r\n",
			val ? "ok" : "nok");
	}
	return 0;
}
// test 11
static int timer_pause_recover(zebu_test_mod_t *mod, void *para)
{
	u32 ret, i, val = 0, val2;
	u32 reg_base = mod->reg_base;
	unsigned int pval = 0;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);

	timer_interrupt_enable_by_addr(mod->reg_base, 0);
	bsta1000_timer_clear_all_intr(reg_base);

	for (i = 1; i <= timer_ch_cnt; i++) {
		bsta1000_timer_disable(reg_base, i);
		// val= 0x8fffff;
		bsta1000_timer_load_val(reg_base, i, 0x8fffff);
		// hprintf(DEBUG, "write---timerNloadcount:%x\r\n",val);
		bsta1000_timer_enable(reg_base, i);
		ret = bsta1000_timer_current_val(reg_base, i);
		val2 = bsta1000_timer_current_val(reg_base, i);
		hprintf(DEBUG, "ret:%x,val2:%x\r\n", ret, val2);
		val = ((ret >= 0x8fffff) ? 1 : 0);

		hprintf(DEBUG, "counter decrease progressively:%s\r\n",
			val ? "nok" : "ok");
		// pause timer
		if (reg_base == TIMER0_PWM0_BASE_ADDR) {
			pval = REG32_READ(LSP0_CRM_ADDR + LSP0_GLB_CTRL_REG);
			pval &= ~(LSP0_TIMER_CH1_PAUSE_EN | LSP0_TIMER_CH2_PAUSE_EN);
			if (i == 1) {
				REG32_WRITE(LSP0_CRM_ADDR + LSP0_GLB_CTRL_REG, pval | LSP0_TIMER_CH1_PAUSE_EN);
			}
			if (i == 2) {
				REG32_WRITE(LSP0_CRM_ADDR + LSP0_GLB_CTRL_REG, pval | LSP0_TIMER_CH2_PAUSE_EN);
			}
			ret = bsta1000_timer_current_val(reg_base, i);
			val2 = bsta1000_timer_current_val(reg_base, i);
			val = ((ret != val2) ? 1 : 0);
			hprintf(DEBUG, "ret:%xval2:%x\r\n", ret, val2);
			hprintf(DEBUG, "pause:%s\r\n", val ? "nok" : "ok");
			// recover timer
			pval &= ~(LSP0_TIMER_CH1_PAUSE_EN | LSP0_TIMER_CH2_PAUSE_EN);
			REG32_WRITE(LSP0_CRM_ADDR + LSP0_GLB_CTRL_REG, pval);
			ret = bsta1000_timer_current_val(reg_base, i);
			val2 = bsta1000_timer_current_val(reg_base, i);
			val = ((ret >= val2) ? 1 : 0);
			hprintf(DEBUG, "ret:%xval2:%x\r\n", ret, val2);
			hprintf(DEBUG, "recover:%s\r\n", val ? "ok" : "nok");
		}
		if (reg_base == TIMER1_PWM1_BASE_ADDR) {
			pval = REG32_READ(LSP1_CRM_ADDR + LSP1_GLB_CTRL_REG);
			pval &= ~(LSP1_TIMER_CH1_PAUSE_EN | LSP1_TIMER_CH2_PAUSE_EN);
			if (i == 1) {
				REG32_WRITE(LSP1_CRM_ADDR + LSP1_GLB_CTRL_REG, pval | LSP1_TIMER_CH1_PAUSE_EN);
			}
			if (i == 2) {
				REG32_WRITE(LSP1_CRM_ADDR + LSP1_GLB_CTRL_REG, pval | LSP1_TIMER_CH1_PAUSE_EN);
			}
			ret = bsta1000_timer_current_val(reg_base, i);
			val2 = bsta1000_timer_current_val(reg_base, i);
			val = ((ret != val2) ? 1 : 0);
			hprintf(DEBUG, "ret:%xval2:%x\r\n", ret, val2);
			hprintf(DEBUG, "pause:%s\r\n", val ? "nok" : "ok");
			// recover timer
			pval &= ~(LSP1_TIMER_CH1_PAUSE_EN | LSP1_TIMER_CH2_PAUSE_EN);
			REG32_WRITE(LSP1_CRM_ADDR + LSP1_GLB_CTRL_REG, pval);
			ret = bsta1000_timer_current_val(reg_base, i);
			val2 = bsta1000_timer_current_val(reg_base, i);
			val = ((ret >= val2) ? 1 : 0);
			hprintf(DEBUG, "ret:%xval2:%x\r\n", ret, val2);
			hprintf(DEBUG, "recover:%s\r\n", val ? "ok" : "nok");
		}
	}

	return val;
}

// test interrupt line

void irq_time(int int_num)
{
	unsigned int regval;
	u32 reg;

	printf_intr("\r\n");
	printf_intr("int_num: %d \r\n", int_num);
	switch (int_num) {
	case FIQ_IRQ_LSP0_TIMER0:
		reg = TIMER0_PWM0_BASE_ADDR + TIMERNEOI(1);
		break;
	case FIQ_IRQ_LSP0_TIMER1:
		reg = TIMER0_PWM0_BASE_ADDR + TIMERNEOI(2);
		break;
	case FIQ_IRQ_LSP1_TIMER0:
		reg = TIMER1_PWM1_BASE_ADDR + TIMERNEOI(1);
		break;
	case FIQ_IRQ_LSP1_TIMER1:
		reg = TIMER1_PWM1_BASE_ADDR + TIMERNEOI(2);
		break;
#ifdef CONFIG_A55
	case FIQ_IRQ_A55_TIMER0:
		reg = A55_TIMER_BASE_ADDR + TIMERNEOI(1);
		break;
	case FIQ_IRQ_A55_TIMER1:
		reg = A55_TIMER_BASE_ADDR + TIMERNEOI(2);
		break;
	case FIQ_IRQ_A55_TIMER2:
		reg = A55_TIMER_BASE_ADDR + TIMERNEOI(3);
		break;
	case FIQ_IRQ_A55_TIMER3:
		reg = A55_TIMER_BASE_ADDR + TIMERNEOI(4);
		break;
	case FIQ_IRQ_A55_TIMER4:
		reg = A55_TIMER_BASE_ADDR + TIMERNEOI(5);
		break;
	case FIQ_IRQ_A55_TIMER5:
		reg = A55_TIMER_BASE_ADDR + TIMERNEOI(6);
		break;
	case FIQ_IRQ_A55_TIMER6:
		reg = A55_TIMER_BASE_ADDR + TIMERNEOI(7);
		break;
	case FIQ_IRQ_A55_TIMER7:
		reg = A55_TIMER_BASE_ADDR + TIMERNEOI(8);
		break;
#endif
	default:
		reg = TIMER0_PWM0_BASE_ADDR + TIMERNEOI(1);
		break;
	}
	regval = REG32_READ(reg);
	iqr_flag = 1;
}

static int timer_irq_test(zebu_test_mod_t *mod, void *para)
{
	unsigned int val1 = 0, val2 = 0;
	unsigned int temp_percent = 0, percent = 0;
	int i;
	u32 reg_base = mod->reg_base, channel;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);

	iqr_flag = 0;

	hprintf(DEBUG, "current reg_base: 0x%08x \r\n", mod->reg_base);

	timer_interrupt_enable_by_addr(mod->reg_base, 1);
	for (channel = 1; channel <= timer_ch_cnt; channel++) {
		//*****timer 0*****//
		// disable timer
		bsta1000_timer_disable(reg_base, channel);
		// load count
		bsta1000_timer_load_val(reg_base, channel, TIMER_LOAD_VALUE);
		// timer bit2=0,intr unmask
		bsta1000_timer_intr_mask_set(reg_base, channel, 0);
		bsta1000_timer_mode_set(reg_base, channel, 1);
		bsta1000_timer_enable(reg_base, channel);

		val1 = bsta1000_timer_current_val(reg_base, channel);
		val2 = bsta1000_timer_current_val(reg_base, channel);
		while (1) {
			if (val2 > val1) {
				break;
			}
			udelay(100);
			val1 = val2;
			val2 = bsta1000_timer_current_val(reg_base, channel);
			temp_percent =
				100 - get_percent(val2, TIMER_LOAD_VALUE);
			if (temp_percent != percent) {
				percent = temp_percent;
				hprintf(TSENV, "\r %d%%   ", percent);
			}
		}
		hprintf(TSENV, "\r %d%%   \r\n", 100);
		for (i = 0; i < 500 && !iqr_flag; i++) {
			asm("nop");
		}
		if (iqr_flag) {
			hprintf(DEBUG,
				"timer interrupt connenct line is ok\r\n");
		} else {
			hprintf(DEBUG,
				"timer interrupt connenct line is Failed\r\n");
		}
		iqr_flag = 0;

		REG32_WRITE(reg_base + 8 + (channel - 1) * 0x14, 0);
	}
	timer_interrupt_enable_by_addr(mod->reg_base, 0);
	return 0;
}

#if !defined(CONFIG_ZEBU)
static int timer_max_count_test(zebu_test_mod_t *mod, void *para)
{
	u32 i, val1 = 0, val2 = 0, allstatus_raw, allstatus;
	u32 reg_base = mod->reg_base;
	u32 load_count = 0xffffffff;
	u32 timer_ch_cnt = 0;

	timer_ch_cnt = bst_a1000_timer_ch_max_mod(mod);

	i = 1;
	bsta1000_timer_clear_all_intr(reg_base);
	for (i = 1; i <= timer_ch_cnt; i++) {
		bsta1000_timer_disable(reg_base, i);
		bsta1000_timer_load_val(reg_base, i, load_count);
		bsta1000_timer_mode_set(reg_base, i, 1); // user mode
		bsta1000_timer_pwm_enable(reg_base, i, 0); // disable pwm
		bsta1000_timer_0n100_pwm_enable(reg_base, i,
						0); // disable 0n100pwm
		bsta1000_timer_intr_mask_set(reg_base, i,
					     0); // unmask interrupt
		bsta1000_timer_clear_intr(reg_base, i);

		hprintf(DEBUG, "write---timerNloadcount:0x%08x\r\n",
			load_count);
		bsta1000_timer_enable(reg_base, i);

		val1 = bsta1000_timer_current_val(reg_base, i);
		hprintf(TSENV, "val1 is 0x%x\r\n", val1);
		val2 = bsta1000_timer_current_val(reg_base, i);
		hprintf(TSENV, "val2 is 0x%x\r\n", val2);
		while (1) {
			if (val2 > val1) {
				break;
			}
			val1 = val2;
			val2 = bsta1000_timer_current_val(reg_base, i);
			// hprintf(TSENV, "val2 is 0x%x\r\n",val2);
		}
		allstatus_raw = bsta1000_timer_all_raw_intr(reg_base);
		allstatus = bsta1000_timer_all_intr_status(reg_base);
		hprintf(TSENV,
			"timer chn:%d TIMERSINTSTATUS:%x  TIMERSRAWINTSTATUS:%x\r\n",
			i, allstatus, allstatus_raw);

		bsta1000_timer_clear_intr(reg_base, i);

		bsta1000_timer_disable(reg_base, i);
		hprintf(DEBUG, "timer count end\r\n");
	}
	return 0;
}
#endif

/*==================================Timer ZEBU
 * TEST=====================================================*/

static zebu_test_mod_t zebu_test_timer_mods[] = {
	{ "TIMER0", TIMER0_PWM0_BASE_ADDR },
	{ "TIMER1", TIMER1_PWM1_BASE_ADDR },
#if defined(CONFIG_A55)
	{ "A55_TIMER", A55_TIMER_BASE_ADDR },
#endif
};

zebu_test_entry_t zebu_test_timer_entrys[] = {
	{ '1', 0, "Reg Dump", timer_test_registers_dump }, // test2
	{ '2', 0, "Reg W/R test", timer_test_registers_wr }, // test3
	{ '3', 1, "USRmode mask Intr", timerRun_usrmode_mask_intr }, // test6
	{ '4', 1, "FREEmode mask Intr", timerRun_freemode_mask_intr }, // test7
	{ '5', 1, "USRmode UMMask Intr",
	  timerRun_usrmode_unmask_intr }, // test8
	{ '6', 1, "FREEmode UMMask Intr", timerRun_freemode_unmask_intr },
	{ '7', 0, "pwmMode assign valid test", pwmMode_assign_valid }, // test9
	{ '8', 1, "count reload test", timer_reload }, // test4,5
	{ '9', 0, "pwmMode assign no valid test",
	  pwmMode_assign_no_valid }, // test10
	{ 'b', 1, "timer pause and recover", timer_pause_recover }, // test4,5
	{ 'e', 1, "timer interrupt connenct line",
	  timer_irq_test }, // test timer interrupt connect line with cpu
#if !defined(CONFIG_ZEBU)
	{ 'f', 1, "timer max load count test",
	  timer_max_count_test }, // test timer max load count
#endif
};

static zebu_test_ctrl_t zebu_mod_test_ctrl = { "timer",
					       SIZE_ARRAY(zebu_test_timer_mods),
					       zebu_test_timer_mods,
					       SIZE_ARRAY(
						       zebu_test_timer_entrys),
					       zebu_test_timer_entrys,
					       0 };

#if 0
void pwm_irq_handler (unsigned long para)
{
	hprintf(TSENV, "Catch IRQ:%d.\r\n",para);	
}
#endif

void pwmMain(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "auto test err %d!!!!\r\n", ret);
	} else {
		hprintf(TSENV, "enter pwm test!\r\n");
		TestLoop(&zebu_mod_test_ctrl);
	}
}

CMD_DEFINE(pwm, pwmMain, 0, "pwm test", CMD_ATTR_AUTO_TEST);

IRQ_DEFINE(FIQ_IRQ_LSP0_TIMER0, irq_time, FIQ_IRQ_LSP0_TIMER0,
	   "lsp0 timer0 isr", ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_TIMER1, irq_time, FIQ_IRQ_LSP0_TIMER1,
	   "lsp0 timer1 isr", ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP1_TIMER0, irq_time, FIQ_IRQ_LSP1_TIMER0,
	   "lsp1 timer0 isr", ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP1_TIMER1, irq_time, FIQ_IRQ_LSP1_TIMER1,
	   "lsp1 timer1 isr", ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);

#if defined(CONFIG_A55)
IRQ_DEFINE(FIQ_IRQ_A55_TIMER0, irq_time, FIQ_IRQ_A55_TIMER0, "a55 timer0 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_TIMER1, irq_time, FIQ_IRQ_A55_TIMER1, "a55 timer1 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_TIMER2, irq_time, FIQ_IRQ_A55_TIMER2, "a55 timer2 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_TIMER3, irq_time, FIQ_IRQ_A55_TIMER3, "a55 timer3 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_TIMER4, irq_time, FIQ_IRQ_A55_TIMER4, "a55 timer4 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_TIMER5, irq_time, FIQ_IRQ_A55_TIMER5, "a55 timer5 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_TIMER6, irq_time, FIQ_IRQ_A55_TIMER6, "a55 timer6 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_A55_TIMER7, irq_time, FIQ_IRQ_A55_TIMER7, "a55 timer7 isr",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
#endif
