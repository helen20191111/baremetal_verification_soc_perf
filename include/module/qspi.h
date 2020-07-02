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

#ifndef _QSPI_H
#define _QSPI_H

//#ifdef CONFIG_R5_SECURE
//#define QSPI0_BASE_ADDR     0x10000000
//#else
//#define QSPI0_BASE_ADDR     0x00000000
//#endif
//#define QSPI1_BASE_ADDR     0x14000000

// register utility defines ----------------------------------------------------------------

#define QSPI_INTERRUPT_TXEI (1 << 0)
#define QSPI_INTERRUPT_TXOI (1 << 1)
#define QSPI_INTERRUPT_RXUI (1 << 2)
#define QSPI_INTERRUPT_RXOI (1 << 3)
#define QSPI_INTERRUPT_RXFI (1 << 4)
#define QSPI_INTERRUPT_MSTI (1 << 5)
#define QSPI_INTERRUPT_XRXOI (1 << 6)
#define QSPI_INTERRUPT_ALL (0x7f)

#define QSPI_REG_TOP_CFG_R (LSP0_CRM_ADDR + 0x10)

#define DMA_CH1 1
#define DMA_CH2 2

#define QSPI_REG_CTRLR0 (0x0)
#define QSPI_REG_CTRLR1 (0x4)
#define QSPI_REG_SSIENR (0x8)
#define QSPI_REG_MWCR (0xC)
#define QSPI_REG_SER (0x10)
#define QSPI_REG_BAUDR (0x14)
#define QSPI_REG_TXFTLR (0x18)
#define QSPI_REG_RXFTLR (0x1C)
#define QSPI_REG_TXFLR (0x20)
#define QSPI_REG_RXFLR (0x24)
#define QSPI_REG_SR (0x28)
#define QSPI_REG_IMR (0x2C)
#define QSPI_REG_ISR (0x30)
#define QSPI_REG_DMACR (0x4C)
#define QSPI_REG_DMATDLR (0x50)
#define QSPI_REG_DMARDLR (0x54)
#define QSPI_REG_DR0 (0x60)
#define QSPI_REG_SPI_CTRLR0 (0xF4)
#define QSPI_REG_DDR_DRIVE_EDGE (0xF8)

#define QSPI_CTRLR0_SPI_HYPERBUS_EN (24)
#define QSPI_CTRLR0_SPI_FRF (22)
#define QSPI_CTRLR0_CFS (16)
#define QSPI_CTRLR0_SSTE (14)
#define QSPI_CTRLR0_SRL (13)
#define QSPI_CTRLR0_SLV_OE (12)
#define QSPI_CTRLR0_TMOD (10)
#define QSPI_CTRLR0_SCPOL (9)
#define QSPI_CTRLR0_SCPH (8)
#define QSPI_CTRLR0_FRF (6)
#define QSPI_CTRLR0_DFS (0)

#define QSPI_TXFTLR_TXFTLR (16)

#define QSPI_SPICTRLR0_CLK_STRETCH_EN (30)
#define QSPI_SPICTRLR0_XIP_PREFETCH_EN (29)
#define QSPI_SPICTRLR0_XIP_MBL (26)
#define QSPI_SPICTRLR0_SPI_RXDS_SIG_EN (25)
#define QSPI_SPICTRLR0_SPI_DM_EN (24)
#define QSPI_SPICTRLR0_SSIC_XIP_CONT_XFER_EN (21)
#define QSPI_SPICTRLR0_XIP_INST_EN (20)
#define QSPI_SPICTRLR0_DFS_HC (19)
#define QSPI_SPICTRLR0_SPI_RXDS_EN (18)
#define QSPI_SPICTRLR0_INST_DDR_EN (17)
#define QSPI_SPICTRLR0_SPI_DDR_EN (16)
#define QSPI_SPICTRLR0_WAIT_CYCLES (11)
#define QSPI_SPICTRLR0_INST_L (8)
#define QSPI_SPICTRLR0_XIP_MD_BIT_EN (7)
#define QSPI_SPICTRLR0_ADDR_L (2)
#define QSPI_SPICTRLR0_TRANS_TYPE (0)

#define QSPI_RW_TEST_LEN (16)
#define QSPI_SDMA_TEST_LEN (32)
#define FLASH_ID_LEN (3)
#define QSPI_DFS_8_TEST_LEN (32)
#define FLASH_UNIQUE_ID_LEN (16)
#define FLASH_PP_MAX_LEN (64)

