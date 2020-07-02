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
#include <common.h>
#include <uart_sync.h>
#include <ut_test.h>
#include <int_num.h>
#include <module/qspi.h>

static const u32 qspi_regs_default_table1[] = {
	0x0040441f, 0x00000000, 0x00000001, 0x00000001, 0x00000001, 0x00000002,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000006, 0x0000007f,
	0x00000001, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0x3130322a
};

static const u32 qspi_regs_default_table2[] = { 0x00000000, 0x40004218,
						0x00000000, 0x00000000 };

extern unsigned char getc(void);
extern void putc(unsigned char);

u32 data[QSPI_RW_TEST_LEN] = { 0 };
u32 datard[QSPI_RW_TEST_LEN] = { 0 };
u32 datard_std[QSPI_RW_TEST_LEN] = { 0 };
u32 datard_dual[QSPI_RW_TEST_LEN] = { 0 };
u32 datard_quad[QSPI_RW_TEST_LEN] = { 0 };
u32 datard_octa[QSPI_RW_TEST_LEN] = { 0 };
u32 data_sdma[QSPI_SDMA_TEST_LEN] = { 0 };
u32 data_sdma_rd[QSPI_SDMA_TEST_LEN] = { 0 };
u8 data_dfs8[QSPI_DFS_8_TEST_LEN] = { 0 };
u8 data_dfs8_rd[QSPI_DFS_8_TEST_LEN] = { 0 };

u32 dbg_print = 0;
static volatile u32 int_flag = 0, int_test = 0;

reg_struct qspi_reg_default_val_table[] = {
	//{UART_REG_RBR, 0x0, 0x1FF, "UART_REG_RBR"},
	//{UART_REG_DLL, 0x0, 0xFF, "UART_REG_DLL"},
	//{UART_REG_THR, 0x0, 0xFF, "UART_REG_THR"},

};

reg_struct qspi_reg_rw_table[] = {
	//{UART_REG_RBR, 0x0, 0x1FF, "UART_REG_RBR"},
	//{UART_REG_DLL, 0x0, 0xFF, "UART_REG_DLL"},
	//{UART_REG_THR, 0x0, 0xFF, "UART_REG_THR"},

};

reg_struct qspi_reg_ro_table[] = {
	//{UART_REG_RBR, 0x0, 0x1FF, "UART_REG_RBR"},
	//{UART_REG_DLL, 0x0, 0xFF, "UART_REG_DLL"},
};

/*
 * when read qspi fifo(DR0) data should call this to enable endian trnasform
 */
static void qspi_en_endian(u32 base)
{
	if (QSPI0_BASE_ADDR == base) {
		REG32_WRITE(QSPI_REG_TOP_CFG_R,
			    REG32_READ(QSPI_REG_TOP_CFG_R) | (0x1 << 2));
	} else {
		REG32_WRITE(QSPI_REG_TOP_CFG_R,
			    REG32_READ(QSPI_REG_TOP_CFG_R) | (0x1 << 3));
	}

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_TOP_CFG_R=0x%08x\r\n", __FUNCTION__,
			REG32_READ(QSPI_REG_TOP_CFG_R));
	}
}
/*
 * when read/write register should call this api to disable endian transform
 */
static void qspi_de_endian(u32 base)
{
	if (QSPI0_BASE_ADDR == base) {
		REG32_WRITE(QSPI_REG_TOP_CFG_R,
			    REG32_READ(QSPI_REG_TOP_CFG_R) & (~(0x1 << 2)));
	} else {
		REG32_WRITE(QSPI_REG_TOP_CFG_R,
			    REG32_READ(QSPI_REG_TOP_CFG_R) & (~(0x1 << 3)));
	}

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_TOP_CFG_R=0x%08x\r\n", __FUNCTION__,
			REG32_READ(QSPI_REG_TOP_CFG_R));
	}
}

/* 
 * SSI Busy Flag.
 * When set, indicates that a serial transfer is in progress; when
 * cleared indicates that the DWC_ssi is idle or disabled. 
 */
static u32 qspi_is_busy(u32 base)
{
	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SR : 0x%08x = 0x%08x\r\n", __FUNCTION__,
			base + QSPI_REG_SR, REG32_READ(base + QSPI_REG_SR));
	}
	return (REG32_READ(base + QSPI_REG_SR) & 0x1);
}

u32 qspi_tf_is_empty(u32 base)
{
	u32 sr_val = REG32_READ(base + QSPI_REG_SR);
	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SR=0x%08x\r\n", __FUNCTION__,
			sr_val);
	}
	return ((sr_val >> 2) & 0x1);
}

u32 qspi_rf_not_empty(u32 base)
{
	u32 sr_val = REG32_READ(base + QSPI_REG_SR);
	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SR=0x%08x\r\n", __FUNCTION__,
			sr_val);
	}
	return ((sr_val >> 3) & 0x1);
}

/*
 * SSI Enable.
 * Enables and disables all DWC_ssi operations. When
 * disabled, all serial transfers are halted immediately. Transmit
 * and receive FIFO buffers are cleared when the device is
 * disabled. It is impossible to program some of the DWC_ssi
 * control registers when enabled. When disabled, the ssi sleep
 * output is set (after delay) to inform the system that it is safe
 * to remove the ssi_clk, thus saving power consumption in the system
 */
static void qspi_disable(u32 base)
{
	u32 ssienr_base = base + QSPI_REG_SSIENR;

	REG32_WRITE(ssienr_base, 0x0);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SSIENR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ssienr_base));
	}
}

static void qspi_enable(u32 base)
{
	u32 ssienr_base = base + QSPI_REG_SSIENR;

	REG32_WRITE(ssienr_base, 0x1);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SSIENR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ssienr_base));
	}
}

/*
 * eXecute In Place
 */
void qspi_exit_xip(u32 base)
{
	if (QSPI0_BASE_ADDR == base) {
		REG32_WRITE(QSPI_REG_TOP_CFG_R,
			    REG32_READ(QSPI_REG_TOP_CFG_R) & (~(0x1 << 0)));
	} else {
		REG32_WRITE(QSPI_REG_TOP_CFG_R,
			    REG32_READ(QSPI_REG_TOP_CFG_R) & (~(0x1 << 1)));
	}
	qspi_de_endian(base); //default to enable read/write register
}

void qspi_enable_xip(u32 base)
{
	if (QSPI0_BASE_ADDR == base) {
		REG32_WRITE(QSPI_REG_TOP_CFG_R,
			    REG32_READ(QSPI_REG_TOP_CFG_R) | (0x1 << 0));
	} else {
		REG32_WRITE(QSPI_REG_TOP_CFG_R,
			    REG32_READ(QSPI_REG_TOP_CFG_R) | (0x1 << 1));
	}
	qspi_en_endian(base);
}
/*
 * Frame Format.
 * Selects which serial protocol transfers the data.
 * Values:
 * 0x0 (SPI): Motorola SPI Frame Format
 * 0x1 (SSP): Texas Instruments SSP Frame Format
 * 0x2 (MICROWIRE): National Semiconductors Microwire Frame Format
 * 0x3 (RESERVED): Reserve
 */
static void qspi_set_frf(u32 base, u32 frf)
{
	u32 ctrlr0_base = base + QSPI_REG_CTRLR0;
	u32 val = 0;

	val = REG32_READ(ctrlr0_base);
	val = val & (~(0x3 << QSPI_CTRLR0_FRF));
	val = val | ((frf & 0x3) << QSPI_CTRLR0_FRF);
	REG32_WRITE(ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_CTRLR0=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ctrlr0_base));
	}
}

/*
 * SPI Frame Format
 * Selects data frame format for Transmitting/Receiving the
 * data. Bits only valid when SSIC_SPI_MODE is either set to
 * "Dual" or "Quad" or "Octal" mode.
 * Values:
 * 0x0 (SPI_STANDARD): Standard SPI Format
 * 0x1 (SPI_DUAL): Dual SPI Format
 * 0x2 (SPI_QUAD): Quad SPI Format
 * 0x3 (SPI_OCTAL): Octal SPI Format
 */
static void qspi_set_spi_frf(u32 base, u32 spi_frf)
{
	u32 ctrlr0_base = base + QSPI_REG_CTRLR0;
	u32 val = 0;

	val = REG32_READ(ctrlr0_base);
	val = val & (~(0x3 << QSPI_CTRLR0_SPI_FRF));
	val = val | ((spi_frf & 0x3) << QSPI_CTRLR0_SPI_FRF);
	REG32_WRITE(ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_CTRLR0=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ctrlr0_base));
	}
}

/*
 * Data Frame Size.
 * Selects the data frame length. When the data frame size is
 * programmed to be less than 32 bits, the receive data is
 * automatically right-justified by the receive logic, with the
 * upper bits of the receive FIFO zero-padded.
 * You must right-justify transmit data before writing into the
 * transmit FIFO. The transmit logic ignores the upper unused
 * bits when transmitting the data.
 * Note: When SSIC_SPI_MODE is set to "Dual", "Quad" or
 * "Octal" mode and SPI_FRF is not set to 2'b00:
 * DFS value must be a multiple of 2 if SPI_FRF = 01
 * DFS value must be multiple of 4 if SPI_FRF = 10
 * DFS value must be multiple of 8 if SPI_FRF = 11
 *
 * 0x3 (DFS_04_BIT): 04-bit serial data transfer
 * 0x4 (DFS_05_BIT): 05-bit serial data transfer
 * 0x5 (DFS_06_BIT): 06-bit serial data transfer
 * 0x6 (DFS_07_BIT): 07-bit serial data transfer
 * ......
 */
static void qspi_set_spi_dfs(u32 base, u32 dfs)
{
	u32 ctrlr0_base = base + QSPI_REG_CTRLR0;
	u32 val = 0;

	val = REG32_READ(ctrlr0_base);
	val = val & (~(0x1F));
	val = val | ((dfs - 1) & 0x1f);
	REG32_WRITE(ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_CTRLR0=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ctrlr0_base));
	}
}

/*
 * Transfer Mode.
 * Selects the mode of transfer for serial communication. This
 * field does not affect the transfer duplicity. Only indicates
 * whether the receive or transmit data are valid.
 * In transmit-only mode, data received from the external
 * device is not valid and is not stored in the receive FIFO
 * memory; it is overwritten on the next transfer.
 * In receive-only mode, transmitted data are not valid. After the
 * first write to the transmit FIFO, the same word is
 * retransmitted for the duration of the transfer.
 * In transmit-and-receive mode, both transmit and receive data
 * are valid. The transfer continues until the transmit FIFO is
 * empty. Data received from the external device are stored into
 * the receive FIFO memory, where it can be accessed by the
 * host processor.
 * Values:
 * 0x0 (TX_AND_RX): Transmit & Receive; Not Applicable
 *     in enhanced SPI operating mode
 * 0x1 (TX_ONLY): Transmit only mode; Or Write in
 *     enhanced SPI operating mode
 * 0x2 (RX_ONLY): Receive only mode; Or Read in
 *     enhanced SPI operating mode
 * 0x3 (EEPROM_READ): EEPROM Read mode; Not
 *     Applicable in enhanced SPI operating mode
 *
 */
static void qspi_set_mode(u32 base, u32 mode)
{
	u32 ctrlr0_base = base + QSPI_REG_CTRLR0;
	u32 val = 0;

	val = REG32_READ(ctrlr0_base);
	val = val & ~(0x3 << QSPI_CTRLR0_TMOD);
	val = val | (mode << QSPI_CTRLR0_TMOD);
	REG32_WRITE(ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_CTRLR0=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ctrlr0_base));
	}
}
/*
 * Serial Clock Polarity.
 * Valid when the frame format (FRF) is set to Motorola SPI.
 * Used to select the polarity of the inactive serial clock, which
 * is held inactive when the DWC_ssi master is not actively
 * transferring data on the serial bus.
 * Values:
 * 0x0 (INACTIVE_HIGH): Inactive state of serial clock is low
 * 0x1 (INACTIVE_LOW): Inactive state of serial clock is high
 */
static void qspi_set_scpol(u32 base, u32 scpol)
{
	u32 ctrlr0_base = base + QSPI_REG_CTRLR0;
	u32 val = 0;

	val = REG32_READ(ctrlr0_base);
	val = val & ~(0x1 << QSPI_CTRLR0_SCPOL);
	val = val | ((scpol & 0x1) << QSPI_CTRLR0_SCPOL);
	REG32_WRITE(ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_CTRLR0=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ctrlr0_base));
	}
}

/*
 * Serial Clock Phase.
 * Valid when the frame format (FRF) is set to Motorola SPI.
 * The serial clock phase selects the relationship of the serial
 * clock with the slave select signal.
 * When SCPH = 0, data are captured on the first edge of the
 * serial clock. When SCPH = 1, the serial clock starts toggling
 * one cycle after the slave select line is activated, and data are
 * captured on the second edge of the serial clock.
 * Values:
 * 0x1 (START_BIT): Serial clock toggles at start of first bit
 * 0x0 (MIDDLE_BIT): Serial clock toggles in middle of first bit
 */
static void qspi_set_scph(u32 base, u32 scph)
{
	u32 ctrlr0_base = base + QSPI_REG_CTRLR0;
	u32 val = 0;

	val = REG32_READ(ctrlr0_base);
	val = val & ~(0x1 << QSPI_CTRLR0_SCPH);
	val = val | ((scph & 0x1) << QSPI_CTRLR0_SCPH);
	REG32_WRITE(ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_CTRLR0=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ctrlr0_base));
	}
}

/*
 * Slave Select Enable Flag.
 * Each bit in this register corresponds to a slave select line
 * (ss_x_n) from the DWC_ssi master. When a bit in this
 * register is set (1), the corresponding slave select line from
 * the master is activated when a serial transfer begins. It
 * should be noted that setting or clearing bits in this register
 * have no effect on the corresponding slave select outputs until
 * a transfer is started. Before beginning a transfer, you should
 * enable the bit in this register that corresponds to the slave
 * device with which the master wants to communicate. When
 * not operating in broadcast mode, only one bit in this field
 * should be set.
 * 1: Selected
 * 0: Not Selected
 */
static void qspi_set_ser(u32 base, u32 ser)
{
	u32 ser_base = base + QSPI_REG_SER;
	u32 val = 0;

	val = REG32_READ(ser_base);
	val = val & ~(0x3);
	val = val | (ser & 0x3);
	REG32_WRITE(ser_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SER=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ser_base));
	}
}

/*
 * Slave Select Toggle Enable.
 * While operating in SPI mode with clock phase (SCPH) set to
 * 0, this register controls the behavior of the slave select line
 * (ss_*_n) between data frames.
 * Values:
 * 0x1 (TOGGLE_EN): ss_*_n line will toggle between
 *     consecutive data frames, with the serial clock (sclk) being
 *     held to its default value while ss_*_n is high
 * 0x0 (TOGGLE_DISABLE): ss_*_n will stay low and sclk
 *     will run continuously for the duration of the transfer
 */
static void qspi_set_spi_sste(u32 base, u32 sste)
{
	u32 ctrlr0_base = base + QSPI_REG_CTRLR0;
	u32 val = 0;

	val = REG32_READ(ctrlr0_base);
	val = val & ~(0x1 << QSPI_CTRLR0_SSTE);
	val = val | ((sste & 0x1) << QSPI_CTRLR0_SSTE);
	REG32_WRITE(ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_CTRLR0=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ctrlr0_base));
	}
}

/*
 * SSI Clock Divider.
 * The LSB for this field is always set to 0 and is unaffected by a
 * write operation, which ensures an even value is held in this
 * register. If the value is 0, the serial output clock (sclk_out) is
 * disabled. The frequency of the sclk_out is derived from the
 * following equation:
 * Fsclk_out = Fssi_clk/SCKDV
 * where SCKDV is any even value between 2 and 65534. For
 * example: for Fssi_clk = 3.6864MHz and SCKDV =2
 * Fsclk_out = 3.6864/2 = 1.8432MHz
 */
static void qspi_set_sckdv(u32 base, u32 sckdv)
{
	u32 baudr_base = base + QSPI_REG_BAUDR;
	u32 val = 0;

	val = REG32_READ(baudr_base);
	val = val & ~(0xFFFF);
	val = val | (sckdv & 0xFFFF);
	REG32_WRITE(baudr_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_BAUDR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(baudr_base));
	}
}

/*
 * Transfer start FIFO level.
 * Used to control the level of entries in transmit FIFO above
 * which transfer will start on serial line. This register can be
 * used to ensure that sufficient data is present in transmit
 * FIFO before starting a write operation on serial line. These
 * field is valid only for Master mode of operation.
 */
static void qspi_set_txftl(u32 base, u32 level)
{
	u32 txftlr_base = base + QSPI_REG_TXFTLR;
	u32 val = 0;

	val = REG32_READ(txftlr_base);
	val = val & ~(0xFFFF << QSPI_TXFTLR_TXFTLR);
	val = val | ((level & 0xFFFF) << QSPI_TXFTLR_TXFTLR);
	REG32_WRITE(txftlr_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_TXFTLR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(txftlr_base));
	}
}

/*
 * Receive FIFO Threshold Level.
 * Controls the level of entries (or above) at which the receive 
 * FIFO controller triggers an interrupt. The FIFO depth is 
 * configurable in the range 8-256. This register is sized to the 
 * number of address bits needed to access the FIFO. If you 
 * attempt to set this value greater than the depth of the FIFO, 
 * this field is not written and retains its current value. When the 
 * number of receive FIFO entries is greater than or equal to 
 * this value + 1, the receive FIFO full interrupt is triggered.
 */
void qspi_set_rxftl(u32 base, u32 level)
{
	u32 rxftlr_base = base + QSPI_REG_RXFTLR;

	REG32_WRITE(rxftlr_base, level);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_TXFTLR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(rxftlr_base));
	}
}

/*
 * Wait cycles in Dual/Quad/Octal mode between control
 * frames transmit and data reception. Specified as number of
 * SPI clock cycles.
 */
static void qspi_set_wait_clock(u32 base, u32 clock)
{
	u32 spi_ctrlr0_base = base + QSPI_REG_SPI_CTRLR0;
	u32 val = 0;

	val = REG32_READ(spi_ctrlr0_base);
	val = val & ~(0x1F << QSPI_SPICTRLR0_WAIT_CYCLES);
	val = val | ((clock & 0x1F) << QSPI_SPICTRLR0_WAIT_CYCLES);
	REG32_WRITE(spi_ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SPI_CTRLR0=0x%08x\r\n",
			__FUNCTION__, REG32_READ(spi_ctrlr0_base));
	}
}

