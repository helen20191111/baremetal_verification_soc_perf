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

#ifndef _NOC_H
#define _NOC_H

#define SYS_CTRL_BASE (0x33000000)
#define SEC_SAFE_SYS_CTRL_BASE                                                 \
	(0x70035000) //(0xC0035000) -> for A55 (0x70035000)

//IRQ 169
#define CPU_P0_RDATA_PTY_INTR (0x1)
#define CPU_P1_RDATA_PTY_INTR (0x2)
#define CPU_PPI_RDATA_PTY_INTR (0x3)
#define GPU_P0_RDATA_PTY_INTR (0x4)
#define GPU_P1_RDATA_PTY_INTR (0x5)
#define GPU_CFG_WDATA_PTY_INTR (0x6)
#define ISP_RDATA_PTY_INTR (0x7)
#define VSP_AHB_SLAVE_WDATA_PTY_INTR (0x8)
#define CV_SLAVE_WDATA_PTY_INTR (0x9)
#define NET_SLAVE_WDATA_PTY_INTR (0xa)
#define CORENOC_APB_WDATA_PTY_INTR (0xb)
#define MIPI0_DATA_APB_SWITCH_PARITY_INTR (0xc)
#define MIPI1_DATA_APB_SWITCH_PARITY_INTR (0xd)
#define MIPI2_DATA_APB_SWITCH_PARITY_INTR (0xe)

//IRQ 234
#define CPU_ETR_RDATA_PTY_INTR (0X41)
#define CORESIGHT_DAP_RDATA_PTY_INTR (0X42)
#define GDMA_RDATA_PTY_INTR (0X43)
#define GDMA_CFG_WDATA_PTY_INTR (0X44)
#define GMAC0_RDATA_PTY_INTR (0X45)
#define GMAC0_WDATA_PTY_INTR (0X46)
#define GMAC1_RDATA_PTY_INTR (0X47)
#define PCIE30_4X_M_RDATA_PTY_INTR (0X48)
#define PCIE30_2X_M_RDATA_PTY_INTR (0X49)
#define PCIE30_4X_S_WDATA_PTY_INTR (0X4a)
#define PCIE30_2X_S_WDATA_PTY_INTR (0X4b)
#define PCIE30_4X_DBI_WDATA_PTY_INTR (0X4c)
#define PCIE30_2X_DBI_WDATA_PTY_INTR (0X4d)
#define USB30_CFG_WDATA_PTY_INTR (0X4E)
#define SYSNOC2SAFENOC_WDATA_PTY_INTR (0X4F)
#define SAFENOC2SYSNOC_WDATA_PTY_INTR (0X50)
#define SYSNOC2CPUNOC_TAR_RDATA_PTY_INTR (0X51)
#define CPUNOC2SYSNOC_TAR_WDATA_PTY_INTR (0X52)
#define CPUNOC2SYSNOC_RDATA_PTY_INTR (0X53)
#define SYSNOC2CORENOC_TAR_WDATA_PTY_INTR (0X54)
#define SYSNOC2CORENOC_RDATA_PTY_INTR (0X55)
#define CORENOC2SYSNOC_PTY_INTR (0X56)
#define USB20_CFG_WDATA_PTY_INTR (0X57)
#define SDEMMC0_CFG_WDATA_PTY_INTR (0X58)
#define SDEMMC1_CFG_WDATA_PTY_INTR (0X59)
#define SYSNOC_APB_WDATA_PTY_INTR (0X5A)
#define SYSNOC_DDR0_P0_WDATA_PTY_INTR (0X5B)
#define SYSNOC_DDR0_P1_WDATA_PTY_INTR (0X5C)

//IRQ 119
#define SAFE2SYS_DUPL_RDATA_PTY_INTR (0X81)
#define SAFE2SYS_MAIN_RDATA_PTY_INTR (0X82)
#define SAFE2SYS_DUPL_WDATA_PTY_INTR (0X83)
#define SAFE2SYS_MAIN_WDATA_PTY_INTR (0X84)
#define SRAM_DUPL_RDATA_PTY_INTR (0X85)
#define SRAM_MAIN_RDATA_PTY_INTR (0X86)
#define SRAM_DUPL_WDATA_PTY_INTR (0X87)
#define SRAM_MAIN_WDATA_PTY_INTR (0X88)
#define QSPI1_DUPL_WDATA_PTY_INTR (0X89)
#define QSPI0_DUPL_WDATA_PTY_INTR (0X8a)
#define DMA_CFG_MAIN_WDATA_PTY_INTR (0X8b)
#define R5_0_MAIN_RDATA_PTY_INTR (0X8c)

//IRQ 120
#define SYS2SAFE_DUPL_RDATA_PTY_INTR (0XC1)
#define SYS2SAFE_MAIN_RDATA_PTY_INTR (0XC2)
#define SYS2SAFE_DUPL_WDATA_PTY_INTR (0XC3)
#define SYS2SAFE_MAIN_WDATA_PTY_INTR (0XC4)
#define PBUS0_DUPL_WDATA_PTY_INTR (0XC5)
#define PBUS0_MAIN_WDATA_PTY_INTR (0XC6)

#endif