#define QSPI_TX_RX (0)
#define QSPI_TX_ONLY (1)
#define QSPI_RX_ONLY (2)
#define QSPI_EEPROM_READ (3)

#define QSPI_STD_SPI (0)
#define QSPI_DUAL_SPI (1)
#define QSPI_QUAD_SPI (2)
#define QSPI_OCTAL_SPI (3)

#define QSPI_ADDR_LEN_NO (0x0)
#define QSPI_ADDR_LEN_24 (0x6)
#define QSPI_ADDR_LEN_32 (0x8)

#define QSPI_CMD_LEN_NO (0x0)
#define QSPI_CMD_LEN_4 (0x1)
#define QSPI_CMD_LEN_8 (0x2)
#define QSPI_CMD_LEN_16 (0x3)

#define CMD_STD_ADDR_STD (0x0)
#define CMD_STD_ADDR_FRF (0x1)
#define CMD_FRF_ADDR_FRF (0x2)

#define QSPI_DFS_8 (8)
#define QSPI_DFS_32 (32)

#define FLASH_QUAD_EN (0x1)
#define FLASH_QUAD_DI (0x0)

#define FLASH_CMD_4BYTE_EN (0xB7)
#define FLASH_CMD_4BYTE_DI (0xE9)
#define FLASH_CMD_WR_EN (0x06)
#define FLASH_CMD_WR_EN_VSR (0x50)
#define FLASH_CMD_WR_DI (0x04)
#define FLASH_CMD_QPI_EN (0x38)
#define FLASH_CMD_QPI_DI (0xFF)
#define FLASH_CMD_PES (0x75)
#define FLASH_CMD_PER (0x7A)
#define FLASH_CMD_BURST (0x77)
#define FLASH_CMD_SECTOR_ER (0xd8)
#define FLASH_CMD_SUBSECTOR_ER_32 (0x52)
#define FLASH_CMD_SUBSECTOR_ER_4 (0x20)
#define FLASH_CMD_CHIP_ER (0x60)
#define FLASH_CMD_PAGE_PROG (0x02)
#define FLASH_CMD_QUAD_PAGE_PROG (0x32)
#define FLASH_CMD_MEMORY_RD (0x03)
#define FLASH_CMD_MEMORY_FAST_RD (0x0B)
#define FLASH_CMD_MEMORY_DUAL_OUT_FAST_RD (0x3B)
#define FLASH_CMD_MEMORY_DUAL_IO_FAST_RD (0xBB)
#define FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD (0x6B)
#define FLASH_CMD_MEMORY_QUAD_IO_FAST_RD (0xEB)
#define FLASH_CMD_MEMORY_QUAD_IO_WORD_FAST_RD (0xE7)
#define FLASH_CMD_MEMORY_DTR_QUAD_OUT_FAST_RD (0x6D)
#define FLASH_CMD_MEMORY_DTR_QUAD_IO_FAST_RD (0xED)
#define FLASH_CMD_REG_ENHANCED_RD (0x65)
#define FLASH_CMD_REG_ENHANCED_WR (0x61)
#define FLASH_CMD_REG_STATUS_RD (0x05)
#define FLASH_CMD_REG_STATUS_RD_HIGH (0x35)
#define FLASH_CMD_REG_STATUS_RD_QPI (0x15)
#define FLASH_CMD_REG_STATUS_WR (0x01)
#define FLASH_CMD_RD_ID_MD (0x90)
#define FLASH_CMD_RD_ID_MMC (0x9F)
#define FLASH_CMD_RD_ID_UNIQUE (0x4B)

/* 本测试用例使用的flash起始地址 */
#define FLASH_BASE_ADDR (0x0)
#define FLASH_BASE_ADDR_4byte (0x0)
#define FLASH_BASE_ADDR_4K (0x1000)
#define FLASH_BASE_ADDR_32K (0x8000)
#define FLASH_BASE_ADDR_64K (0x10000)

/* DMA register */
#define TRANSFER_SIZE (2)
#ifdef CONFIG_A55
#define DMA_BASE_ADDR (0x33200000) //GDMA
#define TOP_CRM_BASE (0x33002000)
#define GDMA_RESETREG (0xE8)
#else
#define DMA_BASE_ADDR (0xc0020000) //SDMA
#endif

