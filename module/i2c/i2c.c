/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <utility_lite.h>
#include <common.h>
#include <test_common.h>
#include <uart_sync.h>
#include <ut_test.h>
#include <int_num.h>
#include <hs_api.h>
#include "i2c.h"

#define I2C_WORK_CLOCK (4500000)
#define CLOCK_PER_US (I2C_WORK_CLOCK / (1000 * 1000))
#define CLOCK_PER_MS (I2C_WORK_CLOCK / 1000)

#define I2C_TEST_WR_LEN 12
#define I2C_SEND_LARGE_DATA_LEN 100

#define comp_one_reg(__base, __offset, realval)                                               \
	do {                                                                                  \
		u32 read_value = REG32_READ(__base + __offset);                               \
		hprintf(TSENV,                                                                \
			"REG_Addr: %08X  NAME: %-20s  read value: %08X  expect_value: %08X ", \
			__base + __offset, #__offset, read_value,                             \
			(u32)realval);                                                        \
		if (read_value != (u32)realval) {                                             \
			hprintf(TSENV, "  test result:   FAILED! \r\n ");                     \
		} else {                                                                      \
			hprintf(TSENV, "   test result:   SUCCESS! \r\n");                    \
		}                                                                             \
	} while (0)

void i2c0_irq_init(void);
void i2c0_irq_close(void);

/*
static int i2c_test_registers_dump(zebu_test_mod_t* mod,void* para)
{
	unsigned int reg_base;
	reg_base = mod->reg_base;
	comp_one_reg(reg_base, I2C_CON,                         0x0000003f);
        comp_one_reg(reg_base, I2C_TAR,                       0x00001055);
	comp_one_reg(reg_base, I2C_SAR,                         0x00000055);
	comp_one_reg(reg_base, I2C_HS_MADDR,                    0x00000001);
	comp_one_reg(reg_base, I2C_DATA_CMD,                    0x00000000);
	comp_one_reg(reg_base, I2C_SS_SCL_HCNT,                 0x00000190);
	comp_one_reg(reg_base, I2C_SS_SCL_LCNT,                 0x000001d6);
	comp_one_reg(reg_base, I2C_FS_SCL_HCNT,                 0x0000003c);
	comp_one_reg(reg_base, I2C_FS_SCL_LCNT,                 0x00000082);
	comp_one_reg(reg_base, I2C_HS_SCL_HCNT,                 0x00000006);
	comp_one_reg(reg_base, I2C_HS_SCL_LCNT,                 0x00000010);
	comp_one_reg(reg_base, I2C_INTR_STAT,                   0x00000000);
	comp_one_reg(reg_base, I2C_INTR_MASK,                   0x000048ff);
	comp_one_reg(reg_base, I2C_RAW_INTR_STAT,               0x00000000);
	comp_one_reg(reg_base, I2C_RX_TL,                       0x00000000);
	comp_one_reg(reg_base, I2C_TX_TL,                       0x00000000);
	comp_one_reg(reg_base, I2C_CLR_INTR,                    0x00000000);
	comp_one_reg(reg_base, I2C_CLR_RX_UNDER,                0x00000000);
	comp_one_reg(reg_base, I2C_CLR_RX_OVER,                 0x00000000);
	comp_one_reg(reg_base, I2C_CLR_TX_OVER,                 0x00000000);
	comp_one_reg(reg_base, I2C_CLR_RD_REQ,                  0x00000000);
	comp_one_reg(reg_base, I2C_CLR_TX_ABRT,                 0x00000000);
	comp_one_reg(reg_base, I2C_CLR_RX_DONE,                 0x00000000);
	comp_one_reg(reg_base, I2C_CLR_ACTIVITY,                0x00000000);
	comp_one_reg(reg_base, I2C_CLR_STOP_DET,                0x00000000);
	comp_one_reg(reg_base, I2C_CLR_START_DET,               0x00000000);
	comp_one_reg(reg_base, I2C_CLR_GEN_CALL,                0x00000000);
	comp_one_reg(reg_base, I2C_ENABLE,                      0x00000000);
	comp_one_reg(reg_base, I2C_STATUS,                      0x00000006);
	comp_one_reg(reg_base, I2C_TXFLR,                       0x00000000);
	comp_one_reg(reg_base, I2C_RXFLR,                       0x00000000);
	comp_one_reg(reg_base, I2C_SDA_HOLD,                    0x00000001);
	comp_one_reg(reg_base, I2C_TX_ABRT_SOURCE,              0x00000000);
	comp_one_reg(reg_base, I2C_SLV_DATA_NACK_ONLY,          0x00000000);
	comp_one_reg(reg_base, I2C_SDA_SETUP,                   0x00000064);
	comp_one_reg(reg_base, I2C_ACK_GENERAL_CALL,            0x00000001);
	comp_one_reg(reg_base, I2C_ENABLE_STATUS,               0x00000000);
	comp_one_reg(reg_base, I2C_FS_SPKLEN,                   0x00000005);
	comp_one_reg(reg_base, I2C_HS_SPKLEN,                   0x00000001);
	comp_one_reg(reg_base, I2C_SCL_STUCK_AT_LOW_TIMEOUT,    0xffffffff);
	comp_one_reg(reg_base, I2C_SDA_STUCK_AT_LOW_TIMEOUT,    0xffffffff);
	comp_one_reg(reg_base, I2C_CLR_SCL_STUCK_DET,           0x00000000);
	comp_one_reg(reg_base, I2C_DEVICE_ID,                   0x00000000);
	comp_one_reg(reg_base, I2C_COMP_PARAM_1,                0x000f0fae);
	comp_one_reg(reg_base, I2C_COMP_VERSION,                0x3230322a);
	comp_one_reg(reg_base, I2C_COMP_TYPE,                   0x44570140); 

        return 0;
}
*/
#define wr_one_reg_test(__base, __offset, __temp, __set_v, __exp_v, __errcnt)                         \
	do {                                                                                          \
		REG32_WRITE(__base + __offset, __set_v);                                              \
		__temp = (u32)REG32_READ(__base + __offset);                                          \
		__errcnt = (((u32)__temp != (u32)__exp_v) ? 1 : 0);                                   \
		hprintf(TSENV,                                                                        \
			"REG:%08X(%-20s) W/R Set-Read(Exp):%08X - %08X(%08X):     result -- %s \r\n", \
			__base + __offset, #__offset, __set_v, __temp,                                \
			__exp_v, (__errcnt ? "Nok" : "ok"));                                          \
	} while (0)

