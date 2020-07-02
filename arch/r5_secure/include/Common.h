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

#include <a1000/sysreg_base.h>

//******globle flag ************//
typedef struct {
	unsigned int otpBasicInSram;
	// unsigned int otpKeyInSram;
} T_SECURE_GLOBAL;

//***********************define iram work addr*********//
#define SECURE_STAGE1_RUN_ADDR (SRAM_BASE_ADDR)
// down for bure or debug
#define UART_DOWN_SIZE (0x20000) // 128k

//***define qspi STORE SZIE INFO*//
#define SAFETY_CODE_SIZE (1024 * 1024)
//#define SAFETY_CODE_SIZE   (64*1024)
#define SAFETY_INFO_SIZE (4 * 1024)
#define SECURE_INFO_SIZE (4 * 1024)

#define SECURE_STAGE1_SIZE_DEFAULT (32 * 1024)

//***QSPI postion info***//
#define APP_CODE_STORE (QSPI0_BASE_ADDR)
#define SAFETY_CODE_STORE (QSPI0_BASE_ADDR)
#define SAFETY_INFO_STORE (QSPI0_BASE_ADDR + SAFETY_CODE_SIZE)
#define SECURE_INFO_STORE                                                      \
	(QSPI0_BASE_ADDR + SAFETY_CODE_SIZE + SAFETY_INFO_SIZE)
#define SECURE_STAGE1_STORE                                                    \
	(QSPI0_BASE_ADDR + SAFETY_CODE_SIZE + SAFETY_INFO_SIZE +               \
	 SECURE_INFO_SIZE)

// secure info
#define SECURE_CODE_HASH_SIZE_DEFAULT (8 * 1024)
#define SECURE_CODE_AES_SIZE (64)

//******************************************************************************//
//***************************************env
//diff*******************************//
//******************************************************************************//

/*
uart  div is some as transactor
uart haps work clock is 5M
uart chip work clock is 25M
*/
#define UART_ZEBU_BAUDRATE_DIV (1)
#define UART_ZEBU_BAUDRATE_DIV_F (0)

// 8.138020833333333
#define UART_5M_BAUDRATE_38400_DIV (8)
#define UART_5M_BAUDRATE_38400_DIV_F (9)
// 2.712673611111111‬
#define UART_5M_BAUDRATE_115200_DIV (2)
#define UART_5M_BAUDRATE_115200_DIV_F (46)

// 40.6901
#define UART_25M_BAUDRATE_38400_DIV (40)
#define UART_25M_BAUDRATE_38400_DIV_F (0x2c)
// 13.563368
#define UART_25M_BAUDRATE_115200_DIV (13)
#define UART_25M_BAUDRATE_115200_DIV_F (0x24)

#ifdef ZEBU_ENV // cpu CLOCK 3,000,000
#define SOFT_LOOP_1S (10000)
#define SOFT_LOOP_300MS (3000)
#define SOFT_LOOP_100MS (1000)
#define SOFT_LOOP_10MS (100)
#define SOFT_LOOP_1MS (10)
#define BACK_DOOR_WAIT SOFT_LOOP_1S

#define PRINT_DEBUG_INFO 1
#define UART_WORK_DIV UART_ZEBU_BAUDRATE_DIV
#define UART_WORK_DIV_F UART_ZEBU_BAUDRATE_DIV_F

#elif defined HAPS_ENV // cpu CLOCK 25,000,000
#define SOFT_LOOP_1S (100000)
#define SOFT_LOOP_500MS (50000)
#define SOFT_LOOP_300MS (15000)
#define SOFT_LOOP_100MS (5000)
#define SOFT_LOOP_10MS (500)
#define SOFT_LOOP_1MS (50)
#define BACK_DOOR_WAIT SOFT_LOOP_1S

#define UART_WORK_DIV UART_25M_BAUDRATE_115200_DIV
#define UART_WORK_DIV_F UART_25M_BAUDRATE_115200_DIV_F
//#define PRINT_DEBUG_INFO  1

#elif defined RTL_ENV // CLOCK is 25,000,000,but change delay time

#define SOFT_LOOP_1S (400)
#define SOFT_LOOP_300MS (100)
#define SOFT_LOOP_100MS (80)
#define SOFT_LOOP_10MS (40)
#define SOFT_LOOP_1MS (10)
#define BACK_DOOR_WAIT (1)

#define UART_WORK_DIV UART_25M_BAUDRATE_38400_DIV
#define UART_WORK_DIV_F UART_25M_BAUDRATE_38400_DIV_F

#elif defined CHIP_ENV // CLOCK 25,000,000
#define SOFT_LOOP_1S (100000)
#define SOFT_LOOP_500MS (50000)
#define SOFT_LOOP_300MS (15000)
#define SOFT_LOOP_100MS (5000)
#define SOFT_LOOP_10MS (500)
#define SOFT_LOOP_1MS (50)
#define BACK_DOOR_WAIT SOFT_LOOP_1S

#define UART_WORK_DIV UART_25M_BAUDRATE_38400_DIV
#define UART_WORK_DIV_F UART_25M_BAUDRATE_38400_DIV_F
#else
// CLOCK 200,000,000
#define SOFT_LOOP_1S (100000)
#define SOFT_LOOP_500MS (50000)
#define SOFT_LOOP_300MS (15000)
#define SOFT_LOOP_100MS (5000)
#define SOFT_LOOP_10MS (500)
#define SOFT_LOOP_1MS (50)
#define BACK_DOOR_WAIT SOFT_LOOP_500MS

#define UART_WORK_DIV UART_25M_BAUDRATE_38400_DIV
#define UART_WORK_DIV_F UART_25M_BAUDRATE_38400_DIV_F
#endif

#endif // common.h
