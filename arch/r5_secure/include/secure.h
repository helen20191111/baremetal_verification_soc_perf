/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __SECURE_H_
#define __SECURE_H_

void Secure_OpenSysIrq(void);
void free_safety(int para);
void free_A55(int para);
void post_info(void);
void isr_init(void);
void irq_enter(void);
void fiq_enter(void);

#endif
