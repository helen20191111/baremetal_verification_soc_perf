/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <utility_lite.h>
#include <uart_sync.h>
#include <common.h>
#include <module/i2c.h>
/////////////////////////////////////putc  getc for printf////////////////////////////////////////
#include <system.h>
#include <RegTest.h>
#include <module/a55_timer.h>
#include <asm/atomic.h> 
#include <hs_api.h>
#include "module/soc_perf.h"

extern unsigned get_core(void);
extern void clk_init(void);

extern void ddr_rw_test0(void);
extern void ddr_rw_test1(void);
extern int bst_a1000_setup_ddr(void);
extern int gic_init(void);

void a55Ipc_OpenIrq(void);

#ifndef CONFIG_APP_BOOT
#define SYS_BASE_TEXT 0x80400000
#elif (defined CONFIG_APP_JTAG)
#define SYS_BASE_TEXT 0x18040000
#else
#define SYS_BASE_TEXT 0x0
#endif

void hard_init(void)
{
}

#ifdef CONFIG_SMP

static volatile u32 smp_cpu_flag = 0;
atomic_t g_v = {0};

void spin_lock(atomic_t *v)
{
	while (1) {
		if (v->counter == 0) {
			atomic_add(1, v);
			break;
		}
	}
}

void spin_unlock(atomic_t *v)
{
	if (v->counter == 0)
		return ;
	atomic_sub(1, v);
}

void core1_tasks(void)
{
	while (1) {
		if (smp_cpu_flag == 1) {
			printf("This cpu 11111\n");
			smp_cpu_flag = 2;
		}
	}
}

void core2_tasks(void)
{
	while (1) {
		if (smp_cpu_flag == 2) {
			printf("This cpu 222222\n");
			smp_cpu_flag = 3;
		}
	}
}

void core3_tasks(void)
{
	while (1) {
		if (smp_cpu_flag == 3) {
			printf("This cpu 33333\n");
			smp_cpu_flag = 4;
		}
	}
}

void core4_tasks(void)
{
	while (1) {
		if (smp_cpu_flag == 4) {
			printf("This cpu 444444\n");
			smp_cpu_flag = 5;
		}
	}
}

void core5_tasks(void)
{
	while (1) {
		if (smp_cpu_flag == 5) {
			printf("This cpu 555555\n");
			smp_cpu_flag = 6;
		}
	}
}

void core6_tasks(void)
{
	while (1) {
		if (smp_cpu_flag == 6) {
			printf("This cpu 666666\n");
			smp_cpu_flag = 7;
		}
	}
}

void core7_tasks(void)
{
	while (1) {
		if (smp_cpu_flag == 7) {
			printf("This cpu 777777\n");
			smp_cpu_flag = 8;
		}
	}
}
#else
extern int core1_tasks(void);
extern int core2_tasks(void);
extern int core3_tasks(void);
extern int core4_tasks(void);
extern int core5_tasks(void);
extern int core6_tasks(void);
extern int core7_tasks(void);
#endif

void printf_init(void)
{
	//use xx putc
	uart_print_init();

	//test puts
	puts("A55-start");
}

void prj_logo(void)
{
	putc('\r');
	putc('\n');
	putc('A');
	putc('5');
	putc('5');
	putc('>');
}

int core0_main(void)
{
	unsigned long reg_data;

	hard_init();
	a55_timer_init();
	printf_init();
	sync_uart_init();
	hprintf(TSENV,
		"\r\n"
		"---------------------------------------------------------------------\r\n"
		"|                           BST HVTE                                |\r\n"
		"---------------------------------------------------------------------\r\n");
	reg_data = read_sysreg(ID_AA64PFR0_EL1);
	hprintf(TSENV, "[%s]%d ID_AA64PFR0_EL1 :%16lx\r\n", __func__, __LINE__,
		reg_data);
	gic_init();
	//switch_to_el1();
	a55Ipc_OpenIrq();
#ifdef CONFIG_EVB
	clk_init();
#ifdef CONFIG_DDR_TEST
	bst_a1000_setup_ddr();
	ddr_rw_test0();
	ddr_rw_test1();
#endif
#endif
	i2c_init();
#ifdef CONFIG_SMP
	cpu_a55_enable(0xff, SYS_BASE_TEXT/4);
	udelay(100);
	smp_cpu_flag = 1;
#endif
#ifdef CONFIG_SOC_PERF_TEST
	soc_perf_init();
#endif
	cli_loop();
	return 0;
}

int main(void)
{
	unsigned my_id;
	my_id = get_core();
	if (my_id == 0)
		core0_main();
	else if (my_id == 1)
		core1_tasks();
	else if (my_id == 2)
		core2_tasks();
	else if (my_id == 3)
		core3_tasks();
	else if (my_id == 4)
		core4_tasks();
	else if (my_id == 5)
		core5_tasks();
	else if (my_id == 6)
		core6_tasks();
	else if (my_id == 7)
		core7_tasks();
	else
		while (1)
			;
	return 0;
}
