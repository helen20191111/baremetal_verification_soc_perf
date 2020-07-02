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
#include <int_num.h>
#include <common.h>
#include <module/gdma.h>
#include "ddr.h"

extern unsigned long my_randul(void);
extern void my_rand_seed(unsigned long seed);

#ifdef CONFIG_A55
#define DDR_RANDOM_INVERSE_ADDR_MASK 0xFFFFFFFFULL
#define DDR_RANDOM_ADDR_MIN 0X81000000ULL
#define DDR_RANDOM_ADDR_MAX 0X280000000ULL
#else
#define DDR_RANDOM_INVERSE_ADDR_MASK 0x3FFFFFFFULL
#define DDR_RANDOM_ADDR_MIN 0X80000000ULL
#define DDR_RANDOM_ADDR_MAX 0XC0000000ULL
#endif

//#define HS_HARDWARE_VER_60 0//config it in makefile later

#define DDR_WORK_CLOCK (4500000)
#define CLOCK_PER_US (DDR_WORK_CLOCK / (1000 * 1000))
#define CLOCK_PER_MS (DDR_WORK_CLOCK / 1000)

#define SIZE_1M (1 << 20)
#define SIZE_512k (1 << 19)
#define SIZE_256k (1 << 18)
#define SIZE_128k (1 << 17)
#define SIZE_64k (1 << 16)
#define SIZE_32k (1 << 15)
#define SIZE_16k (1 << 14)
#define SIZE_8k (1 << 13)
#define SIZE_4k (1 << 12)
#define SIZE_2k (1 << 11)
#define SIZE_1k (1 << 10)

#define DDR_ADDR_START_REGION       (0x8)     //DDR address 0x8000 0000~ 2G
#define DDR_ADDR_END_REGION         (0xF)
#define DDR_ADDR_REGION_SHIFT       (28)
#define PAGE_SPAN_OFFSET            (0x10000)
#define DDR_1M_LEN                  (0x100000)
#define DMA_MAX_BLK_NUM             (0xFFFF)
#define DMA_MAX_XFER_SIZE           (16*(DMA_MAX_BLK_NUM+1)) //max xfer size is 1M

typedef void (*dma_ddr_func)(void);

static void *regBase = (void *)DDRC0_CTRL_PHY_ADDR;
static unsigned long ddrMemStartAddr = DDR_MEMORY0_ADDR_START;
static unsigned long ddrMemEndAddr =
	(DDR_MEMORY0_ADDR_START + DDR_MEMORY0_ADDR_SIZE);
extern unsigned char getc_unblock(void);

#define TOP_CRM_BLOCK_SW_RST0 (TOP_CRM_BASE_ADDR + 0x4)

//static void dumpAll(void){
//	int offset;
//
//	offset = 0;
//	hprintf(TSENV, "addr 0x%x = 0x%x\n",(regBase+offset),readl(regBase+offset));
//}

//static void changeBase(void){
//	char c=0;
//	printf("input ddr controller base N?(0~1)\r\n");
//	c=getc();
//	putc(c);

//	switch (c){
//		case '0':
//			regBase = DDRC0_CTRL_PHY_ADDR;
//			break;
//		case '1':
//			regBase = DDRC1_CTRL_PHY_ADDR;
//			break;
//	}
//
//	dumpAll();
//}

void gdma_reset(void)
{
    u32 reg_data, i;
    
    reg_data = readl(TOP_CRM_BASE + GDMA_RESETREG);
    reg_data = (reg_data & (~(0x1 << 12))); // gdma reset
    writel(reg_data, TOP_CRM_BASE + GDMA_RESETREG);
    for(i = 0; i < 10000; i++) {}
    reg_data = (reg_data & (~(0x1 << 12))) | (0x1 << 12); // relese gdma reset
    writel(reg_data, TOP_CRM_BASE + GDMA_RESETREG);
    for(i = 0; i < 10000; i++) {}
}

