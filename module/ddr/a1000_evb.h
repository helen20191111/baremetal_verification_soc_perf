
#ifndef __A1000_EVB_H__
#define __A1000_EVB_H__

#include <common.h>
#include <a1000/sysreg_base.h>

extern void delay_1k(unsigned int uicnt);

#define DDRC0_NAME		"lpddr4_inst0"
#define DDRC1_NAME		"lpddr4_inst1"

#define A1000BASE_SYSCTRL					A55_CTRL_ADDR
#define A1000BASE_PMMREG					TOP_PMM_REG_BASE_ADDR
#define A1000BASE_TOPCRM					A55_CRM_ADDR

#define REG_SDMMC_SOFTRST_SEL			(0x33002004UL)

#define TOP_CRM_BLOCK_SW_RST0			(A1000BASE_TOPCRM+0x4)
#define A1000BASE_PLL_CPU					(A1000BASE_TOPCRM+0x8)
//#define A1000BASE_PLL_CPU1			(A1000BASE_TOPCRM+0x18)
#define A1000BASE_PLL_DSU					(A1000BASE_TOPCRM+0x28)
#define A1000BASE_PLL_SYSBUS			(A1000BASE_TOPCRM+0x38)
#define A1000BASE_PLL_DISPLAY			(A1000BASE_TOPCRM+0x48)
//#define A1000BASE_PLL_COREBUS		(A1000BASE_TOPCRM+0x58)
#define A1000BASE_PLL_COREIP			(A1000BASE_TOPCRM+0x68)
#define A1000BASE_PLL_HSPLSP			(A1000BASE_TOPCRM+0x78)
#define A1000BASE_PLL_GMAC				(A1000BASE_TOPCRM+0x8C)

/*#define A1000BASE_CLKMUX				(A1000BASE_TOPCRM+0x9C)
#define A1000BASE_CLKMUX_PLL			(A1000BASE_TOPCRM+0xA4)
#define A1000BASE_CLKGATE_EN0			(A1000BASE_TOPCRM+0xA8)
#define A1000BASE_CLKGATE_EN1			(A1000BASE_TOPCRM+0xAC)*/

#define A1000BASE_DDRC						(A1000BASE_TOPCRM+0x1000)
#define A1000BASE_DDR0_PHY				(A1000BASE_TOPCRM+0x3000)
#define A1000BASE_DDR1_PHY				(A1000BASE_TOPCRM+0x3000)


#define A1000BASE_DDRC0						DDRC0_CTRL_PHY_ADDR
#define A1000BASE_DDRC1						DDRC1_CTRL_PHY_ADDR

#define A1000BASE_SAFETYCRM				SAFETY_CRM_ADDR
#define A1000BASE_AONCFG					AON_PMM_REG_BASE_ADDR

#define A1000BASE_PLL_SAFETY			(A1000BASE_SAFETYCRM+0x10)
#define A1000BASE_PLL_SAFETYLSP		(A1000BASE_SAFETYCRM+0x50)

#define A1000BASE_PINMUX					(A1000BASE_AONCFG+0x50)
#define A1000BASE_PINMUX0					(A1000BASE_PINMUX)
#define A1000BASE_PINMUX1					(A1000BASE_PINMUX+0x04)
#define A1000BASE_PINMUX2					(A1000BASE_PINMUX+0x08)
#define A1000BASE_PINMUX3					(A1000BASE_PINMUX+0x0c)
#define A1000BASE_PINMUX4					(A1000BASE_PINMUX+0x10)
#define A1000BASE_PINMUX5					(A1000BASE_PINMUX+0x14)
#define A1000BASE_PINMUX6					(A1000BASE_PINMUX+0x18)
#define A1000BASE_PINMUX7					(A1000BASE_PINMUX+0x1c)
#define A1000BASE_PINMUX8					(A1000BASE_PINMUX+0x20)
#define A1000BASE_PINMUX9					(A1000BASE_PINMUX+0x24)
#define A1000BASE_PINMUX10				(A1000BASE_PINMUX+0x28)
#define A1000BASE_PINMUX11				(A1000BASE_PINMUX+0x30)
#define A1000BASE_PINMUX12				(A1000BASE_PINMUX+0x34)
#define A1000BASE_PINMUX13				(A1000BASE_PINMUX+0x38)
#define A1000BASE_PINMUX14				(A1000BASE_PINMUX+0x3c)
#define A1000BASE_PINMUX15				(A1000BASE_PINMUX+0x40)
#define A1000BASE_PINMUX16				(A1000BASE_PINMUX+0x44)
#define A1000BASE_PINMUX17				(A1000BASE_PINMUX+0x48)
#define A1000BASE_PINMUX18				(A1000BASE_PINMUX+0x50)
#define A1000BASE_PINMUX19				(A1000BASE_PINMUX+0x54)
#define A1000BASE_PINMUX20				(A1000BASE_PINMUX+0x58)
#define A1000BASE_PINMUX21				(A1000BASE_PINMUX+0x5c)

