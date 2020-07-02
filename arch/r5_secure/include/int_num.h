/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _INTNUM_H
#define _INTNUM_H

#include <intr.h>

//*****************chip's interrtup number  ************//
#define IRQ_SECURE_TOTAL_CNT 32

#define IRQ_SECURE_OTP 0
#define IRQ_SECURE_TIMER 1
#define IRQ_SECURE_WATCHDOG 2
#define IRQ_SECURE_TRNG 4
#define IRQ_SECURE_SM 5
#define IRQ_SECURE_CRC 6
#define IRQ_SECURE_PKA 7
#define IRQ_SECURE_MSHA 8
#define IRQ_SECURE_AES 9
#define IRQ_SECURE_RX_INTR 15
#define IRQ_SECURE_MST_TIMEOUT 17
#define IRQ_SECURE_OTP_BIT_ERR 18
#define IRQ_SECURE_ROMPARITY 19
#define IRQ_SECURE_ROMECC 20
#define IRQ_SECURE_RAMPARITY 21
#define IRQ_SECURE_RAMECC 22
#define IRQ_SECURE_NOC_PARITY_0 23
#define IRQ_SECURE_NOC_PARITY_1 24
#define IRQ_SECURE_SM_PARITY 25
#define IRQ_SECURE_SEC_CSR_PARITY 26
#define IRQ_SECURE_OTP_PARITY 27
#define IRQ_SECURE_MSHA_PARITY 28
#define IRQ_SECURE_AES_PARITY 29
#define IRQ_SECURE_CRC_PARITY 30
#define IRQ_SECURE_PKA_PARITY 31

#define FIQ_SECURE_TOTAL_CNT 4
#define FIQ_SECURE_OTP 0

#endif
