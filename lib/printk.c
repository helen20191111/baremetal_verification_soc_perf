/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/printk.h>
#include <linux/string.h>
#include <vsprintf.h>
#include <common.h>

static char log_buf[1024];

#define LOGLEVEL_DEFAULT '4'

int printk(const char *fmt, ...)
{
	va_list args;
	int i;
	int kern_level;
	int level = LOGLEVEL_DEFAULT - '0';
	char *buf = log_buf;
	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);

	while ((kern_level = printk_get_level(buf)) != 0) {
		switch (kern_level) {
		case '0' ... '7':
			level = kern_level - '0';
			/* fallthrough */
			break;
		case 'd': /* KERN_DEFAULT */
		default:
			level = LOGLEVEL_DEFAULT - '0';
			break;
		}
		i -= 2;
		buf += 2;
	}

	if (CONFIG_MESSAGE_LOGLEVEL_DEFAULT >= level)
		for (i = 0; i < strlen(buf); i++) {
			if ('\n' == buf[i])
				putc('\r');
			putc(buf[i]);
		}
	return i;
}