//------------------------------------------------------------------------------
//maybe to be modified
#define FREQ_CRYSTAL									(25*1000000UL)

#define FOUT_PLL_SAFETY_MAIN					(200*1000000UL)
#define FOUT_PLL_SAFETY_LSP0					(200*1000000UL)
#define FOUT_PLL_SAFETY_LSP1					(100*1000000UL)

#define FOUT_PLL_DSU									(1200*1000000UL)
#define FOUT_PLL_CPU									(1400*1000000UL)
#define FOUT_PLL_SYSBUS								(2400*1000000UL)
#define FOUT_PLL_HSPISP								(2000*1000000UL)
#define FOUT_PLL_GMAC									(1000*1000000UL)
#define FOUT_PLL_DISPLAY							(1650*1000000UL)
#define FOUT_PLL_COREIP								(800*1000000UL)

//------------------------------------------------------------------------------
/* mux/divider/gate register define */
#define SECSAFE_SYSCTRL_R_SEC_SAFE_CLK_SEL				(A1000BASE_TOPCRM + 0x00)
#define SECSAFE_SYSCTRL_R_SEC_SAFE_CLK_EN					(A1000BASE_TOPCRM + 0x04)

#define TOPCRM_REG_R_CLKMUX_SEL0									(A1000BASE_TOPCRM + 0x9C)
#define TOPCRM_REG_R_CLKMUX_SEL1									(A1000BASE_TOPCRM + 0xA0)
#define TOPCRM_REG_R_PLL_CLKMUX_SEL								(A1000BASE_TOPCRM + 0xA4)
#define TOPCRM_REG_R_CLKGATE_EN0									(A1000BASE_TOPCRM + 0xA8)
#define TOPCRM_REG_R_CLKGATE_EN1									(A1000BASE_TOPCRM + 0xAC)

//------------------------------------------------
//0x33002000+0x9c [27:26], 																TOPCRM_REG_R_CLKMUX_SEL0
typedef enum {
	SW_CPUCORE_25M=0,
	SW_CPUCORE_1400M,
	SW_CPUCORE_1000M,
	SW_CPUCORE_700M,
	SW_CPUCORE_MODE,
}CLK1400_CPUCORE;
//<-0x33002000+0xa4 [27:27]:{0:25MHz, 1:PLL_CPU}, 				TOPCRM_REG_R_PLL_CLKMUX_SEL
//->0x33002000+0xa8 [29:29]:{0:CLK1400_CPUCORE, 1:1TE}, 	TOPCRM_REG_R_CLKGATE_EN0

//0x33002000+0x9c [28:28], 																TOPCRM_REG_R_CLKMUX_SEL0
typedef enum {
	SW_DSU_CPUCORE=0,
	SW_DSU_1200M,
	SW_DSU_MODE,
}CLK1200_DSU;
//<-0x33002000+0xa4 [28:28]:{0:25MHz, 1:PLL_DSU}, 				TOPCRM_REG_R_PLL_CLKMUX_SEL
//->0x33002000+0xa8 [30:30]:{0:CLK1400_CPUCORE, 1:1TE}, 	TOPCRM_REG_R_CLKGATE_EN0


//PLL_HSPLSP -> CLK2000_HSPLSP
//->0x33002000+0xa8 [22:22]:{0:25MHz, 1:CLK2000_HSPLSP}, 	TOPCRM_REG_R_CLKGATE_EN0

//0x33002000+0x9c [25:24], 																TOPCRM_REG_R_CLKMUX_SEL0
typedef enum {
	SW_SYSNOC_25M=0,
	SW_SYSNOC_400M,
	SW_SYSNOC_200M,
	SW_SYSNOC_100M,
	SW_SYSNOC_MODE,
}CLK400_SYSNOC;
//<-0x33002000+0xa4 [26:26]:{0:25MHz, 1:PLL_SYSBUS},			TOPCRM_REG_R_PLL_CLKMUX_SEL
//-> for debug NOC