/*
 * This bit defines Length of Address to be transmitted. Only
 * after this much bits are programmed in to the FIFO the
 * transfer can begin.
 * Values:
 * 0x0 (ADDR_L0): No Address
 * 0x1 (ADDR_L4): 4 bit Address length
 * 0x2 (ADDR_L8): 8 bit Address length
 * 0x3 (ADDR_L12): 12 bit Address length
 * 0x4 (ADDR_L16): 16 bit Address length
 * 0x5 (ADDR_L20): 20 bit Address length
 * 0x6 (ADDR_L24): 24 bit Address length
 * 0x7 (ADDR_L28): 28 bit Address length
 * 0x8 (ADDR_L32): 32 bit Address length
 * 0x9 (ADDR_L36): 36 bit Address length
 * 0xa (ADDR_L40): 40 bit Address length
 * 0xb (ADDR_L44): 44 bit Address length
 * 0xc (ADDR_L48): 48 bit Address length
 * 0xd (ADDR_L52): 52 bit Address length
 * 0xe (ADDR_L56): 56 bit Address length
 * 0xf (ADDR_L60): 60 bit Address length
 */
static void qspi_set_addr_len(u32 base, u32 len)
{
	u32 spi_ctrlr0_base = base + QSPI_REG_SPI_CTRLR0;
	u32 val = 0;

	val = REG32_READ(spi_ctrlr0_base);
	val = val & ~(0xF << QSPI_SPICTRLR0_ADDR_L);
	val = val | ((len & 0xF) << QSPI_SPICTRLR0_ADDR_L);
	REG32_WRITE(spi_ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SPI_CTRLR0=0x%08x\r\n",
			__FUNCTION__, REG32_READ(spi_ctrlr0_base));
	}
}

/*
 * Dual/Quad/Octal mode instruction length in bits.
 * Values:
 * 0x0 (INST_L0): No Instruction
 * 0x1 (INST_L4): 4 bit Instruction length
 * 0x2 (INST_L8): 8 bit Instruction length
 * 0x3 (INST_L16): 16 bit Instruction length
 */
static void qspi_set_cmd_len(u32 base, u32 len)
{
	u32 spi_ctrlr0_base = base + QSPI_REG_SPI_CTRLR0;
	u32 val = 0;

	val = REG32_READ(spi_ctrlr0_base);
	val = val & ~(0x3 << QSPI_SPICTRLR0_INST_L);
	val = val | ((len & 0x3) << QSPI_SPICTRLR0_INST_L);
	REG32_WRITE(spi_ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SPI_CTRLR0=0x%08x\r\n",
			__FUNCTION__, REG32_READ(spi_ctrlr0_base));
	}
}

/*
 * Address and instruction transfer format.
 * Selects whether DWC_ssi will transmit instruction/address
 * either in Standard SPI mode or the SPI mode selected in
 * CTRLR0.SPI_FRF field.
 * Values:
 * 0x0 (TT0): Instruction and Address will be sent in Standard SPI Mode.
 * 0x1 (TT1): Instruction will be sent in Standard SPI Mode
 *            and Address will be sent in the mode specified by
 *            CTRLR0.SPI_FRF.
 * 0x2 (TT2): Both Instruction and Address will be sent in
 *            the mode specified by SPI_FRF.
 * 0x3 (TT3): Reserved.
 */
static void qspi_set_cmd_addr_line_mode(u32 base, u32 mode)
{
	u32 spi_ctrlr0_base = base + QSPI_REG_SPI_CTRLR0;
	u32 val = 0;

	val = REG32_READ(spi_ctrlr0_base);
	val = val & ~(0x3 << QSPI_SPICTRLR0_TRANS_TYPE);
	val = val | ((mode & 0x3) << QSPI_SPICTRLR0_TRANS_TYPE);
	REG32_WRITE(spi_ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SPI_CTRLR0=0x%08x\r\n",
			__FUNCTION__, REG32_READ(spi_ctrlr0_base));
	}
}

/*
 * 18bit:Read data strobe enable bit. Once this bit is set to 1
 *       DWC_ssi will use Read data strobe (rxds) to capture read data in DDR mode.
 * 17bit:Instruction DDR Enable bit. This will enable Dual-data rate
 *       transfer for Instruction phase.
 * 16bit:SPI DDR Enable bit. This will enable Dual-data rate transfers
 *       in Dual/Quad/Octal frame formats of SPI.
 */
static void qspi_set_ddr_mode(u32 base, u32 mode)
{
	u32 spi_ctrlr0_base = base + QSPI_REG_SPI_CTRLR0;
	u32 val = 0;

	val = REG32_READ(spi_ctrlr0_base);
	val = val & ~(0x7 << 16);
	val = val | ((mode & 0x7) << 16);
	REG32_WRITE(spi_ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SPI_CTRLR0=0x%08x\r\n",
			__FUNCTION__, REG32_READ(spi_ctrlr0_base));
	}
}

/*
 * Number of Data Frames.
 * When TMOD = 10 or TMOD = 11, this register field sets the
 * number of data frames to be continuously received by the
 * DWC_ssi. The DWC_ssi continues to receive serial data until
 * the number of data frames received is equal to this register
 * value plus 1, which enables you to receive up to 64 KB of
 * data in a continuous transfer. When the DWC_ssi is
 * configured as a serial slave, the transfer continues for as
 * long as the slave is selected. Therefore, this register serves
 * no purpose and is not present when the DWC_ssi is
 * configured as a serial slave.
 */
static void qspi_set_read_size(u32 base, u32 n)
{
	u32 ctrlr1_base = base + QSPI_REG_CTRLR1;

	REG32_WRITE(ctrlr1_base, (n & 0xFFFF) - 1);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_CTRLR1=0x%08x\r\n", __FUNCTION__,
			REG32_READ(ctrlr1_base));
	}
}

/*
 * Transmit DMA Enable. This bit enables/disables the transmit
 * FIFO DMA channel.
 * Values:
 * 0x1 (ENABLED): transmit DMA enabled
 * 0x0 (DISABLE): transmit DMA disabled
 */
void qspi_set_tdma_en(u32 base, u32 en)
{
	u32 dmacr_base = base + QSPI_REG_DMACR;
	u32 val = 0;

	val = REG32_READ(dmacr_base);
	val = val & ~(0x1 << 1);
	val = val | ((en & 0x1) << 1);
	REG32_WRITE(dmacr_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_DMACR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(dmacr_base));
	}
}

/*
 * Receive DMA Enable. This bit enables/disables the receive
 * FIFO DMA channel.
 * Values:
 * 0x1 (ENABLED): Receive DMA enabled
 * 0x0 (DISABLE): Receive DMA disabled
 */
void qspi_set_rdma_en(u32 base, u32 en)
{
	u32 dmacr_base = base + QSPI_REG_DMACR;
	u32 val = 0;

	val = REG32_READ(dmacr_base);
	val = val & ~(0x1);
	val = val | (en & 0x1);
	REG32_WRITE(dmacr_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_DMACR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(dmacr_base));
	}
}

/*
 * Transmit Data Level. This bit field controls the level at which
 * a DMA request is made by the transmit logic. It is equal to
 * the watermark level; that is, the dma_tx_req signal is
 * generated when the number of valid data entries in the
 * transmit FIFO is equal to or below this field value, and
 * TDMAE = 1.
 */
void qspi_set_dma_tdl(u32 base, u32 tdl)
{
	u32 dmatdlr_base = base + QSPI_REG_DMATDLR;
	u32 val = 0;

	val = REG32_READ(dmatdlr_base);
	val = val & ~(0xFFFF);
	val = val | (tdl & 0xFFFF);
	REG32_WRITE(dmatdlr_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_DMATDLR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(dmatdlr_base));
	}
}

/*
 * Receive Data Level. This bit field controls the level at which a
 * DMA request is made by the receive logic. The watermark
 * level = DMARDL+1; that is, dma_rx_req is generated when
 * the number of valid data entries in the receive FIFO is equal
 * to or above this field value + 1, and RDMAE=1.
 */
void qspi_set_dma_rdl(u32 base, u32 rdl)
{
	u32 dmardlr_base = base + QSPI_REG_DMARDLR;
	u32 val = 0;

	val = REG32_READ(dmardlr_base);
	val = val & ~(0xFFFF);
	val = val | (rdl & 0xFFFF);
	REG32_WRITE(dmardlr_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_DMARDLR=0x%08x\r\n", __FUNCTION__,
			REG32_READ(dmardlr_base));
	}
}

/*
 * TXD Drive edge register which decided the driving edge of
 * transmit data.
 * The maximum value of this register is = (BAUDR/2) -1.
 */
static void qspi_set_ddr_drive_edge(u32 base, u32 tde)
{
	u32 ddr_drive_edge_base = base + QSPI_REG_DDR_DRIVE_EDGE;

	REG32_WRITE(ddr_drive_edge_base, (tde & 0xFF));

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_DDR_DRIVE_EDGE=0x%08x\r\n",
			__FUNCTION__, REG32_READ(ddr_drive_edge_base));
	}
}

static void qspi_set_clk_stretch(u32 base, u32 en)
{
	u32 spi_ctrlr0_base = base + QSPI_REG_SPI_CTRLR0;
	u32 val = 0;

	val = REG32_READ(spi_ctrlr0_base);
	val = val & ~(0x1 << 30);
	val = val | ((en & 0x1) << 30);
	REG32_WRITE(spi_ctrlr0_base, val);

	if (dbg_print == 1) {
		hprintf(DEBUG, "%s QSPI_REG_SPI_CTRLR0=0x%08x\r\n",
			__FUNCTION__, REG32_READ(spi_ctrlr0_base));
	}
}

/*
 * 1-1-1 mode write: 1 line command, 1 line address, 1 line data
 * addr only support 24-bit address
 * size unit is word(4B)
 */
static void qspi_flash_1_1_1_wr(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	qspi_set_wait_clock(base, 0);
	qspi_set_txftl(base, size); //do not contain cmd and addr stage
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_sste(base, 0);
	qspi_set_sckdv(base, 10);
	qspi_enable(base);
	REG32_WRITE(
		dr0_base,
		(((cmd & 0xFF) << 24) |
		 (addr &
		  0xFFFFFF))); //std mode cmd, addr can in the same fifo location
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		REG32_WRITE(dr0_base, data[i]);
	}
	qspi_de_endian(base);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

static void qspi_flash_1_1_1_wr_dfs_8(u32 base, u8 *data, u32 size, u8 cmd,
				      u32 addr, u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 8);
	//	qspi_set_txftl(base, 6); //do not contain cmd and addr stage
	if (size >= 28) {
		qspi_set_txftl(base, 31);
	} else {
		qspi_set_txftl(base, size - 1);
	}
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_sste(base, 0);
	qspi_set_sckdv(base, 10);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	if (addr_len == QSPI_ADDR_LEN_32)
		REG32_WRITE(dr0_base, (addr >> 24) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 16) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 8) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 0) & 0xFF);
	while (qspi_is_busy(base))
		;
	for (i = 0; i < size; i++) {
		if (REG32_READ(base + QSPI_REG_SR) & (1 << 1)) {
			//tx fifo not full
			REG32_WRITE(dr0_base, data[i]);
		} else {
			//fifo full
			i--;
			putc('f');
		}

		if (REG32_READ(base + QSPI_REG_SR) & (1 << 2)) {
			//tx fifo empty
			putc('E');
		}
	}

	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

/*
 * 1-1-1 mode read: 1 line command, 1 line address, 1 line data
 * addr only support 24-bit address
 * size unit is word(4B)
 */
static void qspi_flash_1_1_1_rd(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i = 0, cnt = 0, val = 0;
	u32 *tmp_data = data;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_spi_sste(base, 0);
	qspi_set_read_size(base, size);
	qspi_set_sckdv(base, 10);
	qspi_enable(base);
	REG32_WRITE(
		dr0_base,
		(((cmd & 0xFF) << 24) |
		 (addr &
		  0xFFFFFF))); //std mode cmd, addr can in the same fifo location
	while (qspi_is_busy(base));
	qspi_en_endian(base);
	while (1) {
		val = REG32_READ(base + QSPI_REG_SR) >> 24;
		if (val & (1 << 3)) {
			//rx fifo not empty
			//            putc('i');
			tmp_data[i++] = REG32_READ(dr0_base);
			if (i >= size) {
				break;
			}
		} else if (val & (1 << 4)) {
			putc('f');
		} else {
			//fifo empty
			putc('e');
			cnt += 0x100;
		}

		if (cnt++ > 0xffffff) {
			hprintf(DEBUG, "read timer out\r\n");
			hprintf(DEBUG, "QSPI_REG_SR : 0x%08x",
				REG32_READ(base + QSPI_REG_SR));
			return;
		}
	}
	qspi_de_endian(base);
}

static void qspi_flash_1_1_1_rd_dfs_8(u32 base, u8 *data, u32 size, u8 cmd,
				      u32 addr, u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i = 0, cnt = 0, val = 0;
	u8 *tmp_data = data;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 8);
	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_spi_sste(base, 0);
	qspi_set_read_size(base, size);
	qspi_set_sckdv(base, 10);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	if (addr_len == QSPI_ADDR_LEN_32)
		REG32_WRITE(dr0_base, (addr >> 24) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 16) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 8) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 0) & 0xFF);
	if ((cmd == FLASH_CMD_MEMORY_FAST_RD) ||
	    (cmd == FLASH_CMD_RD_ID_UNIQUE))
		REG32_WRITE(dr0_base, 0x00); //write dummy byte
	while (qspi_is_busy(base));
	while (1) {
		val = REG32_READ(base + QSPI_REG_SR);
		if (val & (1 << 3)) {
			//rx fifo not empty
			//            putc('i');
			tmp_data[i++] = REG32_READ(dr0_base);
			if (i >= size) {
				break;
			}
		} else if (val & (1 << 4)) {
			putc('f');
		} else {
			//fifo empty
			putc('e');
			cnt += 0x100;
		}

		if (cnt++ > 0xffffff) {
			hprintf(DEBUG, "read timer out\r\n");
			hprintf(DEBUG, "QSPI_REG_SR : 0x%08x",
				REG32_READ(base + QSPI_REG_SR));
			return;
		}
	}
}

void qspi_flash_1_1_2_wr(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	qspi_set_txftl(base,
		       size); //transfer will start when greater than txftl
	qspi_set_spi_frf(base, QSPI_DUAL_SPI); //dual mode
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_sste(base, 0);
	qspi_set_wait_clock(base, 8);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_STD);
	qspi_enable(base);
	if (cmd) {
		REG32_WRITE(dr0_base, cmd);
	}
	REG32_WRITE(dr0_base, addr);
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		REG32_WRITE(dr0_base, data[i]);
	}
	qspi_de_endian(base);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

/*
 * 1-1-2 mode read: 1 line command, 1 line address, 2 line data
 * size unit is word(4B)
*/
static void qspi_flash_1_1_2_rd(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;
	u32 *tmp_data = data;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_DUAL_SPI); //dual mode
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_spi_sste(base, 0);
	qspi_set_wait_clock(base, 8);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_STD);
	qspi_set_read_size(base, size);
	qspi_enable(base);
	if (cmd) {
		REG32_WRITE(
			dr0_base,
			cmd); //dual,quad,oct mode cmd, addr, data should in different fifo location
	}
	REG32_WRITE(dr0_base, addr);

	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		tmp_data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}

static void qspi_flash_1_1_4_wr(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	if (size >= 28) {
		qspi_set_txftl(base, 31);
	} else {
		qspi_set_txftl(
			base,
			size); //transfer will start when greater than txftl
	}
	qspi_set_spi_frf(base, QSPI_QUAD_SPI); //quad mode
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_sste(base, 0);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_STD);
	//    qspi_set_sckdv(base, 2);
	qspi_set_clk_stretch(base, 0);
	qspi_enable(base);
	if (cmd) {
		REG32_WRITE(dr0_base, cmd);
	}
	REG32_WRITE(dr0_base, addr);
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		if ((REG32_READ(base + QSPI_REG_SR) >> 24) & (1 << 1)) {
			//tx fifo not full
			REG32_WRITE(dr0_base, data[i]);
		} else {
			//fifo full
			i--;
			putc('f');
		}

		if ((REG32_READ(base + QSPI_REG_SR) >> 24) & (1 << 2)) {
			//tx fifo empty
			putc('E');
		}
	}

	hprintf(DEBUG, "QSPI_REG_TXFLR : 0x%x\r\n",
		REG32_READ(base + QSPI_REG_TXFLR));
	qspi_de_endian(base);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

/*
 * 1-1-4 mode read: 1 line command, 1 line address, 4 line data
 * size unit is word(4B)
*/
static void qspi_flash_1_1_4_rd(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;
	u32 *tmp_data = data;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI); //quad mode
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_spi_sste(base, 0);
	qspi_set_txftl(base, 1);
	REG32_WRITE(base + QSPI_REG_CTRLR1, size - 1);
	qspi_set_wait_clock(base, 8);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_STD);
	qspi_enable(base);
	if (cmd) {
		REG32_WRITE(
			dr0_base,
			cmd); //dual,quad,oct mode cmd, addr, data should in different fifo location
	}
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		tmp_data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
	//    qspi_set_txftl(base, 0); //reset txftl
}

#if 0
static void qspi_flash_1_1_8_wr(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	qspi_set_txftl(base,
		       size); //transfer will start when greater than txftl
	qspi_set_spi_frf(base, QSPI_OCTAL_SPI); //oct mode
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_sste(base, 0);
	qspi_set_wait_clock(base, 8);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_STD);
	qspi_enable(base);
	if (cmd) {
		REG32_WRITE(dr0_base, cmd);
	}
	REG32_WRITE(dr0_base, addr);
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		REG32_WRITE(dr0_base, data[i]);
	}
	qspi_de_endian(base);
    while(!qspi_tf_is_empty(base));
    while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

/*
 * 1-1-8 mode read: 1 line command, 1 line address, 8 line data
 * size unit is word(4B)
*/
static void qspi_flash_1_1_8_rd(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;
	u32 *tmp_data = data;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_OCTAL_SPI); //oct mode
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_spi_sste(base, 0);
	qspi_set_read_size(base, size);
	qspi_set_wait_clock(base, 8);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_STD);
	qspi_enable(base);
	if (cmd) {
		REG32_WRITE(
			dr0_base,
			cmd); //dual,quad,oct mode cmd, addr, data should in different fifo location
	}
	REG32_WRITE(dr0_base, addr);
    while(!qspi_tf_is_empty(base));
    while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		tmp_data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}
