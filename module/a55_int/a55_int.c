///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           //
//  This file contains proprietary information that is the sole intellectual property        //
//  of Black Sesame Technologies, Inc. and its affiliates.  No portions of this material     //
//  may be reproduced in any form without the written permission of:                         //
//                                                                                           //
//  Black Sesame Technologies, Inc. and its affiliates                                       //
//  2255 Martin Ave. Suite D                                                                 //
//  Santa Clara, CA  95050                                                                   //
//  Copyright \@2018: all right reserved.                                                    //
//                                                                                           //
//  Notice:                                                                                  //
//                                                                                           //
//  You are running an EVALUATION distribution of the neural network tools provided by       //
//  Black Sesame Technologies, Inc. under NDA.                                               //
//  This copy may NOT be used in production or distributed to any third party.               //
//  For distribution or production, further Software License Agreement is required.          //
//                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////

#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <utility_lite.h>
#include <test_common.h>
#include <int_num.h>
#include <module/a55_int.h>

#ifndef CONFIG_A55
#error "This code Only support for A55 Test."
#endif

volatile int test_r5_lock_intr_flag = 0;

void udelay(unsigned long usec);

void isr_a55_crm_soft_int(int irq_num)
{
	int reg = TOP_CRM_REG_BASE_ADDR + TOP_CRM_SOFT_INT_OFF;

	printf_intr("isr_a55_crm_soft_int: irq_num is %d\n", irq_num);
	udelay(10000);
	REG32_WRITE(reg, 0x0);
	printf_intr("REG[TOP_CRM_SOFT_INT] = 0x%08x\r\n", REG32_READ(reg));

	A55_IRQ_DISABLE(FIQ_IRQ_CRM_SOFT_INT);
}

void isr_a55_axi_sram_pty_int(int irq_num)
{
	printf_intr("isr_a55_axi_sram_pty_int: irq_num is %d\n", irq_num);
	A55_IRQ_DISABLE(FIQ_IRQ_AXI_SRAM_PTY_INTR);
}

void isr_a55_axi_sram_ecc_int(int irq_num)
{
	printf_intr("isr_a55_axi_sram_ecc_int: irq_num is %d\n", irq_num);
	A55_IRQ_DISABLE(FIQ_IRQ_AXI_SRAM_ECC_INTR);
}

void isr_a55_r5lock_int(int irq_num)
{
	printf_intr("isr_a55_r5lock_int: irq_num is %d\n", irq_num);
	A55_IRQ_DISABLE(FIQ_IRQ_R5_LOCK_ERR);
}

/* 触发crm_soft_int中断 */
int test_crm_soft_int(zebu_test_mod_t *mod, void *para)
{
	int reg = TOP_CRM_REG_BASE_ADDR + TOP_CRM_SOFT_INT_OFF;
	u32 result = HVTE_MODULE_TEST_PASS;

	A55_IRQ_ENABLE(FIQ_IRQ_CRM_SOFT_INT);

	hprintf(TSENV, "REG[TOP_CRM_SOFT_INT] = 0x%08x\r\n", REG32_READ(reg));
	REG32_WRITE(reg,
		    0x10001); //[31:16]SOFT_INT_REG = 1, [15:0]SOFT_INT_EN = 1
	hprintf(TSENV, "REG[TOP_CRM_SOFT_INT] = 0x%08x\r\n", REG32_READ(reg));

	//A55_IRQ_DISABLE(FIQ_IRQ_CRM_SOFT_INT);

	return result;
}

/* 触发axi_sram_pty_int中断 */
int test_axi_sram_pty_int(zebu_test_mod_t *mod, void *para)
{
	int val = 0;
	u32 result = HVTE_MODULE_TEST_PASS;

	/*需要先使能STRAP_SYS_PARITY_EN，方法是通过外部pin（A_I2S0_CK，拉高）进行配置（类似zebu上操作hw_rstn信号）*/

	A55_IRQ_ENABLE(FIQ_IRQ_AXI_SRAM_PTY_INTR);

	val = REG32_READ(SAFETY_CRM_ADDR + 0x130);
	hprintf(TSENV, "REG[0x70035130] = 0x%08x\r\n", val);
	REG32_WRITE(SAFETY_CRM_ADDR + 0x130,
		    val | (0x3 << 14)); //使能端口数据注错
	hprintf(TSENV, "REG[0x70035130] = 0x%08x\r\n",
		REG32_READ(SAFETY_CRM_ADDR + 0x130));
	REG32_WRITE(SAFETY_CRM_ADDR + 0x124, 0xFFFF); //使能中断上报
	hprintf(TSENV, "REG[0x70035124] = 0x%08x\r\n",
		REG32_READ(SAFETY_CRM_ADDR + 0x124));
	//写sram，注意关cache，否则没有中断上报
	REG32_WRITE(SRAM_BASE_ADDR, 0x0a);
	hprintf(TSENV, "write sram[0x18000000] = 0x0a\r\n");

	hprintf(TSENV, "REG[0x70035118] = 0x%08x\r\n",
		REG32_READ(SAFETY_CRM_ADDR + 0x118));
	hprintf(TSENV, "REG[0x70035130] = 0x%08x\r\n",
		REG32_READ(SAFETY_CRM_ADDR + 0x130));

	A55_IRQ_DISABLE(FIQ_IRQ_AXI_SRAM_PTY_INTR);

	return result;
}

