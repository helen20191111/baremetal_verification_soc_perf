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
#include <module/a55_timer.h>
#include <system.h>
#include <int_num.h>

// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2013
 * David Feng <fenghua@phytium.com.cn>
 */

/*
 * Generic timer implementation of get_tbclk()
 */

enum { NS_PHY_TIMER_INDEX,
       S_PHY_TIMER_INDEX,
       V_TIMER_INDEX,
       HYP_TIMER_INDEX,
       GTIMER_INDEX_MAX };

static volatile int gtimer_intr_test_flag[GTIMER_INDEX_MAX] = { 0 };
unsigned long get_tbclk(void)
{
	unsigned long cntfrq;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq));
	return cntfrq;
}

unsigned long set_tbclk(unsigned long cntfrq)
{
	asm volatile("msr cntfrq_el0 , %0" : "=r"(cntfrq));
	return cntfrq;
}

#ifdef CONFIG_SYS_FSL_ERRATUM_A008585
/*
 * FSL erratum A-008585 says that the ARM generic timer counter "has the
 * potential to contain an erroneous value for a small number of core
 * clock cycles every time the timer value changes".
 * This sometimes leads to a consecutive counter read returning a lower
 * value than the previous one, thus reporting the time to go backwards.
 * The workaround is to read the counter twice and only return when the value
 * was the same in both reads.
 * Assumes that the CPU runs in much higher frequency than the timer.
 */
unsigned long timer_read_counter(void)
{
	unsigned long cntpct;
	unsigned long temp;

	isb();
	asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct));
	asm volatile("mrs %0, cntpct_el0" : "=r"(temp));
	while (temp != cntpct) {
		asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct));
		asm volatile("mrs %0, cntpct_el0" : "=r"(temp));
	}

	return cntpct;
}
#elif defined CONFIG_SUNXI_A64_TIMER_ERRATUM
/*
 * This erratum sometimes flips the lower 11 bits of the counter value
 * to all 0's or all 1's, leading to jumps forwards or backwards.
 * Backwards jumps might be interpreted all roll-overs and be treated as
 * huge jumps forward.
 * The workaround is to check whether the lower 11 bits of the counter are
 * all 0 or all 1, then discard this value and read again.
 * This occasionally discards valid values, but will catch all erroneous
 * reads and fixes the problem reliably. Also this mostly requires only a
 * single read, so does not have any significant overhead.
 * The algorithm was conceived by Samuel Holland.
 */
unsigned long timer_read_counter(void)
{
	unsigned long cntpct;

	isb();
	do {
		asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct));
	} while (((cntpct + 1) & GENMASK(10, 0)) <= 1);

	return cntpct;
}
#else
/*
 * timer_read_counter() using the Arm Generic Timer (aka arch timer).
 */
unsigned long timer_read_counter(void)
{
	unsigned long cntpct;

	isb();
	asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct));

	return cntpct;
}
#endif

unsigned long long get_ticks(void)
{
	unsigned long ticks = timer_read_counter();
	return ticks;
}

unsigned long long get_virtual_ticks(void)
{
	unsigned long ticks;
	isb();
	ticks = read_sysreg(cntvct_el0);
	return ticks;
}

void set_virtual_offset_ticks(unsigned long long offset)
{
	write_sysreg(offset, cntvoff_el2);
}

void gtimer_set_p_cmp_ticks(unsigned long long ticks)
{
	write_sysreg(ticks, cntp_cval_el0);
}

void gtimer_set_sp_cmp_ticks(unsigned long long ticks)
{
	write_sysreg(ticks, cntps_cval_el1);
}

void gtimer_set_v_cmp_ticks(unsigned long long ticks)
{
	write_sysreg(ticks, cntv_cval_el0);
}

void gtimer_set_hyp_cmp_ticks(unsigned long long ticks)
{
	write_sysreg(ticks, cnthp_cval_el2);
}

unsigned long long gtimer_get_p_cmp_ticks(void)
{
	unsigned long long ticks;
	isb();
	ticks = read_sysreg(cntp_cval_el0);
	return ticks;
}

