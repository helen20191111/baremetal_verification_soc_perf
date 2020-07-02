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
#include <module/noc.h>
#include <module/gdma.h>

//#include "../i2s/i2s.h" //use the SDMA register define

extern void ddr_envInit(void);

//******************************ipc event handle***************************//

volatile u32 noc_irq_id = 0;
volatile u32 sysnoc_parity_intr_state0 = 0;
volatile u32 sysnoc_parity_intr_state1 = 0;
volatile u32 sysnoc_parity_intr_state2 = 0;
volatile u32 corenoc_parity_intr_state0 = 0;
volatile u32 corenoc_parity_intr_state1 = 0;
volatile u32 corenoc_parity_intr_state2 = 0;
volatile u32 safenoc_pty_intr_stat0 = 0;
volatile u32 safenoc_pty_intr_stat1 = 0;
volatile u32 safety_irq_trigger = 0;

//avoid test addr is already in cache(especiall run the same case two times)
u32 ddr_oft = 0;
//avoid test addr is already in cache(especiall run the same case two times)
u32 gpu_oft = 0;

/*
 * this function will call both a55 sync error and noc IRQ handler to avoid A55 hang
 */
void a55_disable_noc_err_inject(void)
{
	//cpu_p0_rdata_pty_intr,cpu_p1_rdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) & (~(1 << 19)));
	//cpu_ppi_rdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) & (~(1 << 20))); 
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) & (~(1 << 29)));
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) & (~(1 << 28)));
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) & (~(1 << 27)));

	REG32_WRITE(SYS_CTRL_BASE + 0x114,
		    REG32_READ(SYS_CTRL_BASE + 0x114) & (~(1 << 0)));
	REG32_WRITE(SYS_CTRL_BASE + 0x114,
		    REG32_READ(SYS_CTRL_BASE + 0x114) & (~(1 << 21)));
	REG32_WRITE(SYS_CTRL_BASE + 0x114,
		    REG32_READ(SYS_CTRL_BASE + 0x114) & (~(1 << 22)));
	REG32_WRITE(SYS_CTRL_BASE + 0x114,
		    REG32_READ(SYS_CTRL_BASE + 0x114) & (~(1 << 24)));

	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) & (~(1 << 21)));
	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) & (~(1 << 23)));
	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) & (~(1 << 28)));

	REG32_WRITE(SYS_CTRL_BASE + 0x134,
		    REG32_READ(SYS_CTRL_BASE + 0x134) & (~(1 << 28)));
	REG32_WRITE(SYS_CTRL_BASE + 0x134,
		    REG32_READ(SYS_CTRL_BASE + 0x134) & (~(1 << 29)));

	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x118,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x118) & (~(1 << 16)));

	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x124,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x124) &
			    (~(0xFFFF << 0)));

	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 2)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 3)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 4)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 5)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 14)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 15)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 20)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 21)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 24)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 25)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 26)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 27)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 28)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) & (~(1 << 29)));

	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x134,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x134) & (~(1 << 6)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x134,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x134) & (~(1 << 7)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x134,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x134) & (~(1 << 8)));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x134,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x134) & (~(1 << 9)));
}