/*
static int i2c_test_registers_wr(zebu_test_mod_t* mod,void* para)
{
	u32 val;
	u32 result = 0;
	int reg_base;
	u32 ret = 0;
	reg_base = mod->reg_base;
		
	wr_one_reg_test(reg_base, I2C_CON ,val,                         0xffffffff & (~ 0x0000003f), 0x00000bc6, ret);
	wr_one_reg_test(reg_base, I2C_TAR ,val,                         0xffffffff & (~ 0x00001055), 0x00003fff & (~ 0x00001055), ret);
	wr_one_reg_test(reg_base, I2C_SAR ,val,                         0xffffffff & (~ 0x00000055), 0x000003ff & (~ 0x00000055) ,ret);
	wr_one_reg_test(reg_base, I2C_HS_MADDR	,val,               0xffffffff & (~ 0x00000001), 0x00000007 & (~ 0x00000001) ,ret);
	wr_one_reg_test(reg_base, I2C_DATA_CMD	,val,               0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_SS_SCL_HCNT ,val,                 0xffffffff & (~ 0x00000190), 0x0000ffff & (~ 0x00000190), ret);
	wr_one_reg_test(reg_base, I2C_SS_SCL_LCNT ,val,                 0xffffffff & (~ 0x000001d6), 0x0000ffff & (~ 0x000001d6), ret);
	wr_one_reg_test(reg_base, I2C_FS_SCL_HCNT ,val,                 0xffffffff & (~ 0x0000003c), 0x0000ffff & (~ 0x0000003c), ret);
	wr_one_reg_test(reg_base, I2C_FS_SCL_LCNT ,val,                 0xffffffff & (~ 0x00000082), 0x0000ffff & (~ 0x00000082), ret);
	wr_one_reg_test(reg_base, I2C_HS_SCL_HCNT ,val,                 0xffffffff & (~ 0x00000006), 0x0000ffff & (~ 0x00000006), ret);
	wr_one_reg_test(reg_base, I2C_HS_SCL_LCNT ,val,                 0xffffffff & (~ 0x00000010), 0x0000ffff & (~ 0x00000010), ret);
	wr_one_reg_test(reg_base, I2C_INTR_STAT ,val,                   0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_INTR_MASK ,val,                   0xffffffff & (~ 0x000048ff), 0x00000fff & (~ 0x000048ff),ret);
	wr_one_reg_test(reg_base, I2C_RAW_INTR_STAT ,val,               0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_RX_TL ,val,                       0xffffffff & (~ 0x00000000), 0x0000000f & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_TX_TL ,val,                       0xffffffff & (~ 0x00000000), 0x0000000f & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_INTR ,val,                    0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_RX_UNDER ,val,                0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_RX_OVER	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_TX_OVER	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_RD_REQ	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_TX_ABRT	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_RX_DONE	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_ACTIVITY	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_STOP_DET	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_START_DET ,    val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_GEN_CALL	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_ENABLE	,        val,           0xffffffff & (~ 0x00000000), 0x00000009 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_STATUS	,val,                   0xffffffff & (~ 0x00000006), 0x00000006 ,ret);
	wr_one_reg_test(reg_base, I2C_TXFLR ,            val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_RXFLR ,            val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_SDA_HOLD	,        val,           0xffffffff & (~ 0x00000001), 0x00000001 ,ret);
	wr_one_reg_test(reg_base, I2C_TX_ABRT_SOURCE	,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_SLV_DATA_NACK_ONLY,val,           0xffffffff & (~ 0x00000000), 0x00000000 ,ret);
	wr_one_reg_test(reg_base, I2C_SDA_SETUP ,        val,           0xffffffff & (~ 0x00000064),  0x00000064 ,ret);
	wr_one_reg_test(reg_base, I2C_ACK_GENERAL_CALL	,val,           0xffffffff & (~ 0x00000001), 0x00000001 & (~ 0x00000001) ,ret);
	wr_one_reg_test(reg_base, I2C_ENABLE_STATUS ,    val,           0xffffffff & (~ 0x00000000), 0x00000001 ,ret);
	wr_one_reg_test(reg_base, I2C_FS_SPKLEN ,        val,           0xffffffff & (~ 0x00000005), 0x00000005 ,ret);
	wr_one_reg_test(reg_base, I2C_HS_SPKLEN ,        val,           0xffffffff & (~ 0x00000001), 0x00000001 ,ret);
	wr_one_reg_test(reg_base, I2C_SCL_STUCK_AT_LOW_TIMEOUT,val,     0xffffffff & (~ 0xffffffff), 0xffffffff ,ret);
	wr_one_reg_test(reg_base, I2C_SDA_STUCK_AT_LOW_TIMEOUT,val,     0xffffffff & (~ 0xffffffff), 0xffffffff ,ret);
	wr_one_reg_test(reg_base, I2C_CLR_SCL_STUCK_DET ,val,           0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_DEVICE_ID ,val,                   0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) ,ret);
	wr_one_reg_test(reg_base, I2C_COMP_PARAM_1	,val,           0xffffffff & (~ 0x000f0f8e), 0x000f0fae ,ret);
	wr_one_reg_test(reg_base, I2C_COMP_VERSION	,val,           0xffffffff & (~ 0x3230322a), 0x3230322a ,ret);
	wr_one_reg_test(reg_base, I2C_COMP_TYPE ,        val,           0xffffffff & (~ 0x44570140), 0x44570140,ret);
	return ret;
}
*/
int bsta1000_i2c_status(unsigned int base)
{
	unsigned int val;
	val = REG32_READ(base + I2C_STATUS);
	return val;
}
/*
static int i2c_waiting_for_status_mask(zebu_test_mod_t* mod,unsigned int intrs,int flag,int timeout)
{
	unsigned int status;
	return 0;
	int time_cnt = 0;
	do{
		//delaySoft(1);
		status = bsta1000_i2c_status(mod->reg_base);
		if(time_cnt >= timeout){
			hprintf(TSENV, "[%s]%d.Waiting For I2c Status:%X Timeout(%d).\r\n",__func__,__LINE__,intrs,timeout);
			return -1;
		}
		time_cnt ++ ;
	}while((status & intrs) != flag);

	return 0;
}*/

static int i2c_master_write_read_7bit_addr_test_fs_plus(zebu_test_mod_t *mod,
							void *para)
{
	int base = mod->reg_base;
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) & 0xfffffff0); // disable i2c
	hprintf(TSENV, "set i2c master mode\n");
	REG32_WRITE(base + I2C_CON, REG32_READ(base + I2C_CON) |
					    0x00000001); // enbale master mode
	hprintf(TSENV, "set i2c fast speed mode operation\n");
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xfffffff9) |
			    (0x02 << 1)); // fast speed mode operation
	hprintf(TSENV, "set i2c 7-bit addressing mode\n");
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xffffffef) |
			    0x00000000); // 7-bit addressing mode
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffbf) |
					    (1 << 6)); // slave mode disable
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffdf) |
					    (1 << 5)); // master restart enable
	REG32_WRITE(base + I2C_TAR, (REG32_READ(base + I2C_TAR) & 0xfffffc00) |
					    I2C_SLAVE_ADDR); // set slave addr
	REG32_WRITE(
		base + I2C_TAR,
		(REG32_READ(base + I2C_TAR) & 0xffffefff) |
			0x00000000); // starts transfer in 7-bit addressing mode
	//i2c fs scl  hcnt reg
	REG32_WRITE(base + I2C_FS_SCL_HCNT, CLOCK_PER_US * 3 + 5);
	//i2c fs scl  lcnt reg
	REG32_WRITE(base + I2C_FS_SCL_LCNT, CLOCK_PER_US * 3 + 7);
	//i2c sda hold reg
	REG32_WRITE(base + I2C_SDA_HOLD, CLOCK_PER_US);
	//i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
	// set spike length
	REG32_WRITE(mod->reg_base + I2C_FS_SPKLEN, 0x01);
	hprintf(TSENV, "config i2c finish...\nenable i2c\n");
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) | 0x00000001); // enable i2c

	return 0;
}