#endif

static void qspi_flash_1_2_2_rd(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 8);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_DUAL_SPI);
	qspi_set_mode(base, QSPI_RX_ONLY);
	qspi_set_read_size(base, size);
	qspi_set_txftl(base, 1);
	qspi_enable(base);

	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}

static void qspi_flash_1_2_2_rd_GD25LQ256D(u32 base, u32 *data, u32 size,
					   u8 cmd, u32 addr, u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 4);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_DUAL_SPI);
	qspi_set_mode(base, QSPI_RX_ONLY);
	qspi_set_read_size(base, size);
	qspi_set_txftl(base, 1);
	qspi_enable(base);

	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}

static void qspi_flash_2_2_2_rd(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 8);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_DUAL_SPI);
	qspi_set_mode(base, QSPI_RX_ONLY);
	qspi_set_read_size(base, size);
	qspi_set_txftl(base, 1);
	qspi_enable(base);

	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}

static void qspi_flash_1_4_4_rd(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 10);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_mode(base, QSPI_RX_ONLY);
	qspi_set_read_size(base, size);
	qspi_set_txftl(base, 1);
	qspi_enable(base);

	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}

static void qspi_flash_1_4_4_rd_GD25LQ256D(u32 base, u32 *data, u32 size,
					   u8 cmd, u32 addr, u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	if (cmd == FLASH_CMD_MEMORY_QUAD_IO_WORD_FAST_RD)
		qspi_set_wait_clock(base, 4);
	else
		qspi_set_wait_clock(base, 6);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_mode(base, QSPI_RX_ONLY);
	qspi_set_read_size(base, size);
	qspi_set_txftl(base, 1);
	qspi_enable(base);

	if ((cmd == FLASH_CMD_MEMORY_QUAD_IO_WORD_FAST_RD) &&
	    ((addr & 0x1) != 0)) //A0 must equal 0
		addr = addr & (~(0x1));

	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}

static void qspi_flash_4_4_4_rd(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 10);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_mode(base, QSPI_RX_ONLY);
	qspi_set_read_size(base, size);
	qspi_set_txftl(base, 1);
	qspi_enable(base);

	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}

static void qspi_flash_4_4_4_rd_GD25LQ256D(u32 base, u32 *data, u32 size,
					   u8 cmd, u32 addr, u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 4);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_spi_dfs(base, 32);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_mode(base, QSPI_RX_ONLY);
	qspi_set_read_size(base, size);
	qspi_set_txftl(base, 1);
	qspi_enable(base);

	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		data[i] = REG32_READ(dr0_base);
	}
	qspi_de_endian(base);
}

static void qspi_flash_4_4_4_wr(u32 base, u32 *data, u32 size, u8 cmd, u32 addr,
				u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_dfs(base, 32);
	if (size >= 28) {
		qspi_set_txftl(base, 31);
	} else {
		qspi_set_txftl(
			base,
			size); //transfer will start when greater than txftl
	}
	qspi_set_spi_frf(base, QSPI_QUAD_SPI); //quad mode
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_sste(base, 0);
	//    qspi_set_sckdv(base, 2000);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_clk_stretch(base, 0);
	qspi_enable(base);
	if (cmd) {
		REG32_WRITE(dr0_base, cmd);
	}
	REG32_WRITE(dr0_base, addr);
	qspi_en_endian(base);
	for (i = 0; i < size; i++) {
		if ((REG32_READ(base + QSPI_REG_SR) >> 24) & (1 << 1)) {
			//tx fifo not full
			REG32_WRITE(dr0_base, data[i]);
		} else {
			//fifo full
			i--;
			putc('f');
		}

		if ((REG32_READ(base + QSPI_REG_SR) >> 24) & (1 << 2)) {
			//tx fifo empty
			putc('E');
		}
	}
	qspi_de_endian(base);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

static u32 qspi_reg_1_0_1_rd(u32 base, u8 cmd)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, 0);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_STD);
	qspi_set_ddr_mode(base, 0);

	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_read_size(base, 1);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	return (REG32_READ(dr0_base));
}

static u32 qspi_reg_2_0_2_rd(u32 base, u8 cmd)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, 0);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);

	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_DUAL_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_read_size(base, 1);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	return (REG32_READ(dr0_base));
}

static u32 qspi_reg_4_0_4_rd(u32 base, u8 cmd)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, 0);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);

	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_read_size(base, 1);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	return (REG32_READ(dr0_base));
}

static void qspi_reg_1_0_1_wr(u32 base, u8 cmd, u8 val)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, 0);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_STD_ADDR_STD);
	qspi_set_ddr_mode(base, 0);

	qspi_set_txftl(base, 1);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, val);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

static void qspi_reg_2_0_2_wr(u32 base, u8 cmd, u8 val)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, 0);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);

	qspi_set_txftl(base, 1);
	qspi_set_spi_frf(base, QSPI_DUAL_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, val);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

static void qspi_reg_4_0_4_wr(u32 base, u8 cmd, u8 val)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, 0);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);

	qspi_set_txftl(base, 1);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, val);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

static void qspi_cmd_2_0_0_send(u32 base, u8 cmd)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, 0);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);

	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_DUAL_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
}

static void qspi_cmd_4_0_0_send(u32 base, u8 cmd)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, 0);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
}

void qspi_cmd_4_4_0_send(u32 base, u8 cmd, u32 addr, u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, addr_len);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_txftl(base, 1);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 32);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_clk_stretch(base, 0);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
}

//特殊处理一下，将地址作为数据发出去
static void qspi_cmd_4_4_0_send_qpi(u32 base, u8 cmd, u32 addr, u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_wait_clock(base, 0);
	qspi_set_addr_len(base, QSPI_ADDR_LEN_NO);
	qspi_set_cmd_len(base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(base, 0);
	qspi_set_txftl(base, 1);
	qspi_set_spi_frf(base, QSPI_QUAD_SPI);
	qspi_set_spi_sste(base, 0);
	//	qspi_set_spi_dfs(base, 1);
	if (addr_len == QSPI_ADDR_LEN_32) {
		qspi_set_spi_dfs(base, 32);
	} else {
		qspi_set_spi_dfs(base, 24);
	}
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_clk_stretch(base, 0);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, addr);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
}

static u32 qspi_flash_read_reg(u32 base, u8 cmd)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_read_size(base, 1);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	return (REG32_READ(dr0_base));
}

void qspi_flash_write_reg(u32 base, u8 cmd, u32 val)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_txftl(base, 2);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, val);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	qspi_set_txftl(base, 0); //reset txftl
}

static void qspi_flash_send_cmd(u32 base, u8 cmd)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	while (qspi_is_busy(base));
	qspi_disable(base);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_dfs(base, 8);
	qspi_set_txftl(base, 0); //above set value data will be send
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);	
	while(!qspi_tf_is_empty(base));	
	while(qspi_is_busy(base));
}

static void qspi_flash_send_cmd_with_addr(u32 base, u8 cmd, u32 addr,
					  u32 addr_len)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_txftl(base, 3);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	if (addr_len == QSPI_ADDR_LEN_32)
		REG32_WRITE(dr0_base, (addr >> 24) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 16) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 8) & 0xFF);
	REG32_WRITE(dr0_base, (addr >> 0) & 0xFF);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
}

void qspi_flash_write_ctl(u32 base, u8 en)
{
	u8 cmd = 0;
	u32 flag_status = 0;
	u8 need_clear = 0;

	if (en) {
		cmd = 0x6; //write enable
	} else {
		cmd = 0x4; //write disable
		flag_status = qspi_flash_read_reg(base, 0x70);
		hprintf(DEBUG, "flag status = 0x%x\r\n", flag_status);
		if ((flag_status >> 1) & 0x1 || (flag_status >> 4) & 0x1 ||
		    (flag_status >> 5) & 0x1) {
			need_clear = 1;
		}
		if (need_clear) {
			cmd = 0x50; //CLEAR FLAG STATUS REGISTER
		}
	}

	qspi_flash_send_cmd(base, cmd);
}

void qspi_flash_read_register_all(u32 base)
{
	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(base);
	qspi_de_endian(base);

	hprintf(DEBUG, "status = 0x%x\r\n", qspi_flash_read_reg(base, 0x5));
	hprintf(DEBUG, "flag status = 0x%x\r\n",
		qspi_flash_read_reg(base, 0x70));
	hprintf(DEBUG, "NONVOLATILE cfg = 0x%x\r\n",
		qspi_flash_read_reg(base, 0xb5));
	hprintf(DEBUG, "VOLATILE cfg = 0x%x\r\n",
		qspi_flash_read_reg(base, 0x85));
	hprintf(DEBUG, "ENHANCED VOLATILE cfg = 0x%x\r\n",
		qspi_flash_read_reg(base, 0x65));
	hprintf(DEBUG, "EXTENDED addr = 0x%x\r\n",
		qspi_flash_read_reg(base, 0xc8));
	//hprintf(DEBUG, "GENERAL PURPOSE = 0x%x\r\n", qspi_flash_read_reg(base, 0x96));
}

static void qspi_flash_reset(u32 base)
{
	qspi_flash_send_cmd(base, 0x66); //reset enable

	qspi_flash_send_cmd(base, 0x99); //reset memory
}

void qspi_flash_read_id(u32 base, u8 *rt_data)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_read_size(base, 20);
	qspi_enable(base);
	REG32_WRITE(dr0_base, 0x9e); //read id cmd
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	for (i = 0; i < 20; i++) {
		rt_data[i] = REG8_READ(dr0_base);
	}
}
static void qspi_flash_read_id_GD25LQ256D(u32 base, u8 *rt_data)
{
	u32 dr0_base = base + QSPI_REG_DR0;
	u32 i;

	while (qspi_is_busy(base));
	qspi_disable(base);
	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_read_size(base, 3);
	qspi_enable(base);
	REG32_WRITE(dr0_base, FLASH_CMD_RD_ID_MMC); //read id cmd
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
	for (i = 0; i < 3; i++) {
		rt_data[i] = REG32_READ(dr0_base);
	}
}

static void qspi_write_status_reg_GD25LQ256D(u32 base, u8 cmd, u8 val_l,
					     u8 val_h)
{
	u32 dr0_base = base + QSPI_REG_DR0;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_txftl(base, 2);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_spi_sste(base, 0);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(dr0_base, cmd);
	REG32_WRITE(dr0_base, val_l);
	REG32_WRITE(dr0_base, val_h);
	while(!qspi_tf_is_empty(base));
	while(qspi_is_busy(base));
}

static void qspi_flash_set_quad(u32 base, u32 enable)
{
	u32 status_h, status_l;

	status_l = qspi_flash_read_reg(base, FLASH_CMD_REG_STATUS_RD);
	status_h = qspi_flash_read_reg(base, FLASH_CMD_REG_STATUS_RD_HIGH);

	if (enable == FLASH_QUAD_EN)
		status_h |= (0x1 << 1); //enable
	else if (enable == FLASH_QUAD_DI)
		status_h &= (~(0x1 << 1)); //disable

	//    qspi_flash_send_cmd(base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd(base, FLASH_CMD_WR_EN_VSR);
	qspi_write_status_reg_GD25LQ256D(base, FLASH_CMD_REG_STATUS_WR,
					 status_l, status_h);
	while (qspi_flash_read_reg(base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
}

u32 qspi_register_default_val_test(u32 base, u32 flags, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 final_result = HVTE_MODULE_TEST_PASS;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);

	for (i = 0; i < sizeof(qspi_reg_default_val_table) / sizeof(reg_struct);
	     i++) {
		hprintf(DEBUG, "%s->", qspi_reg_default_val_table[i].name);
		result = HVTE_MODULE_TEST_PASS;
		HCOMP(REG32_READ(qspi_reg_default_val_table[i].addr),
		      qspi_reg_default_val_table[i].default_val);
		if (result == HVTE_MODULE_TEST_PASS) {
			hprintf(DEBUG, "OK\r\n");
		} else {
			final_result = HVTE_MODULE_TEST_FAIL;
		}
	}

	return final_result;
}

u32 qspi_register_rw_test(u32 base, u32 flags, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 final_result = HVTE_MODULE_TEST_PASS;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);

	hprintf(DEBUG, "RW resiter test:\r\n");
	for (i = 0; i < sizeof(qspi_reg_rw_table) / sizeof(reg_struct); i++) {
		result = HVTE_MODULE_TEST_PASS;
		hprintf(DEBUG, "%s->", qspi_reg_default_val_table[i].name);

		REG_RW_TEST(qspi_reg_rw_table[i].addr,
			    qspi_reg_rw_table[i].bit_strobe);

		if (result == HVTE_MODULE_TEST_PASS) {
			hprintf(DEBUG, "OK\r\n");
		} else {
			final_result = HVTE_MODULE_TEST_FAIL;
		}
	}

	hprintf(DEBUG, "RO resiter test:\r\n");
	for (i = 0; i < sizeof(qspi_reg_ro_table) / sizeof(reg_struct); i++) {
		result = HVTE_MODULE_TEST_PASS;
		hprintf(DEBUG, "%s->", qspi_reg_default_val_table[i].name);

		REG_RO_TEST(qspi_reg_ro_table[i].addr,
			    qspi_reg_ro_table[i].bit_strobe);
		if (result == HVTE_MODULE_TEST_PASS) {
			hprintf(DEBUG, "OK\r\n");
		} else {
			final_result = HVTE_MODULE_TEST_FAIL;
		}
	}

	return final_result;
}

int qspi_std_rw_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);
	qspi_flash_1_1_1_rd(
		reg_base, datard_std, QSPI_RW_TEST_LEN, FLASH_CMD_MEMORY_RD,
		FLASH_BASE_ADDR,
		QSPI_ADDR_LEN_24); //read(CMD 03) addr(0x0) N bytes to data
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");

	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));

	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)

	qspi_flash_1_1_1_rd(
		reg_base, datard_std, QSPI_RW_TEST_LEN, FLASH_CMD_MEMORY_RD,
		FLASH_BASE_ADDR,
		QSPI_ADDR_LEN_24); //read(CMD 03) addr(0x0) N bytes to data
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");

	memset(data, 0xaa, sizeof(data));

	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));

	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_1_1_1_rd(
		reg_base, datard_std, QSPI_RW_TEST_LEN, FLASH_CMD_MEMORY_RD,
		FLASH_BASE_ADDR,
		QSPI_ADDR_LEN_24); //read(CMD 03) addr(0x0) N bytes to data
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_std[i]);
	}

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	return result;
}

int qspi_dual_rw_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, 0x0, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");

	qspi_flash_1_1_2_rd(reg_base, datard_dual, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_DUAL_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr dual data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(datard_std[i], datard_dual[i]);
	}

	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));

	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)

	qspi_flash_1_1_2_rd(reg_base, datard_dual, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_DUAL_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr dual data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");

	memset(data, 0xAA, sizeof(data));

	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));

	qspi_flash_1_1_1_wr(
		reg_base, data, QSPI_RW_TEST_LEN, FLASH_CMD_PAGE_PROG, 0x0,
		QSPI_ADDR_LEN_24); //flash do not support dual write cmd

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_1_1_2_rd(reg_base, datard_dual, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_DUAL_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_dual[i]);
	}

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN, 0x3, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	return result;
}

int qspi_quad_rw_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, 0x0, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");

	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(datard_std[i], datard_quad[i]);
	}

	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));

	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)

	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");

	memset(data, 0xAA, sizeof(data));

	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));

	qspi_flash_1_1_1_wr(
		reg_base, data, QSPI_RW_TEST_LEN, FLASH_CMD_PAGE_PROG, 0x0,
		QSPI_ADDR_LEN_24); //flash do not support dual write cmd

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN, 0x3, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	return result;
}

/* Todo：对端的flash不支持 */
#if 0
int qspi_octa_rw_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, 0x0, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");

	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(datard_std[i], datard_quad[i]);
	}

	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));

	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)

	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");

	memset(data, 0xAA, sizeof(data));

	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));

	qspi_flash_1_1_1_wr(
		reg_base, data, QSPI_RW_TEST_LEN, FLASH_CMD_PAGE_PROG, 0x0,
		QSPI_ADDR_LEN_24); //flash do not support dual write cmd

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN, 0x3, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	return result;
}
#endif

int qspi_cmd_addr_dual_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;
	u32 slv_reg, spi_ctrlr0;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, 0x0, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");

	/* dual addr */
	qspi_flash_1_2_2_rd(reg_base, datard_dual, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_DUAL_IO_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd dual addr dual data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(datard_std[i], datard_dual[i]);
	}

	/* dual addr&cmd */
	slv_reg = qspi_reg_1_0_1_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n", slv_reg);
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN); //write enable
	slv_reg &= (~(0x1 << 6)); //enable device dual mode
	qspi_reg_1_0_1_wr(reg_base, FLASH_CMD_REG_ENHANCED_WR, slv_reg);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n",
		qspi_reg_2_0_2_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD));

	qspi_flash_2_2_2_rd(reg_base, datard_dual, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_DUAL_IO_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "dual cmd dual addr dual data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(datard_std[i], datard_dual[i]);
	}

	//exit dual mode
	hprintf(DEBUG, "enhance reg = 0x%x\r\n",
		qspi_reg_2_0_2_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD));
	qspi_cmd_2_0_0_send(reg_base, FLASH_CMD_WR_EN); //write enable
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_reg_2_0_2_rd(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_reg_2_0_2_wr(reg_base, FLASH_CMD_REG_ENHANCED_WR, 0xFF);

	REG32_WRITE(reg_base + QSPI_REG_CTRLR0,
		    (REG32_READ(reg_base + QSPI_REG_CTRLR0) & (~(0x3 << 22))) |
			    (0 << 22)); //std mode
	spi_ctrlr0 = REG32_READ(reg_base + QSPI_REG_SPI_CTRLR0);
	spi_ctrlr0 = (spi_ctrlr0 & (~(0x3 << 0))) | (0 << 0); //addr&cmd use std
	REG32_WRITE(reg_base + QSPI_REG_SPI_CTRLR0, spi_ctrlr0);
	hprintf(DEBUG, "QSPI_REG_CTRLR0(0x%x): 0x%x\r\n",
		reg_base + QSPI_REG_CTRLR0,
		REG32_READ(reg_base + QSPI_REG_CTRLR0));
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	hprintf(DEBUG, "enhance reg = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_ENHANCED_RD));

	/* test sector erase */
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)
	memset(datard, 0x0, sizeof(datard));
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, 0x0, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	return result;
}