unsigned long long gtimer_get_sp_cmp_ticks(void)
{
	unsigned long long ticks;
	isb();
	ticks = read_sysreg(cntps_cval_el1);
	return ticks;
}

unsigned long long gtimer_get_v_cmp_ticks(void)
{
	unsigned long long ticks;
	isb();
	ticks = read_sysreg(cntv_cval_el0);
	return ticks;
}

unsigned long long gtimer_get_hyp_cmp_ticks(void)
{
	unsigned long long ticks;
	isb();
	ticks = read_sysreg(cnthp_cval_el2);
	return ticks;
}

void gtimer_set_p_enable_intr(int flag)
{
	write_sysreg(flag, cntp_ctl_el0);
}

void gtimer_set_sp_enable_intr(int flag)
{
	write_sysreg(flag, cntps_ctl_el1);
}

void gtimer_set_v_enable_intr(int flag)
{
	write_sysreg(flag, cntv_ctl_el0);
}

void gtimer_set_hyp_enable_intr(int flag)
{
	write_sysreg(flag, cnthp_ctl_el2);
}

unsigned long long gtimer_get_p_enable_intr(void)
{
	unsigned long long flag;
	isb();
	flag = read_sysreg(cntp_ctl_el0);
	return flag;
}

unsigned long long gtimer_get_sp_enable_intr(void)
{
	unsigned long long flag;
	isb();
	flag = read_sysreg(cntps_ctl_el1);
	return flag;
}

unsigned long long gtimer_get_v_enable_intr(void)
{
	unsigned long long flag;
	isb();
	flag = read_sysreg(cntv_ctl_el0);
	return flag;
}

unsigned long long gtimer_get_hyp_enable_intr(void)
{
	unsigned long long flag;
	isb();
	flag = read_sysreg(cnthp_ctl_el2);
	return flag;
}

unsigned long usec2ticks(unsigned long usec)
{
	unsigned long ticks;
	if (usec < 1000)
		ticks = ((usec * (get_tbclk() / 1000)) + 500) / 1000;
	else
		ticks = ((usec / 10) * (get_tbclk() / 100000));

	return ticks;
}

unsigned long  timer_get_boot_us(void)
{
	unsigned long long val = get_ticks() * 1000000;

	return val / get_tbclk();
}
/*
static void timer_env_init(void)
{
	//enable cntfrq_el0 timer clock.
	(*(volatile unsigned int *)(0x32707000) = (1));
}
*/
void __udelay(unsigned long usec)
{
	unsigned long long tmp;

	tmp = get_ticks() + usec2ticks(usec); /* get current timestamp */

	while (get_ticks() < tmp + 1) /* loop till event */
		/*NOP*/;
}
void __uvdelay(unsigned long usec)
{
	unsigned long long tmp;

	tmp = get_virtual_ticks() +
	      usec2ticks(usec); /* get current timestamp */

	while (get_virtual_ticks() < tmp + 1) /* loop till event */
		/*NOP*/;
}

/* ------------------------------------------------------------------------- */

void udelay(unsigned long usec)
{
	unsigned long kv;

	do {
		kv = usec > CONFIG_WD_PERIOD ? CONFIG_WD_PERIOD : usec;
		__udelay(kv);
		usec -= kv;
	} while (usec);
}

void uvdelay(unsigned long usec)
{
	unsigned long kv;

	do {
		kv = usec > CONFIG_WD_PERIOD ? CONFIG_WD_PERIOD : usec;
		__uvdelay(kv);
		usec -= kv;
	} while (usec);
}
void a55_timer_init(void)
{
	//enable cntfrq_el0 timer clock.
	(*(volatile unsigned int *)(0x32707000) = (1));
	set_tbclk(A55_CLK_FREQ);
	return;
}

static int a55_gtimer_intr_test_init(void)
{
	int i;
	for (i = 0; i < GTIMER_INDEX_MAX; i++) {
		gtimer_intr_test_flag[i] = 0;
	}
	return 0;
}