static int i2c_master_write_read_7bit_addr_test_hs(zebu_test_mod_t *mod,
						   void *para)
{
	int base = mod->reg_base;
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) & 0xfffffff0); // disable i2c
	hprintf(TSENV, "set i2c master mode\n");
	REG32_WRITE(base + I2C_CON, REG32_READ(base + I2C_CON) |
					    0x00000001); // enbale master mode
	hprintf(TSENV, "set i2c high speed mode operation\n");
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xfffffff9) |
			    (0x03 << 1)); // high speed mode operation
	hprintf(TSENV, "set i2c 7-bit addressing mode\n");
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xffffffef) |
			    0x00000000); // 7-bit addressing mode
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffbf) |
					    (1 << 6)); // slave mode disable
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffdf) |
					    (1 << 5)); // master restart enable
	REG32_WRITE(base + I2C_TAR, (REG32_READ(base + I2C_TAR) & 0xfffffc00) |
					    I2C_SLAVE_ADDR); // set slave addr
	REG32_WRITE(
		base + I2C_TAR,
		(REG32_READ(base + I2C_TAR) & 0xffffefff) |
			0x00000000); // starts transfer in 7-bit addressing mode
	REG32_WRITE(base + I2C_HS_MADDR,
		    (REG32_READ(base + I2C_HS_MADDR) & 0xfffffff8) |
			    0x07); // set master code 0x07
	//i2c hs scl  hcnt reg
	REG32_WRITE(base + I2C_HS_SCL_HCNT, CLOCK_PER_US * 2 + 5);
	//i2c hs scl  lcnt reg
	REG32_WRITE(base + I2C_HS_SCL_LCNT, CLOCK_PER_US * 2 + 7);
	//i2c sda hold reg
	REG32_WRITE(base + I2C_SDA_HOLD, CLOCK_PER_US);
	//i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
	hprintf(TSENV, "config i2c finish...\nenable i2c\n");
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) | 0x00000001); // enable i2c

	return 0;
}

static int i2c_master_write_read_7bit_addr_test_fs(zebu_test_mod_t *mod,
						   void *para)
{
	int base = mod->reg_base;
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) & 0xfffffff0); // disable i2c
	hprintf(TSENV, "set i2c master mode\n");
	REG32_WRITE(base + I2C_CON, REG32_READ(base + I2C_CON) |
					    0x00000001); // enbale master mode
	hprintf(TSENV, "set i2c fast speed mode operation\n");
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xfffffff9) |
			    (0x02 << 1)); // fast speed mode operation
	hprintf(TSENV, "set i2c 7-bit addressing mode\n");
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xffffffef) |
			    0x00000000); // 7-bit addressing mode
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffbf) |
					    (1 << 6)); // slave mode disable
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffdf) |
					    (1 << 5)); // master restart enable
	REG32_WRITE(base + I2C_TAR, (REG32_READ(base + I2C_TAR) & 0xfffffc00) |
					    I2C_SLAVE_ADDR); // set slave addr
	REG32_WRITE(
		base + I2C_TAR,
		(REG32_READ(base + I2C_TAR) & 0xffffefff) |
			0x00000000); // starts transfer in 7-bit addressing mode

	//i2c fs scl  hcnt reg
	REG32_WRITE(base + I2C_FS_SCL_HCNT, CLOCK_PER_US * 3);
	//i2c fs scl  lcnt reg
	REG32_WRITE(base + I2C_FS_SCL_LCNT, CLOCK_PER_US * 3);
	//i2c sda hold reg
	REG32_WRITE(base + I2C_SDA_HOLD, CLOCK_PER_US);
	//i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
	hprintf(TSENV, "config i2c finish...\nenable i2c\n");
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) | 0x00000001); // enable i2c

	return 0;
}

static int i2c_master_write_read_7bit_addr_test(zebu_test_mod_t *mod,
						void *para)
{
	int base = mod->reg_base;
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) & 0xfffffff0); // disable i2c
	hprintf(TSENV, "enbale i2c master mode\n");
	REG32_WRITE(base + I2C_CON, REG32_READ(base + I2C_CON) |
					    0x00000001); // enbale master mode
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xfffffff9) |
			    0x00000002); // standard speed mode operation
	hprintf(TSENV, "config i2c 7-bit addressing mode\n");
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xffffffef) |
			    0x00000000); // 7-bit addressing mode
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffbf) |
					    (1 << 6)); // slave mode disable
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffdf) |
					    (1 << 5)); // master restart enable
	REG32_WRITE(base + I2C_TAR, (REG32_READ(base + I2C_TAR) & 0xfffffc00) |
					    I2C_SLAVE_ADDR); // set slave addr
	REG32_WRITE(
		base + I2C_TAR,
		(REG32_READ(base + I2C_TAR) & 0xffffefff) |
			0x00000000); // starts transfer in 7-bit addressing mode
	//i2c standard slc  hcnt reg
	REG32_WRITE(base + I2C_SS_SCL_HCNT, CLOCK_PER_US * 10);
	//i2c standard slc  lcnt reg
	REG32_WRITE(base + I2C_SS_SCL_LCNT, CLOCK_PER_US * 10);
	//i2c sda hold reg
	REG32_WRITE(base + I2C_SDA_HOLD, CLOCK_PER_US);
	//i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
	hprintf(TSENV, "config i2c finish...\nenable i2c\n");
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) | 0x00000001); // enable i2c

	return 0;
}