void irq_id_check(void)
{
	u32 i;

	for (i = 0; i < 100; i++)
		;
	printf_intr("%x\r\n", sysnoc_parity_intr_state0);
	printf_intr("%x\r\n", sysnoc_parity_intr_state1);
	printf_intr("%x\r\n", sysnoc_parity_intr_state2);
	printf_intr("%x\r\n", corenoc_parity_intr_state0);
	printf_intr("%x\r\n", corenoc_parity_intr_state1);
	printf_intr("%x\r\n", corenoc_parity_intr_state2);
	printf_intr("%x\r\n", safenoc_pty_intr_stat0);
	printf_intr("%x\r\n", safenoc_pty_intr_stat1);
	//printf_intr("\r\n%x\r\n", REG32_READ(SYS_CTRL_BASE+0x90));

	switch (noc_irq_id) {
		/* A55 169 irq */
	case CPU_P0_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 27)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CPU_P0_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CPU_P0_RDATA_PTY_INTR);
		}
		break;
	case CPU_P1_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 26)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CPU_P1_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CPU_P1_RDATA_PTY_INTR);
		}
		break;
	case CPU_PPI_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 25)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CPU_PPI_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CPU_PPI_RDATA_PTY_INTR);
		}
		break;
	case GPU_P0_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 20)) {
			printf_intr("\r\nID %d\npass\r\n",
				    GPU_P0_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    GPU_P0_RDATA_PTY_INTR);
		}
		break;
	case GPU_P1_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 19)) {
			printf_intr("\r\nID %d\npass\r\n",
				    GPU_P1_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    GPU_P1_RDATA_PTY_INTR);
		}
		break;
	case GPU_CFG_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 18)) {
			printf_intr("\r\nID %d\npass\r\n",
				    GPU_CFG_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    GPU_CFG_WDATA_PTY_INTR);
		}
		break;
	case ISP_RDATA_PTY_INTR:
		if (corenoc_parity_intr_state0 & (1 << 18)) {
			printf_intr("\r\nID %d\npass\r\n", ISP_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n", ISP_RDATA_PTY_INTR);
		}
		break;
	case VSP_AHB_SLAVE_WDATA_PTY_INTR:
		if (corenoc_parity_intr_state1 & (1 << 21)) {
			printf_intr("\r\nID %d\npass\r\n",
				    VSP_AHB_SLAVE_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    VSP_AHB_SLAVE_WDATA_PTY_INTR);
		}
		break;
	case CV_SLAVE_WDATA_PTY_INTR:
		if (corenoc_parity_intr_state1 & (1 << 13)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CV_SLAVE_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CV_SLAVE_WDATA_PTY_INTR);
		}
		break;
	case NET_SLAVE_WDATA_PTY_INTR:
		if (corenoc_parity_intr_state1 & (1 << 2)) {
			printf_intr("\r\nID %d\npass\r\n",
				    NET_SLAVE_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    NET_SLAVE_WDATA_PTY_INTR);
		}
		break;
	case CORENOC_APB_WDATA_PTY_INTR:
		if (corenoc_parity_intr_state1 & (1 << 0)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CORENOC_APB_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CORENOC_APB_WDATA_PTY_INTR);
		}
		break;
	case MIPI0_DATA_APB_SWITCH_PARITY_INTR:
		if (corenoc_parity_intr_state2 & (1 << 6)) {
			printf_intr("\r\nID %d\npass\r\n",
				    MIPI0_DATA_APB_SWITCH_PARITY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    MIPI0_DATA_APB_SWITCH_PARITY_INTR);
		}
		break;
	case MIPI1_DATA_APB_SWITCH_PARITY_INTR:
		if (corenoc_parity_intr_state2 & (1 << 4)) {
			printf_intr("\r\nID %d\npass\r\n",
				    MIPI1_DATA_APB_SWITCH_PARITY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    MIPI1_DATA_APB_SWITCH_PARITY_INTR);
		}
		break;
	case MIPI2_DATA_APB_SWITCH_PARITY_INTR:
		if (corenoc_parity_intr_state2 & (1 << 2)) {
			printf_intr("\r\nID %d\npass\r\n",
				    MIPI2_DATA_APB_SWITCH_PARITY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    MIPI2_DATA_APB_SWITCH_PARITY_INTR);
		}
		break;

		/* A55 234 irq */
	case CPU_ETR_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 24)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CPU_ETR_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CPU_ETR_RDATA_PTY_INTR);
		}
		break;
	case CORESIGHT_DAP_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 23)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CORESIGHT_DAP_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CORESIGHT_DAP_RDATA_PTY_INTR);
		}
		break;
	case GDMA_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 22)) {
			printf_intr("\r\nID %d\npass\r\n", GDMA_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n", GDMA_RDATA_PTY_INTR);
		}
		break;
	case GDMA_CFG_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 21)) {
			printf_intr("\r\nID %d\npass\r\n",
				    GDMA_CFG_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    GDMA_CFG_WDATA_PTY_INTR);
		}
		break;
	case GMAC0_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 17)) {
			printf_intr("\r\nID %d\npass\r\n",
				    GMAC0_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    GMAC0_RDATA_PTY_INTR);
		}
		break;
	case GMAC0_WDATA_PTY_INTR:
		printf_intr("\r\nID %d\npass\r\n", GMAC0_WDATA_PTY_INTR);
		break;
	case GMAC1_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 14)) {
			printf_intr("\r\nID %d\npass\r\n",
				    GMAC1_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    GMAC1_RDATA_PTY_INTR);
		}
		break;
	case PCIE30_4X_M_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 11)) {
			printf_intr("\r\nID %d\npass\r\n",
				    PCIE30_4X_M_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    PCIE30_4X_M_RDATA_PTY_INTR);
		}
		break;
	case PCIE30_2X_M_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 8)) {
			printf_intr("\r\nID %d\npass\r\n",
				    PCIE30_2X_M_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    PCIE30_2X_M_RDATA_PTY_INTR);
		}
		break;
	case PCIE30_4X_S_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 4)) {
			printf_intr("\r\nID %d\npass\r\n",
				    PCIE30_4X_S_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    PCIE30_4X_S_WDATA_PTY_INTR);
		}
		break;
	case PCIE30_2X_S_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 2)) {
			printf_intr("\r\nID %d\npass\r\n",
				    PCIE30_2X_S_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    PCIE30_2X_S_WDATA_PTY_INTR);
		}
		break;
	case PCIE30_4X_DBI_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 1)) {
			printf_intr("\r\nID %d\npass\r\n",
				    PCIE30_4X_DBI_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    PCIE30_4X_DBI_WDATA_PTY_INTR);
		}
		break;
	case PCIE30_2X_DBI_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state0 & (1 << 0)) {
			printf_intr("\r\nID %d\npass\r\n",
				    PCIE30_2X_DBI_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    PCIE30_2X_DBI_WDATA_PTY_INTR);
		}
		break;
	case USB30_CFG_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state1 & (1 << 25)) {
			printf_intr("\r\nID %d\npass\r\n",
				    USB30_CFG_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    USB30_CFG_WDATA_PTY_INTR);
		}
		break;
	case SYSNOC2SAFENOC_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state1 & (1 << 23)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYSNOC2SAFENOC_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYSNOC2SAFENOC_WDATA_PTY_INTR);
		}
		break;
	case SAFENOC2SYSNOC_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state1 & (1 << 21)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SAFENOC2SYSNOC_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SAFENOC2SYSNOC_WDATA_PTY_INTR);
		}
		break;
	case SYSNOC2CPUNOC_TAR_RDATA_PTY_INTR:
		printf_intr("\r\nID %d\npass\r\n",
			    SAFENOC2SYSNOC_WDATA_PTY_INTR);
		break;
	case CPUNOC2SYSNOC_TAR_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state1 & (1 << 13)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CPUNOC2SYSNOC_TAR_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CPUNOC2SYSNOC_TAR_WDATA_PTY_INTR);
		}
		break;
	case CPUNOC2SYSNOC_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state1 & (1 << 12)) {
			printf_intr("\r\nID %d\npass\r\n",
				    CPUNOC2SYSNOC_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    CPUNOC2SYSNOC_RDATA_PTY_INTR);
		}
		break;
	case SYSNOC2CORENOC_TAR_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state1 & (1 << 8)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYSNOC2CORENOC_TAR_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYSNOC2CORENOC_TAR_WDATA_PTY_INTR);
		}
		break;
	case SYSNOC2CORENOC_RDATA_PTY_INTR:
		if (sysnoc_parity_intr_state1 & (1 << 7)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYSNOC2CORENOC_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYSNOC2CORENOC_RDATA_PTY_INTR);
		}
		break;
	case CORENOC2SYSNOC_PTY_INTR:
		printf_intr("\r\nID %d\npass\r\n", CORENOC2SYSNOC_PTY_INTR);
		break;
	case USB20_CFG_WDATA_PTY_INTR: //case SYSNOC_APB_WDATA_PTY_INTR merge together
		if (sysnoc_parity_intr_state2 & (1 << 14)) {
			printf_intr("\r\nID %d\npass\r\n",
				    USB20_CFG_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    USB20_CFG_WDATA_PTY_INTR);
		}
		if (sysnoc_parity_intr_state2 & (1 << 8)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYSNOC_APB_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYSNOC_APB_WDATA_PTY_INTR);
		}
		break;
	case SDEMMC0_CFG_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state2 & (1 << 12)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SDEMMC0_CFG_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SDEMMC0_CFG_WDATA_PTY_INTR);
		}
		break;
	case SDEMMC1_CFG_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state2 & (1 << 12)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SDEMMC1_CFG_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SDEMMC1_CFG_WDATA_PTY_INTR);
		}
		break;
	case SYSNOC_APB_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state2 & (1 << 8)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYSNOC_APB_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYSNOC_APB_WDATA_PTY_INTR);
		}
		break;
	case SYSNOC_DDR0_P0_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state2 & (1 << 6)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYSNOC_DDR0_P0_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYSNOC_DDR0_P0_WDATA_PTY_INTR);
		}
		break;
	case SYSNOC_DDR0_P1_WDATA_PTY_INTR:
		if (sysnoc_parity_intr_state2 & (1 << 4)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYSNOC_DDR0_P1_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYSNOC_DDR0_P1_WDATA_PTY_INTR);
		}
		break;

		/* A55 119 irq */
	case SAFE2SYS_DUPL_RDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 23)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SAFE2SYS_DUPL_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SAFE2SYS_DUPL_RDATA_PTY_INTR);
		}
		break;
	case SAFE2SYS_MAIN_RDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 22)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SAFE2SYS_MAIN_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SAFE2SYS_MAIN_RDATA_PTY_INTR);
		}
		break;
	case SAFE2SYS_DUPL_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 21)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SAFE2SYS_DUPL_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SAFE2SYS_DUPL_WDATA_PTY_INTR);
		}
		break;
	case SAFE2SYS_MAIN_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 20)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SAFE2SYS_MAIN_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SAFE2SYS_MAIN_WDATA_PTY_INTR);
		}
		break;
	case SRAM_DUPL_RDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 19)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SRAM_DUPL_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SRAM_DUPL_RDATA_PTY_INTR);
		}
		break;
	case SRAM_MAIN_RDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 18)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SRAM_MAIN_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SRAM_MAIN_RDATA_PTY_INTR);
		}
		break;
	case SRAM_DUPL_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 17)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SRAM_DUPL_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SRAM_DUPL_WDATA_PTY_INTR);
		}
		break;
	case SRAM_MAIN_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 16)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SRAM_MAIN_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SRAM_MAIN_WDATA_PTY_INTR);
		}
		break;
	case QSPI1_DUPL_WDATA_PTY_INTR:
		if ((safenoc_pty_intr_stat0 & (1 << 14)) &&
		    (safenoc_pty_intr_stat0 & (1 << 15))) {
			printf_intr("\r\nID %d\npass\r\n",
				    QSPI1_DUPL_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    QSPI1_DUPL_WDATA_PTY_INTR);
		}
		break;
	case QSPI0_DUPL_WDATA_PTY_INTR:
		if ((safenoc_pty_intr_stat0 & (1 << 12)) &&
		    (safenoc_pty_intr_stat0 & (1 << 13))) {
			printf_intr("\r\nID %d\npass\r\n",
				    QSPI0_DUPL_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    QSPI0_DUPL_WDATA_PTY_INTR);
		}
		break;
	case DMA_CFG_MAIN_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 6)) {
			printf_intr("\r\nID %d\npass\r\n",
				    DMA_CFG_MAIN_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    DMA_CFG_MAIN_WDATA_PTY_INTR);
		}
		break;
	case R5_0_MAIN_RDATA_PTY_INTR:
		if (safenoc_pty_intr_stat0 & (1 << 2)) {
			printf_intr("\r\nID %d\npass\r\n",
				    R5_0_MAIN_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    R5_0_MAIN_RDATA_PTY_INTR);
		}
		break;

		/* A55 120 irq */
	case SYS2SAFE_DUPL_RDATA_PTY_INTR:
		if (safenoc_pty_intr_stat1 & (1 << 19)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYS2SAFE_DUPL_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYS2SAFE_DUPL_RDATA_PTY_INTR);
		}
		break;
	case SYS2SAFE_MAIN_RDATA_PTY_INTR:
		if (safenoc_pty_intr_stat1 & (1 << 18)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYS2SAFE_MAIN_RDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYS2SAFE_MAIN_RDATA_PTY_INTR);
		}
		break;
	case SYS2SAFE_DUPL_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat1 & (1 << 17)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYS2SAFE_DUPL_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYS2SAFE_DUPL_WDATA_PTY_INTR);
		}
		break;
	case SYS2SAFE_MAIN_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat1 & (1 << 16)) {
			printf_intr("\r\nID %d\npass\r\n",
				    SYS2SAFE_MAIN_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    SYS2SAFE_MAIN_WDATA_PTY_INTR);
		}
		break;
	case PBUS0_DUPL_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat1 & (1 << 16)) {
			printf_intr("\r\nID %d\npass\r\n",
				    PBUS0_DUPL_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    PBUS0_DUPL_WDATA_PTY_INTR);
		}
		break;
	case PBUS0_MAIN_WDATA_PTY_INTR:
		if (safenoc_pty_intr_stat1 & (1 << 16)) {
			printf_intr("\r\nID %d\npass\r\n",
				    PBUS0_MAIN_WDATA_PTY_INTR);
		} else {
			printf_intr("\r\nID %d\nfail\r\n",
				    PBUS0_MAIN_WDATA_PTY_INTR);
		}
		break;

	default:
		printf_intr("\r\n wrong irq id\r\n");
		break;
	}
}