//------------------------------------------------
//PLL_SYSBUS -> CLK2400_SYSBUS -> 0x33002000+0xa4 [25:25]:{0:25MHz, 1:CLK2400_SYSBUS} -> {600MHz},																TOPCRM_REG_R_PLL_CLKMUX_SEL
//PLL_SYSBUS -> CLK800_SYSBUS  -> 0x33002000+0xa4 [26:26]:{0:25MHz, 1:CLK800_SYSBUS}  -> {800MHz, 400MHz, 200MHz, 100MHz, 50MHz},	TOPCRM_REG_R_PLL_CLKMUX_SEL

//0x33002000+0xa0 [25:24], 																TOPCRM_REG_R_CLKMUX_SEL1
typedef enum {
	SW_CORENOC_25M=0,
	SW_CORENOC_800M,
	SW_CORENOC_600M,
	SW_CORENOC_400M,
	SW_CORENOC_MODE,
}CLK800_CORENOC;
//<-0x33002000+0xa4 [26:26]:{0:25MHz, 1:PLL_SYSBUS},			TOPCRM_REG_R_PLL_CLKMUX_SEL
//-> fout

//0x33002000+0x9c [23:22], 																TOPCRM_REG_R_CLKMUX_SEL0
typedef enum {
	SW_CPUNOC_25M=0,
	SW_CPUNOC_800M,
	SW_CPUNOC_400M,
	SW_CPUNOC_200M,
	SW_CPUNOC_MODE,
}CLK800_CPUNOC;
// ->fout

/* //0x33002000+0x9c [25:24], 														TOPCRM_REG_R_CLKMUX_SEL0
typedef enum {
	SW_SYSNOC_25M=0,
	SW_SYSNOC_400M,
	SW_SYSNOC_200M,
	SW_SYSNOC_100M,
	SW_SYSNOC_MODE,
}CLK400_SYSNOC;*/

//0x33002000+0x9c [30:29], 																TOPCRM_REG_R_CLKMUX_SEL0
typedef enum {
	SW_GDMA_AXI_25M=0,
	SW_GDMA_AXI_800M,
	SW_GDMA_AXI_400M,
	SW_GDMA_AXI_200M,
	SW_GDMA_AXI_MODE,
}CLK800_GDMA_AXI;
//->0x33002000+0xac [11:11]:{0:CLK800_GDMA_AXI, 1:1TE},		TOPCRM_REG_R_CLKGATE_EN1

//0x33002000+0x9c [9:8], 																	TOPCRM_REG_R_CLKMUX_SEL0
typedef enum {
	SW_SYSBUS_APB_25M=0,
	SW_SYSBUS_APB_200M,
	SW_SYSBUS_APB_100M,
	SW_SYSBUS_APB_50M,
	SW_SYSBUS_APB_MODE,
}CLK200_SYSBUS_APB;
//->0x33002000+0xac [5:5]:{0:CLK200_SYSBUS_APB, 1:1TE},		TOPCRM_REG_R_CLKGATE_EN1

//0x33002000+0xa0 [27:26], 																TOPCRM_REG_R_CLKMUX_SEL1
typedef enum {
	SW_SYSBUS1_APB_25M=0,
	SW_SYSBUS1_APB_100M,
	SW_SYSBUS1_APB_50M,
	SW_SYSBUS1_APB_DEF,
	SW_SYSBUS1_APB_MODE,
}CLK100_SYSBUS_APB;
// ->fout

//0x33002000+0x9c [15:14], 																TOPCRM_REG_R_CLKMUX_SEL0
typedef enum {
	SW_SYSBUS_AXI_DEF=0,
	SW_SYSBUS_AXI_800M,
	SW_SYSBUS_AXI_400M,
	SW_SYSBUS_AXI_200M,
	SW_SYSBUS_AXI_MODE,
}CLK800_SYSBUS_AXI;