static int i2c_master_write_read_10bit_addr_test(zebu_test_mod_t *mod,
						 void *para)
{
	int base = mod->reg_base;
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) & 0xfffffff0); // disable i2c
	hprintf(TSENV, "enable i2c master mode\n");
	REG32_WRITE(base + I2C_CON, REG32_READ(base + I2C_CON) |
					    0x00000001); // enbale master mode
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xfffffff9) |
			    0x00000002); // standard speed mode operation
	hprintf(TSENV, "config i2c 10-bit addressing mode\n");
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffef) |
					    (1 << 4)); // 10-bit addressing mode
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffbf) |
					    (1 << 6)); // slave mode disable
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffdf) |
					    (1 << 5)); // master restart enable
	REG32_WRITE(base + I2C_TAR,
		    (REG32_READ(base + I2C_TAR) & 0xfffffc00) |
			    I2C_SLAVE_ADDR_10bit); // set slave addr
	REG32_WRITE(
		base + I2C_TAR,
		REG32_READ(base + I2C_TAR) |
			(1 << 12)); // starts transfer in 10-bit addressing mode
	//i2c standard slc  hcnt reg
	REG32_WRITE(base + I2C_SS_SCL_HCNT, CLOCK_PER_US * 8);
	//i2c standard slc  lcnt reg
	REG32_WRITE(base + I2C_SS_SCL_LCNT, CLOCK_PER_US * 8);
	//i2c sda hold reg
	REG32_WRITE(base + I2C_SDA_HOLD, CLOCK_PER_US);
	//i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 1);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
	hprintf(TSENV, "config i2c finish...\nenable i2c\n");
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) | 0x00000001); // enable i2c

	return 0;
}
/*
static int i2c_master_send_invalid_addr_7bit(zebu_test_mod_t* mod,void* para)
{
      int i=0;
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffffe );  // disable i2c
      REG32_WRITE( base + I2C_CON,    REG32_READ(base + I2C_CON) | 0x00000001 ); // enbale master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | 0x00000002 ); // standard speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffef) | 0x00000000 ); // 7-bit addressing mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (1<<6) ); // slave mode disable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (1<<5) ); // master restart enable
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xfffffc00) | I2C_INVALID_ADDR ); // set slave addr
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xffffefff) | 0x00000000 ); // starts transfer in 7-bit addressing mode
	//i2c standard slc  hcnt reg
	REG32_WRITE( base + I2C_SS_SCL_HCNT, CLOCK_PER_US*8);
	//i2c standard slc  lcnt reg
	REG32_WRITE( base + I2C_SS_SCL_LCNT, CLOCK_PER_US*8);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
        //i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 1);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | 0x00000001 );  // enable i2c

      REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | 0xff );
      // check slave ack
      while (1){
         i++;
         if(i % 100 == 0){
               hprintf(TSENV, "wait for noack generated: %05X   \n", i);
           }
         if (i > 10000){
                   hprintf(TSENV, "timeout, test failed \n");
                        break;
           }
         if( REG32_READ(base + I2C_TX_ABRT_SOURCE) & 0x00000001 ){
              hprintf(TSENV, " \n No ACK is detected,  test success! \n");  
              break;
           }
       }
}*/
/*
static int i2c_master_scl_stuck_at_low_test(zebu_test_mod_t* mod,void* para)
{
      int i=0;
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffffe );  // disable i2c
      REG32_WRITE( base + I2C_CON,    REG32_READ(base + I2C_CON) | 0x00000001 ); // enbale master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | 0x00000002 ); // standard speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffef) | 0x00000000 ); // 7-bit addressing mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (1<<6) ); // slave mode disable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (1<<5) ); // master restart enable
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xfffffc00) | I2C_SLAVE_ADDR ); // set slave addr
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xffffefff) | 0x00000000 ); // starts transfer in 7-bit addressing mode
	//i2c standard slc  hcnt reg
	REG32_WRITE( base + I2C_SS_SCL_HCNT, CLOCK_PER_US*8);
	//i2c standard slc  lcnt reg
	REG32_WRITE( base + I2C_SS_SCL_LCNT, CLOCK_PER_US*8);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
        //i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
        
      // write SCL_STUCK_TIMEOUT
      REG32_WRITE(mod->reg_base + I2C_SCL_STUCK_AT_LOW_TIMEOUT, 0);
      // disable mask
      REG32_WRITE( mod->reg_base + I2C_INTR_MASK, REG32_READ(mod->reg_base + I2C_INTR_MASK) | (1<<14) );
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | 0x00000001 );  // enable i2c

      // delay
      i = 0;
      while( i < 1000) i++;
      
      if( REG32_READ(base + I2C_RAW_INTR_STAT) & (1<<14) != 0 ){ hprintf(TSENV, "DET SCL_STUCK_AT_LOW Interrupt \n"); }
           else{ hprintf(TSENV, "Cannot Generate SCL_STUCK_AT_LOW Interrupt, Test Failed \n");  return 0;}
      //REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | 0xff );
      //hprintf(TSENV, "Send one byte data 0xff. if the write operation is not blocked, test failed \n");

      i = 0;
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffffe );  // disable i2c
      REG32_WRITE(mod->reg_base + I2C_SCL_STUCK_AT_LOW_TIMEOUT, 0xffffffff);  
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | 0x00000001 );  // enable i2c
      while( REG32_READ(base + I2C_RAW_INTR_STAT) & (1<<14) != 0 ){ 
          
        REG32_READ(base + I2C_CLR_SCL_STUCK_DET); // clear SCL_STUCK_AT_LOW Interrupt
          i++;
          if (i > 1000) { hprintf(TSENV, "recovery timeout, Test Failed \n"); return 0; }
       }
      REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | (1<<9) ); 
        hprintf(TSENV, "Send one byte 0x00 data  again. if the write operation is not blocked, test success \n");
}*/
/*
static int i2c_master_sda_stuck_at_low_test(zebu_test_mod_t* mod,void* para)
{
      int i=0;
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffffe );  // disable i2c
      REG32_WRITE( base + I2C_CON,    REG32_READ(base + I2C_CON) | 0x00000001 ); // enbale master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | 0x00000002 ); // standard speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffef) | 0x00000000 ); // 7-bit addressing mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (1<<6) ); // slave mode disable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (1<<5) ); // master restart enable
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xfffffc00) | I2C_SLAVE_ADDR ); // set slave addr
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xffffefff) | 0x00000000 ); // starts transfer in 7-bit addressing mode
	//i2c standard slc  hcnt reg
	REG32_WRITE( base + I2C_SS_SCL_HCNT, CLOCK_PER_US*8);
	//i2c standard slc  lcnt reg
	REG32_WRITE( base + I2C_SS_SCL_LCNT, CLOCK_PER_US*8);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
        //i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
        
        // write SDA_STUCK_TIMEOUT
        REG32_WRITE(mod->reg_base + I2C_SDA_STUCK_AT_LOW_TIMEOUT, 0x01);
        REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | 0x00000001 );  // enable i2c
       
      
       while(i < 1000){ i++;} // wait time
       if( REG32_READ(base + I2C_TX_ABRT_SOURCE) & (1<<17) != 0 ){ hprintf(TSENV, "DET SDA_STUCK_AT_LOW Interrupt \n"); }
       else{ hprintf(TSENV, "Cannot Generate SDA_STUCK_AT_LOW Interrupt, Test Failed \n"); return 0; }

       //REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | 0xff );
       //hprintf(TSENV, "Send one byte data 0xff. if the write operation is not blocked, test failed \n");

       REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffffe );  // disable i2c
       REG32_WRITE(mod->reg_base + I2C_SDA_STUCK_AT_LOW_TIMEOUT, 0xffffffff);
       REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | (1<<3) | 0x01) ;  // enable sda revocery mode  and enable i2c

       // wait for recovery bit auto clear
       i = 0;
       while( REG32_READ(base + I2C_ENABLE) & (1<<3) != 0 )
         {
            i++;
            if(i > 2000){
                  hprintf(TSENV, " recovery timeout, test failed! \n");
                  return 0;
                  break; 
                }
         }
	 if( REG32_READ(base + I2C_TX_ABRT_SOURCE) & (1<<17) != 0 )
          {
	    hprintf(TSENV, "can not clear interrupt flag, test failed ! \n"); 
             return 0;
          }

        REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | (1<<9) ); 
        hprintf(TSENV, "Send one byte data 0x00 again. if the write operation is not blocked, test success \n");
}*/
/*
static int i2c_master_spike_fs_plus_test(zebu_test_mod_t* mod,void* para)
{
      char tx_buf[I2C_TEST_WR_LEN] = {0};
      char rx_buf[I2C_TEST_WR_LEN] = {0};
      int i, ret;
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffff0 );  // disable i2c
      REG32_WRITE( base + I2C_CON,    REG32_READ(base + I2C_CON) | 0x00000001 ); // enbale master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | (0x02<<1) ); // fast speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffef) | 0x00000000 ); // 7-bit addressing mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (1<<6) ); // slave mode disable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (1<<5) ); // master restart enable
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xfffffc00) | I2C_SLAVE_ADDR ); // set slave addr
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xffffefff) | 0x00000000 ); // starts transfer in 7-bit addressing mode
	//i2c fs scl  hcnt reg
	REG32_WRITE( base + I2C_FS_SCL_HCNT, CLOCK_PER_US*3+5);
	//i2c fs scl  lcnt reg
	REG32_WRITE( base + I2C_FS_SCL_LCNT, CLOCK_PER_US*3+7);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
      //i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
        // set spike length
      REG32_WRITE(mod->reg_base + I2C_FS_SPKLEN,  0x05); 
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | 0x00000001 );  // enable i2c

      // write data to slave
      hprintf(TSENV,"begin send large data ... \n");
      for(i = 0 ;i < I2C_SEND_LARGE_DATA_LEN ; i++){
	  tx_buf[i] = ((i & 1) ? i : ~i ) & 0xff;
        ret = i2c_waiting_for_status_mask(mod, I2C_STAT_TFNF ,I2C_STAT_TFNF ,20);
	  if(ret < 0){
            hprintf(TSENV,"Transmit FIFO is full, count = %d \n", i);
               continue;
	  }
        if(i != I2C_SEND_LARGE_DATA_LEN-1){
            REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | tx_buf[i] );
          }
        else{
            REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | tx_buf[i] | (1<<9) );  // generate stop
          }
        hprintf(TSENV, "I2C Master Send:  %08X  \r\n", tx_buf[i]);	
        }
	return 0;
}*/

