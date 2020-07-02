/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _PRINTF_H
#define _PRINTF_H

void uart_print_init(void);
void putc(unsigned char c);
unsigned char getc(void);
int printf(const char *fmt, ...);
int scanf(const char *fmt, ...);
int printf_intr(const char *fmt, ...);
void closePrintf(void);
void openPrintf(void);
void puts(const char *str);

#endif /* _PRINTF_H */
