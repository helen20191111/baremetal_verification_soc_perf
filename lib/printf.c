/*
 * This file contains proprietary information that is the sole intellectual property of Black Sesame Technologies, Inc. and its affiliates.? 
 * No portions of this material may be reproduced in any form without the written permission of:
 * Black Sesame Technologies, Inc. and its affiliates
 * 2255 Martin Ave. Suite D
 * Santa Clara, CA? 95050
 * Copyright @2016: all right reserved.
*/

#include <vsprintf.h>
#include <printf.h>
#include <linux/string.h>
#include <utility_lite.h>
#include <a1000/sysreg_base.h>
#include <RegTest.h>

#define OUTBUFSIZE 1024
#define INBUFSIZE 1024

static unsigned char g_pcOutBuf[OUTBUFSIZE] = { 0 };
static unsigned char g_pcInBuf[INBUFSIZE] = { 0 };
static unsigned char g_pcOutBuf_intr[OUTBUFSIZE] = { 0 };

volatile static int clsPrt = 0;

void closePrintf(void)
{
	clsPrt = 1;
}
void openPrintf(void)
{
	clsPrt = 0;
}

int printf(const char *fmt, ...)
{
	int i;
	int len;
	va_list args;

	if (clsPrt)
		return 0;

	va_start(args, fmt);
	len = vsprintf((char *)g_pcOutBuf, fmt, args);
	va_end(args);
	for (i = 0; i < strlen((const char *)g_pcOutBuf); i++) {
		if ('\n' == g_pcOutBuf[i]) {
			putc('\r');
		}
		putc(g_pcOutBuf[i]);
	}
	return len;
}

int printf_intr(const char *fmt, ...)
{
	int i;
	int len;
	va_list args;

	va_start(args, fmt);
	len = vsprintf((char *)g_pcOutBuf_intr, fmt, args);
	va_end(args);
	for (i = 0; i < strlen((const char *)g_pcOutBuf_intr); i++) {
		if ('\n' == g_pcOutBuf_intr[i]) {
			putc('\r');
		}
		putc(g_pcOutBuf_intr[i]);
	}
	return len;
}

int scanf(const char *fmt, ...)
{
	int i = 0;
	unsigned char c;
	va_list args;

	while (1) {
		c = getc();
		if ((c == 0x0d) || (c == 0x0a)) {
			g_pcInBuf[i] = '\0';
			break;
		} else {
			g_pcInBuf[i++] = c;
		}
	}

	va_start(args, fmt);
	i = vsscanf((const char *)g_pcInBuf, fmt, args);
	va_end(args);

	return i;
}
