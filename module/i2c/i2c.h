/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __I2C_H_
#define __I2C_H_
#include <a1000/sysreg_base.h>

#define I2C_SLAVE_ADDR 0x00000055
#define I2C_SLAVE_ADDR_10bit 0x000002aa
#define I2C_INVALID_ADDR 0x0000002a

#define PINMUX_REG_BASE AON_PMM_REG_BASE_ADDR

#define AON_REG_0 0x0
#define REG_0_MASK 0x1fffffff
#define PINMUX_I2C1_SCL (0 << 31)
#define PINMUX_I2C0_SDA (0 << 30)
#define PINMUX_I2C0_SCL (0 << 29)

#define AON_REG_1 0x04
#define REG_1_MASK 0xfffffe00
#define PINMUX_I2C5_SDA (0 << 8)
#define PINMUX_I2C5_SCL (0 << 7)
#define PINMUX_I2C4_SDA (0 << 6)
#define PINMUX_I2C4_SCL (0 << 5)
#define PINMUX_I2C3_SDA (0 << 4)
#define PINMUX_I2C3_SCL (0 << 3)
#define PINMUX_I2C2_SDA (0 << 2)
#define PINMUX_I2C2_SCL (0 << 1)
#define PINMUX_I2C1_SDA (0 << 0)

#define I2C_CON 0x0
#define I2C_TAR 0x4
#define I2C_SAR 0x8
#define I2C_HS_MADDR 0xc
#define I2C_DATA_CMD 0x10
#define I2C_SS_SCL_HCNT 0x14
//#define I2C_UFM_SCL_HCNT 		0x14
#define I2C_SS_SCL_LCNT 0x18
//#define I2C_UFM_SCL_LCNT 		0x18
#define I2C_FS_SCL_HCNT 0x1c
//#define I2C_UFM_TBUF_CNT 		0x1c
#define I2C_FS_SCL_LCNT 0x20
#define I2C_HS_SCL_HCNT 0x24
#define I2C_HS_SCL_LCNT 0x28
#define I2C_INTR_STAT 0x2c
#define I2C_INTR_MASK 0x30
#define I2C_RAW_INTR_STAT 0x34
#define I2C_RX_TL 0x38
#define I2C_TX_TL 0x3c
#define I2C_CLR_INTR 0x40
#define I2C_CLR_RX_UNDER 0x44
#define I2C_CLR_RX_OVER 0x48
#define I2C_CLR_TX_OVER 0x4c
#define I2C_CLR_RD_REQ 0x50
#define I2C_CLR_TX_ABRT 0x54
#define I2C_CLR_RX_DONE 0x58
#define I2C_CLR_ACTIVITY 0x5c
#define I2C_CLR_STOP_DET 0x60
#define I2C_CLR_START_DET 0x64
#define I2C_CLR_GEN_CALL 0x68
#define I2C_ENABLE 0x6c
#define I2C_STATUS 0x70
#define I2C_TXFLR 0x74
#define I2C_RXFLR 0x78
#define I2C_SDA_HOLD 0x7c
#define I2C_TX_ABRT_SOURCE 0x80
#define I2C_SLV_DATA_NACK_ONLY 0x84
#define I2C_DMA_CR 0x88
#define I2C_DMA_TDLR 0x8c
#define I2C_DMA_RDLR 0x90
#define I2C_SDA_SETUP 0x94
#define I2C_ACK_GENERAL_CALL 0x98
#define I2C_ENABLE_STATUS 0x9c
#define I2C_FS_SPKLEN 0xa0
//#define I2C_UFM_SPKLEN			0xa0
#define I2C_HS_SPKLEN 0xa4
#define I2C_CLR_RESTART_DET 0xa8
#define I2C_SCL_STUCK_AT_LOW_TIMEOUT 0xac
#define I2C_SDA_STUCK_AT_LOW_TIMEOUT 0xb0
#define I2C_CLR_SCL_STUCK_DET 0xb4
#define I2C_DEVICE_ID 0xb8
#define I2C_SMBUS_CLK_LOW_SEXT 0xbc
#define I2C_SMBUS_CLK_LOW_MEXT 0xc0
#define I2C_SMBUS_THIGH_MAX_IDLE_COUNT 0xc4
#define I2C_SMBUS_INTR_STAT 0xc8
#define I2C_SMBUS_INTR_MASK 0xcc
#define I2C_SMBUS_RAW_INTR_STAT 0xd0
#define I2C_CLR_SMBUS_INTR 0xd4
#define I2C_OPTIONAL_SAR 0xd8
//#define I2C_SMBUS_UDID_LSB		0xdc
#define I2C_SMBUS_UDID_WORD0 0xdc
#define I2C_SMBUS_UDID_WORD1 0xe0
#define I2C_SMBUS_UDID_WORD2 0xe4
#define I2C_SMBUS_UDID_WORD3 0xe8
#define I2C_REG_TIMEOUT_RST 0xf0
#define I2C_COMP_PARAM_1 0xf4
#define I2C_COMP_VERSION 0xf8
#define I2C_COMP_TYPE 0xfc