static int a55_gtimer_intr_test_waiting(unsigned int intr_index)
{
	int i;
	int error = 0;
	int ok = 0;
	int ret;
	unsigned long long start_time = get_ticks();
	do {
		for (i = 0; i < GTIMER_INDEX_MAX; i++) {
			if (gtimer_intr_test_flag[i] == 1) {
				if (i == intr_index) {
					ok = 1;
				} else {
					error++;
				}
			}
		}
		ret = ok | (error << 8);
	} while (!ret && get_ticks() - start_time < (2 * A55_CLK_FREQ));
	return ret;
}

int a55_gtimer_run_test(zebu_test_mod_t *mod, void *para)
{
	unsigned long long timer_val1 = 0;
	unsigned long long timer_val2 = 0;
	unsigned long long virtial_offset = 5 * A55_CLK_FREQ;
	int i = 0;
	int ret = 0;
	hprintf(TSENV, "Now Test phy timer Runing.\n\r");
	timer_val1 = get_ticks();
	delaySoft(1000);
	timer_val2 = get_ticks();
	//hprintf(TSENV, "A55 generic PHY Timer.FirstTimeCnt:%u  SecondTimeCnt:%u \n\r",timer_val1,timer_val2);
	if (timer_val1 == timer_val2) {
		hprintf(ERROR, "A55 generic Timer NOT RUN,Test failed!!\n\r");
		ret++;
		return 1;
	}
	hprintf(TSENV, "A55 generic PHY Timer RUN,Test OK.\n\r");
	hprintf(TSENV, "Now Will Sleep 1s for 5 Times.\n\r");
	for (i = 1; i <= 5; i++) {
		udelay(1000 * 1000);
		hprintf(TSENV, "%d second.\n\r", i);
	}

	hprintf(TSENV, "\n\rNow Test Virtual timer Runing.\n\r");
	set_virtual_offset_ticks(virtial_offset);
	timer_val2 = get_virtual_ticks();
	timer_val1 = get_ticks();
	if (timer_val2 - timer_val1 > 4 * A55_CLK_FREQ) {
		hprintf(TSENV, "A55 Virtual Timer Offset,Test OK.\n\r");
	}
	timer_val1 = get_virtual_ticks();
	delaySoft(1000);
	timer_val2 = get_virtual_ticks();
	//hprintf(TSENV, "A55  Virtual Timer.FirstTimeCnt:%u  SecondTimeCnt:%u \n\r",timer_val1,timer_val2);
	if (timer_val1 == timer_val2) {
		hprintf(ERROR, "A55 Virtual Timer NOT RUN,Test failed!!\n\r");
		ret++;
		return 1;
	}
	hprintf(TSENV, "A55 Virtual Timer RUN,Test OK.\n\r");
	hprintf(TSENV, "Now Will Sleep 1s for 5 Times.with Virtual Timer\n\r");
	for (i = 1; i <= 5; i++) {
		uvdelay(1000 * 1000);
		hprintf(TSENV, "%d second.\n\r", i);
	}

	return 0;
}

int a55_phy_timer_intr_test(zebu_test_mod_t *mod, void *para)
{
	unsigned long long timer_val1 = 0;
	int ret = 0;
	hprintf(TSENV, "Now Test Non-Secure phy timer Interrupt.\n\r");
	a55_gtimer_intr_test_init();
	gtimer_set_p_enable_intr(1);
	timer_val1 = get_ticks();
	timer_val1 += A55_CLK_FREQ;
	gtimer_set_p_cmp_ticks(timer_val1);
	ret = a55_gtimer_intr_test_waiting(NS_PHY_TIMER_INDEX);
	//timer_val1 = get_ticks();
	//timer_val2 = gtimer_get_p_enable_intr();
	//hprintf(TSENV, "gtimer_get_p_cmp_ticks:%u --- 0x%x.\n\r",timer_val1,timer_val2);
	if (ret == 1) {
		hprintf(TSENV,
			"A55 generic Non-Secure PHY Timer intrrupt Test. OK.\n\r");
	} else {
		hprintf(TSENV,
			"A55 generic Non-Secure PHY Timer intrrupt Test. fault(%s).\n\r",
			ret ? "ohter intr" : "no intr");
	}

	return ret >> 8;
}