void a55_noc_irq_169_234_handler(int irq_id)
{
	a55_disable_noc_err_inject();

	sysnoc_parity_intr_state0 = REG32_READ(SYS_CTRL_BASE + 0x90);
	sysnoc_parity_intr_state1 = REG32_READ(SYS_CTRL_BASE + 0x94);
	sysnoc_parity_intr_state2 = REG32_READ(SYS_CTRL_BASE + 0x98);
	corenoc_parity_intr_state0 = REG32_READ(SYS_CTRL_BASE + 0x9c);
	corenoc_parity_intr_state1 = REG32_READ(SYS_CTRL_BASE + 0x100);
	corenoc_parity_intr_state2 = REG32_READ(SYS_CTRL_BASE + 0x104);

	printf_intr("\r\na55 irq:\r\n%d\r\n", irq_id);

	//if read these two register before disable_all_irq_inject(), exception will happen
	safenoc_pty_intr_stat0 = REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x128);
	safenoc_pty_intr_stat1 = REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x12c);

	irq_id_check();
	REG32_WRITE(SYS_CTRL_BASE + 0x88, 0xFFFFFFFF); //clean all irq
	REG32_WRITE(SYS_CTRL_BASE + 0x8C, 0xFFFFFFFF); //clean all irq

	//	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x124, 0xFFFFFFFF); //clean SEC_SAFE_SYS_CTRL_R_SAFENOC_PTY_INTR_CTRL
}

void a55_noc_irq_119_120_handler(int irq_id)
{
	a55_disable_noc_err_inject();

	safety_irq_trigger = 1;
	sysnoc_parity_intr_state0 = REG32_READ(SYS_CTRL_BASE + 0x90);
	sysnoc_parity_intr_state1 = REG32_READ(SYS_CTRL_BASE + 0x94);
	sysnoc_parity_intr_state2 = REG32_READ(SYS_CTRL_BASE + 0x98);
	corenoc_parity_intr_state0 = REG32_READ(SYS_CTRL_BASE + 0x9c);
	corenoc_parity_intr_state1 = REG32_READ(SYS_CTRL_BASE + 0x100);
	corenoc_parity_intr_state2 = REG32_READ(SYS_CTRL_BASE + 0x104);

	printf_intr("\r\na55 irq:\r\n%d\r\n", irq_id);

	//if read these two register before disable_all_irq_inject(), exception will happen
	safenoc_pty_intr_stat0 = REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x128);
	safenoc_pty_intr_stat1 = REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x12c);

	irq_id_check();
	REG32_WRITE(SYS_CTRL_BASE + 0x88, 0xFFFFFFFF); //clean all irq
	REG32_WRITE(SYS_CTRL_BASE + 0x8C, 0xFFFFFFFF); //clean all irq
}

/*
 * func name: ajust_addr_offset
 * description: avoid test addr is already in cache(especiall run the same case two times)
 * 				in this case, CPU will not access ddr so the test scenario not happen
 */
void ajust_ddr_offset(void)
{
	ddr_oft += 10 * 1024 * 1024; //10M step
	if (ddr_oft >= (1000 * 1024 * 1024)) {
		ddr_oft = 0;
	}
}

/*
 * please refer ajust_ddr_offset
 */
void ajust_gpu_offset(void)
{
	gpu_oft += 0; //step 0, GPU region is register
	if (gpu_oft >= (100 * 1024 * 1024)) {
		gpu_oft = 0;
	}
}