typedef enum {
	I2C_ADDR_MODE_7BIT = 0,
	I2C_ADDR_MODE_10BIT,
} I2C_ADDR_MODE_E;

typedef enum {
	I2C_SPEED_MODE_SS = 0,
	I2C_SPEED_MODE_FS,
	I2C_SPEED_MODE_HS,
} I2C_SPEED_MODE_E;

#define I2C_INTR_SCL_STUCK_AT_LOW (1 << 14)
#define I2C_INTR_MASTER_ON_HOLD (1 << 13)
#define I2C_INTR_RESTART_DET (1 << 12)
#define I2C_INTR_GEN_CALL (1 << 11)
#define I2C_INTR_START_DET (1 << 10)
#define I2C_INTR_STOP_DET (1 << 9)
#define I2C_INTR_ACTIVITY (1 << 8)
#define I2C_INTR_RX_DONE (1 << 7)
#define I2C_INTR_TX_ABRT (1 << 6)
#define I2C_INTR_RD_REQ (1 << 5)
#define I2C_INTR_TX_EMPTY (1 << 4)
#define I2C_INTR_TX_OVER (1 << 3)
#define I2C_INTR_RX_FULL (1 << 2)
#define I2C_INTR_RX_OVER (1 << 1)
#define I2C_INTR_RX_UNDER (1 << 0)

#define I2C_INTR_CLEAR_ABLE_MASK                                               \
	(I2C_INTR_RX_UNDER | I2C_INTR_RX_OVER | I2C_INTR_TX_OVER |             \
	 I2C_INTR_RD_REQ | I2C_INTR_TX_ABRT | I2C_INTR_RX_DONE |               \
	 I2C_INTR_ACTIVITY | I2C_INTR_STOP_DET | I2C_INTR_START_DET |          \
	 I2C_INTR_GEN_CALL)

#define I2C_STAT_SMBUS_ALERT_STATUS (1 << 20)
#define I2C_STAT_SMBUS_SUSPEND_STATUS (1 << 19)
#define I2C_STAT_SMBUS_SLAVE_ADDR_RESOLVED (1 << 18)
#define I2C_STAT_SMBUS_SLAVE_ADDR_VALID (1 << 17)
#define I2C_STAT_SMBUS_QUICK_CMD_BIT (1 << 16)
#define I2C_STAT_SDA_STUCK_NOT_RECOVERED (1 << 11)
#define I2C_STAT_SLV_HOLD_RX_FIFO_FULL (1 << 10)
#define I2C_STAT_SLV_HOLD_TX_FIFO_EMPTY (1 << 9)
#define I2C_STAT_MST_HOLD_RX_FIFO_FULL (1 << 8)
#define I2C_STAT_MST_HOLD_TX_FIFO_EMPTY (1 << 7)
#define I2C_STAT_SLV_ACTIVITY (1 << 6)
#define I2C_STAT_MST_ACTIVITY (1 << 5)
#define I2C_STAT_RFF (1 << 4)
#define I2C_STAT_RFNE (1 << 3)
#define I2C_STAT_TFE (1 << 2)
#define I2C_STAT_TFNF (1 << 1)
#define I2C_STAT_ACTIVITY (1 << 0)

#endif
