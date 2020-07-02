/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __UART2_SYNC_H_
#define __UART2_SYNC_H_
#include <a1000/sysreg_base.h>
#include <linux/types.h>

#ifdef CONFIG_UART3_SYNC
extern u32 xtor_dut_sync_read(char *str);
extern void xtor_dut_sync_write(char *str);
extern void sync_uart_init(void);
#else
static inline u32 xtor_dut_sync_read(char *str)
{
	return 0;
}
static inline void xtor_dut_sync_write(char *str)
{
}
static inline void sync_uart_init(void)
{
}
#endif

#endif