int a55_sphy_timer_intr_test(zebu_test_mod_t *mod, void *para)
{
	unsigned long long timer_val1 = 0;
	int ret = 0;

	hprintf(TSENV, "Now Test Secure phy timer Interrupt.\n\r");
	a55_gtimer_intr_test_init();
	timer_val1 = get_ticks();
	timer_val1 += A55_CLK_FREQ;
	gtimer_set_sp_cmp_ticks(timer_val1);
	gtimer_set_sp_enable_intr(1);
	ret = a55_gtimer_intr_test_waiting(S_PHY_TIMER_INDEX);
	//timer_val1 = get_ticks();
	//timer_val2 = gtimer_get_sp_enable_intr();
	//hprintf(TSENV, "gtimer_get_p_cmp_ticks:%u --- 0x%x.\n\r",timer_val1,timer_val2);
	if (ret == 1) {
		hprintf(TSENV,
			"A55 generic  Secure PHY Timer intrrupt Test. OK.\n\r");
	} else {
		hprintf(TSENV,
			"A55 generic Secure PHY Timer intrrupt Test. fault(%s).\n\r",
			ret ? "ohter intr" : "no intr");
	}
	return ret >> 8;
}

int a55_virtual_timer_intr_test(zebu_test_mod_t *mod, void *para)
{
	unsigned long long timer_val1 = 0;
	int ret = 0;

	hprintf(TSENV, "Now Test virtual timer Interrupt.\n\r");
	a55_gtimer_intr_test_init();
	timer_val1 = get_virtual_ticks();
	timer_val1 += A55_CLK_FREQ;
	gtimer_set_v_cmp_ticks(timer_val1);
	gtimer_set_v_enable_intr(1);
	ret = a55_gtimer_intr_test_waiting(V_TIMER_INDEX);
	//timer_val1 = get_virtual_ticks();
	//timer_val2 = gtimer_get_v_enable_intr();
	//hprintf(TSENV, "gtimer_get_p_cmp_ticks:%u --- 0x%x.\n\r",timer_val1,timer_val2);
	if (ret == 1) {
		hprintf(TSENV,
			"A55 generic virtual Timer intrrupt Test. OK.\n\r");
	} else {
		hprintf(TSENV,
			"A55 generic virtual Timer intrrupt Test. fault(%s).\n\r",
			ret ? "ohter intr" : "no intr");
	}
	return ret >> 8;
}

int a55_hyp_timer_intr_test(zebu_test_mod_t *mod, void *para)
{
	unsigned long long timer_val1 = 0;
	int ret = 0;

	hprintf(TSENV, "Now Test Hypervisor timer Interrupt.\n\r");
	a55_gtimer_intr_test_init();
	timer_val1 = get_ticks();
	timer_val1 += A55_CLK_FREQ;
	gtimer_set_hyp_cmp_ticks(timer_val1);
	gtimer_set_hyp_enable_intr(1);
	ret = a55_gtimer_intr_test_waiting(HYP_TIMER_INDEX);
	//timer_val1 = get_ticks();
	//timer_val2 = gtimer_get_hyp_enable_intr();
	//hprintf(TSENV, "gtimer_get_p_cmp_ticks:%u --- 0x%x.\n\r",timer_val1,timer_val2);
	if (ret == 1) {
		hprintf(TSENV,
			"A55 generic Hypervisor Timer intrrupt Test. OK.\n\r");
	} else {
		hprintf(TSENV,
			"A55 generic Hypervisor Timer intrrupt Test. fault(%s).\n\r",
			ret ? "ohter intr" : "no intr");
	}
	return ret >> 8;
}

