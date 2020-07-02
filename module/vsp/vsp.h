/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _VSP_H
#define _VSP_H

#include <module/vsp.h>
#include <a1000/sysreg_base.h>
#include <int_num.h>

#define VSP_BASE_TOP           VSP_BASE
#define VSP_BASE_DBGBUS_BSMEM  (BSMEM_BASE + 0x1000000)
#define VSP_BASE_DBGBUS_XFORM  (BSMEM_BASE + 0x1200000)
#define VSP_BASE_DBGBUS_ME     (BSMEM_BASE + 0x1400000)
#define VSP_BASE_DBGBUS_MD     (BSMEM_BASE + 0x1500000)
#define VSP_BASE_DBGBUS_CODE   (BSMEM_BASE + 0x1600000)
#define VSP_BASE_DBGBUS_CODE_CMEM   (BSMEM_BASE + 0x1680000)
#define VSP_BASE_DBGBUS_BSRAM  (BSMEM_BASE + 0x2000000)

#define VSP_BASE_INT2SYNC      (VSP_BASE + 0x2000)
#define VSP_IPC_CODE2A55CORE0_EN_ADDR     IPC_BASE + 0x2080
#define VSP_IPC_CODE2A55CORE0_EN_VAL      0x4003fffe
#define VSP_IPC_CODE2A55CORE0_TRIG_ADDR   IPC_BASE + 0x2f00
#define VSP_IPC_CODE2A55CORE0_TRIG_VAL    0x00008000
#define VSP_IPC_CODE2A55CORE0_STAT_ADDR   IPC_BASE + 0x2000
#define VSP_IPC_CODE2A55CORE0_STAT_VAL    0x00008000

#endif