#define DMAC_CFGREG (DMA_BASE_ADDR + 0x10)
#define DMAC_CHENREG (DMA_BASE_ADDR + 0x18)
#define DMAC_INTSTATUSREG (DMA_BASE_ADDR + 0x30)
#define DMAC_COMMONREG_INTCLEARREG (DMA_BASE_ADDR + 0x38)
#define DMAC_COMMONREG_INTSTATUS_ENABLEREG (DMA_BASE_ADDR + 0x40)
#define DMAC_COMMONREG_INTSIGNAL_ENABLEREG (DMA_BASE_ADDR + 0x48)
#define DMAC_COMMONREG_INTSTATUSREG (DMA_BASE_ADDR + 0x50)
#define DMAC_RESETREG (DMA_BASE_ADDR + 0x58)
#define DMAC_LOWPOWER_CFGREG (DMA_BASE_ADDR + 0x60)

#define DMAC_CHX_SAR(i) (DMA_BASE_ADDR + 0x100 + (i - 1) * 0x100)
#define DMAC_CHX_DAR(i) (DMA_BASE_ADDR + 0x108 + (i - 1) * 0x100)
#define DMAC_CHX_BLOCK_TS(i) (DMA_BASE_ADDR + 0x110 + (i - 1) * 0x100)
#define DMAC_CHX_CTL(i) (DMA_BASE_ADDR + 0x118 + (i - 1) * 0x100)
#define DMAC_CHX_CFG(i) (DMA_BASE_ADDR + 0x120 + (i - 1) * 0x100)
#define DMAC_CHX_LLP(i) (DMA_BASE_ADDR + 0x128 + (i - 1) * 0x100)
#define DMAC_CHX_STATUSREG(i) (DMA_BASE_ADDR + 0x130 + (i - 1) * 0x100)
#define DMAC_CHX_SWHSSRCREG(i) (DMA_BASE_ADDR + 0x138 + (i - 1) * 0x100)
#define DMAC_CHX_SWHSDSTREG(i) (DMA_BASE_ADDR + 0x140 + (i - 1) * 0x100)
#define DMAC_CHX_BLK_TFR_RESUMEREQREG(i)                                       \
	(DMA_BASE_ADDR + 0x148 + (i - 1) * 0x100)
#define DMAC_CHX_BLK_TFR_RESUMEREQREG(i)                                       \
	(DMA_BASE_ADDR + 0x148 + (i - 1) * 0x100)
#define DMAC_CHX_AXI_IDREG(i) (DMA_BASE_ADDR + 0x150 + (i - 1) * 0x100)
#define DMAC_CHX_AXI_QOSREG(i) (DMA_BASE_ADDR + 0x158 + (i - 1) * 0x100)
#define DMAC_CHX_INTSTATUS_ENABLEREG(i)                                        \
	(DMA_BASE_ADDR + 0x180 + (i - 1) * 0x100)
#define DMAC_CHX_INTSTATUS(i) (DMA_BASE_ADDR + 0x188 + (i - 1) * 0x100)
#define DMAC_CHX_INTSIGNAL_ENABLEREG(i)                                        \
	(DMA_BASE_ADDR + 0x190 + (i - 1) * 0x100)
#define DMAC_CHX_INTCLEARREG(i) (DMA_BASE_ADDR + 0x198 + (i - 1) * 0x100)

#pragma pack(1)
typedef struct {
	u32 dfs : 6;
	u32 frf : 2;
	u32 scph : 1;
	u32 scpol : 1;
	u32 sste : 1;
	u32 sckdv : 16;
	u32 slave : 4;
} s_qspi_conf;
#pragma pack()

#define MT25T_DEFAULT_AFTER_EARSE 0xff

#define BUFFER_CMP(buf1, buf2, size, ret_val)                                  \
	if (memcmp((buf1), (buf2), (size)) != 0) {                             \
		hprintf(DEBUG, "buffer compare faild: %s, %s, %d\r\n",         \
			__FILE__, __FUNCTION__, __LINE__);                     \
		return (ret_val);                                              \
	}

#define BUFFER_CMP_BYTE(buf, cmp, size, ret_val)                               \
	if (memcmpbyte((buf), (cmp), (size)) != 0) {                           \
		hprintf(DEBUG, "buffer compare faild: %s, %s, %d\r\n",         \
			__FILE__, __FUNCTION__, __LINE__);                     \
		return (ret_val);                                              \
	}

#define BUFFER_LENGTH(buf) (sizeof((buf)) / sizeof((buf[0])))

#endif
