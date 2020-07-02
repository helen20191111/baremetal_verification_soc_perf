// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>
#include <command.h>
#include <a1000/sysreg_base.h>
#include <RegTest.h>
#include "get_ipid.h"

static const struct module_ip_struct module_base[] = {
	MODULE_IP(I2C0, "i2c0", I2C0_BASE_ADDR, 0xf8),
	MODULE_IP(I2C1, "i2c1", I2C1_BASE_ADDR, 0xf8),
	MODULE_IP(I2C2, "i2c2", I2C2_BASE_ADDR, 0xf8),
	MODULE_IP(I2C3, "i2c3", I2C3_BASE_ADDR, 0xf8),
	MODULE_IP(I2C4, "i2c4", I2C4_BASE_ADDR, 0xf8),
	MODULE_IP(I2C5, "i2c5", I2C5_BASE_ADDR, 0xf8),
	MODULE_IP(UART0, "uart0", UART0_BASE_ADDR, 0xf8), //not exsit reg
	MODULE_IP(UART1, "uart1", UART1_BASE_ADDR, 0xf8),
	MODULE_IP(UART2, "uart2", UART2_BASE_ADDR, 0xf8),
	MODULE_IP(UART3, "uart3", UART3_BASE_ADDR, 0xf8),
	MODULE_IP(SPI0, "spi0", SPI0_BASE_ADDR, 0x5c),
	MODULE_IP(SPI1, "spi1", SPI1_BASE_ADDR, 0x5c),
	MODULE_IP(QSPI0, "qspi0", QSPI0_BASE_ADDR, 0x0), //esr_elx:0 0x5c
	MODULE_IP(QSPI1, "qspi1", QSPI1_BASE_ADDR, 0x0),
	MODULE_IP(I2S0, "i2s0", I2S0_BASE_ADDR, 0x1f8),
	MODULE_IP(I2S1, "i2s1", I2S1_BASE_ADDR, 0x1f8),
	MODULE_IP(GPIO0, "gpio0", GPIO0_BASE_ADDR, 0x6c),
	MODULE_IP(GPIO1, "gpio1", GPIO1_BASE_ADDR, 0x6c),
	MODULE_IP(CANFD0, "canfd0", CANFD0_BASE_ADDR, 0x0),
	MODULE_IP(CANFD1, "canfd1", CANFD1_BASE_ADDR, 0x0),
	MODULE_IP(TIMER0, "timer0", TIMER0_PWM0_BASE_ADDR, 0xac),
	MODULE_IP(TIMER1, "timer1", TIMER1_PWM1_BASE_ADDR, 0xac),
	MODULE_IP(TIMER2, "timer2", TIMER2_PWM2_BASE_ADDR, 0xac),
	MODULE_IP(TIMER3, "timer3", TIMER3_PWM3_BASE_ADDR, 0xac),
	MODULE_IP(WDT0, "wdt0", WDT0_BASE_ADDR, 0xf8),
	MODULE_IP(WDT1, "wdt1", WDT1_BASE_ADDR, 0xf8),
	MODULE_IP(WDT2, "wdt2", WDT2_BASE_ADDR, 0xf8),
	MODULE_IP(WDT3, "wdt3", WDT3_BASE_ADDR, 0xf8),
	MODULE_IP(USB2, "usb2", USB2_BASE_ADDR, 0xc128),
	MODULE_IP(PCIE_4X, "pcie_4x", PCIE_4X_BASE_ADDR, 0x0),
	MODULE_IP(PCIE_2X, "pcie_2x", PCIE_2X_BASE_ADDR, 0x0),
	MODULE_IP(PCIE_PHY, "pcie_phy", PCIE_PHY_BASE_ADDR, 0x0),
	MODULE_IP(SD_EMMC0, "sd_emmc0", SD_EMMC0_BASE_ADDR, 0x0),
	MODULE_IP(SD_EMMC1, "sd_emmc1", SD_EMMC1_BASE_ADDR, 0x0),
	MODULE_IP(GIC, "gic", GIC_BASE_ADDR, 0x0),
	MODULE_IP(DDRC0, "ddrc0", DDRC0_CTRL_PHY_ADDR, 0x0),
	MODULE_IP(DDRC1, "ddrc1", DDRC1_CTRL_PHY_ADDR, 0x0),
	MODULE_IP(GPU, "gpu", GPU_BASE_ADDR, 0x0),
	MODULE_IP(A55_WDT0, "a55_wdt0", A55_WDT0_BASE_ADDR, 0xf8),
	MODULE_IP(A55_WDT1, "a55_wdt1", A55_WDT1_BASE_ADDR, 0xf8),
	MODULE_IP(A55_WDT2, "a55_wdt2", A55_WDT2_BASE_ADDR, 0xf8),
	MODULE_IP(A55_WDT3, "a55_wdt3", A55_WDT3_BASE_ADDR, 0xf8),
	MODULE_IP(A55_WDT4, "a55_wdt4", A55_WDT4_BASE_ADDR, 0xf8),
	MODULE_IP(A55_WDT5, "a55_wdt5", A55_WDT5_BASE_ADDR, 0xf8),
	MODULE_IP(A55_WDT6, "a55_wdt6", A55_WDT6_BASE_ADDR, 0xf8),
	MODULE_IP(A55_WDT7, "a55_wdt7", A55_WDT7_BASE_ADDR, 0xf8),
	MODULE_IP(A55_TIMER, "a55_timer", A55_TIMER_BASE_ADDR, 0xac),
	MODULE_IP(USB3, "usb3", USB3_BASE_ADDR, 0xc128),
	MODULE_IP(GMAC0, "gmac0", GMAC0_BASE_ADDR, 0x110),
	MODULE_IP(GMAC1, "gmac1", GMAC1_BASE_ADDR, 0x110),
	MODULE_IP(GDMA, "gdma", GDMA_BASE_ADDR, 0x8),
	MODULE_IP(SDMA, "sdma", SDMA_BASE_ADDR, 0x0), //a55 not exist 0x8
};

static void crm_reset(void)
{
	printf("reset top crm reg...\n");
	REG32_WRITE(A55_CRM_ADDR + 0x4, 0xffffffff);
	REG32_WRITE(A55_CRM_ADDR + 0xe8, 0xffffffff);
}

static void get_id(const struct module_ip_struct *module_base)
{
	if ((!module_base) || !module_base->offset)
		return;
	printf("%s is component version %d\n", module_base->name, REG32_READ(module_base->base + module_base->offset));
}

static int do_get_ipid(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int i;
    printf("Get the id of all module IP, if possible: \n");
	crm_reset();
	for (i = 0; i < IP_MAX; i++) {
		get_id(&module_base[i]);
	}
    return 0;
}

BST_CMD(get_ipid, CONFIG_SYS_MAXARGS, 1, do_get_ipid,
	   "get module ip version/type",
	   "\n\r"
	   "	- get module ip all, maybe crash!!!\n\r"
	   "get_ipid command ...\n\r"
	   "	- This file may be deleted later");
