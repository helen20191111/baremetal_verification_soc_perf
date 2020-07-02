/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <a1000/sysreg_base.h>
#include <common.h>

static void clk_wait_pll_lock(void)
{
	int value;

	value = readl(A55_CRM_ADDR + 0x08);
	value |= BIT(5); /* PLL_CPU_REG0 PLLEN */
	writel(value, A55_CRM_ADDR + 0x08);

	value = readl(A55_CRM_ADDR + 0x28);
	value |= BIT(5); /* PLL_DSU_REG0 PLLEN */
	writel(value, A55_CRM_ADDR + 0x28);
	
	value = readl(A55_CRM_ADDR + 0x38);
	value |= BIT(5); /* PLL_SYSBUS_REG0 PLLEN */
	writel(value, A55_CRM_ADDR + 0x38);

	value = readl(A55_CRM_ADDR + 0x48);
	value |= BIT(5); /* PLL_DISPLAY_REG0 PLL EN */
	writel(value, A55_CRM_ADDR + 0x48);

	value = readl(A55_CRM_ADDR + 0x68);
	value |= BIT(5); /* PLL_COREIP_REG0 PLL EN */
	writel(value, A55_CRM_ADDR + 0x68);

	value = readl(A55_CRM_ADDR + 0x78);
	value |= BIT(5); /* PLL_HSP_LSP_REG0 PLL EN */
	writel(value, A55_CRM_ADDR + 0x78);

	value = readl(A55_CRM_ADDR + 0x8c);
	value |= BIT(5); /* PLL_GMAC_REG0 PLL EN */
	writel(value, A55_CRM_ADDR + 0x8c);

    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(A55_CRM_ADDR + 0x10) & 0x1)); /* PLL_CPU_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(A55_CRM_ADDR + 0x30) & 0x1)); /* PLL_DSU_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(A55_CRM_ADDR + 0x40) & 0x1)); /* PLL_SYSBUS_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(A55_CRM_ADDR + 0x50) & 0x1)); /* PLL_DISPLAY_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(A55_CRM_ADDR + 0x70) & 0x1)); /* PLL_COREIP_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(A55_CRM_ADDR + 0x80) & 0x1)); /* PLL_HSP_LSP_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(SAFETY_CRM_ADDR + 0x1c) & 0x1)); /* PLL_MAIN_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(SAFETY_CRM_ADDR + 0x5c) & 0x1)); /* PLL_LSP_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
    while (!(readl(A55_CRM_ADDR + 0x94) & (0x1<<1))); /* PLL_GMAC_LOCK */
    pr_debug("---%s %d\n", __func__, __LINE__);
}

static void clk_ddr_config(void)
{
    int value;

    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa4); /* CLK_800_SYSBUS_SEL */
    pr_debug("---%s %d\n", __func__, __LINE__);
    value |= BIT(26);
    writel(value, A55_CRM_ADDR + 0xa4);
#ifdef CONFIG_CLK_SETP_2
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0x9c); /* CLK_800_SYSBUS_AXI_SEL */
    value &= ~GENMASK(15, 14);
    value |= BIT(14); /* 200m -> 800m */
    writel(value, A55_CRM_ADDR + 0x9c);
    pr_debug("---%s %d\n", __func__, __LINE__);

#else
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0x9c); /* CLK_800_SYSBUS_AXI_SEL */
    value |= GENMASK(15, 14);
    writel(value, A55_CRM_ADDR + 0x9c);
    pr_debug("---%s %d\n", __func__, __LINE__);

#endif
    value = readl(A55_CRM_ADDR + 0xa8); /* DDR_ACLK_GATE  DDR_PCLK_GATE */
    value |= GENMASK(28, 19);
    writel(value, A55_CRM_ADDR + 0xa8);
    pr_debug("---%s %d\n", __func__, __LINE__);

    value = readl(A55_CRM_ADDR + 0xa0); /* CLK_100_SYSBUS_APB_SEL */
    value &= ~GENMASK(27, 26);
    value |= BIT(26);
    writel(value, A55_CRM_ADDR + 0xa0);
    pr_debug("---%s %d\n", __func__, __LINE__);
}