static zebu_test_mod_t zebu_test_a55_timer_mods[] = {
	{ "A55_TIMER0", 0 },
	//	{"I2C1" , I2C1_BASE_ADDR},
	//{"I2C2" , TIMER2_PWM2_BASE_ADDR},
	//{"I2C3" , TIMER3_PWM3_BASE_ADDR},
};

static zebu_test_entry_t zebu_test_a55_timer_entrys[] = {
	{ '1', 1, "generic timer runing test", a55_gtimer_run_test },
	{ '2', 1, "Non secure physical timer intr test",
	  a55_phy_timer_intr_test },
	{ '3', 1, "secure physical timer intr test", a55_sphy_timer_intr_test },
	{ '4', 1, "virtual timer intr test", a55_virtual_timer_intr_test },
	{ '5', 1, "Hypervisor timer intr test", a55_hyp_timer_intr_test },
};

static zebu_test_ctrl_t zebu_mod_a55_timer_test_ctrl = {
	"A55 TIMER",
	SIZE_ARRAY(zebu_test_a55_timer_mods),
	zebu_test_a55_timer_mods,
	SIZE_ARRAY(zebu_test_a55_timer_entrys),
	zebu_test_a55_timer_entrys,
	0
};

#define CPU_CORESIGHT_BASE 0x32700000

//Timestamp
#define CPU_CS_TS_BASE (CPU_CORESIGHT_BASE + 0x7000)

#define CPU_CS_TS_CNTCR (*(volatile unsigned *)(CPU_CS_TS_BASE + 0x00))

void a55_generic_timer_pre_init(void)
{
	CPU_CS_TS_CNTCR |= 1;
}

void a55_generic_timer_test_Main(int para)
{
	int ret;
	zebu_mod_a55_timer_test_ctrl.current_mod = 0;
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_a55_timer_test_ctrl);
		hprintf(TSENV, "auto test err %d!!!!\r\n", ret);
	} else {
		hprintf(TSENV, "enter test[%s]!\r\n",
			zebu_mod_a55_timer_test_ctrl.alias);
		TestLoop(&zebu_mod_a55_timer_test_ctrl);
	}
}

void gtimer_isr(int para)
{
	//printf_intr("[%s]%d: catch:%d\n\r",__func__,__LINE__,para);
	switch (para) {
	case A55_NS_PHY_TIMER_INT:
		gtimer_intr_test_flag[NS_PHY_TIMER_INDEX] = 1;
		gtimer_set_p_enable_intr(0);
		break;
	case A55_S_PHY_TIMER_INT:
		gtimer_intr_test_flag[S_PHY_TIMER_INDEX] = 1;
		gtimer_set_sp_enable_intr(0);
		break;
	case A55_V_TIMER_INT:
		gtimer_intr_test_flag[V_TIMER_INDEX] = 1;
		gtimer_set_v_enable_intr(0);
		break;
	case A55_HYP_TIMER_INT:
		gtimer_intr_test_flag[HYP_TIMER_INDEX] = 1;
		gtimer_set_hyp_enable_intr(0);
		break;
	default:
		break;
	}
}

CMD_DEFINE(gtimer, a55_generic_timer_test_Main, 0, "a55 generic timer test",
	   CMD_ATTR_AUTO_TEST);

IRQ_DEFINE(A55_NS_PHY_TIMER_INT, gtimer_isr, A55_NS_PHY_TIMER_INT,
	   "a55 Non-secure phy Timer", ISR_ATTR_A55);
IRQ_DEFINE(A55_S_PHY_TIMER_INT, gtimer_isr, A55_S_PHY_TIMER_INT,
	   "a55 secure phy Timer", ISR_ATTR_A55);
IRQ_DEFINE(A55_V_TIMER_INT, gtimer_isr, A55_V_TIMER_INT, "a55 Vitual Timer",
	   ISR_ATTR_A55);
IRQ_DEFINE(A55_HYP_TIMER_INT, gtimer_isr, A55_HYP_TIMER_INT,
	   "a55 Hypervisor Timer", ISR_ATTR_A55);