int qspi_cmd_addr_quad_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;
	u32 slv_reg, spi_ctrlr0;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, 0x0, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");

	/* quad addr */
	qspi_flash_1_4_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_IO_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd quad addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(datard_std[i], datard_quad[i]);
	}

	/* quad addr&cmd */
	slv_reg = qspi_reg_1_0_1_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n", slv_reg);
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN); //write enable
	slv_reg &= (~(0x1 << 7)); //enable device quad mode
	qspi_reg_1_0_1_wr(reg_base, FLASH_CMD_REG_ENHANCED_WR, slv_reg);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n",
		qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD));

	qspi_flash_4_4_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_IO_FAST_RD, 0x0,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "quad cmd quad addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(datard_std[i], datard_quad[i]);
	}

	//exit quad mode
	hprintf(DEBUG, "enhance reg = 0x%x\r\n",
		qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD));
	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_WR_EN); //write enable
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD));
	qspi_reg_4_0_4_wr(reg_base, FLASH_CMD_REG_ENHANCED_WR, 0xFF);

	REG32_WRITE(reg_base + QSPI_REG_CTRLR0,
		    (REG32_READ(reg_base + QSPI_REG_CTRLR0) & (~(0x3 << 22))) |
			    (0 << 22)); //std mode
	spi_ctrlr0 = REG32_READ(reg_base + QSPI_REG_SPI_CTRLR0);
	spi_ctrlr0 = (spi_ctrlr0 & (~(0x3 << 0))) | (0 << 0); //addr&cmd use std
	REG32_WRITE(reg_base + QSPI_REG_SPI_CTRLR0, spi_ctrlr0);
	hprintf(DEBUG, "QSPI_REG_CTRLR0(0x%x): 0x%x\r\n",
		reg_base + QSPI_REG_CTRLR0,
		REG32_READ(reg_base + QSPI_REG_CTRLR0));
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	hprintf(DEBUG, "enhance reg = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_ENHANCED_RD));

	/* test sector erase */

	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0,
				      QSPI_ADDR_LEN_24); //sector erase
	hprintf(DEBUG, "status = 0x%x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD));
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)
	memset(datard, 0x0, sizeof(datard));
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	return result;
}

/**
 * @brief get qspi default config
 * 
 * @return s_qspi_conf Config: scph(0), scpol(0), dfs(32), sste(0), frf(QSPI_STD_SPI)
 */
s_qspi_conf qspi_get_default_conf(void)
{
	s_qspi_conf s_conf;
	s_conf.scph = 0;
	s_conf.scpol = 0;
	s_conf.dfs = 32;
	s_conf.sste = 0;
	s_conf.frf = QSPI_STD_SPI;
	s_conf.sckdv = 8;
	s_conf.slave = 1;
	return s_conf;
}

#if defined(CONFIG_R5_SAFETY)
void qspi_interrupt_enable(u32 base)
{
	if (QSPI0_BASE_ADDR == base)
		SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_QSPI0);
	else if (QSPI1_BASE_ADDR == base)
		SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP1_QSPI1);
}

void qspi_interrupt_disable(u32 base)
{
	if (QSPI0_BASE_ADDR == base)
		SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_QSPI0);
	else if (QSPI1_BASE_ADDR == base)
		SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP1_QSPI1);
}

#endif

#if defined(CONFIG_A55)
void qspi_interrupt_enable(u32 base)
{
	if (QSPI0_BASE_ADDR == base)
		A55_IRQ_ENABLE(FIQ_IRQ_LSP0_QSPI0);
	else if (QSPI1_BASE_ADDR == base)
		A55_IRQ_ENABLE(FIQ_IRQ_LSP1_QSPI1);
}

static void qspi_interrupt_disable(u32 base)
{
	if (QSPI0_BASE_ADDR == base)
		A55_IRQ_DISABLE(FIQ_IRQ_LSP0_QSPI0);
	else if (QSPI1_BASE_ADDR == base)
		A55_IRQ_DISABLE(FIQ_IRQ_LSP1_QSPI1);
}

#endif

/**
 * @brief config part properties of qspi
 * 
 * @param s_conf Contains scph, scpol, dfs, sste, frf
 */
void qspi_set_conf(u32 base, s_qspi_conf s_conf)
{
	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_frf(base, s_conf.frf);
	qspi_set_scph(base, s_conf.scph);
	qspi_set_scpol(base, s_conf.scpol);
	qspi_set_spi_dfs(base, s_conf.dfs);
	qspi_set_spi_sste(base, s_conf.sste);
	qspi_set_sckdv(base, s_conf.sckdv);
	qspi_set_ser(base, s_conf.slave);
}

/**
 * @brief set cmd to flash
 * 
 * @param cmd 
 */
void mt25t_cmd(u32 base, s_qspi_conf s_conf, u8 cmd)
{
	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_conf(base, s_conf);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_dfs(base, 8);
	qspi_set_txftl(base, 0); //above set value data will be send
	qspi_enable(base);
	REG32_WRITE(base + QSPI_REG_DR0, cmd);
}

/**
 * @brief read register data
 * 
 * @param reg 
 * @return u32 
 */
int mt25t_read_reg(u32 base, s_qspi_conf s_conf, u8 reg)
{
	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_conf(base, s_conf);
	qspi_set_txftl(base, 0);
	qspi_set_spi_frf(base, s_conf.frf);
	qspi_set_spi_sste(base, s_conf.sste);
	qspi_set_spi_dfs(base, 8);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_read_size(base, 1);
	qspi_enable(base);

	REG32_WRITE(base + QSPI_REG_DR0, reg);
	while (qspi_is_busy(base))
		;

	return REG32_READ(base + QSPI_REG_DR0);
}

void qspi_init(u32 base)
{
	qspi_exit_xip(base);
}

/**
 * @brief earse sector
 * 
 * @param addr 
 * @param sector 
 */
void mt25t_erase(u32 base, s_qspi_conf s_conf, u32 sector)
{
	while (qspi_is_busy(base))
		;
	mt25t_cmd(base, s_conf, FLASH_CMD_WR_EN);
	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_conf(base, s_conf);
	qspi_set_txftl(base, 0);
	qspi_set_spi_dfs(base, 32);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_enable(base);
	REG32_WRITE(base + QSPI_REG_DR0,
		    (FLASH_CMD_SECTOR_ER << 24) | (sector & 0xffffff));
	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	while (mt25t_read_reg(base, s_conf, 0x05) & 0x1)
		;
}

void mt25t_earse_one(u32 base, u32 sector)
{
	s_qspi_conf s_conf = qspi_get_default_conf();
	mt25t_erase(base, s_conf, sector);
}

/**
 * @brief qspi write data to flash, use qspi0, operate (s_conf.dfs * size) / 8 bytes
 * 
 * @param s_conf 
 * @param addr 
 * @param p_buf 
 * @param size 
 */

void mt25t_write(u32 base, s_qspi_conf s_conf, u32 addr, void *p_buf,
			u32 size)
{
	u32 data, offset, field, i;
	u32 *p_u32;

	while (qspi_is_busy(base))
		;
	mt25t_cmd(base, s_conf, FLASH_CMD_WR_EN);
	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_conf(base, s_conf);
	qspi_set_spi_dfs(base, s_conf.dfs);
	qspi_set_txftl(base, size + 32 / s_conf.dfs - 1);
	qspi_set_spi_frf(base, s_conf.frf);
	qspi_set_mode(base, QSPI_TX_ONLY);
	qspi_set_spi_sste(base, s_conf.sste);
	REG32_WRITE(base + QSPI_REG_IMR,
		    ~QSPI_INTERRUPT_ALL); // close all interrupt
	qspi_enable(base);

	data = (FLASH_CMD_PAGE_PROG << 24) | (addr & 0xffffff);
	for (i = s_conf.dfs; i <= 32; i += s_conf.dfs) {
		offset = 32 - i;
		field = ((1 << s_conf.dfs) - 1);
		if (field == 0)
			field = 0xffffffff;
		REG32_WRITE(base + QSPI_REG_DR0, (data >> offset) & field);
	}
	p_u32 = (u32 *)p_buf;
	hprintf(DEBUG, "write to mt25t: \r\n");
	hprintf(DEBUG, "dfs: %d, size: %d, data: \r\n", s_conf.dfs, size);
	for (i = 0; i < size; i++) {
		data = *(p_u32 + i * s_conf.dfs / 32);
		offset = ((32 - (i + 1) * s_conf.dfs % 32) % 32);
		field = ((1 << s_conf.dfs) - 1);
		if (field == 0)
			field = 0xffffffff;
		hprintf(DEBUG, "0x%08x ", (data >> offset) & field);
		// get buffer field data to write
		REG32_WRITE(base + QSPI_REG_DR0, (data >> offset) & field);
	}
	hprintf(DEBUG, "\r\n");
#if (defined(CONFIG_R5_SAFETY) || defined(CONFIG_A55))
	if (int_test) { // cant test call back, only need to check gic status
		REG32_WRITE(base + QSPI_REG_IMR,
			    ~QSPI_INTERRUPT_ALL); // close all interrupt
		REG32_WRITE(base + QSPI_REG_IMR,
			    REG32_READ(base + QSPI_REG_IMR) |
				    QSPI_INTERRUPT_TXEI);
		hprintf(DEBUG, "imr: %08x\r\n",
			REG32_READ(base + QSPI_REG_IMR));
		qspi_interrupt_enable(base);
	}
#endif
	while (qspi_is_busy(base))
		;

	while (mt25t_read_reg(base, s_conf, 0x05) & 0x1)
		;
}

void mt25t_write_bytes(u32 base, u32 addr, u8 *p_buf, u32 size)
{
	u32 i = 0, txftl, data;
	s_qspi_conf s_conf = qspi_get_default_conf();
	s_conf.dfs = 8;

	while (size) {
		while (qspi_is_busy(base))
			;
		mt25t_cmd(base, s_conf, FLASH_CMD_WR_EN);
		while (qspi_is_busy(base))
			;
		qspi_disable(base);
		qspi_set_conf(base, s_conf);
		qspi_set_spi_dfs(base, 8);
		txftl = size > 28 ? 28 : size;
		qspi_set_txftl(base, txftl + 3);
		qspi_set_spi_frf(base, s_conf.frf);
		qspi_set_mode(base, QSPI_TX_ONLY);
		qspi_set_spi_sste(base, s_conf.sste);
		REG32_WRITE(base + QSPI_REG_IMR,
			    QSPI_INTERRUPT_ALL); // enable all interrupt

		qspi_enable(base);
		data = (FLASH_CMD_PAGE_PROG << 24) | (addr & 0xffffff);
		REG32_WRITE(base + QSPI_REG_DR0, data >> 24);
		REG32_WRITE(base + QSPI_REG_DR0, (data >> 16) & 0xff);
		REG32_WRITE(base + QSPI_REG_DR0, (data >> 8) & 0xff);
		REG32_WRITE(base + QSPI_REG_DR0, data & 0xff);

		for (i = 0; i < txftl; i++) {
			REG32_WRITE(base + QSPI_REG_DR0, *p_buf);
			p_buf++;
		}

		while (qspi_is_busy(base))
			;
		while (mt25t_read_reg(base, s_conf, 0x05) & 0x1)
			;

		addr += txftl;
		size -= txftl;
	}
}

/**
 * @brief qspi read data from flash, use qspi0, operate (s_conf.dfs * size) / 8 bytes
 * 
 * @param s_conf 
 * @param addr 
 * @param p_buf 
 * @param size 
 */
void mt25t_read(u32 base, s_qspi_conf s_conf, u32 addr, void *p_buf,
		       u32 size)
{
	u32 data, offset, field, i, r;
	u32 *p_u32;

	while (qspi_is_busy(base))
		;
	qspi_disable(base);
	qspi_set_conf(base, s_conf);
	qspi_set_spi_dfs(base, s_conf.dfs);
	qspi_set_spi_frf(base, QSPI_STD_SPI);
	qspi_set_mode(base, QSPI_EEPROM_READ);
	qspi_set_spi_sste(base, s_conf.sste);
	qspi_set_read_size(base, size);
	qspi_set_txftl(base, 32 / s_conf.dfs - 1);
	qspi_enable(base);

	data = (FLASH_CMD_MEMORY_RD << 24) | (addr & 0xffffff);
	for (i = s_conf.dfs; i <= 32; i += s_conf.dfs) {
		offset = 32 - i;
		field = ((1 << s_conf.dfs) - 1);
		if (field == 0)
			field = 0xffffffff;
		REG32_WRITE(base + QSPI_REG_DR0, (data >> offset) & field);
	}
	while (qspi_is_busy(base))
		;
	p_u32 = (u32 *)p_buf;
	hprintf(DEBUG, "read data from mt25t: \r\n");
	hprintf(DEBUG, "dfs: %d, size: %d, data: \r\n", s_conf.dfs, size);
	for (i = 0; i < size; i++) {
		r = REG32_READ(base + QSPI_REG_DR0);
		hprintf(DEBUG, "0x%08x ", r);
		// assign data to buffer field
		field = (1 << s_conf.dfs) - 1;
		if (field == 0)
			field = 0xffffffff;
		*(p_u32 + i * s_conf.dfs / 32) &=
			~(field << ((32 - (i + 1) * s_conf.dfs % 32) % 32));
		*(p_u32 + i * s_conf.dfs / 32) |=
			(r & field) << ((32 - (i + 1) * s_conf.dfs % 32) % 32);
	}
	hprintf(DEBUG, "\r\n");
}

void mt25t_read_bytes(u32 base, u32 addr, u8 *p_buf, u32 size)
{
	u32 i = 0, rxftl, data;
	s_qspi_conf s_conf = qspi_get_default_conf();
	s_conf.dfs = 8;

	while (size) {
		while (qspi_is_busy(base))
			;
		qspi_disable(base);
		qspi_set_conf(base, s_conf);
		qspi_set_spi_dfs(base, s_conf.dfs);
		qspi_set_spi_frf(base, QSPI_STD_SPI);
		qspi_set_mode(base, QSPI_EEPROM_READ);
		qspi_set_spi_sste(base, s_conf.sste);
		rxftl = size > 28 ? 28 : size;
		qspi_set_read_size(base, rxftl);
		qspi_set_txftl(base, 3);

		qspi_enable(base);
		data = (FLASH_CMD_MEMORY_RD << 24) | (addr & 0xffffff);
		REG32_WRITE(base + QSPI_REG_DR0, data >> 24);
		REG32_WRITE(base + QSPI_REG_DR0, (data >> 16) & 0xff);
		REG32_WRITE(base + QSPI_REG_DR0, (data >> 8) & 0xff);
		REG32_WRITE(base + QSPI_REG_DR0, data & 0xff);

		while (qspi_is_busy(base))
			;
		for (i = 0; i < rxftl; i++) {
			*p_buf = REG32_READ(base + QSPI_REG_DR0);
			p_buf++;
		}

		addr += rxftl;
		size -= rxftl;
	}
}

/**
 * @brief make buffer data increment
 * 
 * @param p_buf 
 * @param len 
 */
void mem_data_increment(void *p_buf, u32 len)
{
	u8 *p_u8 = (u8 *)p_buf;
	u32 i = 0;
	for (i = 0; i < len; i++)
		p_u8[i] = i;
}

/**
 * @brief check all byte in buffer is equal to cmp
 * 
 * @param pBuf 
 * @param cmp 
 * @param len 
 * @return int 
 */
int memcmpbyte(void *pBuf, u8 cmp, size_t len)
{
	u8 *pBufData = (u8 *)pBuf;
	u32 i = 0;
	for (i = 0; i < len; i++) {
		if (pBufData[i] != cmp)
			return -1;
	}
	return 0;
}

void print_buffer(u32 *buf, u32 size)
{
	u32 print_buffer_i;

	for (print_buffer_i = 0; print_buffer_i < size; print_buffer_i++)
		hprintf(DEBUG, "0x%08x ", buf[print_buffer_i]);
	hprintf(DEBUG, "\r\n");
}
/**
 * @brief qspi_restore_regs Restore qspi registers
 * @param id
 */
void qspi_restore_regs(u32 base)
{
	u32 i = 0;
	qspi_disable(base);

	for (i = 0; i < BUFFER_LENGTH(qspi_regs_default_table1); i++)
		REG32_WRITE(base + i * 4, qspi_regs_default_table1[i]);
	for (i = 0; i < BUFFER_LENGTH(qspi_regs_default_table2); i++)
		REG32_WRITE(base + i * 4 + 0xf0, qspi_regs_default_table2[i]);
}

/**
 * @brief mt25t standard test, only need to change s_conf
 * 
 * @param s_conf 
 * @param p_name 
 * @return u32 
 */
u32 mt25t_test_standard(u32 base, s_qspi_conf s_conf)
{
	u32 rslt = HVTE_MODULE_TEST_FAIL;
	u32 a_data[8] = { 0 };
	u32 a_buf[8] = { 0 };

	mem_data_increment(a_data, sizeof(a_data));

	qspi_exit_xip(base); //change to normal mode
	qspi_de_endian(base);
	hprintf(DEBUG, "sckdv: %d, data size: %d, dfs: %d, data write: \r\n",
		s_conf.sckdv, BUFFER_LENGTH(a_data), s_conf.dfs);
	print_buffer(a_data, BUFFER_LENGTH(a_buf) * s_conf.dfs / 32);

	mt25t_read_reg(base, s_conf, FLASH_CMD_REG_STATUS_RD);
	mt25t_erase(base, s_conf, FLASH_BASE_ADDR);
	mt25t_write(base, s_conf, FLASH_BASE_ADDR, a_data,
		    BUFFER_LENGTH(a_data));
	mt25t_read(base, s_conf, FLASH_BASE_ADDR, a_buf, BUFFER_LENGTH(a_buf));
	hprintf(DEBUG, "buffer after earse->write->read: \r\n");
	print_buffer(a_buf, BUFFER_LENGTH(a_buf) * s_conf.dfs / 32);
	BUFFER_CMP(a_data, a_buf, BUFFER_LENGTH(a_buf) * s_conf.dfs / 8,
		   HVTE_MODULE_TEST_FAIL);

	mt25t_erase(base, s_conf, FLASH_BASE_ADDR);
	mt25t_read(base, s_conf, FLASH_BASE_ADDR, a_buf, BUFFER_LENGTH(a_buf));
	hprintf(DEBUG, "buffer after earse->read: \r\n");
	print_buffer(a_buf, BUFFER_LENGTH(a_buf) * s_conf.dfs / 32);
	BUFFER_CMP_BYTE(a_buf, MT25T_DEFAULT_AFTER_EARSE,
			BUFFER_LENGTH(a_buf) * s_conf.dfs / 8,
			HVTE_MODULE_TEST_FAIL);

	rslt = HVTE_MODULE_TEST_PASS;
	return rslt;
}