static void a55_noc_irq_169_234_test_sram(void)
{
	u32 data, i;

	if (((u64)(&data) & 0x18000000) !=
	    0x18000000) { //this case must run on SRAM
		hprintf(DEBUG,
			"Env error: this case A55 must run on SRAM.\r\n");
		return;
	}

	REG32_WRITE(0xc0035008, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x330020e8, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002004, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002000 + 0xC0, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002000 + 0xC4, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33000080, 0xFFFFFFFF);

	A55_IRQ_ENABLE(169);
	A55_IRQ_ENABLE(234);
	REG32_WRITE(SYS_CTRL_BASE + 0x80, 0xFFFFFFFF); //enable parity
	REG32_WRITE(SYS_CTRL_BASE + 0x84, 0xFFFFFFFF); //enable parity
	REG32_WRITE(SYS_CTRL_BASE + 0x10c, 0xabcd1234);

/* A55 169 irq */
#if 1
	//irq 169:case 1,cpu_p0_rdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) | (1 << 19));
	noc_irq_id = CPU_P0_RDATA_PTY_INTR;
	data = REG32_READ(0x80000000 + ddr_oft);
	hprintf(DEBUG, "cpu_p0_rdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
	ajust_ddr_offset();

	//irq 169:case 2,cpu_p1_rdata_pty_intr, cache must enable
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) | (1 << 19));
	noc_irq_id = CPU_P1_RDATA_PTY_INTR;
	data = REG32_READ(
		0x80000800 +
		ddr_oft); //shengnan.li:P1 port must access address bit[11]=1 region
	hprintf(DEBUG, "cpu_p1_rdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
	ajust_ddr_offset();

	//irq 169:case 3,cpu_ppi_rdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) | (1 << 20));
	noc_irq_id = CPU_PPI_RDATA_PTY_INTR;
	data = REG32_READ(0x33300000 + gpu_oft); //GPU addr
	hprintf(DEBUG, "cpu_ppi_rdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
	ajust_gpu_offset();

	//irq 169:case 4,gpu_p0_rdata_pty_intr
	hprintf(DEBUG,
		"zebu tag62. without GPU so gpu_p0_rdata_pty_intr cannot test\r\n");
	hprintf(DEBUG, "gpu_p0_rdata_pty_intr test done\r\n");

	//irq 169:case 5,gpu_p1_rdata_pty_intr
	hprintf(DEBUG,
		"zebu tag62. without GPU so gpu_p1_rdata_pty_intr cannot test\r\n");
	hprintf(DEBUG, "gpu_p1_rdata_pty_intr test done\r\n");
#endif

#if 0
	//irq 169:case 6,gpu_cfg_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x110, REG32_READ(SYS_CTRL_BASE+0x110)|(1<<27));
	noc_irq_id = GPU_CFG_WDATA_PTY_INTR;
	REG32_WRITE(0x33300000+gpu_oft, 0x0); //GPU addr
	hprintf(DEBUG, "gpu_cfg_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
	ajust_gpu_offset();
#endif

	//irq 169:case 7,isp_rdata_pty_intr
	hprintf(DEBUG,
		"zebu tag62. without ISP so isp_rdata_pty_intr cannot test\r\n");
	hprintf(DEBUG, "isp_rdata_pty_intr test done\r\n");

/* will hang when A55 access VSP addr, project do not include VSP hardware code */
#if 0 
	//irq 169:case 8,vsp_ahb_slave_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x110, REG32_READ(SYS_CTRL_BASE+0x110)|(1<<28)|(1<<29));
	noc_irq_id = VSP_AHB_SLAVE_WDATA_PTY_INTR;
	REG32_WRITE(0x53000000, 0x0); //VSP addr
	hprintf(DEBUG, "vsp_ahb_slave_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

/* will hang when A55 access CV addr, project do not include CV hardware code */
#if 0 
	//irq 169:case 9,cv_slave_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x110, REG32_READ(SYS_CTRL_BASE+0x110)|(1<<28)|(1<<29));
	noc_irq_id = CV_SLAVE_WDATA_PTY_INTR;
	REG32_WRITE(0x51030000, 0x0); //CV addr
	hprintf(DEBUG, "cv_slave_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

/* will hang when A55 access NET addr, project do not include NET hardware code */
#if 0
	//irq 169:case 10,net_slave_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x110, REG32_READ(SYS_CTRL_BASE+0x110)|(1<<28)|(1<<29));
	noc_irq_id = NET_SLAVE_WDATA_PTY_INTR;
	REG32_WRITE(0x50000000, 0x0); //NET addr
	hprintf(DEBUG, "net_slave_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

	//irq 169:case 11,corenoc_apb_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) | (1 << 28));
	noc_irq_id = CORENOC_APB_WDATA_PTY_INTR;
	REG32_WRITE(0x33100000, 0x0); //IPC addr
	hprintf(DEBUG, "corenoc_apb_wdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test

#if 0
	//irq 169:case 12,mipi0_data_apb_switch_parity_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<0));
	noc_irq_id = MIPI0_DATA_APB_SWITCH_PARITY_INTR;
	REG32_WRITE(0x30f00000, 0x0); //MIPI0 addr
	hprintf(DEBUG, "mipi0_data_apb_switch_parity_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 169:case 13,mipi1_data_apb_switch_parity_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<0));
	noc_irq_id = MIPI1_DATA_APB_SWITCH_PARITY_INTR;
	REG32_WRITE(0x31000000, 0x0); //MIPI1 addr
	hprintf(DEBUG, "mipi1_data_apb_switch_parity_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 169:case 14,mipi2_data_apb_switch_parity_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<0));
	noc_irq_id = MIPI2_DATA_APB_SWITCH_PARITY_INTR;
	REG32_WRITE(0x31100000, 0x0); //MIPI2 addr
	hprintf(DEBUG, "mipi2_data_apb_switch_parity_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

		/* A55 234 irq */

/* case cannot test, detail please refer word doc */
#if 0 
	//irq 234:case 12,pcie30_4x_dbi_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = PCIE30_4X_DBI_WDATA_PTY_INTR;
	REG32_WRITE(0x30600000, 0x0); //PCIE 4x dbi addr
	hprintf(DEBUG, "pcie30_4x_dbi_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif
/* case cannot test, detail please refer word doc */
#if 0
	//irq 234:case 13,pcie30_2x_dbi_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = PCIE30_2X_DBI_WDATA_PTY_INTR;
	REG32_WRITE(0x30A00000, 0x0); //PCIE 2x dbi addr
	hprintf(DEBUG, "pcie30_2x_dbi_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif
/* case cannot test, detail please refer word doc  */
#if 0
	//irq 234:case 14,usb30_cfg_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = USB30_CFG_WDATA_PTY_INTR;
	REG32_WRITE(0x30E00000, 0x0);
	hprintf(DEBUG, "usb30_cfg_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 15,sysnoc2safenoc_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = SYSNOC2SAFENOC_WDATA_PTY_INTR;
	REG32_WRITE(0x20000000, 0x0);
	hprintf(DEBUG, "sysnoc2safenoc_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

	//irq 234:case 16,safenoc2sysnoc_wdata_pty_intr -> please refer a55_noc_irq_119_120_test

#if 1
	//irq 234:case 18,cpunoc2sysnoc_tar_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) | (1 << 28) | (1 << 29));
	noc_irq_id = CPUNOC2SYSNOC_TAR_WDATA_PTY_INTR;
	REG32_WRITE(0x30F00000, 0x0);
	hprintf(DEBUG, "cpunoc2sysnoc_tar_wdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
#endif

#if 1
	//irq 234:case 19,cpunoc2sysnoc_rdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) | (1 << 21));
	noc_irq_id = CPUNOC2SYSNOC_RDATA_PTY_INTR;
	data = REG32_READ(0x33410000);
	hprintf(DEBUG, "cpunoc2sysnoc_rdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 20,sysnoc2corenoc_tar_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = SYSNOC2CORENOC_TAR_WDATA_PTY_INTR;
	REG32_WRITE(0x33100000, 0x0);
	hprintf(DEBUG, "sysnoc2corenoc_tar_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 1
	//irq 234:case 21,sysnoc2corenoc_rdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x114,
		    REG32_READ(SYS_CTRL_BASE + 0x114) | (1 << 22));
	noc_irq_id = SYSNOC2CORENOC_RDATA_PTY_INTR;
	data = REG32_READ(0x33100000);
	hprintf(DEBUG, "sysnoc2corenoc_rdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 23,usb20_cfg_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = USB20_CFG_WDATA_PTY_INTR;
	REG32_WRITE(0x30300000, 0x0);
	hprintf(DEBUG, "usb20_cfg_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 24,sdemmc0_cfg_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = SDEMMC0_CFG_WDATA_PTY_INTR;
	REG32_WRITE(0x30400000, 0x0);
	hprintf(DEBUG, "sdemmc0_cfg_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 25,sdemmc0_cfg_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = SDEMMC0_CFG_WDATA_PTY_INTR;
	REG32_WRITE(0x30500000, 0x0);
	hprintf(DEBUG, "sdemmc0_cfg_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 26,sysnoc_apb_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = SYSNOC_APB_WDATA_PTY_INTR;
	REG32_WRITE(0x30300000, 0x0);
	hprintf(DEBUG, "sysnoc_apb_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 27,sysnoc_ddr0_p0_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x110,
		    REG32_READ(SYS_CTRL_BASE + 0x110) | (1 << 28) | (1 << 29));
	noc_irq_id = SYSNOC_DDR0_P0_WDATA_PTY_INTR;
	REG32_WRITE(0x80000000, 0x0);
	hprintf(DEBUG, "sysnoc_ddr0_p0_wdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
#endif
}

#ifdef CONFIG_R5_SECURE
static void secure_noc_irq_169_234_test_ddr(void)
{
	u32 data, i, reg;

	hprintf(DEBUG, "secure_noc_irq_169_234_test_ddr\r\n");
	REG32_WRITE(0xc0035008, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x330020e8, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002004, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002000 + 0xC0, 0xFFFFFFFF);
	REG32_WRITE(0x33002000 + 0xC4, 0xFFFFFFFF);

	//REG32_WRITE(0x33000080, 0xFFFFFFFF); //if enable hprintf will hang
	reg = REG32_READ(SYS_CTRL_BASE + 0x80);
	REG32_WRITE(SYS_CTRL_BASE + 0x80,
		    0xFFFFFFFF); //enable parity, if enable hprintf will hang
	//hprintf(DEBUG, "SYS_CTRL_BASE+0x80:%x\r\n", REG32_READ(SYS_CTRL_BASE+0x80));
	REG32_WRITE(SYS_CTRL_BASE + 0x84,
		    0xFFFFFFFF); //enable parity, if enable hprintf will hang
	for (i = 0; i < 3000; i++)
		;
	REG32_WRITE(SYS_CTRL_BASE + 0x10c,
		    0xabcd1234); //if enable hprintf will hang
	//please ensure A_I2S0_CK force to 1 to enable safety error inject

/* A55 234 irq */
#if 0
	//irq 234:case 14,usb30_cfg_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	for(i = 0; i < 3000; i++);
	data = REG32_READ(SYS_CTRL_BASE+0x118); //some nop to delay
	data ++;
	data = REG32_READ(SYS_CTRL_BASE+0x114);
	data ++;
	data = REG32_READ(SYS_CTRL_BASE+0x110);
	data ++;
	REG32_WRITE(0x30200000, 0x0);
	for(i = 0; i < 3000; i++);
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)&(~(1<<23))); //clear
	REG32_WRITE(SYS_CTRL_BASE+0x80, reg);
	REG32_WRITE(SYS_CTRL_BASE+0x88, 0xFFFFFFFF); //clean all irq
	REG32_WRITE(SYS_CTRL_BASE+0x8C, 0xFFFFFFFF); //clean all irq
#endif

#if 1
	//irq 234:case 12,pcie30_4x_dbi_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) | (1 << 23));
	for (i = 0; i < 3000; i++)
		;
	REG32_WRITE(0x30600000, 0x0);
	for (i = 0; i < 3000; i++)
		;
	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) & (~(1 << 23))); //clear
	REG32_WRITE(SYS_CTRL_BASE + 0x80, reg);
	REG32_WRITE(SYS_CTRL_BASE + 0x88, 0xFFFFFFFF); //clean all irq
	REG32_WRITE(SYS_CTRL_BASE + 0x8C, 0xFFFFFFFF); //clean all irq
#endif
}
#endif

#ifdef CONFIG_R5_SAFETY
static void safety_noc_irq_169_234_test_ddr(void)
{
	u32 data, i, reg;

	REG32_WRITE(0xc0035008, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x330020e8, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002004, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002000 + 0xC0, 0xFFFFFFFF);
	REG32_WRITE(0x33002000 + 0xC4, 0xFFFFFFFF);

	//REG32_WRITE(0x33000080, 0xFFFFFFFF); //if enable hprintf will hang
	reg = REG32_READ(SYS_CTRL_BASE + 0x80);
	REG32_WRITE(SYS_CTRL_BASE + 0x80,
		    0xFFFFFFFF); //enable parity, if enable hprintf will hang
	//hprintf(DEBUG, "SYS_CTRL_BASE+0x80:%x\r\n", REG32_READ(SYS_CTRL_BASE+0x80));
	REG32_WRITE(SYS_CTRL_BASE + 0x84,
		    0xFFFFFFFF); //enable parity, if enable hprintf will hang
	REG32_WRITE(SYS_CTRL_BASE + 0x10c,
		    0xabcd1234); //if enable hprintf will hang
	//please ensure A_I2S0_CK force to 1 to enable safety error inject

/* A55 234 irq */
#if 1
	//irq 234:case 14,usb30_cfg_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) | (1 << 23));
	for (i = 0; i < 3000; i++)
		;
	data = REG32_READ(SYS_CTRL_BASE + 0x118); //some nop to delay
	data++;
	data = REG32_READ(SYS_CTRL_BASE + 0x114);
	data++;
	data = REG32_READ(SYS_CTRL_BASE + 0x110);
	data++;
	REG32_WRITE(0x30E00000, 0x0);
	for (i = 0; i < 3000; i++)
		;
	REG32_WRITE(SYS_CTRL_BASE + 0x118,
		    REG32_READ(SYS_CTRL_BASE + 0x118) & (~(1 << 23))); //clear
	REG32_WRITE(SYS_CTRL_BASE + 0x80, reg);
	REG32_WRITE(SYS_CTRL_BASE + 0x88, 0xFFFFFFFF); //clean all irq
	REG32_WRITE(SYS_CTRL_BASE + 0x8C, 0xFFFFFFFF); //clean all irq
#endif

#if 0
	//irq 234:case 23 and 26,usb20_cfg_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = USB20_CFG_WDATA_PTY_INTR;
	for(i = 0; i < 3000; i++);
	data = REG32_READ(SYS_CTRL_BASE+0x118); //some nop to delay
	data ++;
	data = REG32_READ(SYS_CTRL_BASE+0x114);
	data ++;
	data = REG32_READ(SYS_CTRL_BASE+0x110);
	data ++;
	REG32_WRITE(0x30300000, 0x0);
	REG32_WRITE(0x30E01000, 0x0);
	for(i = 0; i < 3000; i++);
	data = REG32_READ(0x33000000+0x98);
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)&(~(1<<23))); //clear
	REG32_WRITE(SYS_CTRL_BASE+0x80, reg);
	REG32_WRITE(SYS_CTRL_BASE+0x88, 0xFFFFFFFF); //clean all irq
	REG32_WRITE(SYS_CTRL_BASE+0x8C, 0xFFFFFFFF); //clean all irq
#endif

/* need waveform to check */
#if 0
	//irq 234:case 26,sysnoc_apb_wdata_pty_intr 
	//hprintf(DEBUG, "sysnoc_apb_wdata_pty_intr test start\r\n"); 
	//hprintf(DEBUG, "please use safety R5 to run noc case i\r\n");
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = SYSNOC_APB_WDATA_PTY_INTR;
	REG32_WRITE(0x33001000, 0x0);
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)&(~(1<<23))); //clear
	hprintf(DEBUG, "sysnoc_apb_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test

	if(data & (1<<14)) { //check irq bit
		hprintf(DEBUG, "usb20_cfg_wdata_pty_intr test pass.\r\n");
	} else {
		hprintf(DEBUG, "usb20_cfg_wdata_pty_intr test fail.\r\n");
	}
	REG32_WRITE(SYS_CTRL_BASE+0x88, 0xFFFFFFFF); //clean all irq
	REG32_WRITE(SYS_CTRL_BASE+0x8C, 0xFFFFFFFF); //clean all irq
#endif
}
#endif

static void a55_noc_irq_169_234_test_ddr(void)
{
	u32 i;

#if 0
	u32 data;

	if(((u32)(&data) & 0x80000000) != 0x80000000) { //this case must run on SRAM
		hprintf(DEBUG, "Env error: this case A55 must run on SRAM.\r\n");
		return;
	}

		REG32_WRITE(0xc0035008, 0xFFFFFFFF); //relese software reset
		REG32_WRITE(0x330020e8, 0xFFFFFFFF); //relese software reset
		REG32_WRITE(0x33002004, 0xFFFFFFFF); //relese software reset
		REG32_WRITE(0x33002000+0xC0, 0xFFFFFFFF);
		REG32_WRITE(0x33002000+0xC4, 0xFFFFFFFF);
	
	//REG32_WRITE(0x33000080, 0xFFFFFFFF); //if enable hprintf will hang
		A55_IRQ_ENABLE(169);
		A55_IRQ_ENABLE(234);
		REG32_WRITE(SYS_CTRL_BASE+0x80, 0xFFFFFFFF); //enable parity, if enable hprintf will hang
		//hprintf(DEBUG, "SYS_CTRL_BASE+0x80:%x\r\n", REG32_READ(SYS_CTRL_BASE+0x80));
		REG32_WRITE(SYS_CTRL_BASE+0x84, 0xFFFFFFFF); //enable parity, if enable hprintf will hang
		REG32_WRITE(SYS_CTRL_BASE+0x10c, 0xabcd1234); //if enable hprintf will hang
		//please ensure A_I2S0_CK force to 1 to enable safety error inject
#endif

	/* A55 234 irq */
/* case cannot test, detail please refer word doc */
#if 1 
	//irq 234:case 12,pcie30_4x_dbi_wdata_pty_intr
	hprintf(DEBUG, "pcie30_4x_dbi_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case a\r\n");
	//REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = PCIE30_4X_DBI_WDATA_PTY_INTR;
	while (!safety_irq_trigger) {
	}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "pcie30_4x_dbi_wdata_pty_intr test done\r\n");
	for (i = 0; i < 300000; i++)
		; //software delay to ensure irq happen before next test
#endif

/* case cannot test, detail please refer word doc */
#if 0
	//irq 234:case 13,pcie30_2x_dbi_wdata_pty_intr
	hprintf(DEBUG, "pcie30_2x_dbi_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case b\r\n");
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = PCIE30_2X_DBI_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "pcie30_2x_dbi_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

/* case cannot test, detail please refer word doc */
#if 0
	//irq 234:case 14,usb30_cfg_wdata_pty_intr
	hprintf(DEBUG, "usb30_cfg_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use secure R5 to run noc case z\r\n");
	//REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = USB30_CFG_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "usb30_cfg_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 15,sysnoc2safenoc_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<21));
	noc_irq_id = SYSNOC2SAFENOC_WDATA_PTY_INTR;
	REG32_WRITE(0x20000000, 0x0);
	hprintf(DEBUG, "sysnoc2safenoc_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

	//irq 234:case 16,safenoc2sysnoc_wdata_pty_intr -> please refer a55_noc_irq_119_120_test

#if 0
	//irq 234:case 18,cpunoc2sysnoc_tar_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x110, REG32_READ(SYS_CTRL_BASE+0x110)|(1<<28)|(1<<29));
	noc_irq_id = CPUNOC2SYSNOC_TAR_WDATA_PTY_INTR;
	REG32_WRITE(0x30F00000, 0x0);
	hprintf(DEBUG, "cpunoc2sysnoc_tar_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 19,cpunoc2sysnoc_rdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<21));
	noc_irq_id = CPUNOC2SYSNOC_RDATA_PTY_INTR;
	data = REG32_READ(0x33410000);
	hprintf(DEBUG, "cpunoc2sysnoc_rdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 20,sysnoc2corenoc_tar_wdata_pty_intr
	hprintf(DEBUG, "sysnoc2corenoc_tar_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case e\r\n");
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = SYSNOC2CORENOC_TAR_WDATA_PTY_INTR;
	return;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "sysnoc2corenoc_tar_wdata_pty_intr test done\r\n");
	for(i = 0; i < 300000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 21,sysnoc2corenoc_rdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x114, REG32_READ(SYS_CTRL_BASE+0x114)|(1<<22));
	noc_irq_id = SYSNOC2CORENOC_RDATA_PTY_INTR;
	data = REG32_READ(0x33100000);
	hprintf(DEBUG, "sysnoc2corenoc_rdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 23 and 26,usb20_cfg_wdata_pty_intr
	hprintf(DEBUG, "usb20_cfg_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "sysnoc_apb_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case z\r\n");
	noc_irq_id = USB20_CFG_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "usb20_cfg_wdata_pty_intr test done\r\n");
	hprintf(DEBUG, "sysnoc_apb_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 24,sdemmc0_cfg_wdata_pty_intr
	hprintf(DEBUG, "sdemmc0_cfg_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case g\r\n");
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = SDEMMC0_CFG_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "sdemmc0_cfg_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 25,sdemmc1_cfg_wdata_pty_intr
	hprintf(DEBUG, "sdemmc1_cfg_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case h\r\n");
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = SDEMMC1_CFG_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "sdemmc1_cfg_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 26,sysnoc_apb_wdata_pty_intr
	hprintf(DEBUG, "sysnoc_apb_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case i\r\n");
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<23));
	noc_irq_id = SYSNOC_APB_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "sysnoc_apb_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

#if 0
	//irq 234:case 27,sysnoc_ddr0_p0_wdata_pty_intr
	REG32_WRITE(SYS_CTRL_BASE+0x110, REG32_READ(SYS_CTRL_BASE+0x110)|(1<<28)|(1<<29));
	noc_irq_id = SYSNOC_DDR0_P0_WDATA_PTY_INTR;
	REG32_WRITE(0x80000000, 0x0);
	hprintf(DEBUG, "sysnoc_ddr0_p0_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif
}

static void a55_noc_irq_119_120_test_ddr(void)
{
	u32 data, i;

	if (((u64)(&data) & 0x80000000) !=
	    0x80000000) { //this case must run on DDR
		hprintf(DEBUG, "Env error: this case A55 must run on DDR.\r\n");
		return;
	}

	REG32_WRITE(0xc0035008, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x330020e8, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002004, 0xFFFFFFFF); //relese software reset
	REG32_WRITE(0x33002000 + 0xC0, 0xFFFFFFFF);
	REG32_WRITE(0x33002000 + 0xC4, 0xFFFFFFFF);

	//REG32_WRITE(0x33000080, 0xFFFFFFFF); //if enable hprintf will hang
	A55_IRQ_ENABLE(119);
	A55_IRQ_ENABLE(120);
	//REG32_WRITE(SYS_CTRL_BASE+0x80, 0xFFFFFFFF); //enable parity, if enable hprintf will hang
	//hprintf(DEBUG, "SYS_CTRL_BASE+0x80:%x\r\n", REG32_READ(SYS_CTRL_BASE+0x80));
	REG32_WRITE(SYS_CTRL_BASE + 0x84,
		    0xFFFFFFFF); //enable parity, if enable hprintf will hang
	REG32_WRITE(SYS_CTRL_BASE + 0x10c,
		    0xabcd1234); //if enable hprintf will hang
	//please ensure A_I2S0_CK force to 1 to enable safety error inject

/* irq 119 test cases */
/* case cannot test, detail please refer word doc */
#if 0 
	//irq 119:case 3,safe2sys_dupl_wdata_pty_intr
	hprintf(DEBUG, "safe2sys_dupl_rdata_pty_intr test start\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x130, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x130)|(1<<26)|(1<<27)|(1<<28)|(1<<29));
	noc_irq_id = SAFE2SYS_DUPL_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "safe2sys_dupl_rdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
	/* need manual release safety R5 and issue 0x80000000 address write */
#endif

/* case cannot test, detail please refer word doc */
#if 0
	//irq 119:case 4,safe2sys_main_wdata_pty_intr
	hprintf(DEBUG, "safe2sys_main_wdata_pty_intr test start\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x130, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x130)|(1<<26)|(1<<27)|(1<<28)|(1<<29));
	noc_irq_id = SAFE2SYS_MAIN_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "safe2sys_main_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
	/* need manual release safety R5 and issue 0x80000000 address write */
#endif

/* case cannot test, detail please refer word doc */
#if 0 /*xiangning.he*/
	//irq 119:case 5,sram_dupl_rdata_pty_intr
	//hprintf(DEBUG, "sram_dupl_rdata_pty_intr test start\r\n");
	putc('X');
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x118,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x118) | (1 << 16));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x124,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x124) | (0xFFFF << 0));
	noc_irq_id = SRAM_DUPL_RDATA_PTY_INTR;
	data = REG32_READ(0x18000000);
	while (!safety_irq_trigger) {
		putc('-');
	}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "sram_dupl_rdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
#endif

/* case cannot test, detail please refer word doc */
#if 0 /*xiangning.he*/
	//irq 119:case 6,sram_main_rdata_pty_intr
	hprintf(DEBUG, "sram_main_rdata_pty_intr test start\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x118,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x118) | (1 << 16));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x124,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x124) | (0xFFFF << 0));
	data = REG32_READ(0x18000000);
	noc_irq_id = SRAM_MAIN_RDATA_PTY_INTR;
	while (!safety_irq_trigger) {
		putc('-');
	}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "sram_main_rdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
#endif

/* case cannot test, detail please refer word doc */
#if 0
	//irq 119:case 7,sram_dupl_wdata_pty_intr
	hprintf(DEBUG, "sram_dupl_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case 2\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) | (1 << 20) |
			    (1 << 21));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x124,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x124) | (0xFFFF << 0));
	noc_irq_id = SRAM_DUPL_WDATA_PTY_INTR;
	//REG32_WRITE(0x18000000, 0x0);
	while (!safety_irq_trigger)
		; //{putc('-');}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "sram_dupl_wdata_pty_intr test done\r\n");
	for (i = 0; i < 300000; i++)
		; //software delay to ensure irq happen before next test
		/* need manual release safety R5 and issue 0x18000000 address write */
#endif

/* case cannot test, detail please refer word doc */
#if 0
	//irq 119:case 8,sram_main_wdata_pty_intr
	hprintf(DEBUG, "sram_main_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case 2\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) | (1 << 20) |
			    (1 << 21));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x124,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x124) | (0xFFFF << 0));
	noc_irq_id = SRAM_MAIN_WDATA_PTY_INTR;
	while (!safety_irq_trigger) {
	}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "sram_main_wdata_pty_intr test done\r\n");
	for (i = 0; i < 300000; i++)
		; //software delay to ensure irq happen before next test
		/* need manual release safety R5 and issue 0x18000000 address write */
#endif

/* case cannot test, detail please refer word doc */
#if 0 
	//irq 119:case 9,qspi1_dupl_wdata_pty_intr
	hprintf(DEBUG, "qspi1_dupl_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case 3\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x130, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x130)|(1<<20)|(1<<21));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x124, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x124)|(0xFFFF<<0));
	noc_irq_id = QSPI1_DUPL_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "qspi1_dupl_wdata_pty_intr test done\r\n");
	for(i = 0; i < 300000; i++); //software delay to ensure irq happen before next test
	/* need manual release safety R5 and issue 0x14000000 address write */
#endif

// case cannot test, detail please refer word doc 
#if 0
	//irq 119:case 10,qspi0_dupl_wdata_pty_intr
	hprintf(DEBUG, "qspi0_dupl_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case 3\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) | (1 << 20) |
			    (1 << 21));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x124,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x124) | (0xFFFF << 0));
	noc_irq_id = QSPI0_DUPL_WDATA_PTY_INTR;
	while (!safety_irq_trigger) {
	}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "qspi0_dupl_wdata_pty_intr test done\r\n");
	for (i = 0; i < 300000; i++)
		; //software delay to ensure irq happen before next test
		/* need manual release safety R5 and issue 0x14000000 address write */
#endif

/* case cannot test, detail please refer word doc */
#if 0
	//irq 119:case 11,dma_cfg_main_wdata_pty_intr
	hprintf(DEBUG, "dma_cfg_main_wdata_pty_intr test start\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x130, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x130)|(1<<26)|(1<<27)|(1<<28)|(1<<29));
	noc_irq_id = DMA_CFG_MAIN_WDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "dma_cfg_main_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
	/* need manual release safety R5 and issue 0x14000000 address write */
#endif

/* case cannot test, detail please refer word doc */
#if 1
	//irq 119:case 12,r5_0_main_rdata_pty_intr
	hprintf(DEBUG, "r5_0_main_rdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case 4\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x130, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x130)|(1<<24)|(1<<25));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x124, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x124)|(0xFFFF<<0));
	noc_irq_id = R5_0_MAIN_RDATA_PTY_INTR;
	while(! safety_irq_trigger){}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "r5_0_main_rdata_pty_intr test done\r\n");
	for(i = 0; i < 300000; i++); //software delay to ensure irq happen before next test
	/* need manual release safety R5 and issue 0x18000000 address write */
#endif

/* irq 120 test cases */
/* case cannot test, detail please refer word doc  */
#if 0
	//irq 120:case 1,sys2safe_dupl_rdata_pty_intr
	hprintf(DEBUG, "sys2safe_dupl_rdata_pty_intr test start\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x134, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x134)|(1<<6)|(1<<7));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x124, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x124)|(0xFFFF<<0));
	noc_irq_id = SYS2SAFE_DUPL_RDATA_PTY_INTR;
	data = REG32_READ(0x18000000);
	while(! safety_irq_trigger){putc('-');}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "sys2safe_dupl_rdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

/* case cannot test, detail please refer word doc */
#if 0 
	//irq 120:case 2,sys2safe_main_rdata_pty_intr
	hprintf(DEBUG, "sys2safe_dupl_rdata_pty_intr test start\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x134, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x134)|(1<<6)|(1<<7));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x124, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x124)|(0xFFFF<<0));
	noc_irq_id = SYS2SAFE_DUPL_RDATA_PTY_INTR;
	data = REG32_READ(0x14000000);
	while(! safety_irq_trigger){putc('-');}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "sys2safe_dupl_rdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

/* case cannot test, detail please refer word doc */
#if 0 
	//irq 120:case 3,sys2safe_dupl_wdata_pty_intr
	hprintf(DEBUG, "sys2safe_dupl_wdata_pty_intr test start\r\n");
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<24));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x124, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x124)|(0xFFFF<<0));
	noc_irq_id = SYS2SAFE_DUPL_WDATA_PTY_INTR;
	REG32_WRITE(0x18000000, 0x0);
	while(! safety_irq_trigger){putc('-');}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "sys2safe_dupl_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

/* case cannot test, detail please refer word doc */
#if 0 
	//irq 120:case 4,sys2safe_main_wdata_pty_intr
	hprintf(DEBUG, "sys2safe_dupl_wdata_pty_intr test start\r\n");
	REG32_WRITE(SYS_CTRL_BASE+0x118, REG32_READ(SYS_CTRL_BASE+0x118)|(1<<24));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x124, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x124)|(0xFFFF<<0));
	noc_irq_id = SYS2SAFE_DUPL_WDATA_PTY_INTR;
	REG32_WRITE(0x18000000, 0x0);
	while(! safety_irq_trigger){putc('-');}
	safety_irq_trigger = 0;
	putc('T');
	hprintf(DEBUG, "sys2safe_dupl_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
#endif

/* case cannot test, detail please refer word doc */
#if 0/*xiangning.he*/
	//irq 120:case 5,pbus0_dupl_wdata_pty_intr
	hprintf(DEBUG, "pbus0_dupl_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case 5\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) | (1 << 20) |
			    (1 << 21));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x124,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x124) | (0xFFFF << 0));
	noc_irq_id = PBUS0_DUPL_WDATA_PTY_INTR;
	while (!safety_irq_trigger) {
	}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "pbus0_dupl_wdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
		/* need manual release safety R5 and issue 0x20000000 address write */
#endif

/* case cannot test, detail please refer word doc */
#if 0/*xiangning.he*/
	//irq 120:case 6,pbus0_dupl_wdata_pty_intr
	hprintf(DEBUG, "pbus0_main_wdata_pty_intr test start\r\n");
	hprintf(DEBUG, "please use safety R5 to run noc case 5\r\n");
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x130,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x130) | (1 << 20) |
			    (1 << 21));
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x124,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x124) | (0xFFFF << 0));
	noc_irq_id = PBUS0_MAIN_WDATA_PTY_INTR;
	while (!safety_irq_trigger) {
	}
	safety_irq_trigger = 0;
	hprintf(DEBUG, "pbus0_main_wdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
		/* need manual release safety R5 and issue 0x20000000 address write */
#endif

/* special case */
/* case cannot test, detail please refer word doc */
#if 0
	//irq 234:case 16,safenoc2sysnoc_wdata_pty_intr
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE+0x130, REG32_READ(SEC_SAFE_SYS_CTRL_BASE+0x130)(1<<26)|(1<<27)|(1<<28)|(1<<29));
	noc_irq_id = SAFENOC2SYSNOC_WDATA_PTY_INTR;
	//REG32_WRITE(0x20000000, 0x0);
	hprintf(DEBUG, "safenoc2sysnoc_wdata_pty_intr test done\r\n");
	for(i = 0; i < 3000; i++); //software delay to ensure irq happen before next test
	/* need manual release safety R5 and issue address access */