/* 触发axi_sram_ecc_int中断 */
int test_axi_sram_ecc_int(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	/*需要先使能STRAP_SYS_PARITY_EN，方法是通过外部pin（A_I2S0_CK，拉高）进行配置（类似zebu上操作hw_rstn信号）*/
	/*需要先使能STRAP_SYS_ECC_EN，方法是通过外部pin（A_SPI1_SCLK，拉高）进行配置（类似zebu上操作hw_rstn信号）*/

	A55_IRQ_ENABLE(FIQ_IRQ_AXI_SRAM_ECC_INTR);

	/*通过后门在sram(0x18000000)写入0x27(正确的应该是0x24), 可以修改Ecc_safety.raw的第一个字节，load改后的文件，即通过后门写入sram*/
	REG32_WRITE(SAFETY_CRM_ADDR + 0x124, 0xFFFF); //使能中断上报
	hprintf(TSENV, "REG[0x70035124] = 0x%08x\r\n",
		REG32_READ(SAFETY_CRM_ADDR + 0x124));

	/*再在secure_r5的串口上执行dump命令读mem 0x18000000，即可触发中断*/
	//hprintf(TSENV, "read sram[0x18000000] = 0x%08x\r\n", REG32_READ(sram_addr));

	//A55_IRQ_DISABLE(FIQ_IRQ_AXI_SRAM_ECC_INTR);

	return result;
}

/** 
 * 由于r5 lock（A55中断号124）注错寄存器位于safety内部，只能由safety访问，其他CPU访问不了。
 * 所以r5 lock err测试时，需要启动safety然后在safety上输入命令：r5lock，
 * 并观察A55终端是否有打印中断124相关的信息，如果有打印，则判断为测试通过。 
 * 备注一下，该中断A55侧无法清除，所以只能屏蔽（在a55的中断处理函数中屏蔽），
 * 如果需要测试多次，就要在a55的串口下，执行a55_int，再执行用例3，打开屏蔽，
 * 这时再safety上再执行r5lock，a55上会再报一次124中断。
 */
int test_r5_lock_int_enable(zebu_test_mod_t *mod, void *para)
{
	A55_IRQ_ENABLE(FIQ_IRQ_R5_LOCK_ERR);
	return HVTE_MODULE_TEST_PASS;
}

/*==================================A55 INT ZEBU TEST=====================================================*/

zebu_test_mod_t zebu_test_a55int_mods[] = {
	{ "a55int", 0x0 },
};

zebu_test_entry_t zebu_test_a55int_entrys[] = {
	{ '0', 0, "a55int test crm_soft_int", test_crm_soft_int },
	{ '1', 0, "a55int test axi_sram_pty_int", test_axi_sram_pty_int },
	{ '2', 0, "a55int test axi_sram_ecc_int", test_axi_sram_ecc_int },
	{ '3', 0, "a55int test r5_lock_err IRQ enable",
	  test_r5_lock_int_enable },

};

static zebu_test_ctrl_t zebu_mod_test_ctrl = {
	"a55int",
	SIZE_ARRAY(zebu_test_a55int_mods),
	zebu_test_a55int_mods,
	SIZE_ARRAY(zebu_test_a55int_entrys),
	zebu_test_a55int_entrys,
	0
};

void a55int_Main(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "！！auto test err %d!!!!\r\n", ret);
	} else {
		TestLoop(&zebu_mod_test_ctrl);
	}
}

CMD_DEFINE(a55_int, a55int_Main, 0, "a55 int test", CMD_ATTR_AUTO_TEST);
IRQ_DEFINE(FIQ_IRQ_CRM_SOFT_INT, isr_a55_crm_soft_int, FIQ_IRQ_CRM_SOFT_INT,
	   "a55 crm_soft_int", ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_AXI_SRAM_PTY_INTR, isr_a55_axi_sram_pty_int,
	   FIQ_IRQ_AXI_SRAM_PTY_INTR, "a55 axi_sram_pty_int",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_AXI_SRAM_ECC_INTR, isr_a55_axi_sram_ecc_int,
	   FIQ_IRQ_AXI_SRAM_ECC_INTR, "a55 axi_sram_ecc_int",
	   ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_R5_LOCK_ERR, isr_a55_r5lock_int, FIQ_IRQ_R5_LOCK_ERR,
	   "a55 axi_sram_ecc_int", ISR_ATTR_A55 | ISR_ATTR_LEVEL);
