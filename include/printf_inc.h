/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _PRINTF_INC_H
#define _PRINTF_INC_H

#include <printf.h>

#define TSENV (0x1 << 0) //test env log
#define ERROR (0x1 << 1) //error log
#define DEBUG (0x1 << 2) //debug log
#define MESG (0x1 << 3) //debug log

#define LOGON (TSENV | ERROR | DEBUG)

#define hprintf(level, ...)                                                    \
	do {                                                                   \
		if (LOGON & level) {                                           \
			printf(__VA_ARGS__);                                   \
		}                                                              \
	} while (0)

void putc(unsigned char c);
unsigned char getc(void);
int putc_nb(unsigned char c, unsigned int delayLoop);
int getc_nb(unsigned char *c, unsigned int delayLoop);

#endif