static void clk_i2c_config(void)
{
    int value;

    value = readl(SAFETY_CRM_ADDR + 0x0);
    value &= ~GENMASK(9, 0);
    value |= BIT(2);  /* clk_main_200m */
	value |= BIT(3);  /* clk_bus_100m */
    writel(value, SAFETY_CRM_ADDR + 0x0);

    value = readl(SAFETY_CRM_ADDR + 0x4);  /* enable lsp0/lsp1 wclk pclk*/
    value |= BIT(13); /* enable lsp0_pclk */
    value |= BIT(14); /* enable lsp0_wclk */
    value |= BIT(16); /* enable lsp1_pclk */
    value |= BIT(17); /* enable lsp1_wclk */
    writel(value, SAFETY_CRM_ADDR + 0x4);

    value = readl(LSP0_CRM_ADDR + 0x4); /* lsp0 select 200M */
    value &= ~BIT(14);
    writel(value, LSP0_CRM_ADDR + 0x4);

    value = readl(LSP1_CRM_ADDR + 0x4); /* lsp1 select 200M */
    value &= ~BIT(22);
    writel(value, LSP1_CRM_ADDR + 0x4);

    value = readl(LSP0_CRM_ADDR + 0x4); /* i2c0-i2c2 enable */
    value |= GENMASK(2, 0);
    writel(value, LSP0_CRM_ADDR + 0x4);

    value = readl(LSP1_CRM_ADDR + 0x4); /* i2c3-i2c5 enable */
    value |= GENMASK(1, 0);
    value |= BIT(26);
    writel(value, LSP1_CRM_ADDR + 0x4);
    
	value = readl(LSP0_CRM_ADDR + 0x4); /* gpio0 enable */
    value |= BIT(7);
    writel(value, LSP0_CRM_ADDR + 0x4);
	
	value = readl(LSP1_CRM_ADDR + 0x4); /* gpio1 enable */
    value |= BIT(11);
    writel(value, LSP1_CRM_ADDR + 0x4);
}

#ifdef CONFIG_CLK_SETP_1
static void clk_noc_config(void)
{
    int value;

    value = readl(A55_CRM_ADDR + 0xa4);
    value |= BIT(26); /* clk_sysnoc_work 25 -> 800 */
    writel(value, A55_CRM_ADDR + 0xa4);

    value = readl(A55_CRM_ADDR + 0xa4);
    value |= BIT(24); /* clk_display_work 25 -> 1650 */
    writel(value, A55_CRM_ADDR + 0xa4);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa0);
    value &= ~GENMASK(25, 24);
    value |= BIT(24); /* clk_corenoc_work_sel 800m */
    writel(value, A55_CRM_ADDR + 0xa0);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa0);
    value &= ~GENMASK(21, 20);
    value |= BIT(20); /* clk_dspcore_work_sel 800m */
    writel(value, A55_CRM_ADDR + 0xa0);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa0);
    value &= ~GENMASK(19, 18);
    value |= BIT(18); /* clk_net_work_sel 800m */
    writel(value, A55_CRM_ADDR + 0xa0);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa0);
    value &= ~GENMASK(17, 16);
    value |= BIT(16); /* clk_cv_core_work_sel 800m */
    writel(value, A55_CRM_ADDR + 0xa0);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa0);
    value |= GENMASK(15, 14); /*  sdemmc  200m */
    writel(value, A55_CRM_ADDR + 0xa0);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa0);
    value &= ~GENMASK(13, 12);
    value |= BIT(12); /* clk_sysbus_work_sel 200m */
    writel(value, A55_CRM_ADDR + 0xa0);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(23, 22);
    value |= BIT(22); /* clk_cpunoc_work_sel 800m */
    writel(value, A55_CRM_ADDR + 0x9c);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa4);
    value |= BIT(25); /* clk_2400_sysbus_sel */
    writel(value, A55_CRM_ADDR + 0xa4);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0xa4);
    value |= BIT(23); /* clk_coreip_sel 800m */
    writel(value, A55_CRM_ADDR + 0xa4);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(25, 24);
    value |= BIT(24); /* clk_400_sysnoc_work_sel */
    writel(value, A55_CRM_ADDR + 0x9c);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(11, 10);
    value |= BIT(10); /* clk_200_sysbus_ahb_sel 200m */
    writel(value, A55_CRM_ADDR + 0x9c);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(9, 8);
    value |= BIT(8); /* clk_200_sysbus_apb_sel */
    writel(value, A55_CRM_ADDR + 0x9c);
    pr_debug("---%s %d\n", __func__, __LINE__);
    /* safety*/
    value = readl(SAFETY_CRM_ADDR + 0x0);
    value |= GENMASK(3, 0);
    writel(value, SAFETY_CRM_ADDR + 0x0);
    pr_debug("---%s %d\n", __func__, __LINE__);
    /* qspi */
   // value = readl(SAFETY_CRM_ADDR + 0x0);
   // value |= BIT(0); /*  */
   // value |= BIT(8);
   // writel(value, SAFETY_CRM_ADDR + 0x0);
    pr_debug("---%s %d\n", __func__, __LINE__);
    value = readl(SAFETY_CRM_ADDR + 0x4);
    value |= BIT(9); /*  */
    value |= BIT(11);
    writel(value, SAFETY_CRM_ADDR + 0x4);
    pr_debug("---%s %d\n", __func__, __LINE__);
}
#else
static inline void clk_noc_config(void) {}
#endif