/*
static int i2c_master_spike_hs_test(zebu_test_mod_t* mod,void* para)
{
      char tx_buf[I2C_TEST_WR_LEN] = {0};
      char rx_buf[I2C_TEST_WR_LEN] = {0};
      int i, ret;
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffff0 );  // disable i2c
      REG32_WRITE( base + I2C_CON,    REG32_READ(base + I2C_CON) | 0x00000001 ); // enbale master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | (0x03<<1) ); // high speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffef) | 0x00000000 ); // 7-bit addressing mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (1<<6) ); // slave mode disable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (1<<5) ); // master restart enable
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xfffffc00) | I2C_SLAVE_ADDR ); // set slave addr
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xffffefff) | 0x00000000 ); // starts transfer in 7-bit addressing mode
      REG32_WRITE( base + I2C_HS_MADDR,    (REG32_READ(base + I2C_HS_MADDR) & 0xfffffff8) | 0x07 ); // set master code 0x07 
	//i2c hs scl  hcnt reg
	REG32_WRITE( base + I2C_HS_SCL_HCNT, CLOCK_PER_US*2 + 5);
	//i2c hs scl  lcnt reg
	REG32_WRITE( base + I2C_HS_SCL_LCNT, CLOCK_PER_US*2 + 7);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
      //i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
      // spike length
      REG32_WRITE(mod->reg_base + I2C_HS_SPKLEN,  0x05);
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | 0x00000001 );  // enable i2c

      // write data to slave
      hprintf(TSENV,"begin send large data ... \n");
      for(i = 0 ;i < I2C_SEND_LARGE_DATA_LEN ; i++){
	  tx_buf[i] = ((i & 1) ? i : ~i ) & 0xff;
        ret = i2c_waiting_for_status_mask(mod, I2C_STAT_TFNF ,I2C_STAT_TFNF ,20);
	  if(ret < 0){
            //hprintf(TSENV,"Transmit FIFO is full, count = %d \n", i);
               continue;
	  }
        if(i != I2C_SEND_LARGE_DATA_LEN-1){
            REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | tx_buf[i] );
          }
        else{
            REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | tx_buf[i] | (1<<9) );  // generate stop
          }
        //hprintf(TSENV, "I2C Master Send:  %08X  \r\n", tx_buf[i]);	
        }
	return 0;
}*/

/*
static int i2c_master_spike_fs_test(zebu_test_mod_t* mod,void* para)
{
      char tx_buf[I2C_TEST_WR_LEN] = {0};
      char rx_buf[I2C_TEST_WR_LEN] = {0};
      int i, ret;
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffff0 );  // disable i2c
      REG32_WRITE( base + I2C_CON,    REG32_READ(base + I2C_CON) | 0x00000001 ); // enbale master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | (0x02<<1) ); // fast speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffef) | 0x00000000 ); // 7-bit addressing mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (1<<6) ); // slave mode disable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (1<<5) ); // master restart enable
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xfffffc00) | I2C_SLAVE_ADDR ); // set slave addr
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xffffefff) | 0x00000000 ); // starts transfer in 7-bit addressing mode
	//i2c fs scl  hcnt reg
	REG32_WRITE( base + I2C_FS_SCL_HCNT, CLOCK_PER_US*3);
	//i2c fs scl  lcnt reg
	REG32_WRITE( base + I2C_FS_SCL_LCNT, CLOCK_PER_US*3);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
      //i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
        // set spike length
      REG32_WRITE(mod->reg_base + I2C_FS_SPKLEN,  0x05); 
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | 0x00000001 );  // enable i2c

      // write data to slave
      hprintf(TSENV,"begin send large data ... \n");
      for(i = 0 ;i < I2C_SEND_LARGE_DATA_LEN ; i++){
	  tx_buf[i] = ((i & 1) ? i : ~i ) & 0xff;
        ret = i2c_waiting_for_status_mask(mod, I2C_STAT_TFNF ,I2C_STAT_TFNF ,20);
	  if(ret < 0){
            //hprintf(TSENV,"Transmit FIFO is full, count = %d \n", i);
               continue;
	  }
        if(i != I2C_SEND_LARGE_DATA_LEN-1){
            REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | tx_buf[i] );
          }
        else{
            REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | tx_buf[i] | (1<<9) );  // generate stop
          }
        //hprintf(TSENV, "I2C Master Send:  %08X  \r\n", tx_buf[i]);	
        }
	return 0;
}*/
/*
static int i2c_master_spike_ss_test(zebu_test_mod_t* mod,void* para)
{
      char tx_buf[I2C_TEST_WR_LEN] = {0};
      char rx_buf[I2C_TEST_WR_LEN] = {0};
      int i, ret;
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) & 0xfffffff0 );  // disable i2c
      REG32_WRITE( base + I2C_CON,    REG32_READ(base + I2C_CON) | 0x00000001 ); // enbale master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | 0x00000002 ); // standard speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffef) | 0x00000000 ); // 7-bit addressing mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (1<<6) ); // slave mode disable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (1<<5) ); // master restart enable
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xfffffc00) | I2C_SLAVE_ADDR ); // set slave addr
      REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & 0xffffefff) | 0x00000000 ); // starts transfer in 7-bit addressing mode
	//i2c standard slc  hcnt reg
	REG32_WRITE( base + I2C_SS_SCL_HCNT, CLOCK_PER_US*10);
	//i2c standard slc  lcnt reg
	REG32_WRITE( base + I2C_SS_SCL_LCNT, CLOCK_PER_US*10);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
        //i2c rx tl reg(fifo threshold level)
	REG32_WRITE(mod->reg_base + I2C_RX_TL, 0);
	REG32_WRITE(mod->reg_base + I2C_TX_TL, 0);
        // set spike length
      REG32_WRITE(mod->reg_base + I2C_FS_SPKLEN,  0x05); 
      REG32_WRITE( base + I2C_ENABLE, REG32_READ(base + I2C_ENABLE) | 0x00000001 );  // enable i2c

      // write data to slave
      hprintf(TSENV, "begin send data... \n");
      for(i = 0 ;i < I2C_SEND_LARGE_DATA_LEN ; i++){
	  tx_buf[i] = ((i & 1) ? i : ~i ) & 0xff;
        ret = i2c_waiting_for_status_mask(mod, I2C_STAT_TFNF ,I2C_STAT_TFNF ,20);
	  if(ret < 0){
            //hprintf(TSENV,"Transmit FIFO is full, count = %d \n", i);
               continue;
	  }
        if(i != I2C_SEND_LARGE_DATA_LEN - 1){
            REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | tx_buf[i] );
          }
        else{
            REG32_WRITE( base + I2C_DATA_CMD, (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) | tx_buf[i] | (1<<9) );  // generate stop
          }	
        }
	return 0;
}
*/
static void wr_cmd(int base)
{
	int i, ret;
	u32 value;
	char tx_buf[I2C_TEST_WR_LEN] = { 0 };
	char rx_buf[I2C_TEST_WR_LEN] = { 0 };
	// write data to slave
	for (i = 0; i < I2C_TEST_WR_LEN; i++) {
		tx_buf[i] = ((i & 1) ? i : ~i) & 0xff;
		//   ret = i2c_waiting_for_status_mask(mod, I2C_STAT_TFNF ,I2C_STAT_TFNF ,20);
		//  if(ret < 0){
		//       hprintf(TSENV,"Transmit FIFO is full, count = %d \n", i);
		//         continue;
		//  }
		if (i != I2C_TEST_WR_LEN - 1) {
			REG32_WRITE(base + I2C_DATA_CMD,
				    (REG32_READ(base + I2C_DATA_CMD) &
				     0xffffff00) |
					    tx_buf[i]);
		} else {
			REG32_WRITE(
				base + I2C_DATA_CMD,
				(REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) |
					tx_buf[i] | (1 << 9)); // generate stop
		}
		hprintf(TSENV, "I2C Master Send:  %08X  \r\n", tx_buf[i]);
	}

	// read data from slave
	REG32_WRITE(base + I2C_DATA_CMD,
		    (REG32_READ(base + I2C_DATA_CMD) & 0xffffff00) |
			    0xff); // write reg to be read
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) & 0xfffffff0); // disable i2c
	REG32_WRITE(base + I2C_ACK_GENERAL_CALL, 0x01); // generate ack
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) | 0x00000001); // enable i2c
	for (i = 0; i < I2C_TEST_WR_LEN; i++) {
		REG32_WRITE(base + I2C_DATA_CMD,
			    REG32_READ(base + I2C_DATA_CMD) |
				    (1 << 8)); // master read command
		hprintf(TSENV, " enter one reading \n");
		ret = 1000;
		while (ret > 0) {
			ret--;
			if ((REG32_READ(base + I2C_STATUS) & (1 << 3)) != 0x0) {
				break;
			}
			if (ret == 0) {
				hprintf(TSENV,
					"Reveive FIFO is empty, count = %d \n",
					i);
				continue;
			}
		}
		value = REG32_READ(base + I2C_DATA_CMD);
		if ((value & (1 << 11)) == 1 << 11)
			hprintf(TSENV,
				"The first received byte flag is detected.\n");
		rx_buf[i] = value & 0xff;
		hprintf(TSENV, "I2C Master read:  %08X  \r\n", rx_buf[i]);
	}
	REG32_WRITE(base + I2C_DATA_CMD, REG32_READ(base + I2C_DATA_CMD) |
						 (1 << 9)); // generate stop
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) & 0x00000000); // disable i2c
}

