/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __A55_TIMER_H__
#define __A55_TIMER_H__

#define A55_CLK_FREQ 400000000UL

#define CONFIG_WD_PERIOD (10 * 1000 * 1000) /* 10 seconds default */

unsigned long long get_ticks(void);
unsigned long timer_read_counter(void);
unsigned long usec2ticks(unsigned long usec);
unsigned long timer_get_boot_us(void);
void a55_timer_init(void);
void udelay(unsigned long usec);

#endif