// QSPI_时钟模式
int qspi_clock_mode_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	s_qspi_conf s_conf = qspi_get_default_conf();
	s_conf.dfs = 16;

	hprintf(DEBUG,
		"*******************SCPOL=0，SCPH=0*******************\r\n");
	s_conf.scpol = 0;
	s_conf.scph = 0;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************SCPOL=0，SCPH=1*******************\r\n");
	s_conf.scpol = 0;
	s_conf.scph = 1;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************SCPOL=1，SCPH=0*******************\r\n");
	s_conf.scpol = 1;
	s_conf.scph = 0;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************SCPOL=1，SCPH=1*******************\r\n");
	s_conf.scpol = 1;
	s_conf.scph = 1;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}
	return result;
}

// QSPI_数据长度_设置不同数据帧长度传输数据
int qspi_date_frame_size_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	s_qspi_conf s_conf = qspi_get_default_conf();

	hprintf(DEBUG,
		"*****************date frame size : 4*****************\r\n");
	s_conf.dfs = 4;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*****************date frame size : 16*****************\r\n");
	s_conf.dfs = 16;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*****************date frame size : 32*****************\r\n");
	s_conf.dfs = 32;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}
	return result;
}

// QSPI_速率测试
int qspi_baudr_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	s_qspi_conf s_conf = qspi_get_default_conf();
	s_conf.dfs = 16;

	hprintf(DEBUG,
		"*********************SCKDV : 8*********************\r\n");
	s_conf.sckdv = 8;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*********************SCKDV : 10*********************\r\n");
	s_conf.sckdv = 10;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*********************SCKDV : 12*********************\r\n");
	s_conf.sckdv = 12;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*********************SCKDV : 14*********************\r\n");
	s_conf.sckdv = 14;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*********************SCKDV : 16*********************\r\n");
	s_conf.sckdv = 16;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*********************SCKDV : 18*********************\r\n");
	s_conf.sckdv = 18;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*********************SCKDV : 20*********************\r\n");
	s_conf.sckdv = 20;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}
	return result;
}

//QSPI_以最高速率持续发送数据测试
int qspi_max_baudr_continue_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS, i;
	s_qspi_conf s_conf = qspi_get_default_conf();
	result = HVTE_MODULE_TEST_PASS;
	s_conf.dfs = 16;
	s_conf.sckdv = 8;

	for (i = 0; i < 100; i++)
		result = mt25t_test_standard(reg_base, s_conf);
	return result;
}

//QSPI_EEPROM READ模式下能正常通信
int qspi_eeprom_read_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	s_qspi_conf s_conf = qspi_get_default_conf();
	s_conf.dfs = 32;
	s_conf.scpol = 0;
	s_conf.scph = 0;

	hprintf(DEBUG, "test eeprom read\r\n");
	return mt25t_test_standard(reg_base, s_conf);
}

//QSPI_EEPROM READ模式下不能读取超过地址范围的数据
int qspi_eeprom_read_exceed_addr_range_test(zebu_test_mod_t *mod,
						   void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_FAIL;
	u32 a_data[8] = { 0 };
	u32 a_buf[8] = { 0 };

	s_qspi_conf s_conf = qspi_get_default_conf();
	mem_data_increment(a_data, sizeof(a_data));

	hprintf(DEBUG, "test read out of mem addr, at sheet raw 17\r\n");
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);
	qspi_restore_regs(reg_base);
	hprintf(DEBUG, "sckdv: %d data size: %d, data write: \r\n",
		s_conf.sckdv, (u16)BUFFER_LENGTH(a_data));
	print_buffer(a_data, BUFFER_LENGTH(a_data));

	mt25t_read_reg(reg_base, s_conf, FLASH_CMD_REG_STATUS_RD);
	mt25t_erase(reg_base, s_conf, 0xffffff);
	mt25t_write(reg_base, s_conf, 0xffffff, a_data, BUFFER_LENGTH(a_data));
	mt25t_read(reg_base, s_conf, 0xffffff, a_buf, BUFFER_LENGTH(a_buf));
	hprintf(DEBUG, "buffer after earse->write->read: \r\n");
	print_buffer(a_buf, BUFFER_LENGTH(a_buf));
	if (memcmp((a_data), (a_buf),
		   (BUFFER_LENGTH(a_buf) * s_conf.dfs / 8)) != 0) {
		hprintf(DEBUG, "buffer compare faild: %s, %s, %d\r\n", __FILE__,
			__FUNCTION__, __LINE__);
		return HVTE_MODULE_TEST_PASS;
	}

	result = HVTE_MODULE_TEST_FAIL;
	return result;
}

//QSPI_片选控制
int qspi_cs_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	s_qspi_conf s_conf = qspi_get_default_conf();
	s_conf.dfs = 32;
	s_conf.scpol = 0;
	s_conf.scph = 0;
	s_conf.sste = 0;

	hprintf(DEBUG,
		"*********************slave : 1*********************\r\n");
	s_conf.slave = 1;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*********************slave : 2*********************\r\n");
	s_conf.slave = 2;
	if (mt25t_test_standard(reg_base, s_conf) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	s_conf.slave = 1;
	qspi_set_conf(reg_base, s_conf);
	return result;
}

#if (defined(CONFIG_R5_SAFETY) || defined(CONFIG_A55))
// QSPI中断
int qspi_intr_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_FAIL;
	s_qspi_conf s_conf = qspi_get_default_conf();
	s_conf.dfs = 32;
	s_conf.scpol = 0;
	s_conf.scph = 0;
	s_conf.sste = 0;
	while (qspi_is_busy(reg_base))
		;
	qspi_disable(reg_base);

	int_test = 1;
	result = mt25t_test_standard(reg_base, s_conf);
	if (result == HVTE_MODULE_TEST_FAIL)
		return result;
	;

	hprintf(DEBUG, "int_flag: %d\r\n", int_flag);
	if (int_flag == 1)
		result = HVTE_MODULE_TEST_PASS;
	else
		result = HVTE_MODULE_TEST_FAIL;
	int_flag = 0;
	int_test = 0;
	return result;
}
#endif

/*
 验证指令和地址使用标准模式传输，数据使用增强模式传输，可以向从机读数据

 预置条件：
 1. SOC设置为Master模式,外设QSPI设备设置为Slave模式
 2. 复位,去使能SSI
 3. 帧格式为Motorola SPI,SER[1:0]=01,SCPOL=0,SCPH=0
 4. SPI_CTRLR0.INST_L=16bit,SPI_CTRLR0.ADDR_L=16bit
 5. CTRLR0.DFS=16bit
 6. BAUDR.SCKDV=200,SPI_CTRLR0.WAIT_CYCLES=0x1f
 7. SPI_CTRLR0.TRANS_TYPE=0x0,CTRLR0.SPI_FRF=0x1

 操作步骤：
 1. 使能SSI
 2. 主机Drx写入16bit指令+16bit地址,从机数据寄存器写入16bit data1
 3. 检查主机接收数据以及数据传输电平状态
    指令和地址使用标准模式传输，数据使用Dual模式传输，主机成功接收数据data1
 4. 去使能SSI,其他预置条件保持不变,设置CTRLR0.SPI_FRF=0x2,重复步骤1~3
    指令和地址使用标准模式传输，数据使用Quad模式传输，主机成功接收数据data1
 5. 去使能SSI,其他预置条件保持不变,设置CTRLR0.SPI_FRF=0x3,重复步骤1~3
    指令和地址使用标准模式传输，数据使用Octal模式传输，主机成功接收数据data1
 */
int qspi_data_enhance_rd_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i, reg_temp;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	memset(data, 0xaf, sizeof(data));
	memset(datard_dual, 0x0, sizeof(datard_dual));
	memset(datard_quad, 0x0, sizeof(datard_quad));

	/*write data*/
	reg_temp = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "status = 0x%x\r\n", reg_temp);
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	reg_temp = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "status = 0x%x\r\n", reg_temp);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	qspi_flash_1_1_2_rd(reg_base, datard_dual, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_DUAL_OUT_FAST_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr dual data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_dual[i]);
	}

	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	//TODO 1_1_8_rd
	//void qspi_flash_1_1_8_rd(u32 *data, u32 size, u8 cmd, u32 addr)

	return result;
}

/*
 验证指令使用标准模式,地址和数据使用增强模式传输，可以向从机读数据

 预置条件：
 1. SOC设置为Master模式,外设QSPI设备设置为Slave模式
 2. 复位,去使能SSI
 3. 帧格式为Motorola SPI,SER[1:0]=01,SCPOL=0,SCPH=0
 4. SPI_CTRLR0.INST_L=16bit,SPI_CTRLR0.ADDR_L=16bit
 5. CTRLR0.DFS=16bit
 6. BAUDR.SCKDV=200,SPI_CTRLR0.WAIT_CYCLES=0x1f
 7. SPI_CTRLR0.TRANS_TYPE=0x1,CTRLR0.SPI_FRF=0x1

 操作步骤：
 1. 使能SSI
 2. 主机Drx写入16bit指令+16bit地址,从机数据寄存器写入16bit data1
 3. 检查主机接收数据以及数据传输电平状态
    指令和地址使用标准模式传输，数据使用Dual模式传输，主机成功接收数据data1
 4. 去使能SSI,其他预置条件保持不变,设置CTRLR0.SPI_FRF=0x2,重复步骤1~3
    指令和地址使用标准模式传输，数据使用Quad模式传输，主机成功接收数据data1
 5. 去使能SSI,其他预置条件保持不变,设置CTRLR0.SPI_FRF=0x3,重复步骤1~3
    指令和地址使用标准模式传输，数据使用Octal模式传输，主机成功接收数据data1
 */
int qspi_addr_data_enhance_rd_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	memset(data, 0xaf, sizeof(data));
	memset(datard_dual, 0x0, sizeof(datard_dual));
	memset(datard_quad, 0x0, sizeof(datard_quad));

	qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	/*write data*/
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_1_2_2_rd(reg_base, datard_dual, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_DUAL_IO_FAST_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd dual addr dual data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_dual[i]);
	}

	qspi_flash_1_4_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_IO_FAST_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd quad addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	//TODO 1_8_8_rd
	//void qspi_flash_1_8_8_rd(u32 *data, u32 size, u8 cmd, u32 addr)

	return result;
}

/* QSPI_DDR模式_Quad传输模式写数据到从机 (flash(mt25t) not support )*/
#if 0
int qspi_ddr_quad_wr_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i, reg_temp;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	/* 擦除 */
	memset(datard, 0x0, sizeof(data));
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "read data:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	reg_temp = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "status = 0x%x\r\n", reg_temp);
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR,
				      QSPI_ADDR_LEN_24); //erase
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "read erased data:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	memset(data, 0x5f, sizeof(data));

	/* dual addr&cmd */
	reg_temp = qspi_reg_1_0_1_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n", reg_temp);
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN); //write enable
	reg_temp &= (~(0xA0)); //enable device dual mode
	qspi_reg_1_0_1_wr(reg_base, FLASH_CMD_REG_ENHANCED_WR, reg_temp);
	reg_temp = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n", reg_temp);

	reg_temp = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "status = 0x%x\r\n", reg_temp);
	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_WR_EN); //write enable
	reg_temp = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "status = 0x%x\r\n", reg_temp);

	qspi_set_wait_clock(reg_base, 0);
	qspi_set_addr_len(reg_base, 0);
	qspi_set_cmd_len(reg_base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(reg_base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(reg_base, 0);

	qspi_set_txftl(reg_base, 8);
	qspi_set_spi_frf(reg_base, QSPI_QUAD_SPI);
	qspi_set_spi_sste(reg_base, 0);
	qspi_set_spi_dfs(reg_base, 32);
	qspi_set_mode(reg_base, QSPI_TX_ONLY);
	qspi_enable(reg_base);
	REG32_WRITE(reg_base + QSPI_REG_DR0,
		    ((FLASH_CMD_PAGE_PROG << 24) | FLASH_BASE_ADDR));
	qspi_en_endian(reg_base);
	for (i = 0; i < 8; i++) {
		REG32_WRITE(reg_base + QSPI_REG_DR0, data[i]);
	}
	qspi_de_endian(reg_base);
	hprintf(DEBUG, "5\n");
	while (qspi_is_busy(reg_base)) {
		putc('c');
	}
	hprintf(DEBUG, "5\n");
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)
	hprintf(DEBUG, "6\n");

	/*quit ddr */
	reg_temp = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n", reg_temp);
	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_WR_EN); //write enable
	qspi_reg_4_0_4_wr(reg_base, FLASH_CMD_REG_ENHANCED_WR, 0xFF);
	reg_temp = qspi_reg_1_0_1_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n", reg_temp);

	memset(datard, 0x0, sizeof(datard));
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "read the written data:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < 8; i++) {
		HCOMP(data[i], datard[i]);
		if (result == HVTE_MODULE_TEST_FAIL)
			return result;
	}

	return result;
}
#endif

/* QSPI_DDR模式_Quad传输模式读数据到从机 */
int qspi_ddr_quad_rd_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i, reg_temp;
	u8 baudr = 10;
	u32 drive_edge = 1;

	hprintf(DEBUG, "this is %s.   baudr : %d	  drive_edge : %d\r\n",
		__FUNCTION__, baudr, drive_edge);

	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	for (i = 0; i < QSPI_RW_TEST_LEN; i++)
		data[i] = 0x12345678;
	memset(datard, 0x0, sizeof(datard));

	qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	/*write data*/
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	/* dual addr&cmd */
	reg_temp = qspi_reg_1_0_1_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "1 : enhance reg: 0x%x\r\n", reg_temp);
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN); //write enable
	reg_temp &= (~(0xA0)); //enable device dual mode
	qspi_reg_1_0_1_wr(reg_base, FLASH_CMD_REG_ENHANCED_WR, reg_temp);
	reg_temp = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "2 : enhance reg: 0x%x\r\n", reg_temp);

	while (qspi_is_busy(reg_base))
		;
	qspi_disable(reg_base);
	qspi_set_scpol(reg_base, 0);
	qspi_set_scph(reg_base, 0);
	qspi_set_sckdv(reg_base, baudr);
	qspi_set_ser(reg_base, 0x1);
	qspi_set_wait_clock(reg_base, 8);
	qspi_set_addr_len(reg_base, QSPI_ADDR_LEN_24);
	qspi_set_cmd_len(reg_base, QSPI_CMD_LEN_8);
	qspi_set_spi_frf(reg_base, QSPI_QUAD_SPI);
	qspi_set_cmd_addr_line_mode(reg_base, CMD_FRF_ADDR_FRF);
	qspi_set_ddr_mode(reg_base, 0x1);
	qspi_set_ddr_drive_edge(reg_base, drive_edge);
	qspi_set_spi_dfs(reg_base, 32);
	qspi_set_mode(reg_base, QSPI_RX_ONLY);
	qspi_set_read_size(reg_base, QSPI_RW_TEST_LEN);
	qspi_enable(reg_base);
	REG32_WRITE(reg_base + QSPI_REG_DR0,
		    FLASH_CMD_MEMORY_DTR_QUAD_IO_FAST_RD);
	REG32_WRITE(reg_base + QSPI_REG_DR0, FLASH_BASE_ADDR);

	while (qspi_is_busy(reg_base))
		;
	qspi_en_endian(reg_base);
	hprintf(DEBUG, "RXTFL reg: 0x%x\r\n", REG32_READ(reg_base + 0x24));
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		datard[i] = REG32_READ(reg_base + QSPI_REG_DR0);
	}
	qspi_de_endian(reg_base);
	hprintf(DEBUG, "RXTFL reg: 0x%x\r\n", REG32_READ(reg_base + 0x24));
	hprintf(DEBUG, "read data:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard[i]);
	}

	/*quit ddr */
	reg_temp = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n", reg_temp);
	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_WR_EN); //write enable
	reg_temp &= (0xFF); //enable device dual mode
	qspi_reg_4_0_4_wr(reg_base, FLASH_CMD_REG_ENHANCED_WR, 0xFF);
	reg_temp = qspi_reg_1_0_1_rd(reg_base, FLASH_CMD_REG_ENHANCED_RD);
	hprintf(DEBUG, "enhance reg: 0x%x\r\n", reg_temp);

	return result;
}

