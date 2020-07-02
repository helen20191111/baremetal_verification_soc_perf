/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _UART_H
#define _UART_H

#define TXDREADY (1 << 6)
#define RXDREADY (1)

//1000000/9600/16 = 6.5104166666666666666666666666667

#define UART_BASE_ADDR UART2_BASE_ADDR

#define UART_REG_DLL 0x0
#define UART_REG_THR 0x0
#define UART_REG_RBR 0x0
#define UART_REG_DLH 0x4
#define UART_REG_IER 0x4
#define UART_REG_FCR 0x8
#define UART_REG_IIR 0x8
#define UART_REG_LCR 0xC
#define UART_REG_MCR 0x10
#define UART_REG_LSR 0x14
#define UART_REG_MSR 0x18
//#define UART_REG_SCR			(   UART_BASE_ADDR + 0x1C  )
//#define UART_REG_LPDLL			(   UART_BASE_ADDR + 0x20  )
//#define UART_REG_LPDLH			(   UART_BASE_ADDR + 0x24  )
//#define UART_REG_SRBR_BASE		(   UART_BASE_ADDR + 0x30  )
//#define UART_REG_STHR_BASE		(   UART_BASE_ADDR + 0x30  )
//#define UART_REG_FAR			(   UART_BASE_ADDR + 0x70  )
//#define UART_REG_TFR			(   UART_BASE_ADDR + 0x74  )
//#define UART_REG_RFW			(   UART_BASE_ADDR + 0x78  )
//#define UART_REG_USR			(   UART_BASE_ADDR + 0x7C  )
//#define UART_REG_TFL			(   UART_BASE_ADDR + 0x80  )
//#define UART_REG_RFL			(   UART_BASE_ADDR + 0x84  )
//#define UART_REG_SRR			(   UART_BASE_ADDR + 0x88  )
//#define UART_REG_SRTS			(   UART_BASE_ADDR + 0x8C  )
//#define UART_REG_SBCR			(   UART_BASE_ADDR + 0x90  )
//#define UART_REG_SDMAM			(   UART_BASE_ADDR + 0x94  )
//#define UART_REG_SFE			(   UART_BASE_ADDR + 0x98  )
//#define UART_REG_SRT			(   UART_BASE_ADDR + 0x9C  )
//#define UART_REG_STET			(   UART_BASE_ADDR + 0xA0  )
#define UART_REG_HTX 0xA4
#define UART_REG_DMASA 0xA8
//#define UART_REG_TCR			(   UART_BASE_ADDR + 0xAC  )
//#define UART_REG_DE_EN			(   UART_BASE_ADDR + 0xB0  )
//#define UART_REG_RE_EN			(   UART_BASE_ADDR + 0xB4  )
//#define UART_REG_DET			(   UART_BASE_ADDR + 0xB8  )
//#define UART_REG_TAT			(   UART_BASE_ADDR + 0xBC  )
#define UART_REG_DLF 0xC0
//#define UART_REG_RAR			(   UART_BASE_ADDR + 0xC4  )
//#define UART_REG_TAR			(   UART_BASE_ADDR + 0xC8  )
//#define UART_REG_LCR_EXT		(   UART_BASE_ADDR + 0xCC  )
#define UART_REG_PROT_LEVEL 0xD0
//#define UART_REG_TIMEOUT_RST	(   UART_BASE_ADDR + 0xD4  )
//#define UART_REG_CPR			(   UART_BASE_ADDR + 0xF4  )
//#define UART_REG_UCV			(   UART_BASE_ADDR + 0xF8  )
//#define UART_REG_CTR			(   UART_BASE_ADDR + 0xFC  )

//#define UART_REG_SRBR(n)		(UART_REG_SRBR_BASE+(n)*4)
//#define UART_REG_STHR(n)		(UART_REG_STHR_BASE+(n)*4)

#define MAX_BAUD (921600)
#define MIN_BAUD (2400)

#define UART_EVEN_PARITY (1)
#define UART_ODD_PARITY (0)

#define FIFO_DEPTH (32)
#define RX_FIFO_FULL (1 << 4)
#define RX_FIFO_NOT_EMPTY (1 << 3)
#define NO_INTERRUPT_PENDING (0x1)

#define DLF_SIZE (6)

/* DMA register */
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

#define UART_SRC_CLK (25000000UL)
//#define UART_SRC_CLK		(5000000UL)

typedef enum _stop_bit {
	STOP_1_BIT,
	STOP_2_BIT,
} stop_bit;

typedef enum _data_bit {
	DATA_5_BIT,
	DATA_6_BIT,
	DATA_7_BIT,
	DATA_8_BIT,
} data_bit;

typedef enum _uart_irq_id {
	UART_IRQ_ERBFI,
	UART_IRQ_ETBEI,
	UART_IRQ_ELSI,
	UART_IRQ_EDSSI,
	UART_IRQ_ELCOLR,
	UART_IRQ_RSVD1,
	UART_IRQ_RSVD2,
	PTIME,
	UART_IRQ_ALL,
} uart_irq_id;

void uart_pinmux_init(unsigned int reg_base);
void uart_init(unsigned int reg_base);
void uart_baud_set(unsigned int reg_base, u32 baud);
void uart_stop_bit_set(unsigned int reg_base, stop_bit stop_bit);
void uart_data_bit_set(unsigned int reg_base, data_bit data_bit);
void uart_parity_enable(unsigned int reg_base, u32 enable);
void uart_fifo_enable(unsigned int reg_base, bool_enum enable);
void uart_stick_parity_enable(unsigned int reg_base, bool_enum enable);
void uart_even_parity_set(unsigned int reg_base, bool_enum even_parity);
void uart_break_control_enable(unsigned int reg_base, bool_enum enable);
void rx_fifo_reset(unsigned int reg_base);
void tx_fifo_reset(unsigned int reg_base);
void uart_rts_set(unsigned int reg_base, bool_enum rts);
void uart_loopback_set(unsigned int reg_base, bool_enum loopback);
void uart_afc_set(unsigned int reg_base, bool_enum afce);
void uart_irq_set(unsigned int reg_base, uart_irq_id irq, bool_enum enable);
#endif /*_UART_H*/