void sdelay(int n)
{
	while (n--)
		;
}

/*==============================SLAVE TEST=========================================*/
static int i2c_slave_write_read_7bit_addr_test(zebu_test_mod_t *mod, void *para)
{
	int i = 0;
	int j;
	u32 temp;
	char tx_buf[I2C_TEST_WR_LEN] = { 0 };
	char rx_buf[I2C_TEST_WR_LEN] = { 0 };
	int base = mod->reg_base;

	hprintf(TSENV, "start i2c slave wr 7bit test\nclose i2c irq\n");
	i2c0_irq_close();

	REG32_WRITE(base + I2C_ENABLE,
		    (REG32_READ(base + I2C_ENABLE) & 0xfffffffe) |
			    (0x0 << 0)); // disable i2c
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xfffffffe) |
					    (0x0 << 0)); // disable master mode
	//     REG32_WRITE( base + I2C_TAR,    (REG32_READ(base + I2C_TAR) & (~(0x1<<12))) | (0x0<<12) );
	REG32_WRITE(base + I2C_CON,
		    (REG32_READ(base + I2C_CON) & 0xfffffff9) |
			    (0x01 << 1)); // standard speed mode operation
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xfffffff7) |
					    (0x0 << 3)); // 7-bit slave addr
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffbf) |
					    (0 << 6)); // slave mode enable
	REG32_WRITE(base + I2C_CON, (REG32_READ(base + I2C_CON) & 0xffffffdf) |
					    (0 << 5)); // master restart disable
	REG32_WRITE(base + I2C_SAR, (REG32_READ(base + I2C_SAR) & 0xfffffc00) |
					    I2C_SLAVE_ADDR); // set slave addr

	//i2c standard slc  hcnt reg
	REG32_WRITE(base + I2C_SS_SCL_HCNT, 500);
	//i2c standard slc  lcnt reg
	REG32_WRITE(base + I2C_SS_SCL_LCNT, 470);
	//i2c sda hold reg
	REG32_WRITE(base + I2C_SDA_HOLD, CLOCK_PER_US);
	// sda_setup_cnt
	REG32_WRITE(base + I2C_SDA_SETUP, 8);
	// ack general call
	REG32_WRITE(base + I2C_ACK_GENERAL_CALL,
		    0x00000001); // enable ack general call
	//i2c rx tl reg(fifo threshold level)
	REG32_WRITE(base + I2C_RX_TL, 12);
	REG32_WRITE(base + I2C_TX_TL, 0);

	// set interrupt mask
	REG32_WRITE(base + I2C_INTR_MASK,
		    (REG32_READ(base + I2C_INTR_MASK) & 0xffffffdf) |
			    (0 << 5)); // mask rd_req interrupt
	REG32_WRITE(base + I2C_INTR_MASK,
		    (REG32_READ(base + I2C_INTR_MASK) & 0xffffffbf) |
			    (0 << 6)); // mask tx_abrt interrupt
	REG32_WRITE(base + I2C_INTR_MASK,
		    (REG32_READ(base + I2C_INTR_MASK) & 0xfffffffb) |
			    (0 << 2)); // mask rx_full interrupt
	// enable i2c
	REG32_WRITE(base + I2C_ENABLE,
		    REG32_READ(base + I2C_ENABLE) | (0x1 << 0)); // enable i2c
	hprintf(TSENV, "enable i2c\n");
	xtor_dut_sync_write("sync");
	hprintf(TSENV, "\n finish time sync\n");

	// receive data from master
	i = 0;
	j = 0;
	while ((REG32_READ(base + I2C_STATUS) & (1 << 4)) == 0)
		;
	hprintf(TSENV, "RX fifo is full...\n");
	for (i = 0; i < I2C_TEST_WR_LEN; i++) {
		rx_buf[i] = REG32_READ(base + I2C_DATA_CMD) & 0xff;
		hprintf(TSENV, "read [%08X]:  %08X  \r\n", i, rx_buf[i]);
	}
	hprintf(TSENV,
		"\n finish receiving data !\nstart send data to master...\n");

	// slave transmitter
	while ((REG32_READ(base + I2C_RAW_INTR_STAT) & (1 << 6)) !=
	       0x00) // wait for tx_abrt clear
	{
		hprintf(TSENV, "tx_abrt_source: %08X \n",
			REG32_READ(base + I2C_TX_ABRT_SOURCE));
		REG32_READ(base + I2C_CLR_TX_ABRT);
	}
	hprintf(TSENV, "tx_abrt cleared \n");

	temp = REG32_READ(base + I2C_DATA_CMD);
	for (i = 0; i < I2C_TEST_WR_LEN; i++) {
		tx_buf[i] = i & 0xff;
		while ((REG32_READ(base + I2C_RAW_INTR_STAT) & (1 << 5)) ==
		       0x00)
			; // wait for rd_req
		REG32_WRITE(base + I2C_DATA_CMD, temp | tx_buf[i]);
		//hprintf( TSENV, "write: %08X \n", tx_buf[i] );
	}
	hprintf(TSENV, "test success! index: %x \n", i);
	i2c0_irq_init();
	return 0;
}
/*
static int i2c_slave_write_read_10bit_addr_test(zebu_test_mod_t* mod,void* para)
{
      int i=0;
	int ret;
      char tx_buf[I2C_TEST_WR_LEN] = {0};
      char rx_buf[I2C_TEST_WR_LEN] = {0};
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, (REG32_READ(base + I2C_ENABLE) & 0xfffffffe) | (0x0<<0));  // disable i2c
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffffe) | (0x0<<0) ); // disable master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | (0x01<<1) ); // standard speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff7) | (0x1<<3) ); // 10-bit slave addr
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (0<<6) ); // slave mode enable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (0<<5) ); // master restart disable
      REG32_WRITE( base + I2C_SAR,    (REG32_READ(base + I2C_SAR) & 0xfffffc00) | I2C_SLAVE_ADDR_10bit ); // set slave addr
	//i2c standard slc  hcnt reg
	REG32_WRITE( base + I2C_SS_SCL_HCNT, CLOCK_PER_US*10);
	//i2c standard slc  lcnt reg
	REG32_WRITE( base + I2C_SS_SCL_LCNT, CLOCK_PER_US*10);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
        //i2c rx tl reg(fifo threshold level)
	REG32_WRITE( base + I2C_RX_TL, 0);
	REG32_WRITE( base + I2C_TX_TL, 0);
	// set interrupt mask
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xffffffdf) | (0<<5) ); // mask rd_req interrupt
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xffffffbf) | (0<<6) ); // mask tx_abrt interrupt
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xfffffffb) | (0<<2) ); // mask rx_full interrupt
	// enable i2c
      REG32_WRITE( base + I2C_ENABLE,     REG32_READ(base + I2C_ENABLE) | (0x1<<0) );  // enable i2c

	// receive data from master
      for(i = 0 ;i < I2C_TEST_WR_LEN+1; i++){
		while( REG32_READ(base + I2C_STATUS) & (1<<3) == 0x0);
		rx_buf[i] = REG32_READ( base + I2C_DATA_CMD) & 0xff;
		hprintf(TSENV, "read:  %08X  \r\n", rx_buf[i]);
        }	

	// slave transmitter
	for(i = 0; i < I2C_TEST_WR_LEN; i++){
		tx_buf[i] = i & 0xff;
		while( REG32_READ( base + I2C_RAW_INTR_STAT ) & (1<<5) == 0x00 ); // wait for rd_req
		while( REG32_READ( base + I2C_RAW_INTR_STAT ) & (1<<6) != 0x00 ); // wait for tx_abrt clear
		REG32_WRITE( base + I2C_DATA_CMD, REG32_READ( base + I2C_DATA_CMD ) & 0xfffffe00 | (0<<8) | tx_buf[i] );
		hprintf( TSENV, "write: %08X \n", tx_buf[i] );
	}
	hprintf( TSENV, "test success! \n" );
	return 0;
}*/
/*
static int i2c_slave_write_read_7bit_addr_test_fs(zebu_test_mod_t* mod,void* para)
{
      int i=0;
	int ret;
      char tx_buf[I2C_TEST_WR_LEN] = {0};
      char rx_buf[I2C_TEST_WR_LEN] = {0};
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, (REG32_READ(base + I2C_ENABLE) & 0xfffffffe) | (0x0<<0));  // disable i2c
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffffe) | (0x0<<0) ); // disable master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | (0x02<<1) ); // fast speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff7) | (0x0<<3) ); // 7-bit slave addr
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (0<<6) ); // slave mode enable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (0<<5) ); // master restart disable
      REG32_WRITE( base + I2C_SAR,    (REG32_READ(base + I2C_SAR) & 0xfffffc00) | I2C_SLAVE_ADDR ); // set slave addr
	//i2c fs slc  hcnt reg
	REG32_WRITE( base + I2C_SS_SCL_HCNT, CLOCK_PER_US*3);
	//i2c fs slc  lcnt reg
	REG32_WRITE( base + I2C_SS_SCL_LCNT, CLOCK_PER_US*3);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
        //i2c rx tl reg(fifo threshold level)
	REG32_WRITE( base + I2C_RX_TL, 0);
	REG32_WRITE( base + I2C_TX_TL, 0);
	// set interrupt mask
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xffffffdf) | (0<<5) ); // mask rd_req interrupt
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xffffffbf) | (0<<6) ); // mask tx_abrt interrupt
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xfffffffb) | (0<<2) ); // mask rx_full interrupt
	// enable i2c
      REG32_WRITE( base + I2C_ENABLE,     REG32_READ(base + I2C_ENABLE) | (0x1<<0) );  // enable i2c

	// receive data from master
      for(i = 0 ;i < I2C_TEST_WR_LEN+1; i++){
		while( REG32_READ(base + I2C_STATUS) & (1<<3) == 0x0);
		rx_buf[i] = REG32_READ( base + I2C_DATA_CMD) & 0xff;
		hprintf(TSENV, "read:  %08X  \r\n", rx_buf[i]);
        }	

	// slave transmitter
	for(i = 0; i < I2C_TEST_WR_LEN; i++){
		tx_buf[i] = i & 0xff;
		while( REG32_READ( base + I2C_RAW_INTR_STAT ) & (1<<5) == 0x00 ); // wait for rd_req
		while( REG32_READ( base + I2C_RAW_INTR_STAT ) & (1<<6) != 0x00 ); // wait for tx_abrt clear
		REG32_WRITE( base + I2C_DATA_CMD, REG32_READ( base + I2C_DATA_CMD ) & 0xfffffe00 | (0<<8) | tx_buf[i] );
		hprintf( TSENV, "write: %08X \n", tx_buf[i] );
	}
	hprintf( TSENV, "test success! \n" );
	return 0;
}*/
/*
static int i2c_slave_write_read_7bit_addr_test_hs(zebu_test_mod_t* mod,void* para)
{
      int i=0;
	int ret;
      char tx_buf[I2C_TEST_WR_LEN] = {0};
      char rx_buf[I2C_TEST_WR_LEN] = {0};
	int base = mod->reg_base;
      REG32_WRITE( base + I2C_ENABLE, (REG32_READ(base + I2C_ENABLE) & 0xfffffffe) | (0x0<<0));  // disable i2c
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffffe) | (0x0<<0) ); // disable master mode
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff9) | (0x03<<1) ); // high speed mode operation
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xfffffff7) | (0x0<<3) ); // 7-bit slave addr
      REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffbf) | (0<<6) ); // slave mode enable 
	REG32_WRITE( base + I2C_CON,    (REG32_READ(base + I2C_CON) & 0xffffffdf) | (0<<5) ); // master restart disable
      REG32_WRITE( base + I2C_SAR,    (REG32_READ(base + I2C_SAR) & 0xfffffc00) | I2C_SLAVE_ADDR ); // set slave addr
	//i2c hs slc  hcnt reg
	REG32_WRITE( base + I2C_SS_SCL_HCNT, CLOCK_PER_US + 5);
	//i2c hs slc  lcnt reg
	REG32_WRITE( base + I2C_SS_SCL_LCNT, CLOCK_PER_US + 7);
      //i2c sda hold reg
	REG32_WRITE( base + I2C_SDA_HOLD, CLOCK_PER_US);
        //i2c rx tl reg(fifo threshold level)
	REG32_WRITE( base + I2C_RX_TL, 0);
	REG32_WRITE( base + I2C_TX_TL, 0);
	// set interrupt mask
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xffffffdf) | (0<<5) ); // mask rd_req interrupt
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xffffffbf) | (0<<6) ); // mask tx_abrt interrupt
	REG32_WRITE( base + I2C_INTR_MASK, (REG32_READ(base + I2C_INTR_MASK) & 0xfffffffb) | (0<<2) ); // mask rx_full interrupt
	// enable i2c
      REG32_WRITE( base + I2C_ENABLE,     REG32_READ(base + I2C_ENABLE) | (0x1<<0) );  // enable i2c

	// receive data from master
      for(i = 0 ;i < I2C_TEST_WR_LEN+1; i++){
		while( REG32_READ(base + I2C_STATUS) & (1<<3) == 0x0);
		rx_buf[i] = REG32_READ( base + I2C_DATA_CMD) & 0xff;
		hprintf(TSENV, "read:  %08X  \r\n", rx_buf[i]);
        }	

	// slave transmitter
	for(i = 0; i < I2C_TEST_WR_LEN; i++){
		tx_buf[i] = i & 0xff;
		while( REG32_READ( base + I2C_RAW_INTR_STAT ) & (1<<5) == 0x00 ); // wait for rd_req
		while( REG32_READ( base + I2C_RAW_INTR_STAT ) & (1<<6) != 0x00 ); // wait for tx_abrt clear
		REG32_WRITE( base + I2C_DATA_CMD, REG32_READ( base + I2C_DATA_CMD ) & 0xfffffe00 | (0<<8) | tx_buf[i] );
		hprintf( TSENV, "write: %08X \n", tx_buf[i] );
	}
	hprintf( TSENV, "test success! \n" );
	return 0;
}

*/
/*==================================Timer ZEBU TEST=====================================================*/