#if (defined(CONFIG_R5_SAFETY))
/* QSPI_DMA模式_DMA模式下发送数据 cant run on a55*/
int qspi_dma_tx_data(u32 reg_base, u32 chx, u32 tdl)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i, reg_temp;

	memset(data_sdma, 0x5f, sizeof(data_sdma));
	memset(data_sdma_rd, 0x0, sizeof(data_sdma_rd));
	data_sdma[0] = ((FLASH_CMD_PAGE_PROG & 0xFF) << 24) |
		       (FLASH_BASE_ADDR & 0xFFFFFF);

	/* 擦除 */
	reg_temp = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "status = 0x%x\r\n", reg_temp);
	qspi_flash_send_cmd(
		reg_base,
		FLASH_CMD_WR_EN); //write enable, send before program/write/erase operation
	reg_temp = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "status = 0x%x\r\n", reg_temp);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR,
				      QSPI_ADDR_LEN_24); //sector erase
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, data_sdma_rd, QSPI_SDMA_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "read erased data:\r\n");
	for (i = 0; i < QSPI_SDMA_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", data_sdma_rd[i]);
	}
	hprintf(DEBUG, "\r\n");

	/* SDMA setting */

	hprintf(DEBUG, "DMA ID(0x%x):0x%x\r\n", DMA_BASE_ADDR + 0x0,
		REG32_READ(DMA_BASE_ADDR + 0x0));
	hprintf(DEBUG, "DMA ID(0x%x):0x%x\r\n", DMA_BASE_ADDR + 0x4,
		REG32_READ(DMA_BASE_ADDR + 0x4));
	/* DMAC_CFGREG */
	REG32_WRITE(DMAC_CFGREG, REG32_READ(DMAC_CFGREG) | 0x1); //enable dma

	/* DMA_ChEnReg:select available channel */
	reg_temp = REG32_READ(DMAC_CHENREG);
	if (chx == DMA_CH1) {
		reg_temp =
			(reg_temp & (~(0x1 << 0))) |
			(0x0
			 << 0); //CH1_EN, this bit auto clear after transfer done
		reg_temp = (reg_temp & (~(0x1 << 8))) |
			   (0x1 << 8); //CH1_EN_WE, write only bit
	} else if (chx == DMA_CH2) {
		reg_temp =
			(reg_temp & (~(0x1 << 1))) |
			(0x0
			 << 1); //CH2_EN, this bit auto clear after transfer done
		reg_temp = (reg_temp & (~(0x1 << 9))) |
			   (0x1 << 9); //CH2_EN_WE, write only bit
	}
	REG32_WRITE(DMAC_CHENREG, reg_temp);

	/* CHX_CFG */
	reg_temp = REG32_READ(DMAC_CHX_CFG(chx));
	reg_temp =
		(reg_temp & (~(0x3 << 0))) |
		(0x2
		 << 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	reg_temp =
		(reg_temp & (~(0x3 << 2))) |
		(0x2
		 << 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	if (QSPI0_BASE_ADDR == reg_base)
		reg_temp =
			(reg_temp & (~(0xF << 11))) |
			(0
			 << 11); //DST_PER,assign a hardware handshaking interface to the destination of channelx
	else if (QSPI1_BASE_ADDR == reg_base)
		reg_temp = (reg_temp & (~(0xF << 11))) | (2 << 11);
	REG32_WRITE(DMAC_CHX_CFG(chx), reg_temp);
	hprintf(DEBUG, "DMAC_CHX_CFG(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_CFG(chx), REG32_READ(DMAC_CHX_CFG(chx)));

	reg_temp = REG32_READ(DMAC_CHX_CFG(chx) + 0x4); //high 32 bit
	reg_temp =
		(reg_temp & (~(0x7 << 0))) |
		(0x1
		 << 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	reg_temp =
		(reg_temp & (~(0x1 << 3))) |
		(0x1
		 << 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	reg_temp =
		(reg_temp & (~(0x1 << 4))) |
		(0x0
		 << 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	reg_temp = (reg_temp & (~(0xF << 23))) |
		   (0x0 << 23); //SRC_OSR_LMT,source outstanding request limit
	reg_temp =
		(reg_temp & (~(0xF << 27))) |
		(0x0 << 27); //DST_OSR_LMT,destination outstanding request limit
	REG32_WRITE(DMAC_CHX_CFG(chx) + 0x4, reg_temp);
	hprintf(DEBUG, "DMAC_CHX_CFG(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_CFG(chx) + 0x4, REG32_READ(DMAC_CHX_CFG(chx) + 0x4));

	/* CHX_SAR */
	REG32_WRITE(DMAC_CHX_SAR(chx), (u64)data_sdma); //source addr
	REG32_WRITE(DMAC_CHX_SAR(chx) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_SAR(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_SAR(chx), REG32_READ(DMAC_CHX_SAR(chx)));

	/* CHX_DAR */
	REG32_WRITE(DMAC_CHX_DAR(chx), reg_base + QSPI_REG_DR0);
	REG32_WRITE(DMAC_CHX_DAR(chx) + 0x4, 0);

	hprintf(DEBUG, "DMAC_CHX_DAR(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_DAR(chx), REG32_READ(DMAC_CHX_DAR(chx)));

	/* CHX_BLOCK_TS: block transfer size */
	REG32_WRITE(DMAC_CHX_BLOCK_TS(chx), QSPI_SDMA_TEST_LEN - 1);
	REG32_WRITE(DMAC_CHX_BLOCK_TS(chx) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_BLOCK_TS(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_BLOCK_TS(chx), REG32_READ(DMAC_CHX_BLOCK_TS(chx)));

	/* CHX_CTRL */
	reg_temp = REG32_READ(DMAC_CHX_CTL(chx));
	reg_temp =
		(reg_temp & (~(0x1 << 4))) |
		(0x0
		 << 4); //source address increment: 0(increment), 1(no change)
	reg_temp =
		(reg_temp & (~(0x1 << 6))) |
		(0x1
		 << 6); //destination address increment: 0(increment), 1(no change)
	reg_temp = (reg_temp & (~(0x7 << 8))) |
		   (0x2 << 8); //source transfer width: 2(32 bits)
	reg_temp = (reg_temp & (~(0x7 << 11))) |
		   (0x2 << 11); //destination transfer width: 2(32 bits)
	reg_temp =
		(reg_temp & (~(0xF << 14))) |
		(0x0
		 << 14); //source burst transaction length: 3(burst length = 16)
	reg_temp =
		(reg_temp & (~(0xF << 18))) |
		(0x0
		 << 18); //destination burst transaction length: 3(burst length = 16)
	REG32_WRITE(DMAC_CHX_CTL(chx), reg_temp);
	hprintf(DEBUG, "DMAC_CHX_CTL(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_CTL(chx), REG32_READ(DMAC_CHX_CTL(chx)));

	reg_temp = REG32_READ(DMAC_CHX_CTL(chx) + 0x4); //high 32 bit
	reg_temp = (reg_temp & (~(0x1 << 6))) |
		   (0x1 << 6); //source burst length enable
	reg_temp =
		(reg_temp & (~(0xFF << 7))) | (0x1 << 7); //source burst length
	reg_temp = (reg_temp & (~(0x1 << 15))) |
		   (0x1 << 15); //destination burst length enable
	reg_temp = (reg_temp & (~(0xFF << 16))) |
		   (0x0 << 16); //destination burst length
	reg_temp = (reg_temp & (~(0x1 << 26))) |
		   (0x1 << 26); //Interrput on completion of block transfer
	reg_temp = (reg_temp & (~(0x1 << 30))) |
		   (0x1 << 30); //Last shadow register/linked list item
	reg_temp = (reg_temp & (~(0x1 << 31))) |
		   (0x1 << 31); //shadow register comntent/link list item valid
	REG32_WRITE(DMAC_CHX_CTL(chx) + 0x4, reg_temp);
	hprintf(DEBUG, "DMAC_CHX_CTL(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_CTL(chx) + 0x4, REG32_READ(DMAC_CHX_CTL(chx)) + 0x4);

	/* QSPI */
	qspi_exit_xip(reg_base);
	qspi_de_endian(reg_base);
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);

	while (qspi_is_busy(reg_base))
		;
	qspi_disable(reg_base);
	qspi_set_sckdv(reg_base, 0x8);
	qspi_set_spi_dfs(reg_base, 32);
	qspi_set_txftl(reg_base, 0);
	qspi_set_spi_frf(reg_base, QSPI_STD_SPI);
	qspi_set_mode(reg_base, QSPI_TX_ONLY);
	qspi_set_spi_sste(reg_base, 0);
	qspi_set_tdma_en(reg_base, 0x1);
	qspi_set_dma_tdl(reg_base, tdl);
	qspi_enable(reg_base);
	while (qspi_is_busy(reg_base))
		;

	/* DMA_ChEnReg:select available channel */
	reg_temp = REG32_READ(DMAC_CHENREG);
	if (chx == DMA_CH1) {
		reg_temp =
			(reg_temp & (~(0x1 << 0))) |
			(0x1
			 << 0); //CH1_EN, this bit auto clear after transfer done
		reg_temp = (reg_temp & (~(0x1 << 8))) |
			   (0x1 << 8); //CH1_EN_WE, write only bit
	} else if (chx == DMA_CH2) {
		reg_temp =
			(reg_temp & (~(0x1 << 1))) |
			(0x1
			 << 1); //CH2_EN, this bit auto clear after transfer done
		reg_temp = (reg_temp & (~(0x1 << 9))) |
			   (0x1 << 9); //CH2_EN_WE, write only bit
	}
	REG32_WRITE(DMAC_CHENREG, reg_temp);

	while ((REG32_READ(DMAC_CHX_INTSTATUS(chx)) & 0x1) == 0x0)
		; //wait block transfer done interrupt

	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		; //bit0: 1(write in progress), 0(idle)
	qspi_flash_1_1_1_rd(reg_base, data_sdma_rd, QSPI_SDMA_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "read the written data by DMA:\r\n");
	for (i = 0; i < QSPI_SDMA_TEST_LEN; i++) {
		hprintf(DEBUG, "%d : 0x%08x \t", i, data_sdma_rd[i]);
	}
	hprintf(DEBUG, "\r\n");

	memset(data_sdma, 0x5f, sizeof(data_sdma));
	for (i = 0; i < QSPI_SDMA_TEST_LEN - 1; i++) {
		HCOMP(data_sdma[i], data_sdma_rd[i]);
	}

	/*disable transmit dma*/
	while (qspi_is_busy(reg_base))
		;
	qspi_disable(reg_base);
	qspi_set_tdma_en(reg_base, 0x0);
	qspi_set_dma_tdl(reg_base, 0x0);
	qspi_enable(reg_base);
	qspi_disable(reg_base);
	return result;
}

/* QSPI_DMA模式_DMA模式下接收数据 cant run on a55*/
int qspi_dma_rx_data(u32 reg_base, u32 chx, u32 rdl)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i, reg_temp;

	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	memset(data, 0xaf, sizeof(data));

	/*write data*/
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR + 16 * 4,
			    QSPI_ADDR_LEN_24);
	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR + 16 * 4,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	/* SDMA setting */
	hprintf(DEBUG, "datard_std addr:%x\r\n", data_sdma_rd);
	hprintf(DEBUG, "DMA ID(0x%x):0x%x\r\n", DMA_BASE_ADDR + 0x0,
		REG32_READ(DMA_BASE_ADDR + 0x0));
	hprintf(DEBUG, "DMA ID(0x%x):0x%x\r\n", DMA_BASE_ADDR + 0x4,
		REG32_READ(DMA_BASE_ADDR + 0x4));
	/* DMAC_CFGREG */
	REG32_WRITE(DMAC_CFGREG, REG32_READ(DMAC_CFGREG) | 0x1); //enable dma

	/* DMA_ChEnReg:select available channel */
	reg_temp = REG32_READ(DMAC_CHENREG);
	if (chx == DMA_CH1) {
		reg_temp =
			(reg_temp & (~(0x1 << 0))) |
			(0x0
			 << 0); //CH1_EN, this bit auto clear after transfer done
		reg_temp = (reg_temp & (~(0x1 << 8))) |
			   (0x1 << 8); //CH1_EN_WE, write only bit
	} else if (chx == DMA_CH2) {
		reg_temp =
			(reg_temp & (~(0x1 << 1))) |
			(0x0
			 << 1); //CH2_EN, this bit auto clear after transfer done
		reg_temp = (reg_temp & (~(0x1 << 9))) |
			   (0x1 << 9); //CH2_EN_WE, write only bit
	}
	REG32_WRITE(DMAC_CHENREG, reg_temp);
	hprintf(DEBUG, "DMAC_CHENREG(0x%x):0x%x\r\n", DMAC_CHENREG,
		REG32_READ(DMAC_CHENREG));

	/* CHX_CFG */
	reg_temp = REG32_READ(DMAC_CHX_CFG(chx));
	reg_temp =
		(reg_temp & (~(0x3 << 0))) |
		(0x2
		 << 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	reg_temp =
		(reg_temp & (~(0x3 << 2))) |
		(0x2
		 << 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	if (QSPI0_BASE_ADDR == reg_base)
		reg_temp = (reg_temp & (~(0xF << 4))) | (1 << 4); //SRC_PER
	else if (QSPI1_BASE_ADDR == reg_base)
		reg_temp = (reg_temp & (~(0xF << 4))) | (3 << 4);

	REG32_WRITE(DMAC_CHX_CFG(chx), reg_temp);
	hprintf(DEBUG, "data:0x%x\r\n", reg_temp);
	hprintf(DEBUG, "DMAC_CHX_CFG(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_CFG(chx), REG32_READ(DMAC_CHX_CFG(chx)));

	reg_temp = REG32_READ(DMAC_CHX_CFG(chx) + 0x4); //high 32 bit
	reg_temp =
		(reg_temp & (~(0x7 << 0))) |
		(0x2
		 << 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	reg_temp =
		(reg_temp & (~(0x1 << 3))) |
		(0x0
		 << 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	reg_temp =
		(reg_temp & (~(0x1 << 4))) |
		(0x1
		 << 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	reg_temp = (reg_temp & (~(0xF << 23))) |
		   (0x0 << 23); //SRC_OSR_LMT,source outstanding request limit
	reg_temp =
		(reg_temp & (~(0xF << 27))) |
		(0x0 << 27); //DST_OSR_LMT,destination outstanding request limit
	REG32_WRITE(DMAC_CHX_CFG(chx) + 0x4, reg_temp);
	hprintf(DEBUG, "DMAC_CHX_CFG(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_CFG(chx) + 0x4, REG32_READ(DMAC_CHX_CFG(chx) + 0x4));

	/* CHX_SAR */
	/* SDMA看到的QSPI0的基地址为0x0000_0000         */

	REG32_WRITE(DMAC_CHX_SAR(chx), reg_base + QSPI_REG_DR0);
	REG32_WRITE(DMAC_CHX_SAR(chx) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_SAR(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_SAR(chx), REG32_READ(DMAC_CHX_SAR(chx)));

	/* CHX_DAR */
	REG32_WRITE(DMAC_CHX_DAR(chx), (u64)data_sdma_rd); //destination addr
	REG32_WRITE(DMAC_CHX_DAR(chx) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_DAR(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_DAR(chx), REG32_READ(DMAC_CHX_DAR(chx)));

	/* CHX_BLOCK_TS: block transfer size */
	REG32_WRITE(DMAC_CHX_BLOCK_TS(chx), QSPI_SDMA_TEST_LEN - 1);
	REG32_WRITE(DMAC_CHX_BLOCK_TS(chx) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_BLOCK_TS(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_BLOCK_TS(chx), REG32_READ(DMAC_CHX_BLOCK_TS(chx)));

	/* CHX_CTL */
	reg_temp = REG32_READ(DMAC_CHX_CTL(chx));
	reg_temp =
		(reg_temp & (~(0x1 << 4))) |
		(0x1
		 << 4); //source address increment: 0(increment), 1(no change)
	reg_temp =
		(reg_temp & (~(0x1 << 6))) |
		(0x0
		 << 6); //destination address increment: 0(increment), 1(no change)
	reg_temp = (reg_temp & (~(0x7 << 8))) |
		   (0x2 << 8); //source transfer width: 2(32 bits)
	reg_temp = (reg_temp & (~(0x7 << 11))) |
		   (0x2 << 11); //destination transfer width: 2(32 bits)
	reg_temp =
		(reg_temp & (~(0xF << 14))) |
		(0x2
		 << 14); //source burst transaction length: 2(burst length = 8)(MAX_MSIZE = 8)
	reg_temp =
		(reg_temp & (~(0xF << 18))) |
		(0x0
		 << 18); //destination burst transaction length: 1(burst length = 4)
	REG32_WRITE(DMAC_CHX_CTL(chx), reg_temp);
	hprintf(DEBUG, "DMAC_CHX_CTL(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_CTL(chx), REG32_READ(DMAC_CHX_CTL(chx)));

	reg_temp = REG32_READ(DMAC_CHX_CTL(chx) + 0x4); //high 32 bit
	reg_temp = (reg_temp & (~(0x1 << 6))) |
		   (0x1 << 6); //source burst length enable
	reg_temp =
		(reg_temp & (~(0xFF << 7))) | (0x1 << 7); //source burst length
	reg_temp = (reg_temp & (~(0x1 << 15))) |
		   (0x1 << 15); //destination burst length enable
	reg_temp = (reg_temp & (~(0xFF << 16))) |
		   (0x0 << 16); //destination burst length
	reg_temp = (reg_temp & (~(0x1 << 26))) |
		   (0x1 << 26); //Interrput on completion of block transfer
	reg_temp = (reg_temp & (~(0x1 << 30))) |
		   (0x1 << 30); //Last shadow register/linked list item
	reg_temp = (reg_temp & (~(0x1 << 31))) |
		   (0x1 << 31); //shadow register comntent/link list item valid
	REG32_WRITE(DMAC_CHX_CTL(chx) + 0x4, reg_temp);
	hprintf(DEBUG, "DMAC_CHX_CTL(%d)(0x%x):0x%x\r\n", chx,
		DMAC_CHX_CTL(chx) + 0x4, REG32_READ(DMAC_CHX_CTL(chx)) + 0x4);

	while (qspi_is_busy(reg_base))
		;
	qspi_disable(reg_base);
	qspi_set_sckdv(reg_base, 0x8);
	qspi_set_spi_dfs(reg_base, 32);
	qspi_set_spi_frf(reg_base, QSPI_STD_SPI);
	qspi_set_mode(reg_base, QSPI_EEPROM_READ);
	qspi_set_spi_sste(reg_base, 0);
	qspi_set_read_size(reg_base, QSPI_SDMA_TEST_LEN);
	qspi_set_rdma_en(reg_base, 0x1);
	qspi_set_dma_rdl(reg_base, rdl);
	qspi_enable(reg_base);
	REG32_WRITE(
		reg_base + QSPI_REG_DR0,
		(FLASH_CMD_MEMORY_RD << 24) |
			(FLASH_BASE_ADDR &
			 0xFFFFFF)); //std mode cmd, addr can in the same fifo location
	while (qspi_is_busy(reg_base))
		;
	qspi_en_endian(reg_base);

	reg_temp = REG32_READ(DMAC_CHENREG);
	if (chx == DMA_CH1) {
		reg_temp =
			(reg_temp & (~(0x1 << 0))) |
			(0x1
			 << 0); //CH1_EN, this bit auto clear after transfer done
		reg_temp = (reg_temp & (~(0x1 << 8))) |
			   (0x1 << 8); //CH1_EN_WE, write only bit
	} else if (chx == DMA_CH2) {
		reg_temp =
			(reg_temp & (~(0x1 << 1))) |
			(0x1
			 << 1); //CH2_EN, this bit auto clear after transfer done
		reg_temp = (reg_temp & (~(0x1 << 9))) |
			   (0x1 << 9); //CH2_EN_WE, write only bit
	}
	REG32_WRITE(DMAC_CHENREG, reg_temp);

	while ((REG32_READ(DMAC_CHX_INTSTATUS(chx)) & 0x1) == 0x0)
		; //wait block transfer done interrupt

	qspi_de_endian(reg_base);

	hprintf(DEBUG, "read data by DMA:\r\n");
	for (i = 0; i < QSPI_SDMA_TEST_LEN; i++) {
		hprintf(DEBUG, "%d : 0x%08x \t", i, data_sdma_rd[i]);
	}
	hprintf(DEBUG, "\r\n");

	memset(data_sdma, 0xaf, sizeof(data_sdma));
	for (i = 0; i < QSPI_SDMA_TEST_LEN; i++) {
		HCOMP(data_sdma[i], data_sdma_rd[i]);
	}

	/*disable receive dma*/
	while (qspi_is_busy(reg_base))
		;
	qspi_disable(reg_base);
	qspi_set_rdma_en(reg_base, 0x0);
	qspi_set_dma_rdl(reg_base, 0x0);
	qspi_enable(reg_base);
	qspi_disable(reg_base);
	return result;
}

int qspi_dma_tx_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 chx, tdl;

	hprintf(DEBUG,
		"*******************dma channel=1，dma tdl=31*******************\r\n");
	chx = 1;
	tdl = 31;
	if (qspi_dma_tx_data(reg_base, chx, tdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************dma channel=2，dma tdl=31*******************\r\n");
	chx = 2;
	tdl = 31;
	if (qspi_dma_tx_data(reg_base, chx, tdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************dma channel=1，dma tdl=31*******************\r\n");
	chx = 1;
	tdl = 28;
	if (qspi_dma_tx_data(reg_base, chx, tdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************dma channel=1，dma tdl=31*******************\r\n");
	chx = 1;
	tdl = 24;
	if (qspi_dma_tx_data(reg_base, chx, tdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************dma channel=1，dma tdl=31*******************\r\n");
	chx = 1;
	tdl = 0;
	if (qspi_dma_tx_data(reg_base, chx, tdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	return result;
}
int qspi_dma_rx_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 chx, rdl;

	hprintf(DEBUG,
		"*******************dma channel=1，dma rdl=31*******************\r\n");
	chx = 1;
	rdl = 0;
	if (qspi_dma_rx_data(reg_base, chx, rdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************dma channel=2，dma rdl=31*******************\r\n");
	chx = 2;
	rdl = 0;
	if (qspi_dma_rx_data(reg_base, chx, rdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************dma channel=1，dma rdl=31*******************\r\n");
	chx = 1;
	rdl = 3;
	if (qspi_dma_rx_data(reg_base, chx, rdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	hprintf(DEBUG,
		"*******************dma channel=1，dma rdl=31*******************\r\n");
	chx = 1;
	rdl = 7;
	if (qspi_dma_rx_data(reg_base, chx, rdl) == HVTE_MODULE_TEST_FAIL) {
		result = HVTE_MODULE_TEST_FAIL;
		return result;
	}

	return result;
}
#endif

int qspi_3byte_addr_test_GD25LQ256D(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;
	u8 status_h;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		//        data[i] = (i << 24) | (i << 16) | (i << 8) | i;
		data[i] = 0x12345678;
	}
	hprintf(DEBUG, "write data :\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", data[i]);
	}
	hprintf(DEBUG, "\r\n");
	memset(datard_std, 0x0, sizeof(datard_std));
	memset(datard_dual, 0x0, sizeof(datard_dual));
	memset(datard_quad, 0x0, sizeof(datard_quad));

	/*write data*/
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_std[i]);
	}

	qspi_flash_1_1_2_rd(reg_base, datard_dual, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_DUAL_OUT_FAST_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr dual data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_dual[i]);
	}

	//enable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);

	hprintf(DEBUG, "std cmd std addr quad data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	//disable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "disable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	return result;
}

int qspi_4byte_addr_test_GD25LQ256D(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u8 status_h;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	//write
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		data_dfs8[i] = i;
	}
	hprintf(DEBUG, "write data :\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8[i]);
	}
	hprintf(DEBUG, "\r\n");
	data[0] = 0x03020100;
	data[1] = 0x07060504;
	data[2] = 0x0b0a0908;
	data[3] = 0x0f0e0d0c;
	data[4] = 0x13121110;
	data[5] = 0x17161514;
	data[6] = 0x1b1a1918;
	data[7] = 0x1f1e1d1c;
	memset(data_dfs8_rd, 0x0, sizeof(data_dfs8_rd));
	memset(datard_dual, 0x0, sizeof(datard_dual));
	memset(datard_quad, 0x0, sizeof(datard_quad));

	//enable 4byte
	qspi_flash_send_cmd(reg_base, FLASH_CMD_4BYTE_EN);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG,
		"enable 4byte address. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	//erase
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	hprintf(DEBUG, "erase 64kb\r\n");

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr_dfs_8(reg_base, data_dfs8, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	while (qspi_is_busy(reg_base))
		;
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_1_1_1_rd_dfs_8(reg_base, data_dfs8_rd, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8_rd[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		HCOMP(data_dfs8[i], data_dfs8_rd[i]);
	}

	qspi_flash_1_1_2_rd(reg_base, datard_dual, QSPI_DFS_8_TEST_LEN / 4,
			    FLASH_CMD_MEMORY_DUAL_OUT_FAST_RD,
			    FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr dual data read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		HCOMP(data[i], datard_dual[i]);
	}

	//enable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	qspi_flash_1_1_4_rd(reg_base, datard_quad, QSPI_DFS_8_TEST_LEN / 4,
			    FLASH_CMD_MEMORY_QUAD_OUT_FAST_RD,
			    FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr quad data read:\r\n");
	//disable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	//disable 4byte
	qspi_flash_send_cmd(reg_base, FLASH_CMD_4BYTE_DI);
	hprintf(DEBUG, "disable 4byte ===> status high 8bit: 0x%02x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH));
	return result;
}

int qspi_3byte_addr_fast_test_GD25LQ256D(zebu_test_mod_t *mod,
						void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u8 status_h;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		//        data[i] = (i << 24) | (i << 16) | (i << 8) | i;
		data[i] = 0x12345678;
	}
	hprintf(DEBUG, "write data :\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", data[i]);
	}
	hprintf(DEBUG, "\r\n");
	data_dfs8[0] = 0x78;
	data_dfs8[1] = 0x56;
	data_dfs8[2] = 0x34;
	data_dfs8[3] = 0x12;
	memset(data_dfs8_rd, 0x0, sizeof(data_dfs8_rd));
	memset(datard_dual, 0x0, sizeof(datard_dual));
	memset(datard_quad, 0x0, sizeof(datard_quad));

	/*write data*/
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	//enable quad
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_4_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_QUAD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	hprintf(DEBUG, "write data complite\r\n");

	//disable quad
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "disable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	qspi_flash_1_1_1_rd_dfs_8(reg_base, data_dfs8_rd, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_MEMORY_FAST_RD, FLASH_BASE_ADDR,
				  QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8_rd[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		HCOMP(data_dfs8[i % 4], data_dfs8_rd[i]);
	}

	qspi_flash_1_2_2_rd_GD25LQ256D(reg_base, datard_dual, QSPI_RW_TEST_LEN,
				       FLASH_CMD_MEMORY_DUAL_IO_FAST_RD,
				       FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr dual data fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_dual[i]);
	}

	//enable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	qspi_flash_1_4_4_rd_GD25LQ256D(reg_base, datard_quad, QSPI_RW_TEST_LEN,
				       FLASH_CMD_MEMORY_QUAD_IO_FAST_RD,
				       FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	memset(datard_quad, 0x0, sizeof(datard_quad));
	qspi_flash_1_4_4_rd_GD25LQ256D(reg_base, datard_quad, QSPI_RW_TEST_LEN,
				       FLASH_CMD_MEMORY_QUAD_IO_WORD_FAST_RD,
				       FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data word fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	//disable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "disable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	return result;
}

int qspi_4byte_addr_fast_test_GD25LQ256D(zebu_test_mod_t *mod,
						void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u8 status_h;
	u32 i;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	//write
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		data_dfs8[i] = i;
	}
	hprintf(DEBUG, "write data :\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8[i]);
	}
	hprintf(DEBUG, "\r\n");
	data[0] = 0x03020100;
	data[1] = 0x07060504;
	data[2] = 0x0b0a0908;
	data[3] = 0x0f0e0d0c;
	data[4] = 0x13121110;
	data[5] = 0x17161514;
	data[6] = 0x1b1a1918;
	data[7] = 0x1f1e1d1c;
	memset(data_dfs8_rd, 0x0, sizeof(data_dfs8_rd));
	memset(datard_dual, 0x0, sizeof(datard_dual));
	memset(datard_quad, 0x0, sizeof(datard_quad));

	//enable 4byte
	qspi_flash_send_cmd(reg_base, FLASH_CMD_4BYTE_EN);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG,
		"enable 4byte address. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	//erase
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	hprintf(DEBUG, "erase 64kb\r\n");

	//enable quad
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_4_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_QUAD_PAGE_PROG, FLASH_BASE_ADDR_4byte,
			    QSPI_ADDR_LEN_32);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	hprintf(DEBUG, "write data complite\r\n");

	//disable quad
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "disable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	qspi_flash_1_1_1_rd_dfs_8(reg_base, data_dfs8_rd, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_MEMORY_FAST_RD,
				  FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr std data fast read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8_rd[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		HCOMP(data_dfs8[i], data_dfs8_rd[i]);
	}

	qspi_flash_1_2_2_rd_GD25LQ256D(reg_base, datard_dual,
				       QSPI_DFS_8_TEST_LEN / 4,
				       FLASH_CMD_MEMORY_DUAL_IO_FAST_RD,
				       FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr dual data fast read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		hprintf(DEBUG, "0x%08x ", datard_dual[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		HCOMP(data[i], datard_dual[i]);
	}

	//enable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	qspi_flash_1_4_4_rd_GD25LQ256D(reg_base, datard_quad,
				       QSPI_DFS_8_TEST_LEN / 4,
				       FLASH_CMD_MEMORY_QUAD_IO_FAST_RD,
				       FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr quad data fast read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		HCOMP(data[i], datard_quad[i]);
	}
	memset(datard_quad, 0x0, sizeof(datard_quad));
	qspi_flash_1_4_4_rd_GD25LQ256D(reg_base, datard_quad,
				       QSPI_DFS_8_TEST_LEN / 4,
				       FLASH_CMD_MEMORY_QUAD_IO_WORD_FAST_RD,
				       FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr quad data word fast read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN / 4; i++) {
		HCOMP(data[i], datard_quad[i]);
	}

	//disable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "disable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	//disable 4byte
	qspi_flash_send_cmd(reg_base, FLASH_CMD_4BYTE_DI);
	hprintf(DEBUG, "disable 4byte ===> status high 8bit: 0x%02x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH));
	return result;
}

int qspi_read_id_test_GD25LQ256D(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i;
	u8 flash_id_u8[FLASH_ID_LEN] = { 0 };
	u8 flash_unique_id[FLASH_UNIQUE_ID_LEN] = { 0 };

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	hprintf(DEBUG,
		"*************************Read Identification*************************\r\n");
	qspi_flash_read_id_GD25LQ256D(reg_base, flash_id_u8); //0x9F
    hprintf(DEBUG, "Manufacturer ID : 0x%02x\r\n", flash_id_u8[0]);
    hprintf(DEBUG, "Memory Type : 0x%02x\r\n", flash_id_u8[1]);
    hprintf(DEBUG, "Capacity : 0x%02x\r\n", flash_id_u8[2]);

	hprintf(DEBUG,
		"*************Read Manufacture ID/ Device ID, address=0x0*************\r\n");
	qspi_flash_1_1_1_rd_dfs_8(reg_base, flash_id_u8, 2, FLASH_CMD_RD_ID_MD, 0x0,
			    QSPI_ADDR_LEN_24);
    hprintf(DEBUG, "Manufacturer ID : 0x%02x\r\n", flash_id_u8[0]);
    hprintf(DEBUG, "Device ID : 0x%02x\r\n", flash_id_u8[1]);

	hprintf(DEBUG,
		"*************Read Manufacture ID/ Device ID, address=0x1*************\r\n");
	qspi_flash_1_1_1_rd_dfs_8(reg_base, flash_id_u8, 2, FLASH_CMD_RD_ID_MD, 0x1,
			    QSPI_ADDR_LEN_24);
    hprintf(DEBUG, "Device ID : 0x%02x\r\n", flash_id_u8[0]);
    hprintf(DEBUG, "Manufacturer ID : 0x%02x\r\n", flash_id_u8[1]);

	hprintf(DEBUG,
		"****************************Read Unique ID****************************\r\n");
	qspi_flash_1_1_1_rd_dfs_8(reg_base, flash_unique_id,
				  FLASH_UNIQUE_ID_LEN, FLASH_CMD_RD_ID_UNIQUE,
				  0x0, QSPI_ADDR_LEN_24);
	for (i = 0; i < FLASH_UNIQUE_ID_LEN; i++) {
		hprintf(DEBUG, "%02x ", flash_unique_id[i]);
	}
	hprintf(DEBUG, "\r\n");

	return result;
}

int qspi_3byte_erase_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i = 0;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		//        data[i] = (i << 24) | (i << 16) | (i << 8) | i;
		data[i] = 0x12345678;
	}

	hprintf(DEBUG,
		"**************************GD25LQ256D 4K sector erase**************************\r\n");
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SUBSECTOR_ER_4,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(0xffffffff, datard_std[i]);
	}

	hprintf(DEBUG,
		"**************************GD25LQ256D 32Kb block erase**************************\r\n");
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_std[i]);
	}
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SUBSECTOR_ER_32,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(0xffffffff, datard_std[i]);
	}

	hprintf(DEBUG,
		"**************************GD25LQ256D 64Kb block erase**************************\r\n");
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_std[i]);
	}
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(0xffffffff, datard_std[i]);
	}

	hprintf(DEBUG,
		"**************************GD25LQ256D chip erase**************************\r\n");
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_std[i]);
	}
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd(reg_base, FLASH_CMD_CHIP_ER);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd(reg_base, datard_std, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_std[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(0xffffffff, datard_std[i]);
	}

	return result;
}

int qspi_4byte_erase_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i = 0;
	u8 status_h;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		data_dfs8[i] = i;
	}

	//enable 4byte
	qspi_flash_send_cmd(reg_base, FLASH_CMD_4BYTE_EN);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG,
		"enable 4byte address. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	hprintf(DEBUG,
		"**************************GD25LQ256D 4K sector erase(4byte address)**************************\r\n");
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SUBSECTOR_ER_4,
				      FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd_dfs_8(reg_base, data_dfs8_rd, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8_rd[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		HCOMP(0xff, data_dfs8_rd[i]);
	}

	hprintf(DEBUG,
		"**************************GD25LQ256D 32Kb block erase(4byte address)**************************\r\n");
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr_dfs_8(reg_base, data_dfs8, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd_dfs_8(reg_base, data_dfs8_rd, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8_rd[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		HCOMP(data_dfs8[i], data_dfs8_rd[i]);
	}
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SUBSECTOR_ER_32,
				      FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd_dfs_8(reg_base, data_dfs8_rd, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8_rd[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		HCOMP(0xff, data_dfs8_rd[i]);
	}

	hprintf(DEBUG,
		"**************************GD25LQ256D 64Kb block erase(4byte address)**************************\r\n");
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr_dfs_8(reg_base, data_dfs8, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd_dfs_8(reg_base, data_dfs8_rd, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8_rd[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		HCOMP(data_dfs8[i], data_dfs8_rd[i]);
	}
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	qspi_flash_1_1_1_rd_dfs_8(reg_base, data_dfs8_rd, QSPI_DFS_8_TEST_LEN,
				  FLASH_CMD_MEMORY_RD, FLASH_BASE_ADDR_4byte,
				  QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr std data read:\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%02x ", data_dfs8_rd[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_DFS_8_TEST_LEN; i++) {
		HCOMP(0xff, data_dfs8_rd[i]);
	}

	//disable 4byte
	qspi_flash_send_cmd(reg_base, FLASH_CMD_4BYTE_DI);
	hprintf(DEBUG, "disable 4byte ===> status high 8bit: 0x%02x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH));
	return result;
}

int qspi_suspend_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u8 status_l_1 = 0, status_l_2 = 0, status_h_1 = 0, status_h_2 = 0;
	u32 i = 0;

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	status_l_1 = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "write enable. ===>  status low 8bit: 0x%02x\r\n",
		status_l_1);

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_DI);
	status_l_2 = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "write disable. ===>  status low 8bit: 0x%02x\r\n",
		status_l_2);
	if ((((status_l_1 >> 1) & 0x1) == 1) &&
	    (((status_l_2 >> 1) & 0x1) == 0)) {
		hprintf(DEBUG, "write disable test success\r\n");
	} else {
		hprintf(DEBUG, "write disable test failed\r\n");
		result = HVTE_MODULE_TEST_FAIL;
	}

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER,
				      FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);

	//suspend
	qspi_flash_send_cmd(reg_base, FLASH_CMD_PES);
	status_l_1 = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG,
		"Program/Erase Suspend. ===>  status low 8bit: 0x%02x\r\n",
		status_l_1);
	status_h_1 =
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG,
		"Program/Erase Suspend. ===>  status high 8bit: 0x%02x\r\n",
		status_h_1);

	qspi_flash_1_1_1_rd(reg_base, datard, QSPI_RW_TEST_LEN,
			    FLASH_CMD_MEMORY_RD, 0x0, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr std data fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard[i]);
	}
	hprintf(DEBUG, "\r\n");

	//resume
	qspi_flash_send_cmd(reg_base, FLASH_CMD_PER);
	status_l_1 = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG,
		"Program/Erase Resume. ===>  status low 8bit: 0x%02x\r\n",
		status_l_1);
	status_h_2 =
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG,
		"Program/Erase Resume. ===>  status high 8bit: 0x%02x\r\n",
		status_h_2);

	if ((((status_h_1 >> 7) & 0x1) == 1) &&
	    (((status_h_2 >> 7) & 0x1) == 0)) {
		hprintf(DEBUG, "erase suspend test success\r\n");
	} else {
		hprintf(DEBUG, "erase suspend test failed\r\n");
		result = HVTE_MODULE_TEST_FAIL;
	}

	return result;
}

int qspi_3byte_qpi_mode_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u8 status_h = 0, status_l = 0;
	u32 i = 0;

	//enable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	//enable QPI mode
	qspi_flash_send_cmd(reg_base, FLASH_CMD_QPI_EN);
	hprintf(DEBUG, "flash status : 0x%x\r\n",
		qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_QPI));
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_QPI);
	hprintf(DEBUG, "read status register test (0x15): 0x%x\r\n", status_l);

	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_WR_EN);
	status_h = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "write enable ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "write enable ===>  status low 8bit: 0x%02x\r\n",
		status_l);
	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_QPI);
	hprintf(DEBUG, "read status register test (0x15): 0x%x\r\n", status_l);

	qspi_cmd_4_4_0_send_qpi(reg_base, FLASH_CMD_SUBSECTOR_ER_32,
				FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	//    qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_CHIP_ER);
	status_h = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "write done ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "write done ===>  status low 8bit: 0x%02x\r\n",
		status_l);
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	hprintf(DEBUG, "11111111111111111111111111111111\r\n");

	memset(datard_quad, 0x0, sizeof(datard_quad));
	qspi_flash_4_4_4_rd_GD25LQ256D(reg_base, datard_quad, QSPI_RW_TEST_LEN,
				       FLASH_CMD_MEMORY_QUAD_IO_FAST_RD,
				       FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data word fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(0xffffffff, datard_quad[i]);
	}
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	for (i = 0; i < QSPI_RW_TEST_LEN; i++)
		data[i] = 0xae12eb34;
	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_WR_EN);
	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "write enable ===>  status low 8bit: 0x%02x\r\n",
		status_l);
	qspi_flash_4_4_4_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR,
			    QSPI_ADDR_LEN_24);
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	memset(datard_quad, 0x0, sizeof(datard_quad));
	qspi_flash_4_4_4_rd_GD25LQ256D(reg_base, datard_quad, QSPI_RW_TEST_LEN,
				       FLASH_CMD_MEMORY_QUAD_IO_FAST_RD,
				       FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	//disable QPI mode
	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_QPI_DI);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	//disable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "disable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	return result;
}

int qspi_4byte_qpi_mode_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u8 status_h = 0, status_l = 0;
	u32 i = 0;

	//enable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	//enable 4byte
	qspi_flash_send_cmd(reg_base, FLASH_CMD_4BYTE_EN);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG,
		"enable 4byte address. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	//enable QPI mode
	qspi_flash_send_cmd(reg_base, FLASH_CMD_QPI_EN);
	hprintf(DEBUG, "flash status : 0x%x\r\n",
		qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_QPI));
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_QPI);
	hprintf(DEBUG, "read status register test (0x15): 0x%x\r\n", status_l);

	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_WR_EN);
	status_h = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "write enable ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "write enable ===>  status low 8bit: 0x%02x\r\n",
		status_l);
	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_QPI);
	hprintf(DEBUG, "read status register test (0x15): 0x%x\r\n", status_l);

	qspi_cmd_4_4_0_send_qpi(reg_base, FLASH_CMD_SUBSECTOR_ER_32,
				FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	//    qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_CHIP_ER);
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	status_h = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "write done ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "write done ===>  status low 8bit: 0x%02x\r\n",
		status_l);

	memset(datard_quad, 0x0, sizeof(datard_quad));
	qspi_flash_4_4_4_rd_GD25LQ256D(reg_base, datard_quad, QSPI_RW_TEST_LEN,
				       FLASH_CMD_MEMORY_QUAD_IO_FAST_RD,
				       FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr quad data word fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(0xffffffff, datard_quad[i]);
	}
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	for (i = 0; i < QSPI_RW_TEST_LEN; i++)
		data[i] = 0x12345678;
	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_WR_EN);
	status_l = qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD);
	hprintf(DEBUG, "write enable ===>  status low 8bit: 0x%02x\r\n",
		status_l);
	qspi_flash_4_4_4_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR_4byte,
			    QSPI_ADDR_LEN_32);
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	memset(datard_quad, 0x0, sizeof(datard_quad));
	qspi_flash_4_4_4_rd_GD25LQ256D(reg_base, datard_quad, QSPI_RW_TEST_LEN,
				       FLASH_CMD_MEMORY_QUAD_IO_FAST_RD,
				       FLASH_BASE_ADDR_4byte, QSPI_ADDR_LEN_32);
	hprintf(DEBUG, "std cmd std addr quad data fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}
	while (qspi_reg_4_0_4_rd(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	//disable QPI mode
	qspi_cmd_4_0_0_send(reg_base, FLASH_CMD_QPI_DI);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;
	//disable 4byte
	qspi_flash_send_cmd(reg_base, FLASH_CMD_4BYTE_DI);
	hprintf(DEBUG, "disable 4byte ===> status high 8bit: 0x%02x\r\n",
		qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH));
	//disable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "disable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);
	return result;
}

#if 0
int qspi_dual_continuous_test(zebu_test_mod_t* mod, void* para)
{
    u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
    u8 status_h = 0; 
    u32 flash_addr = 0x0;
    u32 i = 0, j = 0, val = 0, cnt =0;
    
	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

    for(i = 0; i < QSPI_RW_TEST_LEN; i++)
    {
        data[i] = 0x12345678;
    }
//    hprintf(DEBUG, "write data :\r\n");
//    for(i = 0; i < QSPI_RW_TEST_LEN; i++) {
//        hprintf(DEBUG, "0x%08x ", data[i]);
//    }
//    hprintf(DEBUG, "\r\n");

    /*write data*/  
    qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
    qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
    while(qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1);

    qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
    qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN, FLASH_CMD_PAGE_PROG, FLASH_BASE_ADDR, QSPI_ADDR_LEN_24);
    while(qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1);


    memset(data_dfs8, 0x0, sizeof(data_dfs8));
	while(qspi_is_busy(reg_base));
	qspi_disable(reg_base);
	qspi_set_wait_clock(reg_base, 0);
	qspi_set_addr_len(reg_base, QSPI_ADDR_LEN_24);
	qspi_set_cmd_len(reg_base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(reg_base, CMD_STD_ADDR_FRF);
	qspi_set_spi_dfs(reg_base, 8);
	qspi_set_spi_frf(reg_base, QSPI_DUAL_SPI);
	qspi_set_mode(reg_base, QSPI_RX_ONLY);
	qspi_set_read_size(reg_base, QSPI_DFS_8_TEST_LEN);
	qspi_set_txftl(reg_base, 3);
//    qspi_set_sckdv(reg_base, 2000);
	qspi_enable(reg_base);
	
    flash_addr = FLASH_BASE_ADDR;
	REG32_WRITE(reg_base + QSPI_REG_DR0, FLASH_CMD_MEMORY_DUAL_IO_FAST_RD);
    for(j = 0; j < 3; j++){
//        if(j >= 1)            
//            qspi_enable(reg_base);
        flash_addr = flash_addr + j * QSPI_DFS_8_TEST_LEN;

        REG32_WRITE(reg_base + QSPI_REG_DR0, (flash_addr>>16)&0xFF);
    	REG32_WRITE(reg_base + QSPI_REG_DR0, (flash_addr>>8)&0xFF);
    	REG32_WRITE(reg_base + QSPI_REG_DR0, (flash_addr>>0)&0xFF);
//        REG32_WRITE(reg_base + QSPI_REG_DR0, 0x0);
        REG32_WRITE(reg_base + QSPI_REG_DR0, 0x20);
        i = 0;
        while(1){
            val = REG32_READ(reg_base + QSPI_REG_SR);
//            hprintf(DEBUG, "QSPI_REG_SR : 0x%02x\r\n", val);
            if (val & (1<<3)){
                //rx fifo not empty            
//                putc('i');
//                hprintf(DEBUG, "i : %d QSPI_REG_RXFLR : 0x%02x\r\n", i, REG32_READ(reg_base + QSPI_REG_RXFLR));
                data_dfs8[i++] = REG32_READ(reg_base + QSPI_REG_DR0);            
                if (i>=QSPI_DFS_8_TEST_LEN){
                    break;
                }            
            }else if(val & (1<<4)){
                putc('f');            
            }else{
                //fifo empty            
                putc('e');
                cnt += 0x100;
            }        
            
            if (cnt++ > 0xffffff){
                hprintf(DEBUG, "read timer out\r\n");  
                hprintf(DEBUG, "QSPI_REG_SR : 0x%08x", REG32_READ(reg_base + QSPI_REG_SR));
                return;
            }
        }
        for(i = 0 ; i < QSPI_DFS_8_TEST_LEN ; i++) {
    		hprintf(DEBUG, "0x%02x  ", data_dfs8[i]);
    	}
        hprintf(DEBUG, "\r\n");
        hprintf(DEBUG, "QSPI_REG_SR : 0x%02x\r\n", val);
    }
    while(qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1);    

//    qspi_flash_reset(reg_base);
    return result;
}
#endif

int qspi_burst_with_wrap_test(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	u8 status_h = 0;
	u32 i = 0;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	qspi_exit_xip(reg_base); //change to normal mode
	qspi_de_endian(reg_base);

	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		data[i] = 0x12345678;
	}
	hprintf(DEBUG, "write data :\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", data[i]);
	}
	hprintf(DEBUG, "\r\n");

	/*write data*/
	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_send_cmd_with_addr(reg_base, FLASH_CMD_SECTOR_ER, 0x0,
				      QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	qspi_flash_send_cmd(reg_base, FLASH_CMD_WR_EN);
	qspi_flash_1_1_1_wr(reg_base, data, QSPI_RW_TEST_LEN,
			    FLASH_CMD_PAGE_PROG, 0x0, QSPI_ADDR_LEN_24);
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	memset(datard_quad, 0x0, sizeof(datard_quad));
	//enable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_EN);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "enable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	//enable burst with wrap
	while (qspi_is_busy(reg_base))
		;
	qspi_disable(reg_base);
	qspi_set_spi_dfs(reg_base, 32);
	qspi_set_txftl(reg_base,
		       1); //transfer will start when greater than txftl
	qspi_set_spi_frf(reg_base, QSPI_QUAD_SPI); //quad mode
	qspi_set_mode(reg_base, QSPI_TX_ONLY);
	qspi_set_spi_sste(reg_base, 0);
	qspi_set_wait_clock(reg_base, 0);
	qspi_set_addr_len(reg_base, QSPI_ADDR_LEN_NO);
	qspi_set_cmd_len(reg_base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(reg_base, CMD_STD_ADDR_FRF);
	qspi_enable(reg_base);
	REG32_WRITE(reg_base + QSPI_REG_DR0, FLASH_CMD_BURST);
	REG32_WRITE(reg_base + QSPI_REG_DR0, 0x00000060);
	while (!qspi_tf_is_empty(reg_base));
	while (qspi_is_busy(reg_base));
	hprintf(DEBUG, "enable burst with wrap (64 byte)\r\n");

	memset(datard_quad, 0x0, sizeof(datard_quad));
	qspi_flash_1_4_4_rd_GD25LQ256D(reg_base, datard_quad, QSPI_RW_TEST_LEN,
				       FLASH_CMD_MEMORY_QUAD_IO_FAST_RD, 0x10,
				       QSPI_ADDR_LEN_24);
	hprintf(DEBUG, "std cmd std addr quad data fast read:\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		hprintf(DEBUG, "0x%08x ", datard_quad[i]);
	}
	hprintf(DEBUG, "\r\n");
	for (i = 0; i < QSPI_RW_TEST_LEN; i++) {
		HCOMP(data[i], datard_quad[i]);
	}
	while (qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD) & 0x1)
		;

	//disable burst with wrap
	while (qspi_is_busy(reg_base))
		;
	qspi_disable(reg_base);
	qspi_set_spi_dfs(reg_base, 32);
	qspi_set_txftl(reg_base,
		       1); //transfer will start when greater than txftl
	qspi_set_spi_frf(reg_base, QSPI_QUAD_SPI); //quad mode
	qspi_set_mode(reg_base, QSPI_TX_ONLY);
	qspi_set_spi_sste(reg_base, 0);
	qspi_set_wait_clock(reg_base, 0);
	qspi_set_addr_len(reg_base, QSPI_ADDR_LEN_NO);
	qspi_set_cmd_len(reg_base, QSPI_CMD_LEN_8);
	qspi_set_cmd_addr_line_mode(reg_base, CMD_STD_ADDR_FRF);
	qspi_enable(reg_base);
	REG32_WRITE(reg_base + QSPI_REG_DR0, FLASH_CMD_BURST);
	REG32_WRITE(reg_base + QSPI_REG_DR0, 0x00000010);
	while (!qspi_tf_is_empty(reg_base))
		;
	while (qspi_is_busy(reg_base))
		;
	hprintf(DEBUG, "disable burst with wrap (64 byte)\r\n");

	//disable QE
	qspi_flash_set_quad(reg_base, FLASH_QUAD_DI);
	status_h = qspi_flash_read_reg(reg_base, FLASH_CMD_REG_STATUS_RD_HIGH);
	hprintf(DEBUG, "disable quad. ===>  status high 8bit: 0x%02x\r\n",
		status_h);

	return result;
}

