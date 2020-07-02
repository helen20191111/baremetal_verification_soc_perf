/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdbool.h>
#include <stddef.h>
#include <linux/string.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/bitmap.h>
#include <linux/io.h>
#include <printf.h>
#include <vsprintf.h>
#include <cli.h>
#include <printf_inc.h>
#include <errno.h>

#ifdef __LP64__
#define CONFIG_SYS_SUPPORT_64BIT_DATA
#endif

#ifdef CONFIG_A55
#include <module/a55_timer.h>
#else
extern void delaySoft(unsigned long loopcnt);
#define udelay(t) delaySoft(t)
#endif

#endif