#ifdef CONFIG_CLK_SETP_3
static void clk_a55cpu_config(void)
{
    int value;

    pr_debug("---%s %d\n", __func__, __LINE__);
    /* pll_dsu 1300m -> 1200m */
    value = readl(A55_CRM_ADDR + 0x28);
    value &= ~BIT(5); /* disable pll */
    writel(value, A55_CRM_ADDR + 0x28);
    pr_debug("---%s %d\n", __func__, __LINE__);

    value = readl(A55_CRM_ADDR + 0x28);
    value &= ~GENMASK(27, 16);
    value |= 0x30<<16; /* PLL_DSU_FBDIV */
    value |= BIT(5); /* enable pll */
    writel(value, A55_CRM_ADDR + 0x28);
    pr_debug("---%s %d\n", __func__, __LINE__);

    while(!(readl(A55_CRM_ADDR + 0x30) & 0x1));
    pr_debug("---%s %d\n", __func__, __LINE__);

    value = readl(A55_CRM_ADDR + 0xa4);
    value |= BIT(28); /* pll_cpu_1400 clk_dsu_1300 */
    writel(value, A55_CRM_ADDR + 0xa4);

    value = readl(A55_CRM_ADDR + 0x9c);
    value |= BIT(28); /* dsu select 1300 */
    writel(value, A55_CRM_ADDR + 0x9c);

    value = readl(A55_CRM_ADDR + 0xa8);
    value |= BIT(30); /* enable cpu_dsu */
    writel(value, A55_CRM_ADDR + 0xa8);

    value = readl(A55_CRM_ADDR + 0xa4);
    value |= BIT(27); /* clk_cpu_1400_sel 1400m */
    writel(value, A55_CRM_ADDR + 0xa4);

    value = readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(27, 26);
    value |= BIT(26); /* clk_1400_cpucore_sel 1400MHz */
    writel(value, A55_CRM_ADDR + 0x9c);

    value = readl(A55_CRM_ADDR + 0xa8);
    value |= BIT(29); /* enable lb_cpu_clk */
    writel(value, A55_CRM_ADDR + 0xa8);
}
#else
static inline void clk_a55cpu_config(void) {}
#endif

#ifdef CONFIG_CLK_SETP_4
static void clk_per_config(void)
{
    int value;

    /* gmac config */
    value = readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(13, 12);
    value |= BIT(12); /* clk_400_sysbus_axi_sel 400m */
    writel(value, A55_CRM_ADDR + 0x9c);

    value =readl(A55_CRM_ADDR + 0xa4);
    value |= BIT(22);
    writel(value, A55_CRM_ADDR + 0xa4);

    value =readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(21, 20);
    writel(value, A55_CRM_ADDR + 0x9c);

    value = readl(A55_CRM_ADDR + 0x8c);
    value &= ~BIT(21); /* 25m*0x28 -> 1000m pll_gmac */
    writel(value, A55_CRM_ADDR + 0x8c);

    value = readl(A55_CRM_ADDR + 0xa4);
    value &= ~GENMASK(20, 19); /* pll_gmac_ref_clk_sel 0 25m */
    writel(value, A55_CRM_ADDR + 0xa4);

    value = readl(A55_CRM_ADDR + 0xa8);
    value |= GENMASK(18, 11); /* enable gmac0/1 pclk wclk */
    writel(value, A55_CRM_ADDR + 0xa8);

    /* usb3 axi sel */
    value = readl(A55_CRM_ADDR + 0xac);
    value |= BIT(29);
    writel(value, A55_CRM_ADDR + 0xac);

    value = readl(A55_CRM_ADDR + 0xa8);
    value |= GENMASK(10, 9);
    writel(value, A55_CRM_ADDR + 0xa8);

    /* gdma */
    value = readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(30, 29);
    value |= BIT(29); /* clk_800_gdma_axi_sel 800m */
    writel(value, A55_CRM_ADDR + 0x9c);

    value = readl(A55_CRM_ADDR + 0x9c);
    value &= ~GENMASK(17, 16);
    value |= BIT(16); /* clk_400_gdma_core_sel 400m */
    writel(value, A55_CRM_ADDR + 0x9c);

    value = readl(A55_CRM_ADDR + 0xac);
    value |= BIT(3);
    value |= GENMASK(11, 10); /* enable gdma clk */
    writel(value, A55_CRM_ADDR + 0xac);
}
#else
static inline void clk_per_config(void) {}
#endif

/*
 * Clock initialization USES the default Settings and 
 * is divided into four steps. Other modules do not 
 * need to configure their own clocks for the time being. 
 * This initialization is only used for bring up.
 */
void clk_init(void)
{
    pr_debug("---%s %d\n", __func__, __LINE__);
    clk_wait_pll_lock();
    pr_debug("---%s %d\n", __func__, __LINE__);
    clk_a55cpu_config();
    pr_debug("---%s %d\n", __func__, __LINE__);
    clk_noc_config();
    pr_debug("---%s %d\n", __func__, __LINE__);
    clk_ddr_config();
    pr_debug("---%s %d\n", __func__, __LINE__);
    clk_i2c_config();
    pr_debug("---%s %d\n", __func__, __LINE__);
    clk_per_config();
    pr_debug("---0x9c: %08x\n", readl(A55_CRM_ADDR + 0x9c));
    pr_debug("---0xa0: %08x\n", readl(A55_CRM_ADDR + 0xa0));
    pr_debug("---0xa4: %08x\n", readl(A55_CRM_ADDR + 0xa4));
}