#endif

#if 1
	//irq 234:case 28,sysnoc_ddr0_p1_wdata_pty_intr
	REG32_WRITE(SEC_SAFE_SYS_CTRL_BASE + 0x114,
		    REG32_READ(SEC_SAFE_SYS_CTRL_BASE + 0x114) | (1 << 18));
	noc_irq_id = SYSNOC_DDR0_P1_WDATA_PTY_INTR;
	//REG32_WRITE(0x80000000, 0x0);
	hprintf(DEBUG, "sysnoc_ddr0_p1_wdata_pty_intr test done\r\n");
	for (i = 0; i < 3000; i++)
		; //software delay to ensure irq happen before next test
		/* need manual release safety R5 and issue address access */
#endif
}

IRQ_DEFINE(169, a55_noc_irq_169_234_handler, 169, "a55 noc irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(234, a55_noc_irq_169_234_handler, 234, "a55 noc irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(119, a55_noc_irq_119_120_handler, 119, "a55 noc irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(120, a55_noc_irq_119_120_handler, 120, "a55 noc irq",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);

#if defined(CONFIG_R5_SAFETY)
void safety_sdma_data_transfer(u32 src, u32 dst, u32 byte_size)
{
	u32 data;

	REG32_WRITE(0xc0035008,
		    REG32_READ(0xc0035008) & (~(1 << 6))); // enable dma reset
	for (data = 0; data < 1000; data++)
		;
	REG32_WRITE(0xc0035008,
		    REG32_READ(0xc0035008) | (1 << 6)); //disable dma reset

	/* SDMA setting */
	hprintf(DEBUG, "source addr:0x%x\r\n", src);
	hprintf(DEBUG, "destination addr:0x%x\r\n", dst);
	hprintf(DEBUG, "transfer size(bytes):0x%x\r\n", byte_size);

	/* DMAC_CFGREG */
	REG32_WRITE(DMAC_CFGREG, REG32_READ(DMAC_CFGREG) | 0x1); //enable dma

	/* DMA_ChEnReg:select available channel */
	data = REG32_READ(DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 1))) |
	       (0x0 << 1); //CH2_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE, write only bit
	data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE, write only bit
	REG32_WRITE(DMAC_CHENREG, data);

	/* CHX_CFG */
	data = REG32_READ(DMAC_CHX_CFG(1));
	data = (data & (~(0x3 << 0))) |
	       (0x1
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x1
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	REG32_WRITE(DMAC_CHX_CFG(1), data);

	data = REG32_READ(DMAC_CHX_CFG(1) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x0
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x1
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	//data = (data&(~(0xF<<12))) | (0x4<<12); //DST_PER,assign a hardware handshaking interface to the destination of channelx
	data = (data & (~(0xF << 23))) |
	       (0x0 << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0x0 << 27); //DST_OSR_LMT,destination outstanding request limit
	REG32_WRITE(DMAC_CHX_CFG(1) + 0x4, data);

	/* CHX_SAR */
	REG32_WRITE(DMAC_CHX_SAR(1), src); //source addr
	REG32_WRITE(DMAC_CHX_SAR(1) + 0x4, 0);

	/* CHX_DAR */
	REG32_WRITE(DMAC_CHX_DAR(1), dst); //destination addr
	REG32_WRITE(DMAC_CHX_DAR(1) + 0x4, 0);

	/* CHX_BLOCK_TS: block transfer size */
	REG32_WRITE(DMAC_CHX_BLOCK_TS(1), byte_size / 4 - 1);
	REG32_WRITE(DMAC_CHX_BLOCK_TS(1) + 0x4, 0);

	/* CHX_CTRL */
	data = REG32_READ(DMAC_CHX_CTL(1));
	data = (data & (~(0x1 << 4))) |
	       (0x0
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x0
		<< 6); //destination address increment: 0(increment), 1(no change)
	data = (data & (~(0x7 << 8))) |
	       (0x2 << 8); //source transfer width: 2(32 bits)
	data = (data & (~(0x7 << 11))) |
	       (0x2 << 11); //destination transfer width: 2(32 bits)
	data = (data & (~(0xF << 14))) |
	       (0x0
		<< 14); //source burst transaction length: 0(burst length = 1)
	data = (data & (~(0xF << 18))) |
	       (0x0
		<< 18); //destination burst transaction length: 0(burst length = 1)
	REG32_WRITE(DMAC_CHX_CTL(1), data);

	data = REG32_READ(DMAC_CHX_CTL(1) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 6))) | (0x1 << 6); //source burst length enable
	data = (data & (~(0xFF << 7))) | (0x1 << 7); //source burst length
	data = (data & (~(0x1 << 15))) |
	       (0x1 << 15); //destination burst length enable
	data = (data & (~(0xFF << 16))) |
	       (0x0 << 16); //destination burst length
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	REG32_WRITE(DMAC_CHX_CTL(1) + 0x4, data);

	/* DMA_ChEnReg:select available channel */
	data = REG32_READ(DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x1 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 1))) |
	       (0x1 << 1); //CH2_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
	REG32_WRITE(DMAC_CHENREG, data);
}