static zebu_test_mod_t zebu_test_i2c_mods[] = {
	{ "I2C0", I2C0_BASE_ADDR }, { "I2C1", I2C1_BASE_ADDR },
	{ "I2C2", I2C2_BASE_ADDR }, { "I2C3", I2C3_BASE_ADDR },
	{ "I2C4", I2C4_BASE_ADDR }, { "I2C5", I2C5_BASE_ADDR },
};

/* z 3 4 5 6 7 */
// do not use char: c, a, r, d , s, q, ? ,  because these charactor has been used in test common
static zebu_test_entry_t zebu_test_i2c_entrys[] = {
	//{'1', 1, "case 1: Reg Dump" , i2c_test_registers_dump},   											// pass
	//{'2', 1, "case 2: Reg W/R test" , i2c_test_registers_wr}, 											// pass
	{ 'z', 1, "slave case  4 & 5 & 6 & 28 & 29: slave wr 7bit ss test",
	  i2c_slave_write_read_7bit_addr_test }, // pass
	{ '3', 1,
	  "master case 4 & 5 & 28 & 29: Master W/R TEST SS WITH 7-bit addr",
	  i2c_master_write_read_7bit_addr_test }, // pass   I2C0 I2C1 I2C2 I2C3 I2C4 I2C5  pass
	{ '4', 1, "master case 7 & 8:  Master W/R TEST SS WITH 10-bit addr",
	  i2c_master_write_read_10bit_addr_test }, // pass
	{ '5', 1, "master case 30 & 31: Master W/R TEST FS WITH 7-bit",
	  i2c_master_write_read_7bit_addr_test_fs }, // pass
	{ '6', 1, "master case 34 & 35: Master W/R TEST HS WITH 7-bit",
	  i2c_master_write_read_7bit_addr_test_hs }, // pass
	{ '7', 1, "master case 32 & 33: Master W/R TEST FS Plus WITH 7-bit",
	  i2c_master_write_read_7bit_addr_test_fs_plus }, // pass
	//{'8', 0, "[to test  in A1000] case 44 : SS Master SDL/SCL spike suppression", i2c_master_spike_ss_test},
	//{'9', 0, "[to test in A1000] case 45 : FS Master SDL/SCL spike suppression", i2c_master_spike_fs_test},
	//{'0', 0, "[to test in A1000] case 46 : FS+ Master SDL/SCL spike suppression", i2c_master_spike_fs_plus_test},
	//{'-', 0, "[to test in A1000] case 47 : HS Master SDL/SCL spike suppression", i2c_master_spike_hs_test},
	//{'t', 0, "[to test in A1000] case 19: SCL_STUCK_AT_LOW test", i2c_master_scl_stuck_at_low_test},
	//{'u', 0, "[to test in A1000] case 27: SDA_STUCK_AT_LOW test", i2c_master_sda_stuck_at_low_test},
	//{'y', 1, "master case 9: invalid slave addr test", i2c_master_send_invalid_addr_7bit},                                	// pass
};