int qspi_reset_flash(zebu_test_mod_t *mod, void *para)
{
	u32 reg_base = mod->reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	
	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);

	qspi_exit_xip(reg_base); //change to normal mode
	qspi_flash_reset(reg_base);
	return result;
}

static int qspi_debug(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;

	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);
	dbg_print = dbg_print ? 0 : 1;
	hprintf(DEBUG, "dbg_print = %d\r\n", dbg_print);
	return result;
}

/*==================================QSPI ZEBU TEST=====================================================*/
static zebu_test_mod_t zebu_test_qspi_mods[] = {
	{ "QSPI0", QSPI0_BASE_ADDR },
#if defined(CONFIG_ZEBU) /* test qspi1, only in zebu */
	{ "QSPI1", QSPI1_BASE_ADDR },
#endif 
};

// do not use char: c, a, r, d , s, q, ? ,  because these charactor has been used in test common
static zebu_test_entry_t zebu_test_qspi_entrys[] = {
#if defined(CONFIG_ZEBU)
	{ '0', 1, "qspi standard read and write test(1-1-1)",
	  qspi_std_rw_test },
	{ '1', 1, "qspi dual read test(1-1-2)", qspi_dual_rw_test },
	{ '2', 1, "qspi quad read test(1-1-4)", qspi_quad_rw_test },
	{ '3', 1, "qspi cmd addr data dual read test(2-2-2)",
	  qspi_cmd_addr_dual_test },
	{ '4', 1, "qspi cmd addr data quad read test(4-4-4)",
	  qspi_cmd_addr_quad_test },
	{ '5', 1, "qspi clock mode test", qspi_clock_mode_test },
	{ '6', 1, "qspi data frame size test", qspi_date_frame_size_test },
	{ '7', 1, "qspi baudr test", qspi_baudr_test },
	{ '8', 1, "qspi max baudr continue test",
	  qspi_max_baudr_continue_test },
	{ '9', 1, "qspi eeprom read mode test", qspi_eeprom_read_test },
	{ 'b', 1, "qspi eeprom read exceed address range test",
	  qspi_eeprom_read_exceed_addr_range_test },
	{ 'e', 1, "qspi cs test", qspi_cs_test },
	{ 'f', 1, "qspi interrupt test", qspi_intr_test },
	{ 'g', 1, "qspi data enhance read test(1-1-X)",
	  qspi_data_enhance_rd_test },
	{ 'h', 1, "qspi address and data enhance read test(1-X-X)",
	  qspi_addr_data_enhance_rd_test },
	{ 'i', 1, "qspi ddr quad read test", qspi_ddr_quad_rd_test },
#if (defined(CONFIG_R5_SAFETY))
	{ 'j', 1, "qspi dma tx test", qspi_dma_tx_test },
	{ 'k', 1, "qspi dma rx test", qspi_dma_rx_test },
#endif
#endif /*run in zebu*/

#if (defined(CONFIG_HAPS) || defined(CONFIG_EVB))
	{ 'l', 1, "qspi 3byte addr test(1-1-X)",
	  qspi_3byte_addr_test_GD25LQ256D },
	{ 'm', 1, "qspi 4byte addr test(1-1-X)",
	  qspi_4byte_addr_test_GD25LQ256D },
	{ 'n', 1, "qspi 3byte addr fast I/O test(1-X-X)",
	  qspi_3byte_addr_fast_test_GD25LQ256D },
	{ 'o', 1, "qspi 4byte addr fast I/O test(1-X-X)",
	  qspi_4byte_addr_fast_test_GD25LQ256D },
	{ 'p', 1, "qspi 3byte qpi mode test", qspi_3byte_qpi_mode_test },
	{ 't', 1, "qspi 4byte qpi mode test", qspi_4byte_qpi_mode_test },
	{ 'u', 1, "qspi 3byte erase test", qspi_3byte_erase_test },
	{ 'v', 1, "qspi 4byte erase test", qspi_4byte_erase_test },
	{ 'w', 1, "qspi read id test", qspi_read_id_test_GD25LQ256D },
	{ 'x', 1, "qspi write disable and erase suspend test",
	  qspi_suspend_test },
	//    {'A', 1, "qspi dual I/O fast read with “Continuous Read Mode”", qspi_dual_continuous_test},
	{ 'B', 1, "qspi burst with wrap test", qspi_burst_with_wrap_test },
#endif /*run in haps or evb*/	
	{ 'y', 1, "qspi reset flash", qspi_reset_flash },
	{ 'z', 1, "qspi debug", qspi_debug },
};