static int ddrc_init(void *ddr_base)
{
	void *ddr_local_base_addr = ddr_base;
	void *ddrc_base_addr = ddr_base + 0x1000;
	// unsigned int ddr_phy_base_addr   = ddr_base+0x2000;

	unsigned int r_data, w_data;

	pr_debug("start ddr init,ddr_local_base_addr\n");


	if (ddr_base == (void *)DDRC0_CTRL_PHY_ADDR) {
		r_data = readl(TOP_CRM_BASE_ADDR + 0x4);
		r_data |= (0x1 << 5);
		//r_data &= ~(0x1<<5);
		//r_data |= (0x1<<14);
		writel(r_data, TOP_CRM_BASE_ADDR + 0x4);
		pr_debug("config ddr0\n");
	} else if (ddr_base == (void *)DDRC1_CTRL_PHY_ADDR) {
		r_data = readl(TOP_CRM_BASE_ADDR + 0x4);
		r_data |= (0x1 << 4);
		//r_data &= ~(0x1<<4);
		//r_data |= (0x1<<13);
		writel(r_data, TOP_CRM_BASE_ADDR + 0x4);
		pr_debug("config ddr1\n");
	} else {
		printf("base error,ddr init fail\n");
	}

	//bst_realtime.delay_ns(100);
	r_data = readl(ddr_local_base_addr + 0x0); //disabe
	pr_debug("config pwokin deassert\n");
#ifdef CONFIG_ZEBU_HARDWARE_60
	w_data = r_data | (1 << 9); //pwokin deassert
	writel(w_data, ddr_local_base_addr + 0x0);
#else
	udelay(100);
	r_data |= (0x30 << 10);
	r_data &= ~(0xF << 10);
	writel(r_data, ddr_local_base_addr + 0x0); //DDR_LOCAL.CLOCK_AND_RESET
	udelay(100);
	r_data = readl(ddr_local_base_addr + 0x0);
	w_data = r_data | (1 << 9); //pwokin deassert
	writel(w_data, ddr_local_base_addr + 0x0);
#endif
	pr_debug("config ddrc_base_addr \n");
	//------------------------------ DDR CTRL ----------------------------------//
	writel(0x0001, ddrc_base_addr + 0x0304); //DDRC.DBG1
	writel(0x0001, ddrc_base_addr + 0x0030); //DDRC.PWRCTL

	r_data = readl(ddrc_base_addr +
			    0x0004); //DDRC.STAT, r_data should be 0x0
	if (r_data) {
		printf("DDRC.STAT read data is not 0x0\n");
	}

	writel(0x80080020, ddrc_base_addr + 0x0000); //DDRC.MSTR
	writel(0x0010, ddrc_base_addr + 0x0010); //DDRC.MRCTRL0
	writel(0x0001c8d0, ddrc_base_addr + 0x0014); //DDRC.MRCTRL1
	writel(0x0202, ddrc_base_addr + 0x0020); //DDRC.DERATEEN
	writel(0x89c8aba2, ddrc_base_addr + 0x0024); //DDRC.DERATEINT
	writel(0x0001, ddrc_base_addr + 0x002c); //DDRC.DERATECTL
	writel(0x0000, ddrc_base_addr + 0x0030); //DDRC.PWRCTL
	writel(0x0040e804, ddrc_base_addr + 0x0034); //DDRC.PWRTMG
	writel(0x00840000, ddrc_base_addr + 0x0038); //DDRC.HWLPCTL
	writel(0x00210000, ddrc_base_addr + 0x0050); //DDRC.RFSHCTL0
	writel(0x0001, ddrc_base_addr + 0x0060); //DDRC.RFSHCTL3
	writel(0x00618130, ddrc_base_addr + 0x0064); //DDRC.RFSHTMG
	writel(0x00480000, ddrc_base_addr + 0x0068); //DDRC.RFSHTMG1
	writel(0x063f7f10, ddrc_base_addr + 0x0070); //DDRC.ECCCFG0
	writel(0x07a0, ddrc_base_addr + 0x0074); //DDRC.ECCCFG1
	writel(0x0300, ddrc_base_addr + 0x007c); //DDRC.ECCCTL
	writel(0x045a, ddrc_base_addr + 0x00b8); //DDRC.ECCPOISONADDR0
	writel(0x07016196, ddrc_base_addr + 0x00bc); //DDRC.ECCPOISONADDR1
	writel(0x0000, ddrc_base_addr + 0x00c0); //DDRC.CRCPARCTL0
	writel(0x00020002, ddrc_base_addr + 0x00d0); //DDRC.INIT0
	//REG32_WRITE(ddrc_base_addr+0x00d4,0x0009_0002);      //DDRC.INIT1
	//REG32_WRITE(ddrc_base_addr+0x00d4,0x0030_0002);      //DDRC.INIT1 //2018-12-28
	writel(0x00400002, ddrc_base_addr + 0x00d4); //DDRC.INIT1 //2019-1-28
	writel(0x6105, ddrc_base_addr + 0x00d8); //DDRC.INIT2
	writel(0x0054002d, ddrc_base_addr + 0x00dc); //DDRC.INIT3
	writel(0x00300000, ddrc_base_addr + 0x00e0); //DDRC.INIT4
	writel(0x00040009, ddrc_base_addr + 0x00e4); //DDRC.INIT5
	writel(0x004d, ddrc_base_addr + 0x00e8); //DDRC.INIT6
	writel(0x004d, ddrc_base_addr + 0x00ec); //DDRC.INIT7
	writel(0x0000, ddrc_base_addr + 0x00f0); //DDRC.DIMMCTL
	writel(0x1a203622, ddrc_base_addr + 0x0100); //DDRC.DRAMTMG0
	writel(0x00060630, ddrc_base_addr + 0x0104); //DDRC.DRAMTMG1
	//REG32_WRITE(ddrc_base_addr+0x0108,0x070e_0f14);      //DDRC.DRAMTMG2
	writel(0x0e1c0f14, ddrc_base_addr + 0x0108); //DDRC.DRAMTMG2 //2018-12-28
	writel(0x00b0c000, ddrc_base_addr + 0x010c); //DDRC.DRAMTMG3
	writel(0x0f04080f, ddrc_base_addr + 0x0110); //DDRC.DRAMTMG4
	writel(0x02040c0c, ddrc_base_addr + 0x0114); //DDRC.DRAMTMG5
	writel(0x01010007, ddrc_base_addr + 0x0118); //DDRC.DRAMTMG6
	writel(0x0402, ddrc_base_addr + 0x011c); //DDRC.DRAMTMG7
	writel(0x0101, ddrc_base_addr + 0x0120); //DDRC.DRAMTMG8
	writel(0x00020000, ddrc_base_addr + 0x0130); //DDRC.DRAMTMG12
	writel(0x0c100002, ddrc_base_addr + 0x0134); //DDRC.DRAMTMG13
	writel(0x0136, ddrc_base_addr + 0x0138); //DDRC.DRAMTMG14
	writel(0xc3200018, ddrc_base_addr + 0x0180); //DDRC.ZQCTL0
	writel(0x0288c6a7, ddrc_base_addr + 0x0184); //DDRC.ZQCTL1
	writel(0x0000, ddrc_base_addr + 0x0188); //DDRC.ZQCTL2
	//REG32_WRITE(ddrc_base_addr+0x0190,0x0397_820a);      //DDRC.DFITMG0
	writel(0x0392820a, ddrc_base_addr + 0x0190); //DDRC.DFITMG0,2019-1-7
	writel(0x00090202, ddrc_base_addr + 0x0194); //DDRC.DFITMG1
	writel(0x07607011, ddrc_base_addr + 0x0198); //DDRC.DFILPCFG0
	writel(0xa0400018, ddrc_base_addr + 0x01a0); //DDRC.DFIUPD0
	writel(0x000a00c5, ddrc_base_addr + 0x01a4); //DDRC.DFIUPD1
	writel(0x0000, ddrc_base_addr + 0x01a8); //DDRC.DFIUPD2
	writel(0x0001, ddrc_base_addr + 0x01b0); //DDRC.DFIMISC
	writel(0x170a, ddrc_base_addr + 0x01b4); //DDRC.DFITMG2
	writel(0x0001, ddrc_base_addr + 0x01c0); //DDRC.DBICTL
	writel(0x0000, ddrc_base_addr + 0x01c4); //DDRC.DFIPHYMSTR

	// below code is added by caiwei, 2019-1-18
	writel(0x0000, ddrc_base_addr + 0x0300); //DDRC.DBG0
	writel(0x0000, ddrc_base_addr + 0x0304); //DDRC.DBG1
	writel(0x0000, ddrc_base_addr + 0x030c); //DDRC.DBGCMD
	writel(0x0001, ddrc_base_addr + 0x0320); //DDRC.SWCTL
	writel(0x0000, ddrc_base_addr + 0x0328); //DDRC.SWCTLSTATIC
	writel(0x00803033, ddrc_base_addr + 0x0330); //DDRC.OCPARCFG0
	writel(0x0798, ddrc_base_addr + 0x0334); //DDRC.OCPARCFG1
	writel(0x0011, ddrc_base_addr + 0x036c); //DDRC.POISONCFG
	writel(0x01f0, ddrc_base_addr + 0x0374); //DDRC.ADVECCINDEX
	writel(0x0000, ddrc_base_addr + 0x037c); //DDRC.ECCPOISONPAT0
	writel(0x0000, ddrc_base_addr + 0x0384); //DDRC.ECCPOISONPAT2
	writel(0x0002, ddrc_base_addr + 0x03c0); //DDRC.REGPARCFG
	writel(0x0001, ddrc_base_addr + 0x03e0); //DDRC.OCCAPCFG
	writel(0x0001, ddrc_base_addr + 0x03e8); //DDRC.OCCAPCFG1

	r_data = readl(ddrc_base_addr +
			    0x0060); //DDRC.RFSHCTL3 r_data should be 0x00000001
	if (r_data != 0x1) {
		printf("DDRC.RFSHCTL3 read data is not 0x1\n");
	}

	writel(0x00803032, ddrc_base_addr + 0x0330); //DDRC.OCPARCFG0
	// end add

	//ADDR MAP, 8Gbit
	writel(0x00080808, ddrc_base_addr + 0x0204); //DDRC.ADDRMAP1
	writel(0x0000, ddrc_base_addr + 0x0208); //DDRC.ADDRMAP2
	writel(0x0000, ddrc_base_addr + 0x020c); //DDRC.ADDRMAP3
	writel(0x1f1f, ddrc_base_addr + 0x0210); //DDRC.ADDRMAP4
	writel(0x070f0707, ddrc_base_addr + 0x0214); //DDRC.ADDRMAP5
	writel(0x07070707, ddrc_base_addr + 0x0218); //DDRC.ADDRMAP6
	//REG32_WRITE(ddrc_base_addr+0x021c,0x0007);       //DDRC.ADDRMAP7  // mask by caiwei
	writel(0x0f0f, ddrc_base_addr + 0x021c); //DDRC.ADDRMAP7
	writel(0x07070707, ddrc_base_addr + 0x0224); //DDRC.ADDRMAP9
	writel(0x07070707, ddrc_base_addr + 0x0228); //DDRC.ADDRMAP10
	writel(0x0007, ddrc_base_addr + 0x022c); //DDRC.ADDRMAP11

	writel(0x06080a20, ddrc_base_addr + 0x0240); //DDRC.ODTCFG
	writel(0x0000, ddrc_base_addr + 0x0244); //DDRC.ODTMAP

	//QoS
	//below code add by caiwei
	writel(0x40003f01, ddrc_base_addr + 0x0250); //DDRC.SCHED
	writel(0x0000, ddrc_base_addr + 0x0254); //DDRC.SCHED1
	//REG32_WRITE(ddrc_base_addr+0x025c,0x0f00_0001);       //DDRC.PERFHPR1
	writel(0x1f0003ff, ddrc_base_addr + 0x0264); //DDRC.PERFLPR1
	writel(0x1f0003ff, ddrc_base_addr + 0x026c); //DDRC.PERFWR1
	writel(0x0000, ddrc_base_addr + 0x0400); //DDRC.PCCFG

	//REG32_WRITE(ddrc_base_addr+0x0404,0x0001_1100);       //DDRC.PCFGR_0
	//REG32_WRITE(ddrc_base_addr+0x0408,0x1100);       //DDRC.PCFGW_0
	writel(0x00015100, ddrc_base_addr + 0x0404); //DDRC.PCFGR_0
	writel(0x5100, ddrc_base_addr + 0x0408); //DDRC.PCFGW_0
	writel(0x0001, ddrc_base_addr + 0x0490); //DDRC.PCTRL_0

	//REG32_WRITE(ddrc_base_addr+0x04b4,0x0001_1100);       //DDRC.PCFGR_1
	//REG32_WRITE(ddrc_base_addr+0x04b8,0x1100);       //DDRC.PCFGW_1
	writel(0x00015100, ddrc_base_addr + 0x04b4); //DDRC.PCFGR_1
	writel(0x5100, ddrc_base_addr + 0x04b8); //DDRC.PCFGW_1
	writel(0x0001, ddrc_base_addr + 0x0540); //DDRC.PCTRL_1

	//REG32_WRITE(ddrc_base_addr+0x0564,0x0001_1100);       //DDRC.PCFGR_2
	//REG32_WRITE(ddrc_base_addr+0x0568,0x1100);       //DDRC.PCFGW_2
	writel(0x00015100, ddrc_base_addr + 0x0564); //DDRC.PCFGR_2
	writel(0x5100, ddrc_base_addr + 0x0568); //DDRC.PCFGW_2
	writel(0x0001, ddrc_base_addr + 0x05f0); //DDRC.PCTRL_2

	//REG32_WRITE(ddrc_base_addr+0x0614,0x0001_1100);       //DDRC.PCFGR_3
	//REG32_WRITE(ddrc_base_addr+0x0618,0x1100);       //DDRC.PCFGW_3
	writel(0x00011500, ddrc_base_addr + 0x0614); //DDRC.PCFGR_3
	writel(0x1500, ddrc_base_addr + 0x0618); //DDRC.PCFGW_3
	writel(0x0001, ddrc_base_addr + 0x06a0); //DDRC.PCTRL_3
	//end add
	pr_debug("ddrc init done!\n");

	// release ddrc_core_rstn and axi_port0~3 rstn
	//   `uvm_info("lpddr4_inst0_backdoor_init()",$psprintf("Release %s.Core_ddrc_rstn",name),UVM_NONE);

	pr_debug("Release Core_ddrc_rstn\n");
	r_data = readl(ddr_local_base_addr +
			    0x0); //DDR_LOCAL.CLOCK_AND_RESET
	w_data = r_data | (1 << 8);
	writel(w_data, ddr_local_base_addr + 0x0); //DDR_LOCAL.CLOCK_AND_RESET

	//this.noc_subenv.virtual_sequencer.readReg32("CPU_P0",`TOP_CRM_BASE_ADDR+0xBC,r_data);
	//r_data[31:28]=4'hf;
	//this.noc_subenv.virtual_sequencer.writeReg32("CPU_P0",`TOP_CRM_BASE_ADDR+0xBC,r_data);
	//`uvm_info("lpddr4_inst0_backdoor_init()",$psprintf("Release %s axi_rstn",name),UVM_NONE);
	r_data = readl(ddr_local_base_addr + 0x0); //disabe
	r_data |= (0xF << 10);
	writel(r_data, ddr_local_base_addr + 0x0); //DDR_LOCAL.CLOCK_AND_RESET

	writel(0x0000, ddrc_base_addr + 0x0304); //DDRC.DBG1
	r_data = readl(ddrc_base_addr +
			    0x0030); //DDRC.PWRCTL, r_data should be 0x00000000
	if (r_data != 0x0) {
		printf(" 111DDRC.PWRCTL read data is not 0x0\n");
	}

	writel(0x0000, ddrc_base_addr + 0x0030); //DDRC.PWRCTL
	r_data = readl(ddrc_base_addr +
			    0x0030); //DDRC.PWRCTL, r_data should be 0x00000000
	if (r_data != 0x0) {
		printf(" 222DDRC.PWRCTL read data is not 0x0\n");
	}

	writel(0x0000, ddrc_base_addr + 0x0030); //DDRC.PWRCTL
	writel(0x0000, ddrc_base_addr + 0x0320); //DDRC.SWCTL

	while (1) {
		r_data = readl(ddrc_base_addr + 0x0324); //DDRC.SWSTAT
		if (r_data == 0x0) {
			printf(" DRC.SWSTAT r_data is 0x0,break\n");
			break;
		}
	}

	writel(0x0000, ddrc_base_addr + 0x01b0); //DDRC.DFIMISC
	writel(0x0000, ddrc_base_addr + 0x01b0); //DDRC.DFIMISC
	writel(0x1000, ddrc_base_addr + 0x01b0); //DDRC.DFIMISC

	r_data = readl(ddrc_base_addr +
			    0x00d0); //DDRC.INIT0,  r_data should be 0x00020002
	if (r_data != 0x00020002) {
		printf(" DDRC.INIT0 read data is not 0x0002_0002\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x01c0); //DDRC.DBICTL, r_data should be 0x00000001
	if (r_data != 0x0001) {
		printf(" DDRC.DBICTL read data is not 0x0001\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x00dc); //DDRC.INIT3,  r_data should be 0x0054002d
	if (r_data != 0x0054002d) {
		printf(" DDRC.INIT3 read data is not 0x0054_002d\n");
	}
	//r_data = readl( ddrc_base_addr+0x00dc);              //DDRC.INIT3,  r_data should be 0x0054002d
	//if(r_data!=0x0054002d){printf(" DDRC.INIT3 read data is not 0x0054_002d\n");}
	r_data = readl(ddrc_base_addr +
			    0x00e0); //DDRC.INIT4,  r_data should be 0x00300000
	if (r_data != 0x00300000) {
		printf(" DDRC.INIT4 read data is not 0x0030_0000\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x00e8); //DDRC.INIT6,  r_data should be 0x0000004d
	if (r_data != 0x004d) {
		printf("DDRC.INIT6 read data is not 0x004d\n");
	}
	//r_data = readl( ddrc_base_addr+0x00e8);              //DDRC.INIT6,  r_data should be 0x0000004d
	//if(r_data!=0x004d)begin `uvm_error("lpddr4_inst0_backdoor_init()","DDRC.INIT6 read data is not 0x004d"); end
	r_data = readl(ddrc_base_addr +
			    0x00e0); //DDRC.INIT4,  r_data should be 0x00300000
	if (r_data != 0x00300000) {
		printf("DDRC.INIT4 read data is not 0x0030_0000\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x00ec); //DDRC.INIT7,  r_data should be 0x0000004d
	if (r_data != 0x004d) {
		printf("DDRC.INIT7 read data is not 0x004d\n");
	}
	//r_data = readl( ddrc_base_addr+0x00ec);              //DDRC.INIT7,  r_data should be 0x0000004d
	//if(r_data!=0x004d)begin `uvm_error("lpddr4_inst0_backdoor_init()","DDRC.INIT7 read data is not 0x004d"); end
	r_data = readl(ddrc_base_addr +
			    0x00d0); //DDRC.INIT0,  r_data should be 0x00020002
	if (r_data != 0x00020002) {
		printf("DDRC.INIT0 read data is not 0x0002_0002\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x00d0); //DDRC.INIT0,  r_data should be 0x00020002
	if (r_data != 0x00020002) {
		printf(" DDRC.INIT0 read data is not 0x0002_0002\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x01c0); //DDRC.DBICTL, r_data should be 0x00000001
	if (r_data != 0x0001) {
		printf(" DDRC.DBICTL read data is not 0x0001\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x00dc); //DDRC.INIT3,  r_data should be 0x0054002d
	if (r_data != 0x0054002d) {
		printf(" DDRC.INIT3 read data is not 0x0054_002d\n");
	}
	//r_data = readl( ddrc_base_addr+0x00dc);              //DDRC.INIT3,  r_data should be 0x0054002d
	//if(r_data!=0x0054002d){printf(" DDRC.INIT3 read data is not 0x0054_002d\n");}
	r_data = readl(ddrc_base_addr +
			    0x00e0); //DDRC.INIT4,  r_data should be 0x00300000
	if (r_data != 0x00300000) {
		printf(" DDRC.INIT4 read data is not 0x0030_0000\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x00e8); //DDRC.INIT6,  r_data should be 0x0000004d
	if (r_data != 0x004d) {
		printf("DDRC.INIT6 read data is not 0x004d\n");
	}
	//r_data = readl( ddrc_base_addr+0x00e8);              //DDRC.INIT6,  r_data should be 0x0000004d
	//if(r_data!=0x004d)begin `uvm_error("lpddr4_inst0_backdoor_init()","DDRC.INIT6 read data is not 0x004d"); end
	r_data = readl(ddrc_base_addr +
			    0x00e0); //DDRC.INIT4,  r_data should be 0x00300000
	if (r_data != 0x00300000) {
		printf("DDRC.INIT4 read data is not 0x0030_0000\n");
	}
	r_data = readl(ddrc_base_addr +
			    0x00ec); //DDRC.INIT7,  r_data should be 0x0000004d
	if (r_data != 0x004d) {
		printf("DDRC.INIT7 read data is not 0x004d\n");
	}
	//r_data = readl( ddrc_base_addr+0x00ec);              //DDRC.INIT7,  r_data should be 0x0000004d
	//if(r_data!=0x004d)begin `uvm_error("lpddr4_inst0_backdoor_init()","DDRC.INIT7 read data is not 0x004d"); end
	r_data = readl(ddrc_base_addr +
			    0x00d0); //DDRC.INIT0,  r_data should be 0x00020002
	if (r_data != 0x00020002) {
		printf("DDRC.INIT0 read data is not 0x0002_0002\n");
	}

	// add by caiwei,2019-1-10
	writel(0x1001, ddrc_base_addr + 0x01b0); //ddrc.dfimisc

	// end add
	//  `uvm_info("lpddr4_inst0_backdoor_init()",$psprintf("%s DDRC Init Done",name),UVM_LOW); //TBD
	pr_debug("DDRC Init Done\n");

	//------------------------------ DDR PHY ----------------------------------//

	//--------------------------- DDRC ---------------------------------------//
	writel(0x00001020, ddrc_base_addr + 0x000001b0); //DDRC.DFIMISC
	while (1) {
		r_data = readl(ddrc_base_addr + 0x000001bc); //DDRC.DFISTAT
		if (r_data == 0x00000001) {
			printf("11DRC.DFISTAT r_data is 1\n");
			break;
		}
	}
	writel(0x00001000, ddrc_base_addr + 0x000001b0); //DDRC.DFIMISC
	//< add by caiwei, 2018-12-27
	while (1) {
		r_data = readl(ddrc_base_addr + 0x000001bc); //DDRC.DFISTAT
		if (r_data == 0x00000001) {
			printf("22DRC.DFISTAT r_data is 1\n");
			break;
		}
	}
	//< end add
	writel(0x00001005, ddrc_base_addr + 0x000001b0); //DDRC.DFIMISC
	//REG32_WRITE(ddrc_base_addr+0x0000_01b0,0x0000_1001);      //DDRC.DFIMISC // mask by caiwei, 2019-1-10
	writel(0x00000001, ddrc_base_addr + 0x00000320); //DDRC.SWCTL
	while (1) {
		r_data = readl(ddrc_base_addr + 0x00000324); //DDRC.SWSTAT
		if (r_data == 0x00000001) {
			printf("33DRC.SWSTAT r_data is 1\n");
			break;
		}
	}

	//`uvm_info("lpddr4_inst0_backdoor_init()",$psprintf("Wait lpddr4 initial done"),UVM_NONE);
	pr_debug("Wait lpddr4 initial done\n");
	while (1) {
		r_data = readl(ddrc_base_addr + 0x00000004); //DDRC.STAT
		if (r_data == 0x00000001) {
			//`uvm_info("lpddr4_inst0_backdoor_init()",$psprintf("DDRC.STAT r_data is 0x%0h",r_data),UVM_DEBUG);
			printf("44 DDRC.STAT is 1\n");
			break;
		}
	}
	writel(0x00000000, ddrc_base_addr + 0x00000030); //DDRC.PWRCTL
	writel(0x00000000, ddrc_base_addr + 0x00000030); //DDRC.PWRCTL
	writel(0x00000001, ddrc_base_addr + 0x00000490); //DDRC.PCTRL_0
	writel(0x00000001, ddrc_base_addr + 0x00000540); //DDRC.PCTRL_1
	writel(0x00000001, ddrc_base_addr + 0x000005f0); //DDRC.PCTRL_2
	writel(0x00000001, ddrc_base_addr + 0x000006a0); //DDRC.PCTRL_3

	//`uvm_info("lpddr4_inst0_backdoor_init",$psprintf("%s backdoor init done",name),UVM_NONE);
	pr_debug("backdoor init done\n");

	return 0;
}

static int ddr_ReadWrite(void)
{
	int err = 0;
	int i = 0;

	unsigned long addr;
	unsigned int delta;

	delta = 4;

	pr_debug("ddrMemStartAddr = 0x%lx, ddrMemEndAddr = 0x%lx\r\n",
		ddrMemStartAddr, ddrMemEndAddr);

while (1) {
	i++;
	for (addr = ddrMemStartAddr; addr < (ddrMemEndAddr - 3);) {
		if (!(addr & (SIZE_128k - 1))) {
			printf("addr = 0x%lx\r\n", addr);
		}

		writel(0x12345678, addr);
		if (readl(addr) != 0x12345678) {
			err += 1;
			printf("32bit rw err,val = 0x%x\r\n",
				readl(addr));
		}

		writeb(0x12, addr + 0);
		writeb(0xAA, addr + 1);
		writeb(0x55, addr + 2);
		writeb(0xA5, addr + 3);
		if (readb(addr + 0) != 0x12) {
			err += 1;
			printf("8bit rw err0,val = 0x%x\r\n",
				readb(addr + 0));
		}
		if (readb(addr + 1) != 0xAA) {
			err += 1;
			printf("8bit rw err1,val = 0x%x\r\n",
				readb(addr + 1));
		}
		if (readb(addr + 2) != 0x55) {
			err += 1;
			printf("8bit rw err2,val = 0x%x\r\n",
				readb(addr + 2));
		}
		if (readb(addr + 3) != 0xA5) {
			err += 1;
			printf("8bit rw err3,val = 0x%x\r\n",
				readb(addr + 3));
		}
		if (readl(addr) != 0xA555AA12) {
			err += 1;
			printf("8bit rw err4,val =0x%x\r\n",
				readl(addr));
		}

		writew(0xA5A5, addr + 2);
		if (readw(addr + 2) != 0xA5A5) {
			err += 1;
			printf("16bit rw err1, val = 0x%x\r\n",
				readw(addr + 2));
		}

		if (readl(addr) != 0xA5A5AA12) {
			err += 1;
			printf("16bit rw err2, val = 0x%x\r\n",
				readl(addr));
		}

		addr += delta;
	}
	udelay(100);
	printf("--------i %d\n", i);
	printf("err cnt = %d\r\n", err);
}
	
	return err;
}

static int ddr_write_read_value_test_do64(unsigned long start1,
					  unsigned long start2,
					  unsigned long long val)
{
	unsigned long long u64val[2];
	u64val[0] = val;
	u64val[1] = ~u64val[0];
	writeq(u64val[0], start1);
	if (start2) {
		writeq(u64val[1], start2);
	}
	if (u64val[0] != readq(start1)) {
		printf("RandomWrite 64Bit ERROR:ADDR:%llx (W:%llx.R:%llx).\r\n",
			start1, u64val[0], readq(start1));
	}
	if (start2) {
		if (u64val[1] != readq(start2)) {
			printf("RandomWrite 64Bit ERROR:ADDR:%llx (W:%llx.R:%llx).\r\n",
				start2, u64val[1], readq(start2));
		}
	}
	return 0;
}

static int ddr_write_read_value_test_do32(unsigned long start1,
					  unsigned long start2,
					  unsigned int *val)
{
	unsigned int u32val[2][2];
	unsigned int i;
	unsigned long addr, addr_inverse;

	// 32Bit test
	for (i = 0; i < 2; i++) {
		addr = (start1 + (4 * i));
		u32val[0][i] = (unsigned int)val[i];
		writel(u32val[0][i], addr);
		if (start2) {
			addr_inverse = (start2 + (4 * i));
			u32val[1][i] = ~u32val[0][i];
		}
	}
	for (i = 0; i < 2; i++) {
		addr = (start1 + (4 * i));
		if (u32val[0][i] != readl(addr)) {
			printf("RandomWrite 32Bit ERROR:ADDR:%llx (W:%x.R:%x).\r\n",
				addr, u32val[0][i], readl(addr));
		}
		if (start2) {
			addr_inverse = (start2 + (4 * i));
			if (u32val[1][i] != readl(addr_inverse)) {
				printf("RandomWrite 32Bit ERROR:ADDR:%llx (W:%x.R:%x).\r\n",
					addr_inverse, u32val[1][i],
					readl(addr_inverse));
			}
		}
	}
	return 0;
}

static int ddr_write_read_value_test_do16(unsigned long start1,
					  unsigned long start2,
					  unsigned short *val)
{
	unsigned short u16val[2][4];
	unsigned int i;
	unsigned long addr, addr_inverse;

	// 32Bit test
	for (i = 0; i < 4; i++) {
		addr = (start1 + (2 * i));
		u16val[0][i] = (unsigned short)val[i];
		writew(u16val[0][i], addr);
		if (start2) {
			addr_inverse = (start2 + (2 * i));
			u16val[1][i] = ~u16val[0][i];
		}
	}
	for (i = 0; i < 4; i++) {
		addr = (start1 + (2 * i));
		if (u16val[0][i] != readw(addr)) {
			printf("RandomWrite 16Bit ERROR:ADDR:%llx (W:%x.R:%x).\r\n",
				addr, u16val[0][i], readw(addr));
		}
		if (start2) {
			addr_inverse = (start2 + (2 * i));
			if (u16val[1][i] != readw(addr_inverse)) {
				printf("RandomWrite 16Bit ERROR:ADDR:%llx (W:%x.R:%x).\r\n",
					addr_inverse, u16val[1][i],
					readw(addr_inverse));
			}
		}
	}
	return 0;
}

static int ddr_write_read_value_test_do8(unsigned long start1,
					 unsigned long start2,
					 unsigned char *val)
{
	unsigned char u8val[2][8];
	unsigned int i;
	unsigned long addr, addr_inverse;

	// 32Bit test
	for (i = 0; i < 8; i++) {
		addr = (start1 + i);
		u8val[0][i] = (unsigned short)val[i];
		writeb(u8val[0][i], addr);
		if (start2) {
			addr_inverse = (start2 + (2 * i));
			u8val[1][i] = ~u8val[0][i];
		}
	}
	for (i = 0; i < 8; i++) {
		addr = (start1 + i);
		if (u8val[0][i] != readb(addr)) {
			printf("RandomWrite 16Bit ERROR:ADDR:%llx (W:%x.R:%x).\r\n",
				addr, u8val[0][i], readb(addr));
		}
		if (start2) {
			addr_inverse = (start2 + (2 * i));
			if (u8val[1][i] != readb(addr_inverse)) {
				printf("RandomWrite 16Bit ERROR:ADDR:%llx (W:%x.R:%x).\r\n",
					addr_inverse, u8val[1][i],
					readb(addr_inverse));
			}
		}
	}
	return 0;
}

static int ddr_addr_write_read_value_test(unsigned long start1,
					  unsigned long start2)
{
	int i;
	unsigned long long u64val;
	unsigned int u32val[2];
	unsigned short u16val[4];
	unsigned char u8val[8];
	/*
Do WRITE READ TEST. FOR 64Bit/32Bit/16Bit/8Bit.
first write data as addr.after read addr'data to compare with the writed data.
*/
	// 64Bit test
	u64val = my_randul();
	ddr_write_read_value_test_do64(start1, start2, u64val);

	// 32Bit test
	for (i = 0; i < 2; i++) {
		u32val[i] = (unsigned int)my_randul();
	}

	ddr_write_read_value_test_do32(start1, start2, u32val);
	// 16Bit test
	for (i = 0; i < 4; i++) {
		u16val[i] = (unsigned short)my_randul();
	}
	ddr_write_read_value_test_do16(start1, start2, u16val);

	// 8Bit test
	for (i = 0; i < 8; i++) {
		u8val[i] = (unsigned char)my_randul();
	}
	ddr_write_read_value_test_do8(start1, start2, u8val);

	/***********************************************
Test: Write data as Inverse of addr. then do test.
************************************************/
	// 64Bit test

	ddr_write_read_value_test_do64(start1, start2, ~u64val);
	for (i = 0; i < 2; i++) {
		u32val[i] = (unsigned int)~u32val[i];
	}
	ddr_write_read_value_test_do32(start1, start2, u32val);
	// 16Bit test
	for (i = 0; i < 4; i++) {
		u16val[i] = (unsigned short)~u16val[i];
	}
	ddr_write_read_value_test_do16(start1, start2, u16val);

	// 8Bit test
	for (i = 0; i < 8; i++) {
		u8val[i] = (unsigned char)~u8val[i];
	}
	ddr_write_read_value_test_do8(start1, start2, u8val);
	return 0;
}

static int ddr_addr_write_read_addr_test(unsigned long start1,
					 unsigned long start2)
{
	int i;
	unsigned long long u64val;
	unsigned int u32val[2];
	unsigned short u16val[4];
	unsigned char u8val[8];
	/*
Do WRITE READ TEST. FOR 64Bit/32Bit/16Bit/8Bit.
first write data as addr.after read addr'data to compare with the writed data.
*/
	// 64Bit test

	u64val = start1;
	ddr_write_read_value_test_do64(start1, start2, u64val);

	// 32Bit test
	for (i = 0; i < 2; i++) {
		u32val[i] = (unsigned int)start1 + (2 * i);
	}

	ddr_write_read_value_test_do32(start1, start2, u32val);
	// 16Bit test
	for (i = 0; i < 4; i++) {
		u16val[i] = (unsigned short)start1 + (4 * i);
	}
	ddr_write_read_value_test_do16(start1, start2, u16val);

	// 8Bit test
	for (i = 0; i < 8; i++) {
		u8val[i] = (unsigned char)start1 + i;
	}
	ddr_write_read_value_test_do8(start1, start2, u8val);

	/***********************************************
Test: Write data as Inverse of addr. then do test.
************************************************/
	// 64Bit test
	ddr_write_read_value_test_do64(start1, start2, ~u64val);
	for (i = 0; i < 2; i++) {
		u32val[i] = (unsigned int)~u32val[i];
	}
	ddr_write_read_value_test_do32(start1, start2, u32val);
	// 16Bit test
	for (i = 0; i < 4; i++) {
		u16val[i] = (unsigned short)~u16val[i];
	}
	ddr_write_read_value_test_do16(start1, start2, u16val);

	// 8Bit test
	for (i = 0; i < 8; i++) {
		u8val[i] = (unsigned char)~u8val[i];
	}
	ddr_write_read_value_test_do8(start1, start2, u8val);
	return 0;
}

static int ddr_ReadWrite_random(unsigned long start, unsigned long max_size)
{
	int err = 0;
	unsigned long addr;
	unsigned long addr_inverse;
	unsigned long offset;
	unsigned long cnt = 0;

	printf("Start Random DDR Test.Press anykey to quit\r\n");
	printf("ddrMemStartAddr = 0x%lx, ddrMemMaxSize = 0x%lx\r\n",
		start, max_size);
	my_rand_seed(0x8112643);
	while (1) {
		offset = my_randul();
		offset %= max_size;
		offset &= ~0x7;
		if (offset - 0x8 >= max_size) {
			offset -= 8;
		}

		if (!(cnt & 0x3fff)) {
			printf("\n\r");
		}
		if (!(cnt & 0x3ff)) {
			printf("*");
		}
		addr = start + offset;
		addr_inverse = ((addr & ~(DDR_RANDOM_INVERSE_ADDR_MASK)) |
				~(addr & DDR_RANDOM_INVERSE_ADDR_MASK));
		if (addr_inverse < DDR_RANDOM_ADDR_MIN ||
		    addr_inverse >= DDR_RANDOM_ADDR_MAX) {
			addr_inverse = 0;
		}
		ddr_addr_write_read_addr_test(addr, addr_inverse);
		ddr_addr_write_read_value_test(addr, addr_inverse);
		cnt++;
		if (getc_unblock()) {
			printf("\n\rExit Test.\n\r");
			return 0;
		}
	}

	return err;
}

static void menu(void)
{
	putc('\r');
	putc('\n');
	hprintf(TSENV, "0......init ddrc0\r\n");
	hprintf(TSENV, "1......init ddrc1\r\n");
	hprintf(TSENV, "2......open interleave \r\n");
	hprintf(TSENV, "3......close interleave\r\n");
	hprintf(TSENV, "4......init ddrc0 & ddrc1\r\n");
	hprintf(TSENV, "5......ddr read write test\r\n");
	hprintf(TSENV, "6......Run memtester\r\n");
	hprintf(TSENV, "7......ddr Random ADDRread write test\r\n");
	hprintf(TSENV, "9......set ddr test range\r\n");
	hprintf(TSENV, "a......ddr ctrl irq test\r\n");
	hprintf(TSENV, "c......ddr func irq test\r\n");
	hprintf(TSENV, "d......dump memory\r\n");
	hprintf(TSENV, "f......ddr global reset test\r\n");
	hprintf(TSENV, "g......ddr internal reset test\r\n");
	hprintf(TSENV, "h......ddr enable parity\r\n");
	hprintf(TSENV, "i......ddr bit rotation test\r\n");
	hprintf(TSENV, "j......ddr different length access test\r\n");
	hprintf(TSENV, "s......set32 memory\r\n");
	hprintf(TSENV, "q......quit\r\n");
}

static void logo(void)
{
	putc('\r');
	putc('\n');
	putc('d');
	putc('d');
	putc('r');
	putc('>');
}

static void closeInterleave(void)
{
	writel(1, 0x33000058);
}
void openInterleave(void)
{
	writel(0, 0x33000058);
}

void ddr_envInit(void)
{
	//interleave:off; ecc:off(default); parity:off(default)
	closeInterleave();
}

static void ddr0Init(void)
{
	regBase = (void *)DDRC0_CTRL_PHY_ADDR;
	ddrc_init(regBase);
}

static void ddr1Init(void)
{
	regBase = (void *)DDRC1_CTRL_PHY_ADDR;
	ddrc_init(regBase);
}

void ddr0ddr1Init(void)
{
	regBase = (void *)DDRC0_CTRL_PHY_ADDR;
	ddrc_init(regBase);
	regBase = (void *)DDRC1_CTRL_PHY_ADDR;
	ddrc_init(regBase);

	closeInterleave();
}

static void set_ddr_test_range(void)
{
	printf("input test start addr(hex)\r\n");
	if (getHex(&ddrMemStartAddr)) {
		return;
	}

	printf("input test end addr(hex)\r\n");
	if (getHex(&ddrMemEndAddr)) {
		return;
	}

	printf("ddrMemStartAddr = %lx, ddrMemEndAddr = %lx\r\n",
		ddrMemStartAddr, ddrMemEndAddr);
}

void ddrc_ctrl_irq(void *base_addr)
{
	u32 deratectl, data, i;
	void *ddrc_base = base_addr + 0x1000;

	for (i = 0; i < 10000; i++)
		; //software delay to avoid pre printf unfinished
	printf("\r\nctrl ddrc_base: %x\r\n", ddrc_base);
	printf("\r\nECU irq initial: %x\r\n",
		readl(0x33000000 + 0x130)); //A1000 to ECU interrupt

#if 0
	//parity
	ocparcfg0 = readl(ddrc_base+0x330);
	ocparcfg0 = (ocparcfg0&(~(1<<26))) | (0<<26); //par_raddr_err_intr_force
    ocparcfg0 = (ocparcfg0&(~(1<<25))) | (0<<25); //par_waddr_err_intr_force
    ocparcfg0 = (ocparcfg0&(~(1<<24))) | (0<<24); //par_raddr_err_intr_clr
    ocparcfg0 = (ocparcfg0&(~(1<<23))) | (1<<23); //par_raddr_err_intr_en
    ocparcfg0 = (ocparcfg0&(~(1<<22))) | (0<<22); //par_waddr_err_intr_clr
    ocparcfg0 = (ocparcfg0&(~(1<<21))) | (1<<21); //par_waddr_err_intr_en
    ocparcfg0 = (ocparcfg0&(~(1<<20))) | (0<<20); //par_addr_svlerr_en
    ocparcfg0 = (ocparcfg0&(~(1<<15))) | (0<<15); //par_rdata_err_intr_force
	ocparcfg0 = (ocparcfg0&(~(1<<14))) | (0<<14); //par_rdata_err_intr_clr
    ocparcfg0 = (ocparcfg0&(~(1<<13))) | (1<<13); //par_rdata_err_intr_en
    ocparcfg0 = (ocparcfg0&(~(1<<12))) | (0<<12); //par_rdata_slverr_en
    ocparcfg0 = (ocparcfg0&(~(1<<7))) | (0<<7);  //par_wdata_err_intr_force
    ocparcfg0 = (ocparcfg0&(~(1<<6))) | (0<<6);  //par_wdata_err_intr_clr
    ocparcfg0 = (ocparcfg0&(~(1<<5))) | (0<<5);  //par_wdata_slverr_en
    ocparcfg0 = (ocparcfg0&(~(1<<4))) | (1<<4);  //par_wdata_err_intr_en
    ocparcfg0 = (ocparcfg0&(~(1<<0))) | (1<<0);  //par enable

	//ECC
	ecccfg0 = readl(ddrc_base+0x70);
	ecccfg0 = (ecccfg0&(~(7<<0))) | (4<<0);

	eccctl = readl(ddrc_base+0x7C);
    eccctl = (eccctl&(~(1<<18))) | (0<<18);    //ecc_ap_err_intr_force
    eccctl = (eccctl&(~(1<<17))) | (0<<17);    //ecc_uncorrected_err_intr_force
    eccctl = (eccctl&(~(1<<16))) | (0<<16);    //ecc_ecorrected_err_intr_force
    eccctl = (eccctl&(~(1<<10))) | (1<<10);    //ecc_ap_err_intr_en
    eccctl = (eccctl&(~(1<<9))) | (1<<9);     //ecc_uncorrected_err_intr_en;
    eccctl = (eccctl&(~(1<<8))) | (1<<8);     //ecc_corrected_err_intr_en;
    eccctl = (eccctl&(~(1<<4))) | (0<<4);     //ecc_ap_intr_clr
    eccctl = (eccctl&(~(1<<3))) | (0<<3);     //ecc_uncorr_cnt_clr
    eccctl = (eccctl&(~(1<<2))) | (0<<2);     //ecc_corr_cnt_clr
    eccctl = (eccctl&(~(1<<1))) | (0<<1);     //ecc_uncorrected_err_clr
    eccctl = (eccctl&(~(1<<0))) | (0<<0);     //ecc_corrected_err_clr

	//OCCAP
    occapcfg = readl(ddrc_base+0x3E0);
    occapcfg = (occapcfg&(~(1<<18))) | (0<<18);  //occap_arb_intr_force
    occapcfg = (occapcfg&(~(1<<17))) | (0<<17);  //occap_arb_intr_clr
    occapcfg = (occapcfg&(~(1<<16))) | (1<<16);  //occap_arb_intr_en
    occapcfg = (occapcfg&(~(1<<0))) | (1<<0);   //occap_en

	occapcfg1 = readl(ddrc_base+0x3E8);
    occapcfg1 = (occapcfg1&(~(1<<18))) | (0<<18); //occap_ddrc_ctrl_intr_force
    occapcfg1 = (occapcfg1&(~(1<<17))) | (0<<17); //occap_ddrc_ctrl_intr_clr
    occapcfg1 = (occapcfg1&(~(1<<16))) | (1<<16); //occap_ddrc_ctrl_intr_en
    occapcfg1 = (occapcfg1&(~(1<<2))) | (0<<2);  //occap_ddrc_data_intr_force
    occapcfg1 = (occapcfg1&(~(1<<1))) | (0<<1);  //occap_ddrc_data_intr_clr
    occapcfg1 = (occapcfg1&(~(1<<0))) | (1<<0);  //occap_ddrc_data_intr_en

	//REGPARCFG
	regparcfg = readl(ddrc_base+0x3C0);
    regparcfg = (regparcfg&(~(1<<3))) | (0<<3);  //reg_par_err_intr_force
    regparcfg = (regparcfg&(~(1<<2))) | (0<<2);  //reg_par_err_intr_clr
    regparcfg = (regparcfg&(~(1<<1))) | (1<<1);  //reg_par_err_intr_en
    regparcfg = (regparcfg&(~(1<<0))) | (1<<0);  //reg_par_en
#endif

	//DERATECTL
	deratectl = readl(ddrc_base + 0x2C);
	deratectl = (deratectl & (~(1 << 2))) |
		    (0 << 2); //derate_temp_limit_intr_force
	deratectl = (deratectl & (~(1 << 1))) |
		    (0 << 1); //derate_temp_limit_intr_clr
	deratectl = (deratectl & (~(1 << 0))) |
		    (1 << 0); //derate_temp_limit_intr_en

	writel(0x0, ddrc_base + 0x320); //SWCTL
#if 0
	REG32_WRITE(ddrc_base+0x330, ocparcfg0);
	REG32_WRITE(ddrc_base+0x70, ocparcfg0);
	REG32_WRITE(ddrc_base+0x7C, eccctl);
	REG32_WRITE(ddrc_base+0x3E0, occapcfg);
	REG32_WRITE(ddrc_base+0x3E8, occapcfg1);
	REG32_WRITE(ddrc_base+0x3C0, regparcfg);
#endif
	writel(deratectl, ddrc_base + 0x2C);
	writel(0x1, ddrc_base + 0x320); //SWCTL

	while (readl(ddrc_base + 0x324) != 0x1)
		;

	//only test 1 situation to raise irq
	//DERATECTL
	data = 0;
	deratectl = readl(ddrc_base + 0x2C);
	data = deratectl | (1 << 2);
	writel(data, ddrc_base + 0x2C);

	for (i = 0; i < 100; i++)
		; //software delay

	data = 0;
	deratectl = readl(ddrc_base + 0x2C);
	data = deratectl | (1 << 1);
	writel(data, ddrc_base + 0x2C);

#if 0
    //OCPAR INT Check
	data = 0;
	ocparcfg0 = readl(ddrc_base+0x330);
	//data = ocparcfg0|(1<<26)|(1<<25)|(1<<15)|(1<<7); //assert
	data = ocparcfg0|(1<<26); //assert
	REG32_WRITE(ddrc_base+0x330, data);
	//printf("OCPARSTAT0:%x\r\n", readl(ddrc_base+0x338));
	//printf("OCPARSTAT1:%x\r\n", readl(ddrc_base+0x33C));

	for(i = 0; i < 100; i++); //software delay

	data = 0;
	ocparcfg0 = readl(ddrc_base+0x330);
	data = ocparcfg0|(1<<24)|(1<<22)|(1<<14)|(1<<6); //deassert
	REG32_WRITE(ddrc_base+0x330, data);
	//printf("OCPARSTAT0:%x\r\n", readl(ddrc_base+0x338));
	//printf("OCPARSTAT1:%x\r\n", readl(ddrc_base+0x33C));
#endif
}

void ddr_irq_handler(int irq_id)
{
	u32 i;

	for (i = 0; i < 50000; i++)
		; //software delay to avoid safety R5 lost IRQ

		//clear interrupt source
#ifdef CONFIG_A55
	if (171 == irq_id) {
		writel(readl(DDRC0_CTRL_PHY_ADDR + 0x1000 + 0x2C) | (1<< 1), 
			DDRC0_CTRL_PHY_ADDR + 0x1000 + 0x2C); //clear 171 irq source, if remove irq will re-enter
		//printf_intr("\r\n ECU irq: %x", readl(0x33000000+0x130)); //A1000 to ECU interrupt
		if (readl(0x33000000 + 0x130) &
		    (1 << 6)) { //ddr to ecu set
			printf("ddr0 ecu irq pass.\r\n");
			//software clear the ddr to ecu irq
			writel(0, 0x33000000 + 0x120);
			writel(1, 0x33000000 + 0x120);
			if (readl(0x33000000 + 0x130) & (1 << 6)) {
				printf("ddrc0 to ecu irq clear fail.\n");
			}
		}
	} else if (173 == irq_id) {
		writel(readl(DDRC1_CTRL_PHY_ADDR + 0x1000 + 0x2C) | (1 << 1), 
				DDRC1_CTRL_PHY_ADDR + 0x1000 + 0x2C);
		//printf_intr("\r\n ECU irq: %x", readl(0x33000000+0x130)); //A1000 to ECU interrupt
		if (readl(0x33000000 + 0x130) &
		    (1 << 6)) { //ddr to ecu set
			printf("ddr1 ecu irq pass.\r\n");
			//software clear the ddr to ecu irq
			writel(0, 0x33000000 + 0x120);
			writel(1, 0x33000000 + 0x120);
			if (readl(0x33000000 + 0x130) & (1 << 6)) {
				printf("ddrc1 to ecu irq clear fail.\n");
			}
		}
	}
#endif
	printf("\nddr irq id:\n%d\n", irq_id);

#ifdef CONFIG_A55
	if ((irq_id != 171) &&
	    ((irq_id !=
	      173))) { //if 171 irq call this API CPU may hang(A55 access DDR, DDR return error)
		A55_IRQ_DISABLE(irq_id);
	}
#endif
}

#ifdef CONFIG_A55
//IRQ_DEFINE(170, ddr_irq_handler, 170, "ddr irq", ISR_ATTR_SAFETY|ISR_ATTR_A55|ISR_ATTR_LEVEL);
//IRQ_DEFINE(171, ddr_irq_handler, 171, "ddr irq", ISR_ATTR_SAFETY|ISR_ATTR_A55|ISR_ATTR_LEVEL);
IRQ_DEFINE(172, ddr_irq_handler, 172, "ddr irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(173, ddr_irq_handler, 173, "ddr irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(174, ddr_irq_handler, 174, "ddr irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(175, ddr_irq_handler, 175, "ddr irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
#elif defined CONFIG_R5_SAFETY
//IRQ_DEFINE(103, ddr_irq_handler, 103, "ddr irq", ISR_ATTR_SAFETY|ISR_ATTR_A55|ISR_ATTR_LEVEL);
//IRQ_DEFINE(104, ddr_irq_handler, 104, "ddr irq", ISR_ATTR_SAFETY|ISR_ATTR_A55|ISR_ATTR_LEVEL);
//IRQ_DEFINE(105, ddr_irq_handler, 105, "ddr irq", ISR_ATTR_SAFETY|ISR_ATTR_A55|ISR_ATTR_LEVEL);
//IRQ_DEFINE(106, ddr_irq_handler, 106, "ddr irq", ISR_ATTR_SAFETY|ISR_ATTR_A55|ISR_ATTR_LEVEL);
//IRQ_DEFINE(107, ddr_irq_handler, 107, "ddr irq", ISR_ATTR_SAFETY|ISR_ATTR_A55|ISR_ATTR_LEVEL);
//IRQ_DEFINE(108, ddr_irq_handler, 108, "ddr irq", ISR_ATTR_SAFETY|ISR_ATTR_A55|ISR_ATTR_LEVEL);
#endif

static void ddr_ctrl_irq_test(void)
{
	void *base_addr;

	base_addr = (void *)DDRC0_CTRL_PHY_ADDR;
	ddrc_ctrl_irq(base_addr);
	base_addr = (void *)DDRC1_CTRL_PHY_ADDR;
	ddrc_ctrl_irq(base_addr);
}

static void ddr_bit_rotation_test(void)
{
	unsigned long start_addr = 0x80000000, end_addr = 0xFFFFFFFF, addr, i,
		      round; //default 2G range
	u32 final_result = 0;
	char cmd;

	while (1) {
		printf("\r\npress 's' to set test range or 'd' to use default test range(0x80000000~0xFFFFFFFF)\r\n");
		cmd = getc();
		if ('s' == cmd) {
			printf("input test start addr(hex)\r\n");
			if (getHex(&start_addr)) {
				return;
			}

			printf("input test end addr(hex)\r\n");
			if (getHex(&end_addr)) {
				return;
			}
			break;
		} else if ('d' == cmd) {
			break;
		} else {
			printf("wrong cmd input\r\n");
		}
	}

	printf("start addr:0x%x, end addr:0x%x\r\n", start_addr,
		end_addr);
	// test body

	// bit[round] = 0, all bit[round]=0 is same so just do once
	addr = start_addr;
	for (i = 0; i < 32; i++) {
		printf("addr(0x%x) val(0x%x):", addr, (1 << i));
		writel((1 << i), addr);
		printf("%s\r\n",
			readl(addr) == (1 << i) ? "pass" : "fail");
		if (readl(addr) != (1 << i)) {
			final_result = 1;
		}
	}

	// bit[round] = 1
	for (addr = start_addr, round = 2; addr <= 0xFFFFFFFF;) {
		addr |= (1 << round);
		if ((addr > 0xFFFFFFFF) || (0 == addr)) {
			break;
		}
		for (i = 0; i < 32; i++) {
			printf("addr(0x%x) val(0x%x):", addr, (1 << i));
			writel((1 << i), addr);
			printf("%s\r\n",
				readl(addr) == (1 << i) ? "pass" : "fail");
			if (readl(addr) != (1 << i)) {
				final_result = 1;
				printf("addr(0x%x) wval(0x%x) rval(0x%x)\r\n",
					addr, (1 << i), readl(addr));
			}
		}
		addr &= (~(1 << round)); //clear bit[round] to 0

		round++;
	}

	if (final_result) {
		printf("%s fail.\r\n", __func__);
	} else {
		printf("%s pass.\r\n", __func__);
	}
}

void ddrc_func_irq(void *base_addr)
{
	void *ddrc_base = base_addr + 0x1000;
	u32 data, i;

	printf("\r\nfunc ddrc_base: %x\r\n", ddrc_base);

	//release sbr reset
	data = readl(base_addr + 0x0); //do not use ddrc_base
	data |= (1 << 16);
	writel(data, base_addr + 0x0);
	for (i = 0; i < 1000; i++)
		; //software delay

	// cfg ecccfg0
	writel(0, ddrc_base + 0x320); //SWCTL
	data = readl(ddrc_base + 0x70); //ECCCFG0
	data = (data & (~(3 << 30))) |
	       (3 << 30); //ecc_region_map_granu,1/64 of memory spaces
	data = (data & (~(1 << 29))) |
	       (0 << 29); //ecc_region_map_other,Non-Protected
	data = (data & (~(0X7F << 8))) | (1 << 8); //ecc_region_map
	writel(data, ddrc_base + 0x70); //ECCCFG0
	writel(1, ddrc_base + 0x320); //SWCTL
	while (readl(ddrc_base + 0x324) != 0x1)
		; //SWSTAT

	// cfg SBRSTART0 and SBRSTART0
	writel(0x80000000, ddrc_base + 0xF38); //SBRSTART0, do not overlap text section, bss section, ...
	writel(0x80000100, ddrc_base + 0xF40); //SBRRANGE0

	//< 1) ecc parity region lock
	writel(0, ddrc_base + 0x320); //SWCTL
	data = readl(ddrc_base + 0x74); //ECCCFG1
	data |= (1 << 4);
	writel(data, ddrc_base + 0x74); //ECCCFG1
	writel(1, ddrc_base + 0x320); //SWCTL
	while (readl(ddrc_base + 0x324) != 0x1) {
	}; //SWSTAT

#if 0 //if enable will hang, this step is not must(port2 and port3 is not exist)
	//< 2) port disable
    data = readl(ddrc_base+0x490);    //PCTRL_0
    data =  (data&(~(1<<0))) | (0<<0);
    REG32_WRITE(ddrc_base+0x490, data);   //PCTRL_0

    data = readl(ddrc_base+0x540);     //PCTRL_1
    data =  (data&(~(1<<0))) | (0<<0);
    REG32_WRITE(ddrc_base+0x540, data);   //PCTRL_1

    data = readl(ddrc_base+0x5F0);     //PCTRL_2
    data =  (data&(~(1<<0))) | (0<<0);
    REG32_WRITE(ddrc_base+0x5F0, data);   //PCTRL_2

	data = readl(ddrc_base+0x6A0);     //PCTRL_3
    data =  (data&(~(1<<0))) | (0<<0);
    REG32_WRITE(ddrc_base+0x6A0, data);   //PCTRL_3
#endif

	//< 3) disable ocecc
	//< do it when UMCTL2_OCECC_EN=1

	//< 4), 5) cfg scrub mode, cfg scrub interval
	data = readl(ddrc_base + 0xF24); //SBRCTL
	data |= (1 << 2);
	data = (data & (~(0xFFFFFF << 8))) | (0 << 8);
	writel(data, ddrc_base + 0xF24); //SBRCTL

	//< 6) set desired pattern
	writel(0xFFFF1111, ddrc_base + 0xF2C); //SBRWDATA0

	//< 7) enable scrub
	data = readl(ddrc_base + 0xF24); //SBRCTL
	data |= (1 << 0);
	data = writel(data, ddrc_base + 0xF24); //SBRCTL

	//< 8) wait scrub write cmd sent
	while (!(readl(ddrc_base + 0xF28) & (1 << 1)))
		; //SBRSTAT

	//< 9) wait scrub write data sent
	while ((readl(ddrc_base + 0xF28) & (1 << 0)))
		; //SBRSTAT

	//< 10) disable scrub
	data = readl(ddrc_base + 0xF24); //SBRCTL
	data = data & (~(1 << 0));
	data = writel(data, ddrc_base + 0xF24); //SBRCTL

	//< 11) enable ocecc
	//< do it when UMCTL2_OCECC_EN=1

	//< 12) cfg scrub mode, cfg scrub interval
	data = readl(ddrc_base + 0xF24); //SBRCTL
	data = (data & (~(1 << 2))) | (0 << 2);
	data = (data & (~(0xFFFFFF << 8))) | (2 << 8);
	data = writel(data, ddrc_base + 0xF24); //SBRCTL

	//< 13) enabe scrub
	data = readl(ddrc_base + 0xF24); //SBRCTL
	data = (data & (~(1 << 0))) | (1 << 0);
	data = writel(data, ddrc_base + 0xF24); //SBRCTL

#if 0 //if enable will hang
    //< 14) port enable
    data = readl(ddrc_base+0x490);    //PCTRL_0
    data =  (data&(~(1<<0))) | (1<<0);
    REG32_WRITE(ddrc_base+0x490, data);   //PCTRL_0

    data = readl(ddrc_base+0x540);     //PCTRL_1
    data =  (data&(~(1<<0))) | (1<<0);
    REG32_WRITE(ddrc_base+0x540, data);   //PCTRL_1

    data = readl(ddrc_base+0x5F0);     //PCTRL_2
    data =  (data&(~(1<<0))) | (1<<0);
    REG32_WRITE(ddrc_base+0x5F0, data);   //PCTRL_2

	data = readl(ddrc_base+0x6A0);     //PCTRL_3
    data =  (data&(~(1<<0))) | (1<<0);
    REG32_WRITE(ddrc_base+0x6A0, data);   //PCTRL_3
#endif
}

static void ddr_func_irq_test(void)
{
	void *base_addr;

	base_addr = (void *)DDRC0_CTRL_PHY_ADDR;
	ddrc_func_irq(base_addr);
	base_addr = (void *)DDRC1_CTRL_PHY_ADDR;
	ddrc_func_irq(base_addr);
}

static void ddr_large_addr_access(void)
{
	printf("sizeof(ddrMemStartAddr)= %d, sizeof(ddrMemEndAddr) = %d\r\n",
		sizeof(ddrMemStartAddr), sizeof(ddrMemEndAddr));
	//ddrMemStartAddr = 0x80000000 + 2*1024*1024*1024;
	ddrMemStartAddr = 0x10000000;
	ddrMemEndAddr = ddrMemStartAddr + 0x40000;
	printf("ddrMemStartAddr= 0x%lx, sizeof(ddrMemEndAddr) = 0x%lx\r\n",
		ddrMemStartAddr, ddrMemEndAddr);
	printf("ddrMemStartAddr= 0x%lx, sizeof(ddrMemEndAddr) = 0x%lx\r\n",
		ddrMemStartAddr >> 31, ddrMemEndAddr & 0xFFFFFFFF);

	printf("data = %x\r\n", readl(ddrMemStartAddr));
	ddr_ReadWrite();
}

#ifndef CONFIG_A55
static void ddr_global_reset_test(unsigned int base_addr)
{
	unsigned int ddr_local_base_addr = base_addr;
	unsigned int ddrc_base_addr = base_addr + 0x1000;
	u32 reset_bit = 0, reg_before, reg_after, r_data, w_data, i;

	if (DDRC0_CTRL_PHY_ADDR == base_addr) {
		reset_bit = 5;
	} else if (DDRC1_CTRL_PHY_ADDR == base_addr) {
		reset_bit = 4;
	}

	//keep this step otherwise R/W TOP_CRM_BLOCK_SW_RST0 will hang
	printf("release top crm\r\n");
	printf("SAFETY_RELEASE_CTRL:0x%x\r\n", SAFETY_RELEASE_CTRL);
	//REG32_WRITE(SAFETY_RELEASE_CTRL, readl(SAFETY_RELEASE_CTRL)|(2));
	//while(!(readl(SAFETY_RELEASE_CTRL)&(1<<31)));
	writel(readl(SAFETY_RELEASE_CTRL) | (1 << 30), SAFETY_RELEASE_CTRL); //release top_crm
	while (!(readl(SAFETY_RELEASE_CTRL) & (0xc0)))
		;

	//ddr_init will config this register to non-default value
	reg_before = readl(ddrc_base_addr + 0x0020); //DDRC.DERATEEN
	printf("**********reg_before : 0x%x\r\n", reg_before);

	printf("TOP_CRM_BLOCK_SW_RST0 : 0x%x\r\n",
		readl(TOP_CRM_BLOCK_SW_RST0));
	writel(readl(TOP_CRM_BLOCK_SW_RST0) ^ (1 << reset_bit), TOP_CRM_BLOCK_SW_RST0);
	for (i = 0; i < 5000; i++)
		; //software delay
	printf("delay done!\r\n");
	writel(readl(TOP_CRM_BLOCK_SW_RST0) ^ (1 << reset_bit), TOP_CRM_BLOCK_SW_RST0);
	for (i = 0; i < 5000; i++)
		; //software delay
	printf("delay done!\r\n");
	printf("TOP_CRM_BLOCK_SW_RST0 : 0x%x\r\n",
		readl(TOP_CRM_BLOCK_SW_RST0));

	//for(i = 0; i < 5000; i++); //software delay
	printf("delay done!\r\n");

	//after reset must reconfig and then can access DDRC register
	//so we check the register after reconfig
	if (base_addr == DDRC0_CTRL_PHY_ADDR) {
		r_data = readl(TOP_CRM_BASE_ADDR + 0x4);
		r_data |= (0x1 << 5);
		//r_data &= ~(0x1<<5);
		//r_data |= (0x1<<14);
		writel(r_data, TOP_CRM_BASE_ADDR + 0x4);
		printf("config ddr0\n");
	} else if (base_addr == DDRC1_CTRL_PHY_ADDR) {
		r_data = readl(TOP_CRM_BASE_ADDR + 0x4);
		r_data |= (0x1 << 4);
		//r_data &= ~(0x1<<4);
		//r_data |= (0x1<<13);
		writel(r_data, TOP_CRM_BASE_ADDR + 0x4);
		printf("config ddr1\n");
	}

	//bst_realtime.delay_ns(100);
	r_data = readl(ddr_local_base_addr + 0x0); //disabe
	printf("config pwokin deassert\n");
	for (i = 0; i < 1000; i++)
		; //software delay
	r_data |= (0x30 << 10);
	r_data &= ~(0xF << 10);
	writel(r_data, ddr_local_base_addr + 0x0); //DDR_LOCAL.CLOCK_AND_RESET
	for (i = 0; i < 1000; i++)
		; //software delay
	r_data = readl(ddr_local_base_addr + 0x0);
	w_data = r_data | (1 << 9); //pwokin deassert
	writel(w_data, ddr_local_base_addr + 0x0);

	reg_after = readl(ddrc_base_addr + 0x0020); //DDRC.DERATEEN
	printf("**********reg_after : 0x%x\r\n", reg_after);

	if ((reg_after == reg_before) ||
	    (reg_after != 0x0)) { //DERATEEN default value is 0x0
		printf("reg_after equal to reg_before: global reset test fail.\r\n");
	} else {
		printf("global reset test pass.\r\n");
	}
}
#endif

#if 0
static void ddr_internal_reset_test(unsigned int base_addr)
{
	unsigned int ddr_local_base_addr = base_addr;
    unsigned int ddrc_base_addr      = base_addr+0x1000;
	u32 reg_before, reg_after, i;

#if 0
	if(DDRC0_CTRL_PHY_ADDR == base_addr) {
		reset_bit = 5;
	} else if(DDRC1_CTRL_PHY_ADDR == base_addr) {
		reset_bit = 4;
	}
#endif

	printf("release top crm\r\n");
	printf("SAFETY_RELEASE_CTRL:0x%x\r\n", SAFETY_RELEASE_CTRL);
	//REG32_WRITE(SAFETY_RELEASE_CTRL, readl(SAFETY_RELEASE_CTRL)|(2));
	//while(!(readl(SAFETY_RELEASE_CTRL)&(1<<31)));
	REG32_WRITE(SAFETY_RELEASE_CTRL, readl(SAFETY_RELEASE_CTRL)|(1<<30)); //release top_crm
	while(!(readl(SAFETY_RELEASE_CTRL)&(0xc0)));

	//ddr_init will config this register to non-default value
	reg_before = readl(ddrc_base_addr+0x0020);      //DDRC.DERATEEN
	printf("**********reg_before : 0x%x\r\n", reg_before);

	//release global sw_rstn
	if(DDRC0_CTRL_PHY_ADDR == base_addr) {
		REG32_WRITE(TOP_CRM_BLOCK_SW_RST0, readl(TOP_CRM_BLOCK_SW_RST0)|(1<<5));
	} else if(DDRC1_CTRL_PHY_ADDR == base_addr) {
		REG32_WRITE(TOP_CRM_BLOCK_SW_RST0, readl(TOP_CRM_BLOCK_SW_RST0)|(1<<4));
	}
	for(i = 0; i < 5000; i++); //software delay

	//release umctl prstn
	REG32_WRITE(ddr_local_base_addr+0x0, readl(ddr_local_base_addr+0x0)|(1<<14));
	for(i = 0; i < 5000; i++); //software delay

	//release umctl aresetn
	REG32_WRITE(ddr_local_base_addr+0x0, readl(ddr_local_base_addr+0x0)|(0xF<<10));
	for(i = 0; i < 5000; i++); //software delay
	
	//release phy prstn
	REG32_WRITE(ddr_local_base_addr+0x0, readl(ddr_local_base_addr+0x0)|(1<<15));
	for(i = 0; i < 5000; i++); //software delay

	//release umctl and phy work rstn
	REG32_WRITE(ddr_local_base_addr+0x0, readl(ddr_local_base_addr+0x0)|(1<<8));
	for(i = 0; i < 5000; i++); //software delay

	//release umctl sbr rstn
	REG32_WRITE(ddr_local_base_addr+0x0, readl(ddr_local_base_addr+0x0)|(1<<16));
	for(i = 0; i < 5000; i++); //software delay

	reg_after = readl(ddrc_base_addr+0x0020);      //DDRC.DERATEEN
	printf("**********reg_after : 0x%x\r\n", reg_after);
	
	if((reg_after == reg_before) || (reg_after != 0x0)) { //DERATEEN default value is 0x0
		printf("reg_after equal to reg_before: global reset test fail.\r\n");
	} else {
		printf("global reset test pass.\r\n");
	}
}
#endif

#ifndef CONFIG_A55
static void ddr_internal_reset_test(unsigned int base_addr)
{
	unsigned int ddr_local_base_addr = base_addr;
	unsigned int ddrc_base_addr = base_addr + 0x1000;
	u32 reg_before, reg_after, i;

	//ddr_init will config this register to non-default value
	reg_before = readl(ddrc_base_addr + 0x0020); //DDRC.DERATEEN
	printf("**********reg_before : 0x%x\r\n", reg_before);

	//reset umctl prstn
	writel(readl(ddr_local_base_addr + 0x0) & (~(1 << 14)), ddr_local_base_addr + 0x0);
	for (i = 0; i < 5000; i++)
		; //software delay

	//release umctl prstn
	writel(readl(ddr_local_base_addr + 0x0) | (1 << 14), ddr_local_base_addr + 0x0);
	for (i = 0; i < 5000; i++)
		; //software delay

	reg_after = readl(ddrc_base_addr + 0x0020); //DDRC.DERATEEN
	printf("**********reg_after : 0x%x\r\n", reg_after);

	if ((reg_after == reg_before) ||
	    (reg_after != 0x0)) { //DERATEEN default value is 0x0
		printf("reg_after equal to reg_before: global reset test fail.\r\n");
	} else {
		printf("internal reset test pass.\r\n");
	}
}
#endif

static void ddr_global_rst_test(void)
{
#ifdef CONFIG_A55
	printf("A55 cannot use to test ddr reset function!(A55 run on DDR)");
	printf("Please change to use Secure/Safety CPU run this case!");
#else

	printf("ddr0 global reset test:\r\n");
	ddr0Init();
	ddr_global_reset_test(DDRC0_CTRL_PHY_ADDR);
	printf("ddr1 global reset test:\r\n");
	ddr1Init();
	ddr_global_reset_test(DDRC1_CTRL_PHY_ADDR);

#endif
}

static void ddr_internal_rst_test(void)
{
#ifdef CONFIG_A55
	printf("A55 cannot use to test ddr reset function!(A55 run on DDR)");
	printf("Please change to use Secure/Safety CPU run this case!");
#else

	printf("ddr0 internal reset test:\r\n");
	ddr0Init();
	ddr_internal_reset_test(DDRC0_CTRL_PHY_ADDR);
	printf("ddr1 internal reset test:\r\n");
	ddr1Init();
	ddr_internal_reset_test(DDRC1_CTRL_PHY_ADDR);

#endif
}

#ifndef CONFIG_APP_BOOT
char src[64*1024] __attribute__((aligned(16)));
char dst[64*1024] __attribute__((aligned(16)));
char src_cpu[8192] __attribute__((aligned(16)));
char dst_cpu[8192] __attribute__((aligned(16)));
#else
char *src = (char *)0x80000000;
char *dst = (char *)0x80400000;
char *src_cpu = (char *)0x80800000;
char *dst_cpu = (char *)0x80c00000;
#endif
static u32 count = 0;

static void mem_fill(u8 *addr, u32 len)
{
	u32 i;

	for (i = 0; i < len; i++) {
		*(addr + i) = (u8)(i & (0xFF));
	}
	*(addr + len - 1) = 0x55; //avoid last one is 0
}

static void diff_len_8_bit_test(void)
{
	u32 reg_data, data, channel = 1;
	void *reg_base = (void *)GDMA_BASE_ADDR;
	u32 i;

	//common settings
	/* DMAC_CFGREG */
	writel(readl(reg_base + DMAC_CFGREG) & (~(0x1)), reg_base + DMAC_CFGREG); //disable dma
	for (i = 0; i < 1000; i++)
		;
	writel(readl(reg_base + DMAC_CFGREG) | 0x1, reg_base + DMAC_CFGREG); //enable dma

	/* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE, write only bit
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	writel(data, reg_base + DMAC_CHENREG);

	/* CHX_CFG */
	data = readl(reg_base + DMAC_CHX_CFG(1));
	data = (data & (~(0x3 << 0))) |
	       (0x0
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x0
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	writel(data, reg_base + DMAC_CHX_CFG(1));

	data = readl(reg_base + DMAC_CHX_CFG(1) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x0
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x1
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	data = (data & (~(0xF << 23))) |
	       (0xF << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0xF << 27); //DST_OSR_LMT,destination outstanding request limit
	writel(data, reg_base + DMAC_CHX_CFG(1) + 0x4);

	/* CHX_SAR */
	writel((size_t)src, reg_base + DMAC_CHX_SAR(1)); //source addr
	writel(0, reg_base + DMAC_CHX_SAR(1) + 0x4);

	/* CHX_DAR */
	writel((size_t)dst, reg_base + DMAC_CHX_DAR(1)); //destination addr
	writel(0, reg_base + DMAC_CHX_DAR(1) + 0x4);

	/* CHX_CTRL */
	data = readl(reg_base + DMAC_CHX_CTL(1));
	data = (data & (~(0x1 << 4))) |
	       (0x0
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x0
		<< 6); //destination address increment: 0(increment), 1(no change)
	writel(data, reg_base + DMAC_CHX_CTL(1));

	data = readl(reg_base + DMAC_CHX_CTL(1) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	writel(data, reg_base + DMAC_CHX_CTL(1) + 0x4);
	pr_debug("---%s %d\n", __func__, __LINE__);
	//special settings & case test
	//8 bit data transfer
	mem_fill(src, 1 + 1); //1 byte more for check
	memset(dst, 0, 1 + 1);

	writel(0, reg_base + DMAC_CHX_BLOCK_TS(channel)); /* transfer block number */
	pr_debug("---%s %d\n", __func__, __LINE__);
	reg_data = readl(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x7 << 8))) |
		   (0x0 << 8); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x0 << 11); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 0:1 item, 1:4 item, 2:8 item */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* src msize 0:1 item, 1:4 item, 2:8 item */
	writel(reg_data, reg_base + DMAC_CHX_CTL(channel));

	reg_data = readl(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7); /* src burst len */
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	reg_data =
		(reg_data & (~(0xff << 16))) | (0x1 << 16); /* dst burst len */
	writel(reg_data, reg_base + DMAC_CHX_CTL(channel) + 0X4);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x1 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	writel(data, reg_base + DMAC_CHENREG);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* DMA CHX_INTSTATUS */
	while ((readl(reg_base + DMAC_CHX_INTSTATUS(1)) & 0x11) == 0)
		; //wait block transfer done interrupt
	pr_debug("---%s %d\n", __func__, __LINE__);
	if ((src[0] == dst[0]) && (src[1] != dst[1])) {
		//printf("8-bit pass\n");
	} else {
		printf("8-bit fail\n");
	}
}

static void diff_len_16_bit_test(void)
{
	u32 reg_data, data, channel = 1;
	void *reg_base = (void *)GDMA_BASE_ADDR;
	u32 i;

	//common settings
	/* DMAC_CFGREG */
	writel(readl(reg_base + DMAC_CFGREG) & (~(0x1)), reg_base + DMAC_CFGREG); //disable dma
	for (i = 0; i < 1000; i++)
		;
	pr_debug("---%s %d\n", __func__, __LINE__);
	writel(readl(reg_base + DMAC_CFGREG) | 0x1, reg_base + DMAC_CFGREG); //enable dma

	/* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE, write only bit
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	writel(data, reg_base + DMAC_CHENREG);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* CHX_CFG */
	data = readl(reg_base + DMAC_CHX_CFG(1));
	data = (data & (~(0x3 << 0))) |
	       (0x0
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x0
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	writel(data, reg_base + DMAC_CHX_CFG(1));
	pr_debug("---%s %d\n", __func__, __LINE__);
	data = readl(reg_base + DMAC_CHX_CFG(1) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x0
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x1
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	data = (data & (~(0xF << 23))) |
	       (0xF << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0xF << 27); //DST_OSR_LMT,destination outstanding request limit
	writel(data, reg_base + DMAC_CHX_CFG(1) + 0x4);

	/* CHX_SAR */
	writel((size_t)src, reg_base + DMAC_CHX_SAR(1)); //source addr
	writel(0, reg_base + DMAC_CHX_SAR(1) + 0x4);

	/* CHX_DAR */
	writel((size_t)dst, reg_base + DMAC_CHX_DAR(1)); //destination addr
	writel(0, reg_base + DMAC_CHX_DAR(1) + 0x4);

	/* CHX_CTRL */
	data = readl(reg_base + DMAC_CHX_CTL(1));
	data = (data & (~(0x1 << 4))) |
	       (0x0
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x0
		<< 6); //destination address increment: 0(increment), 1(no change)
	writel(data, reg_base + DMAC_CHX_CTL(1));

	data = readl(reg_base + DMAC_CHX_CTL(1) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	writel(data, reg_base + DMAC_CHX_CTL(1) + 0x4);
	pr_debug("---%s %d\n", __func__, __LINE__);
	//special settings & case test
	//16 bit data transfer
	mem_fill(src, 2 + 1); //1 byte more for check
	memset(dst, 0, 2 + 1);

	writel(0, reg_base + DMAC_CHX_BLOCK_TS(channel)); /* transfer block number */

	reg_data = readl(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x7 << 8))) |
		   (0x1 << 8); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x1 << 11); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 0:1 item, 1:4 item, 2:8 item */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* src msize 0:1 item, 1:4 item, 2:8 item */
	writel(reg_data, reg_base + DMAC_CHX_CTL(channel));
	pr_debug("---%s %d\n", __func__, __LINE__);
	reg_data = readl(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	reg_data = (reg_data & (~(0xff << 7))) | (0x2 << 7); /* src burst len */
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	reg_data =
		(reg_data & (~(0xff << 16))) | (0x2 << 16); /* dst burst len */
	writel(reg_data, reg_base + DMAC_CHX_CTL(channel) + 0x4);

	/* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x1 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	writel(data, reg_base + DMAC_CHENREG);
	/* DMA CHX_INTSTATUS */
	while ((readl(reg_base + DMAC_CHX_INTSTATUS(1)) & 0x11) == 0)
		; //wait block transfer done interrupt
	pr_debug("---%s %d\n", __func__, __LINE__);
	if ((src[0] == dst[0]) && (src[1] == dst[1]) && (src[2] != dst[2])) {
		//printf("16-bit pass\n");
	} else {
		printf("16-bit fail\n");
	}
}

static void inc_burst_len(u32 blk_num, u32 max_burst, u32 msize, u32 width)
{
	u32 reg_data, data, channel = 1;
	void *reg_base = (void *)GDMA_BASE_ADDR;
	u32 i, len = blk_num, result, real_len = 0;

	writel(readl(reg_base + DMAC_CFGREG) & (~(0x1)), reg_base + DMAC_CFGREG); //disable dma
	switch (width) {
	case 0: //8-bit
		real_len = 1;
		break;
	case 1: //16-bit
		real_len = 2;
		break;
	case 2: //32-bit
		real_len = 4;
		break;
	case 3: //64-bit
		real_len = 8;
		break;
	case 4: //128-bit
		real_len = 16;
		break;
	default:
		real_len = 0;
		printf("Error blk_width:%d.\r\n", width);
		break;
	}
	real_len *= blk_num;
	mem_fill(src, real_len + 1); //1 byte more for check
	memset(dst, 0, real_len + 1);

	/* DMAC_CFGREG */
	for (i = 0; i < 1000; i++)
		;

	for (i = 0; i < 8192; i++)
		src_cpu[i] = count++ & 0xff;

	writel(readl(reg_base + DMAC_CFGREG) | 0x1, reg_base + DMAC_CFGREG); //enable dma

	/* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE, write only bit
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	writel(data, reg_base + DMAC_CHENREG);

	/* CHX_CFG */
	data = readl(reg_base + DMAC_CHX_CFG(1));
	data = (data & (~(0x3 << 0))) |
	       (0x0
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x0
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	writel(data, reg_base + DMAC_CHX_CFG(1));

	data = readl(reg_base + DMAC_CHX_CFG(1) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x0
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x1
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	data = (data & (~(0xF << 23))) |
	       (0xF << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0xF << 27); //DST_OSR_LMT,destination outstanding request limit
	writel(data, reg_base + DMAC_CHX_CFG(1) + 0x4);

	/* CHX_SAR */
	writel((size_t)src, reg_base + DMAC_CHX_SAR(1)); //source addr
	writel(0, reg_base + DMAC_CHX_SAR(1) + 0x4);

	/* CHX_DAR */
	writel((size_t)dst, reg_base + DMAC_CHX_DAR(1)); //destination addr
	writel(0, reg_base + DMAC_CHX_DAR(1) + 0x4);

	/* CHX_CTRL */
	data = readl(reg_base + DMAC_CHX_CTL(1));
	data = (data & (~(0x1 << 4))) |
	       (0x0
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x0
		<< 6); //destination address increment: 0(increment), 1(no change)
	writel(data, reg_base + DMAC_CHX_CTL(1));

	data = readl(reg_base + DMAC_CHX_CTL(1) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	writel(data, reg_base + DMAC_CHX_CTL(1) + 0x4);

	//special settings & case test

	writel(len - 1, reg_base + DMAC_CHX_BLOCK_TS(channel)); /* transfer block number */

	reg_data = readl(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x7 << 8))) |
		   (width << 8); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (width << 11); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
	reg_data = (reg_data & (~(0xf << 14))) | (msize << 14);
		/* src msize 0:1 item, 1:4 item, 2:8 item */ //max burst transaction size
	reg_data = (reg_data & (~(0xf << 18))) |
		   (msize << 18); /* dst msize 0:1 item, 1:4 item, 2:8 item */
	writel(reg_data, reg_base + DMAC_CHX_CTL(channel));

	reg_data = readl(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	reg_data = (reg_data & (~(0xff << 7))) |
		   (max_burst << 7); /* src burst len */
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	reg_data = (reg_data & (~(0xff << 16))) |
		   (max_burst << 16); /* dst burst len */
	writel(reg_data, reg_base + DMAC_CHX_CTL(channel) + 0x4);

	/* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x1 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	writel(data, reg_base + DMAC_CHENREG);
	/* DMA CHX_INTSTATUS */
	i = 0;
	while ((readl(reg_base + DMAC_CHX_INTSTATUS(1)) & 0x11) == 0) {
		dst_cpu[count & 8192] = src_cpu[count & 8192];
		count++;
		i++;
	}
	printf("---dst_cpu tranfer %d\n", i);
	 //wait block transfer done interrupt

	for (i = 0; i < real_len; i++) {
		HCOMP(src[i], dst[i]);
	}
	if (src[real_len] == dst[real_len]) {
		printf("diff_len_inc_burst fail\n");
	} else {
		//printf("diff_len_inc_burst pass\n");
	}
}

static void fix_burst_len(u32 blk_num, u32 blk_width, u32 max_burst)
{
	u32 reg_data, data, channel = 1;
	void *reg_base = (void *)GDMA_BASE_ADDR;
	u32 i, len = blk_num, result, real_len;

	//common settings
	writel(readl(reg_base + DMAC_CFGREG) & (~(0x1)), reg_base + DMAC_CFGREG); //disable dma
	switch (blk_width) {
	case 0: //8-bit
	pr_debug("---%s %d\n", __func__, __LINE__);
		real_len = 1;
		break;
	case 1: //16-bit
	pr_debug("---%s %d\n", __func__, __LINE__);
		real_len = 2;
		break;
	case 2: //32-bit
	pr_debug("---%s %d\n", __func__, __LINE__);
		real_len = 4;
		break;
	case 3: //64-bit
	pr_debug("---%s %d\n", __func__, __LINE__);
		real_len = 8;
		break;
	case 4: //128-bit
	pr_debug("---%s %d\n", __func__, __LINE__);
		real_len = 16;
		break;
	default:
		real_len = 0;
		printf("Error blk_width:%d.\r\n", blk_width);
		break;
	}
	mem_fill(src, real_len + 1);
	memset(dst, 0, real_len + 1);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* DMAC_CFGREG */
	for (i = 0; i < 1000; i++)
		;
	writel(readl(reg_base + DMAC_CFGREG) | 0x1, reg_base + DMAC_CFGREG); //enable dma
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE, write only bit
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	writel(data, reg_base + DMAC_CHENREG);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* CHX_CFG */
	data = readl(reg_base + DMAC_CHX_CFG(1));
	data = (data & (~(0x3 << 0))) |
	       (0x0
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x0
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	writel(data, reg_base + DMAC_CHX_CFG(1));
	pr_debug("---%s %d\n", __func__, __LINE__);
	data = readl(reg_base + DMAC_CHX_CFG(1) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x0
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x1
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	data = (data & (~(0xF << 23))) |
	       (0xF << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0xF << 27); //DST_OSR_LMT,destination outstanding request limit
	writel(data, reg_base + DMAC_CHX_CFG(1) + 0x4);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* CHX_SAR */
	writel((size_t)src, reg_base + DMAC_CHX_SAR(1)); //source addr
	writel(0, reg_base + DMAC_CHX_SAR(1) + 0x4);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* CHX_DAR */
	writel((size_t)dst, reg_base + DMAC_CHX_DAR(1)); //destination addr
	writel(0, reg_base + DMAC_CHX_DAR(1) + 0x4);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* CHX_CTRL */
	data = readl(reg_base + DMAC_CHX_CTL(1));
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x1
		<< 6); //destination address increment: 0(increment), 1(no change)
	writel(data, reg_base + DMAC_CHX_CTL(1));
	pr_debug("---%s %d\n", __func__, __LINE__);
	data = readl(reg_base + DMAC_CHX_CTL(1) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	writel(data, reg_base + DMAC_CHX_CTL(1) + 0x4);
	pr_debug("---%s %d\n", __func__, __LINE__);
	//special settings & case test

	writel(len - 1, reg_base + DMAC_CHX_BLOCK_TS(channel)); /* transfer block number */
	pr_debug("---%s %d\n", __func__, __LINE__);
	reg_data = readl(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x7 << 8))) |
		   (blk_width << 8); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
	reg_data =
		(reg_data & (~(0x7 << 11))) |
		(blk_width << 11); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
	reg_data = (reg_data & (~(0xf << 14))) | (0x0 << 14);
		/* src msize 0:1 item, 1:4 item, 2:8 item */ //max burst transaction size
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 0:1 item, 1:4 item, 2:8 item */
	writel(reg_data, reg_base + DMAC_CHX_CTL(channel));
	pr_debug("---%s %d\n", __func__, __LINE__);
	reg_data = readl(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	reg_data = (reg_data & (~(0xff << 7))) |
		   (max_burst << 7); /* src burst len */
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	reg_data = (reg_data & (~(0xff << 16))) |
		   (max_burst << 16); /* dst burst len */
	writel(reg_data, reg_base + DMAC_CHX_CTL(channel) + 0x4);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x1 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	writel(data, reg_base + DMAC_CHENREG);
	pr_debug("---%s %d\n", __func__, __LINE__);
	/* DMA CHX_INTSTATUS */
	while ((readl(reg_base + DMAC_CHX_INTSTATUS(1)) & 0x11) == 0) {
	}
	//wait block transfer done interrupt
	pr_debug("---%s %d\n", __func__, __LINE__);
	for (i = 0; i < real_len; i++) {
		HCOMP(src[i], dst[i]);
	}
	pr_debug("---%s %d\n", __func__, __LINE__);
	if (src[real_len] == dst[real_len]) {
		printf("diff_len_fix_burst fail\n");
	} else {
		//printf("diff_len_fix_burst pass\n");
	}
}

static void diff_inc_burst_size(void)
{
	u32 i;

	for (i = 1; i <= 16; i++) {
		//printf("1:%d\n", i);
		inc_burst_len(i + 1, i, 1, 0);
	}
}

static void diff_fix_burst_size(void)
{
	u32 i;

	for (i = 1; i <= 16; i++) {
		//printf("2:%d\n", i);
		fix_burst_len(2, 4, i);
	}
}

static void page_span_access(void)
{
    char *ptr = src;
	u32 result;
    *ptr = 0xAA;
    *(ptr+PAGE_SPAN_OFFSET) = 0x55;

    printf("page span addr1(0x%x) val(0x%x)\n", ptr, *ptr);
    printf("page span addr2(0x%x) val(0x%x)\n", ptr+PAGE_SPAN_OFFSET, *(ptr+PAGE_SPAN_OFFSET));

    HCOMP(*ptr, 0xAA);
    HCOMP(*(ptr+PAGE_SPAN_OFFSET), 0x55);
}

static void dma_multi_chn_multi_size(void)
{
    u32 *ptr1 = (u32 *)0x80000000;
    u32 *ptr2 = (u32 *)0xA0000000;
    u32 *ptr3 = (u32 *)0xC0000000;
    u32 *ptr4 = (u32 *)0xE0000000;
	u32 *ptr, *ptrdst;
    void *reg_base = (void *)GDMA_BASE_ADDR;
    u32 reg_data, data, i, j, k, channel_num = 8, xfer_cnt;
	u32 result;

    pr_debug("---%s %d\n", __func__, __LINE__);
	ptr = ptr1;
    while((size_t)ptr <= (size_t)(0x9FFFFFFC)) { //0xFFFFFFFF may overflow
        *ptr = ((size_t)ptr>>2)&0xFFFFFFFF;
        ptr ++;
    }

    pr_debug("---%s %d\n", __func__, __LINE__);
    
	ptr = ptr4;
    while((size_t)ptr <= (size_t)(0xFFFFFFFC)) {
        *ptr = ((size_t)ptr>>2)&0xFFFFFFFF;
        ptr ++;
    }
    pr_debug("---%s %d\n", __func__, __LINE__);

    for(i = 0; i < 5; i++) { //DMA copy 5 times
        gdma_reset();
        writel(readl(reg_base + DMAC_CFGREG) & (~(0x1)), reg_base + DMAC_CFGREG); //disable dma
        for (k = 0; k < 1000; k++){}
        writel(readl(reg_base + DMAC_CFGREG) | 0x1, reg_base + DMAC_CFGREG); //enable dma
        for (k = 0; k < 1000; k++){}
        pr_debug("---%s %d, copy times:%d\n", __func__, __LINE__, (i+1));

        /* DMA_ChEnReg:select available channel */
    	data = readl(reg_base + DMAC_CHENREG);
    	data = (data & (~(0xFF << 8))) | (0xFF << 8); //CH1_EN_WE
    	data = (data & (~(0xFF << 0))) | (0x0 << 0); //CH1_EN-8 disable
    	writel(data, reg_base + DMAC_CHENREG);

        //fix parameters set
        for(j = 1; j <= channel_num; j++) {
            data = readl(reg_base + DMAC_CHX_CFG(j));
            data = (data & (~(0x3 << 0))) | (0x0 << 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
            data = (data & (~(0x3 << 2))) | (0x0 << 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
            writel(data, reg_base + DMAC_CHX_CFG(j));

            data = readl(reg_base + DMAC_CHX_CFG(j) + 0x4); //high 32 bit
            data = (data & (~(0x7 << 0))) | (0x0 << 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
            data = (data & (~(0x1 << 3))) | (0x1 << 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
            data = (data & (~(0x1 << 4))) | (0x1 << 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
            data = (data & (~(0xF << 23))) | (0xF << 23); //SRC_OSR_LMT,source outstanding request limit
            data = (data & (~(0xF << 27))) | (0xF << 27); //DST_OSR_LMT,destination outstanding request limit
            writel(data, reg_base + DMAC_CHX_CFG(j) + 0x4);

            #if 0
            if(j <= 4) { //ptr1 -> ptr3
                /* CHX_SAR */
                writel((size_t)(0x80000000+(j-1)*64*(DDR_1M_LEN)), reg_base + DMAC_CHX_SAR(j)); //source addr
                writel(0, reg_base + DMAC_CHX_SAR(j) + 0x4);
                pr_debug("---%s %d\n", __func__, __LINE__);
                /* CHX_DAR */
                writel((size_t)(0xC0000000+(j-1)*64*(DDR_1M_LEN)), reg_base + DMAC_CHX_DAR(j)); //destination addr
                writel(0, reg_base + DMAC_CHX_DAR(j) + 0x4);
            } else { //ptr4 -> ptr2
                /* CHX_SAR */
                writel((size_t)(0xE0000000+(j-1-4)*64*(DDR_1M_LEN)), reg_base + DMAC_CHX_SAR(j)); //source addr
                writel(0, reg_base + DMAC_CHX_SAR(j) + 0x4);
                pr_debug("---%s %d\n", __func__, __LINE__);
                /* CHX_DAR */
                writel((size_t)(0xA0000000+(j-1-4)*64*(DDR_1M_LEN)), reg_base + DMAC_CHX_DAR(j)); //destination addr
                writel(0, reg_base + DMAC_CHX_DAR(j) + 0x4);
            }
            #endif

            /* CHX_CTRL */
            data = readl(reg_base + DMAC_CHX_CTL(j));
            data = (data & (~(0x1 << 4))) | (0x0 << 4); //source address increment: 0(increment), 1(no change)
            data = (data & (~(0x1 << 6))) | (0x0 << 6); //destination address increment: 0(increment), 1(no change)
            writel(data, reg_base + DMAC_CHX_CTL(j));
            
            data = readl(reg_base + DMAC_CHX_CTL(j) + 0x4); //high 32 bit
            data = (data & (~(0x1 << 26))) |
            (0x1 << 26); //Interrput on completion of block transfer
            data = (data & (~(0x1 << 30))) |
            (0x1 << 30); //Last shadow register/linked list item
            data = (data & (~(0x1 << 31))) |
            (0x1 << 31); //shadow register comntent/link list item valid
            writel(data, reg_base + DMAC_CHX_CTL(j) + 0x4);

            writel(DMA_MAX_BLK_NUM, reg_base + DMAC_CHX_BLOCK_TS(j)); /* transfer block number, max only can be 65535! */

            reg_data = readl(reg_base + DMAC_CHX_CTL(j)); /* config channel ctl */
            reg_data = (reg_data & (~(0x7 << 8))) | (4 << 8); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
            reg_data = (reg_data & (~(0x7 << 11))) | (4 << 11); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
            reg_data = (reg_data & (~(0xf << 14))) | (0x2 << 14); /* src msize 0:1 item, 1:4 item, 2:8 item */
            reg_data = (reg_data & (~(0xf << 18))) | (0x2 << 18); /* dst msize 0:1 item, 1:4 item, 2:8 item */
            writel(reg_data, reg_base + DMAC_CHX_CTL(j));

            reg_data = readl(reg_base + DMAC_CHX_CTL(j) + 0x4); /* config channel ctl */
            reg_data = (reg_data & (~(0x1 << 6))) | (0x1 << 6); /* src burst len enable */
            reg_data = (reg_data & (~(0xff << 7))) | (0xF << 7); /* src burst len */
            reg_data = (reg_data & (~(0x1 << 15))) | (0x1 << 15); /* dst burst len enable */
            reg_data = (reg_data & (~(0xff << 16))) | (0xF << 16); /* dst burst len */
            writel(reg_data, reg_base + DMAC_CHX_CTL(j) + 0x4);
        }

        for(xfer_cnt = 0; xfer_cnt < 128*DDR_1M_LEN/DMA_MAX_XFER_SIZE; xfer_cnt++) { //ptr1 -> ptr3 use 4 channels, total 512M, so each channel xfer 128M
            pr_debug("---%s %d, xfer_cnt:%d\n", __func__, __LINE__, xfer_cnt);
            /* DMA_ChEnReg:select available channel */
        	data = readl(reg_base + DMAC_CHENREG);
        	data = (data & (~(0xFF << 8))) | (0xFF << 8); //CH1_EN_WE
        	data = (data & (~(0xFF << 0))) | (0x0 << 0); //CH1_EN-8 disable
        	writel(data, reg_base + DMAC_CHENREG);

            for(j = 1; j <= channel_num; j++) {
                if(j <= 4) { //ptr1 -> ptr3
                    /* CHX_SAR */
                    writel((size_t)(0x80000000+(j-1)*(128)*(DDR_1M_LEN)+xfer_cnt*(DMA_MAX_XFER_SIZE)), reg_base + DMAC_CHX_SAR(j)); //source addr
                    writel(0, reg_base + DMAC_CHX_SAR(j) + 0x4);
                    pr_debug("---%s %d\n", __func__, __LINE__);
                    /* CHX_DAR */
                    writel((size_t)(0xC0000000+(j-1)*(128)*(DDR_1M_LEN)+xfer_cnt*(DMA_MAX_XFER_SIZE)), reg_base + DMAC_CHX_DAR(j)); //destination addr
                    writel(0, reg_base + DMAC_CHX_DAR(j) + 0x4);
                } else { //ptr4 -> ptr2
                    /* CHX_SAR */
                    writel((size_t)(0xE0000000+(j-1-4)*(128)*(DDR_1M_LEN)+xfer_cnt*(DMA_MAX_XFER_SIZE)), reg_base + DMAC_CHX_SAR(j)); //source addr
                    writel(0, reg_base + DMAC_CHX_SAR(j) + 0x4);
                    pr_debug("---%s %d\n", __func__, __LINE__);
                    /* CHX_DAR */
                    writel((size_t)(0xA0000000+(j-1-4)*(128)*(DDR_1M_LEN)+xfer_cnt*(DMA_MAX_XFER_SIZE)), reg_base + DMAC_CHX_DAR(j)); //destination addr
                    writel(0, reg_base + DMAC_CHX_DAR(j) + 0x4);
                }
            }

            /* DMA_ChEnReg:select available channel */
        	data = readl(reg_base + DMAC_CHENREG);
        	data = (data & (~(0xFF << 0))) | (0xFF << 0); //CH1_EN, this bit auto clear after transfer done
        	data = (data & (~(0xFF << 8))) | (0xFF << 8); //CH1_EN_WE
        	writel(data, reg_base + DMAC_CHENREG);
        	pr_debug("---%s %d\n", __func__, __LINE__);

            /* DMA CHX_INTSTATUS, wait 8 channels to finish */
        	while ((readl(reg_base + DMAC_CHX_INTSTATUS(1)) & 0x11) == 0) {}
            pr_debug("---%s %d\n", __func__, __LINE__);
            while ((readl(reg_base + DMAC_CHX_INTSTATUS(2)) & 0x11) == 0) {}
            pr_debug("---%s %d\n", __func__, __LINE__);
            while ((readl(reg_base + DMAC_CHX_INTSTATUS(3)) & 0x11) == 0) {}
            pr_debug("---%s %d\n", __func__, __LINE__);
            while ((readl(reg_base + DMAC_CHX_INTSTATUS(4)) & 0x11) == 0) {}
            pr_debug("---%s %d\n", __func__, __LINE__);
            while ((readl(reg_base + DMAC_CHX_INTSTATUS(5)) & 0x11) == 0) {}
            pr_debug("---%s %d\n", __func__, __LINE__);
            while ((readl(reg_base + DMAC_CHX_INTSTATUS(6)) & 0x11) == 0) {}
            pr_debug("---%s %d\n", __func__, __LINE__);
            while ((readl(reg_base + DMAC_CHX_INTSTATUS(7)) & 0x11) == 0) {}
            pr_debug("---%s %d\n", __func__, __LINE__);
            while ((readl(reg_base + DMAC_CHX_INTSTATUS(8)) & 0x11) == 0) {}
            pr_debug("---%s %d\n", __func__, __LINE__);

            //clear all intterupts
            for(j = 1; j <= 8; j++) {
                writel(0xFFFFFFFF, reg_base + DMAC_CHX_INTCLEARREG(j));
                writel(0xFFFFFFFF, reg_base + DMAC_CHX_INTCLEARREG(j) + 0x4);
            }
        }

    }

    //data check
    pr_debug("---%s %d\n", __func__, __LINE__);
	ptr = ptr1;
    ptrdst = ptr3;
    while((size_t)ptr <= (size_t)(0x9FFFFFFC)) { //0xFFFFFFFF may overflow, so decrease 0x10
        HCOMP(*ptr, *ptrdst);
        ptr ++;
        ptrdst ++;
    }

    pr_debug("---%s %d\n", __func__, __LINE__);
	ptr = ptr4;
    ptrdst = ptr2;
    while((size_t)ptr <= (size_t)(0xFFFFFFFC)) {
        HCOMP(*ptr, *ptrdst);
        ptr ++;
        ptrdst ++;
    }
}


static void diff_inc_burst_len(void)
{
	u32 i;

	//inc_burst_len(16+1, 16, 3, 4);
	for (i = 1; i <= 64; i++) {
		//printf("i:%d\n", i);
		inc_burst_len(i + 1, 16, 3, 4);
	}
}

extern void dcache_disable(void);
extern void dcache_enable(void);

static int dma_buf_addr_check(char *addr) 
{
    u32 region;

    region  = ((size_t)addr>>DDR_ADDR_REGION_SHIFT)&0xF;
    
    if((region >= DDR_ADDR_START_REGION) && (region <= DDR_ADDR_END_REGION)) {
        return 1; //address ok
    } else {
        return 0;
    }
}

static void ddr_diff_len_test(void)
{
	u32 cnt = 0, i = 0, j = 0;
	u32 reg_data, loop = 10, inter_loop = 10;
    #if 0
    dma_ddr_func funp[] = {
        diff_fix_burst_size,
        diff_len_8_bit_test,
        diff_len_16_bit_test,
        diff_inc_burst_size,
        diff_inc_burst_len,
    };
    #endif

	printf("\r\nplease ensure cache is disabled\n");
    printf("check dma test buffer on DDR\n");
    if(dma_buf_addr_check(src)) {
        printf("src dma buffer on ddr check pass.");
    } else {
        printf("src dma buffer on ddr check fail.");
        return;
    }

    if(dma_buf_addr_check(dst)) {
        printf("dst dma buffer on ddr check pass.");
    } else {
        printf("dst dma buffer on ddr check fail.");
        return;
    }

    if(dma_buf_addr_check((char *)&i)) { //auto variable is on stack
        printf("Error: HVTE should run on Non-DDR device.");
        return;
    }
    
	//dcache_disable();
	reg_data = readl(TOP_CRM_BASE + GDMA_RESETREG);
	reg_data =
		(reg_data & (~(0x1 << 12))) | (0x1 << 12); // relese gdma reset
	writel(reg_data, TOP_CRM_BASE + GDMA_RESETREG);
	printf("\r\ntotal loop number: %d\r\n", loop);
	while (1) {
		for (cnt = 0; cnt < loop; cnt++) {
			printf("\rcnt=%d", cnt);

            pr_debug("---%s %d\n", __func__, __LINE__);
            for(j = 0; j < inter_loop; j++) {
                diff_fix_burst_size();
                gdma_reset();
            }

            pr_debug("---%s %d\n", __func__, __LINE__);
            for(j = 0; j < inter_loop; j++) {
                diff_len_8_bit_test();
                gdma_reset();
            }

            pr_debug("---%s %d\n", __func__, __LINE__);
            for(j = 0; j < inter_loop; j++) {
                diff_len_16_bit_test();
                gdma_reset();
            }

            pr_debug("---%s %d\n", __func__, __LINE__);
            for(j = 0; j < inter_loop; j++) {
                diff_inc_burst_size();
                gdma_reset();
            }

            pr_debug("---%s %d\n", __func__, __LINE__);
            for(j = 0; j < inter_loop; j++) {
                diff_inc_burst_len();
                gdma_reset();
            }

            pr_debug("---%s %d\n", __func__, __LINE__);
            for(j = 0; j < inter_loop; j++) {
                diff_inc_burst_len();
                gdma_reset();
            }

            dma_multi_chn_multi_size();
            gdma_reset();
			//putc('A');
		}
		printf("---dma ddr test: %d\n", i++);
	}
	//dcache_enable();
	//printf("\naddr:%x, src:%x\r\n", src, src[0]);
	//printf("addr:%x, dst:%x\r\n", dst, dst[0]);
}

#define __BITSHIFT(x, y) (x << y)
void config_ddr_parity(void *ddr_base)
{
	u32 tmp_cfg = 0;
	tmp_cfg = readl(ddr_base + 0x1330);
	tmp_cfg |= BIT(23) | BIT(21) | BIT(13) | BIT(4) | BIT(0);
	tmp_cfg &= ~(BIT(26) | BIT(25) | BIT(24) | BIT(22) | BIT(21) |
		     BIT(20) | BIT(15) | BIT(12) | BIT(7) | BIT(5) |
		     BIT(1));

	writel(0, ddr_base + 0x1320);
	writel(tmp_cfg, ddr_base + 0x1330);
	writel(1, ddr_base + 0x1320);
	while (!(readl(ddr_base + 0x1324) & BIT(0)))
		;
}

void ddr_enable_parity(void)
{
	config_ddr_parity((void *)DDRC0_CTRL_PHY_ADDR);
	config_ddr_parity((void *)DDRC1_CTRL_PHY_ADDR);
}

/*
 * let ddr bus appear special 0 or 1 sequence
 * this case is @134MHz rate pattern (from HW designer)
 * condition:
 *  interleave must close
 *  address must located in 0x10000 range
 *  single master to access ddr, we select GDMA
 *  pattern refer code
 */
void fix_rate_pattern_test(void)
{
    int i;
    char *ptrsrc = src;
    char *ptrdst, *ptr1, *ptr2;
    void *reg_base = (void *)GDMA_BASE_ADDR;
    u32 reg_data, data;
	u32 result;

#define PATTEN_LEN          (16*12) //128-bit * 12

	printf("\r\nplease ensure cache is disabled\n");
    printf("check dma test buffer on DDR\n");
    if(dma_buf_addr_check(src)) {
        printf("src dma buffer on ddr check pass.\n");
    } else {
        printf("src dma buffer on ddr check fail.\n");
        return;
    }

    if(dma_buf_addr_check((char *)&i)) { //auto variable is on stack
        printf("Error: HVTE should run on Non-DDR device.\n");
        return;
    }

    printf("ddr closeInterleave start.\n");
    closeInterleave();
    printf("ddr closeInterleave end.\n");
    
    //find a address end with 0x..xx0
    while((size_t)ptrsrc & 0xF) {
        ptrsrc ++;
    }
    ptrdst = PATTEN_LEN + ptrsrc;
    while((size_t)ptrdst & 0xF) {
        ptrdst ++;
    }

    // src and dst should in the same 0x10000 range
    if(( ((size_t)ptrsrc)>>16 ) != ( ((size_t)ptrdst)>>16) ) {
        printf("Error: src and dst addr is not in the same 0x10000 range.\n");
        return;
    }

    pr_debug("---%s %d\n", __func__, __LINE__);
    memset(ptrsrc + 16*0, 0xFF, 16*3);
    memset(ptrsrc + 16*3, 0x00, 16*3);
    memset(ptrsrc + 16*6, 0xFF, 16*3);
    memset(ptrsrc + 16*9, 0x00, 16*3);
    
    gdma_reset();

    writel(readl(reg_base + DMAC_CFGREG) & (~(0x1)), reg_base + DMAC_CFGREG); //disable dma
    for (i = 0; i < 1000; i++){}
    writel(readl(reg_base + DMAC_CFGREG) | 0x1, reg_base + DMAC_CFGREG); //enable dma
    for (i = 0; i < 1000; i++){}

    /* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	data = (data & (~(0x1 << 0))) | (0x0 << 0); //CH1_EN-8 disable
	writel(data, reg_base + DMAC_CHENREG);

    //fix parameters set
    data = readl(reg_base + DMAC_CHX_CFG(1));
    data = (data & (~(0x3 << 0))) | (0x0 << 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
    data = (data & (~(0x3 << 2))) | (0x0 << 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
    writel(data, reg_base + DMAC_CHX_CFG(1));

    data = readl(reg_base + DMAC_CHX_CFG(1) + 0x4); //high 32 bit
    data = (data & (~(0x7 << 0))) | (0x0 << 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
    data = (data & (~(0x1 << 3))) | (0x1 << 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
    data = (data & (~(0x1 << 4))) | (0x1 << 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
    data = (data & (~(0xF << 23))) | (0xF << 23); //SRC_OSR_LMT,source outstanding request limit
    data = (data & (~(0xF << 27))) | (0xF << 27); //DST_OSR_LMT,destination outstanding request limit
    writel(data, reg_base + DMAC_CHX_CFG(1) + 0x4);

    /* CHX_CTRL */
    data = readl(reg_base + DMAC_CHX_CTL(1));
    data = (data & (~(0x1 << 4))) | (0x0 << 4); //source address increment: 0(increment), 1(no change)
    data = (data & (~(0x1 << 6))) | (0x0 << 6); //destination address increment: 0(increment), 1(no change)
    writel(data, reg_base + DMAC_CHX_CTL(1));
    
    data = readl(reg_base + DMAC_CHX_CTL(1) + 0x4); //high 32 bit
    data = (data & (~(0x1 << 26))) |
    (0x1 << 26); //Interrput on completion of block transfer
    data = (data & (~(0x1 << 30))) |
    (0x1 << 30); //Last shadow register/linked list item
    data = (data & (~(0x1 << 31))) |
    (0x1 << 31); //shadow register comntent/link list item valid
    writel(data, reg_base + DMAC_CHX_CTL(1) + 0x4);

    writel(11, reg_base + DMAC_CHX_BLOCK_TS(1)); /* transfer block number, max only can be 65535! */

    reg_data = readl(reg_base + DMAC_CHX_CTL(1)); /* config channel ctl */
    reg_data = (reg_data & (~(0x7 << 8))) | (4 << 8); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
    reg_data = (reg_data & (~(0x7 << 11))) | (4 << 11); /* 0:8-bit, 1:16-bit, 2:32-bit; 3:64-bit */
    reg_data = (reg_data & (~(0xf << 14))) | (0x2 << 14); /* src msize 0:1 item, 1:4 item, 2:8 item */
    reg_data = (reg_data & (~(0xf << 18))) | (0x2 << 18); /* dst msize 0:1 item, 1:4 item, 2:8 item */
    writel(reg_data, reg_base + DMAC_CHX_CTL(1));

    reg_data = readl(reg_base + DMAC_CHX_CTL(1) + 0x4); /* config channel ctl */
    reg_data = (reg_data & (~(0x1 << 6))) | (0x1 << 6); /* src burst len enable */
    reg_data = (reg_data & (~(0xff << 7))) | (11 << 7); /* src burst len = 11 */
    reg_data = (reg_data & (~(0x1 << 15))) | (0x1 << 15); /* dst burst len enable */
    reg_data = (reg_data & (~(0xff << 16))) | (11 << 16); /* dst burst len = 11 */
    writel(reg_data, reg_base + DMAC_CHX_CTL(1) + 0x4);

    /* CHX_SAR */
    writel((size_t)(ptrsrc), reg_base + DMAC_CHX_SAR(1)); //source addr
    writel(0, reg_base + DMAC_CHX_SAR(1) + 0x4);
    pr_debug("---%s %d\n", __func__, __LINE__);
    /* CHX_DAR */
    writel((size_t)(ptrdst), reg_base + DMAC_CHX_DAR(1)); //destination addr
    writel(0, reg_base + DMAC_CHX_DAR(1) + 0x4);
        
    /* DMA_ChEnReg:select available channel */
	data = readl(reg_base + DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) | (0x1 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	writel(data, reg_base + DMAC_CHENREG);
	pr_debug("---%s %d\n", __func__, __LINE__);

    /* DMA CHX_INTSTATUS, wait 8 channels to finish */
	while ((readl(reg_base + DMAC_CHX_INTSTATUS(1)) & 0x11) == 0) {}
    pr_debug("---%s %d\n", __func__, __LINE__);

    //clear all intterupts
    writel(0xFFFFFFFF, reg_base + DMAC_CHX_INTCLEARREG(1));
    writel(0xFFFFFFFF, reg_base + DMAC_CHX_INTCLEARREG(1) + 0x4);

    ptr1 = ptrsrc;
    ptr2 = ptrdst;
    while(ptr1 < (ptrsrc+PATTEN_LEN)) {
        HCOMP(*ptr1, *ptr2);
        ptr1 ++;
        ptr2 ++;
    }
}

extern int memtest_main(unsigned long addr, unsigned long size, int cnt);

void ddr_stress_test(void)
{
    u32 count = 0;
    
    while(1) {
        fix_rate_pattern_test();
        page_span_access();
        ddr_diff_len_test();
        memtest_main((unsigned long)0x80800000, 0x100000, 5);
        printf("---dma ddr test: %d\n", count++);
    }
}

static void TstLoop(void)
{
	char c = 0;

	ddr_envInit();

	while (1) {
		menu();
		logo();
		c = getc();
		putc(c);

		switch (c) {
		case '0':
			ddr0Init();
			break;
		case '1':
			ddr1Init();
			break;
		case '2':
			openInterleave();
			break;
		case '3':
			closeInterleave();
			break;
		case '4':
			ddr0ddr1Init();
			break;
		case '5':
			ddr_ReadWrite();
			break;
		case '6':
			//memtest_main(ddrMemStartAddr,ddrMemEndAddr - ddrMemStartAddr + 1,0x1);
			memtest_main((unsigned long)0x80800000, 0x1000, 0x1);
			break;
		case '7':
//memtest_main(ddrMemStartAddr,ddrMemEndAddr - ddrMemStartAddr + 1,0x1);
#ifdef CONFIG_A55
			/*0X80000000 ~ 0x81000000 For system running env. Can not to be test.*/
			ddr_ReadWrite_random((unsigned long)0x81000000,
					     0x7f000000);
#else
			ddr_ReadWrite_random((unsigned long)0x80000000,
					     0x40000000);
#endif
			break;
		case '9':
			set_ddr_test_range();
			break;
		case 'a':
			ddr_ctrl_irq_test(); //ddr inject error irq, and ddr to ECU irq
			break;
		case 'b':
			//ddr_phy_irq_test(); //zebu without phy so cannot test
			break;
		case 'c':
			ddr_func_irq_test(); //ddr scrub related irq
			break;
		case 'd':
			dumpMem(0);
			break;
		case 'e':
			ddr_large_addr_access();
			break;
		case 'f':
			ddr_global_rst_test();
			break;
		case 'g':
			ddr_internal_rst_test();
			break;
		case 'h':
			ddr_enable_parity();
		case 'i':
			ddr_bit_rotation_test();
			break;
		case 'j':
			ddr_diff_len_test(); //use GDMA
			break;
        case 'k': //DDR stress test
            ddr_stress_test();
            break;
        case 'l':
            fix_rate_pattern_test();
            break;
		case 's':
			setMem32(0);
			break;
		case 'q':
			return;
			break;
		default:
			hprintf(TSENV, "wrong case\r\n");
			break;
		}
	}
}

void ddrMain(int para)
{
	int ret;

	if (para) {
		ret = ddr_ReadWrite();
		printf("auto test err %d!!!!\r\n", ret);
	} else {
		printf("enter ddr test!\r\n");
		TstLoop();
	}
}

static int initOnce = 0;
void ddrOpen(int para)
{
	if (initOnce) {
		return;
	}

	closeInterleave();
	ddr0ddr1Init();
	initOnce = 1;
}

CMD_DEFINE(ddr, ddrMain, 0, "ddr test", 0);
CMD_DEFINE(openddr, ddrOpen, 0, "ddr init,close Interleave", 0);