static zebu_test_ctrl_t zebu_mod_test_ctrl = { "I2C",
					       SIZE_ARRAY(zebu_test_i2c_mods),
					       zebu_test_i2c_mods,
					       SIZE_ARRAY(zebu_test_i2c_entrys),
					       zebu_test_i2c_entrys,
					       0 };

void i2c_pinmux_init(void)
{
	// config pinmux to enable i2c
	// reg 0

	REG32_WRITE(PINMUX_REG_BASE + AON_REG_0,
		    REG32_READ(PINMUX_REG_BASE + AON_REG_0) &
			    (REG_0_MASK | PINMUX_I2C1_SCL | PINMUX_I2C0_SDA |
			     PINMUX_I2C0_SCL));

	//  REG32_WRITE( 0XC0038000 + 0x00,  REG32_READ(0XC0038000 + 0x00) & 0x1fffffff );
	// REG 1

	REG32_WRITE(PINMUX_REG_BASE + AON_REG_1,
		    REG32_READ(PINMUX_REG_BASE + AON_REG_1) &
			    (REG_1_MASK | PINMUX_I2C5_SDA | PINMUX_I2C5_SCL |
			     PINMUX_I2C4_SDA | PINMUX_I2C4_SCL |
			     PINMUX_I2C3_SDA | PINMUX_I2C3_SCL |
			     PINMUX_I2C2_SDA | PINMUX_I2C2_SCL |
			     PINMUX_I2C1_SDA));
}

void i2cMain(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;
	i2c_pinmux_init();
	i2c0_irq_init();
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "auto test err %d!!!!\r\n", ret);
	} else {
		hprintf(TSENV, "enter I2C test!\r\n");
		TestLoop(&zebu_mod_test_ctrl);
	}
}

CMD_DEFINE(i2c, i2cMain, 0, "i2c test", CMD_ATTR_AUTO_TEST);

/********************************IRQ TEST**************************************/
void irq_i2cx(int para)
{
	int status;

	hprintf(TSENV, "\nenter i2c interrupt \n");
	status = REG32_READ(zebu_test_i2c_mods[para].reg_base + I2C_INTR_STAT);
	hprintf(TSENV, "intr status is %08x\n", status);

	if ((status & (0x1 << 4)) == (0x1 << 4)) {
		hprintf(TSENV, "enter wr intr");
		wr_cmd(zebu_test_i2c_mods[para].reg_base);
	}
	REG32_READ(zebu_test_i2c_mods[para].reg_base + I2C_CLR_INTR);
}

void i2c0_irq_init(void)
{
//enable all  interrupt
#ifdef CONFIG_A55
	A55_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C0_IRQ);
	A55_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C1_IRQ);
	A55_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C2_IRQ);
	A55_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C3_IRQ);
	A55_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C4_IRQ);
	A55_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C5_IRQ);
#else
	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C0_IRQ);
	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C1_IRQ);
	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C2_IRQ);
	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C3_IRQ);
	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C4_IRQ);
	SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_I2C5_IRQ);
#endif
}

void i2c0_irq_close(void)
{
	//disable all  interrupt
#ifdef CONFIG_A55
	A55_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C0_IRQ);
	A55_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C1_IRQ);
	A55_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C2_IRQ);
	A55_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C3_IRQ);
	A55_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C4_IRQ);
	A55_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C5_IRQ);
#else
	SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C0_IRQ);
	SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C1_IRQ);
	SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C2_IRQ);
	SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C3_IRQ);
	SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C4_IRQ);
	SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_I2C5_IRQ);
#endif
}

#ifdef CONFIG_A55
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C0_IRQ, irq_i2cx, 0, "irq i2c0 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C1_IRQ, irq_i2cx, 1, "irq i2c0 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C2_IRQ, irq_i2cx, 2, "irq i2c0 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C3_IRQ, irq_i2cx, 3, "irq i2c0 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C4_IRQ, irq_i2cx, 4, "irq i2c0 intr", ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C5_IRQ, irq_i2cx, 5, "irq i2c0 intr", ISR_ATTR_A55);
#else
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C0_IRQ, irq_i2cx, 0, "irq i2c0 intr",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C1_IRQ, irq_i2cx, 1, "irq i2c0 intr",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C2_IRQ, irq_i2cx, 2, "irq i2c0 intr",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C3_IRQ, irq_i2cx, 3, "irq i2c0 intr",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C4_IRQ, irq_i2cx, 4, "irq i2c0 intr",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_I2C5_IRQ, irq_i2cx, 5, "irq i2c0 intr",
	   ISR_ATTR_SAFETY | ISR_ATTR_LEVEL);
#endif