static zebu_test_ctrl_t zebu_mod_test_ctrl = { "QSPI",
					       SIZE_ARRAY(zebu_test_qspi_mods),
					       zebu_test_qspi_mods,
					       SIZE_ARRAY(
						       zebu_test_qspi_entrys),
					       zebu_test_qspi_entrys,
					       0 };

void qspiMain(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "auto test err %d!!!!\r\n", ret);
	} else {
		hprintf(TSENV, "enter QSPI test!\r\n");
		TestLoop(&zebu_mod_test_ctrl);
	}
}

#if (defined(CONFIG_R5_SAFETY) || defined(CONFIG_A55))

void qspi1_irq_handler(int parm)
{
	printf_intr("\r\n");
	printf_intr("qspi1 interrupt triggered %08x\r\n",
		    REG32_READ(QSPI1_BASE_ADDR + QSPI_REG_ISR));
	qspi_interrupt_disable(QSPI1_BASE_ADDR);
	int_flag = 1;
}

void qspi0_irq_handler(int parm)
{
	printf_intr("\r\n");
	printf_intr("qspi0 interrupt triggered %08x\r\n",
		    REG32_READ(QSPI0_BASE_ADDR + QSPI_REG_ISR));
	qspi_interrupt_disable(QSPI0_BASE_ADDR);
	int_flag = 1;
}
#endif

IRQ_DEFINE(FIQ_IRQ_LSP1_QSPI1, qspi1_irq_handler, 0, "irq qspi1",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);
IRQ_DEFINE(FIQ_IRQ_LSP0_QSPI0, qspi0_irq_handler, 0, "irq qspi0",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL);

CMD_DEFINE(qspi, qspiMain, 0, "qspi test", CMD_ATTR_AUTO_TEST);