//ddr switchs, 																						TOPCRM_REG_R_CLKGATE_EN0
//->0x33002000+0xa8 [28:28]:{0:enable, 1:disable}, DDR0_S0.
//->0x33002000+0xa8 [27:27]:{0:enable, 1:disable}, DDR0_S1.
//->0x33002000+0xa8 [26:26]:{0:enable, 1:disable}, DDR0_S2.
//->0x33002000+0xa8 [25:25]:{0:enable, 1:disable}, DDR0_S3.
//->0x33002000+0xa8 [24:24]:{0:enable, 1:disable}, DDR1_S0.
//->0x33002000+0xa8 [23:23]:{0:enable, 1:disable}, DDR1_S1.
//->0x33002000+0xa8 [22:22]:{0:enable, 1:disable}, DDR1_S2.
//->0x33002000+0xa8 [21:21]:{0:enable, 1:disable}, DDR1_S3.

//->0x33002000+0xa8 [20:20]:{0:enable, 1:disable}, DDR0_PLK.
//->0x33002000+0xa8 [19:19]:{0:enable, 1:disable}, DDR1_PLK.

//------------------------------------------------
//0xc0035000[9:0]
typedef enum {
	SW_SAFECLK_DEF=0,
	SW_SAFECLK_QSPIREF,
	SW_SAFECLK_800M,
	SW_SAFECLK_200M,
	SW_SAFECLK_MODE,
}SEC_SAFECLK_SEL;


//------------------------------------------------



//------------------------------------------------------------------------------
/* pll config register offset */
#define PLL_REG_CONFIG0     (0x0)
#define PLL_REG_CONFIG1     (0x4)
#define PLL_REG_CONFIG2     (0x8)
#define PLL_REG_STATUS      (0xC)

/* pll config0 item */
#define PLL_PLLEN           (5)
#define PLL_LOCK            (0)
#define PLL_DSMEN           (13) // 0 -> DSM is powered down (integer mode) 1 -> DSM is active (fractional mode)
#define PLL_FBDIV           (16)
#define PLL_FBDIV_MASK      (0xFFF)

/* pll config1 item */
#define PLL_FRAC            (0)
#define PLL_FRAC_MASK       (0xFFFFFF)
#define PLL_FRAC_SHIFT      (24)

/* pll(gmac) special item */
#define PLL_GMAC_LOCK       (1)

/* pll(safety crm) config item */
#define PLL_SEC_PLLEN       (0)
#define PLL_SEC_LOCK        (0)
#define PLL_SEC_DSMEN       (2)
#define PLL_SEC_FBDIV       (4)
#define PLL_SEC_FBDIV_MASK  (0xFFF)
#define PLL_SEC_FRAC        (0)
#define PLL_SEC_FRAC_MASK   (0xFFFFFF)
#define PLL_SEC_FRAC_SHIFT  (24)

#define CLK_EN							0
#define CLK_TE							1

//------------------------------------------------------------------------------
extern int set_pllrate(unsigned long refspeed, unsigned long outspeed, void *base, int dochk);
extern void switch_clk(void *base, int shift, int maskbits, int val);
extern void switch_en(void *base, int shift, int val);

extern void iomux_set_byidx(int idx, int pinstart, int pincnt, int pval);


extern int get_bootmode(void);
extern void init_sys_pll(void);
extern void init_qspi(void);
extern void ddr_rw_test(void);

//extern lpddr4_dwc_ddrphy_phyinit_userCustom_overrideUserInput(void);
//extern lpddr4_dwc_ddrphy_phyinit_userCustom_A_bringupPower(void);
extern void lpddr4_dwc_ddrphy_phyinit_userCustom_B_startClockResetPhy(char *name, void *bst_lpddr4_base_addr);
extern void lpddr4_dwc_ddrphy_phyinit_userCustom_G_waitFwDone(void *ddr_base);
extern void lpddr4_dwc_ddrphy_phyinit_userCustom_H_readMsgBlock(void *ddr_phy_base_addr);
extern void lpddr4_dwc_ddrphy_phyinit_userCustom_customPostTrain(void);

extern void lpddr4_ctrl_post_init(void *ddr_base);
extern void lpddr4_clk_rst_init(char * name, void *ddr_base);
extern void lpddr4_ddr_pll_init(void *ddr_base);
extern unsigned int lpddr4_get_mail(void *ddr_base, int mode_32bits);

extern void lpddr4_apb_config(char *name, void *bst_lpddr4_base_addr);
extern int lpddr4_init_sequence(void *ddr_base);

#endif