void safety_r5_read(u32 addr)
{
	//irq 119:case 12,r5_0_main_rdata_pty_intr
	u32 data;

	data = REG32_READ(addr);
}

void safety_r5_write(u32 addr, u32 val)
{
	//irq 119:case 12,r5_0_main_rdata_pty_intr
	u32 data;

	REG32_WRITE(addr, val);
}

/*
 * safety r5 help to clean noc irq to avoid A55 clean irq fail
 */
void r5_clean_noc_irq(void)
{
	REG32_WRITE(SYS_CTRL_BASE + 0x88, 0xFFFFFFFF); //clean all irq
	REG32_WRITE(SYS_CTRL_BASE + 0x8C, 0xFFFFFFFF); //clean all irq
}

#endif

static void menu(void)
{
	putc('\r');
	putc('\n');
	hprintf(TSENV, "0......noc test\r\n");

	hprintf(TSENV, "d......dump memory\r\n");
	hprintf(TSENV, "s......set32 memory\r\n");
	hprintf(TSENV, "q......quit\r\n");
}

static void logo(void)
{
	putc('\r');
	putc('\n');
	putc('n');
	putc('o');
	putc('c');
	putc('>');
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
#ifdef CONFIG_A55
		case '0':
			a55_noc_irq_169_234_test_sram(); //A55 run on SRAM
			break;
		case '1':
			a55_noc_irq_119_120_test_ddr(); //A55 run on DDR
			break;
		case 'z':
			a55_noc_irq_169_234_test_ddr(); //A55 run on SRAM
			break;
#endif
#ifdef CONFIG_R5_SECURE
		case 'z':
			secure_noc_irq_169_234_test_ddr(); //A55 run on SRAM
			break;
#endif
#ifdef CONFIG_R5_SAFETY
		case 'z':
			safety_noc_irq_169_234_test_ddr(); //A55 run on SRAM
			break;
		case '2':
			safety_sdma_data_transfer(0x80000000, 0x18000000,
						  8); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case '3':
			safety_sdma_data_transfer(0x80000000, 0x00000000,
						  8); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case '4':
			safety_r5_read(0xc0020000);
			//r5_clean_noc_irq();
			break;
		case '5':
			safety_sdma_data_transfer(0x80000000, 0x20000000,
						  8); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case 'a':
			safety_r5_write(0x30600000,
					0x0); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case 'b':
			safety_r5_write(0x30A00000,
					0x0); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case 'c':
			safety_r5_write(0x30E00000,
					0x0); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case 'e':
			safety_r5_write(0x33100000,
					0x0); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case 'f':
			safety_r5_write(0x30300000,
					0x0); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case 'g':
			safety_r5_write(0x30400000,
					0x0); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case 'h':
			safety_r5_write(0x30500000,
					0x0); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;
		case 'i':
			safety_r5_write(0x30300000,
					0x0); //use to trigger noc irq
			//r5_clean_noc_irq();
			break;

#endif
		case 'd':
			dumpMem(0);
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

void nocMain(int para)
{
	if (para) {
		hprintf(TSENV, "auto test err!!!!\r\n");
	} else {
		hprintf(TSENV, "enter noc test!\r\n");
		TstLoop();
	}
}

CMD_DEFINE(noc, nocMain, 0, "noc test", 0);
