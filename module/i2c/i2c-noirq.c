// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2009
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
 */

#include "i2c-noirq.h"

/*
struct dw_scl_sda_cfg sda_cfg = {
	.ss_hcnt = 0x3e8,
	.fs_hcnt = 0xc8,
	.ss_lcnt = 0x3e8,
	.fs_lcnt = 0xc8,
	.sda_hold = 0x50,
};
*/

static int dw_i2c_enable(struct i2c_regs *i2c_base, bool enable)
{
	u32 ena = enable ? IC_ENABLE_0B : 0;
	int timeout = 100;

	do {
		writel(ena, &i2c_base->ic_enable);
		if ((readl(&i2c_base->ic_enable_status) & IC_ENABLE_0B) == ena)
			return 0;

		/*
		 * Wait 10 times the signaling period of the highest I2C
		 * transfer supported by the driver (for 400KHz this is
		 * 25us) as described in the DesignWare I2C databook.
		 */
		udelay(25);
	} while (timeout--);
	printf("timeout in %sabling I2C adapter\n", enable ? "en" : "dis");

	return -ETIMEDOUT;
}

/*
 * i2c_set_bus_speed - Set the i2c speed
 * @speed:	required i2c speed
 *
 * Set the i2c speed.
 */
static unsigned int __dw_i2c_set_bus_speed(struct i2c_regs *i2c_base,
					   struct dw_scl_sda_cfg *scl_sda_cfg,
					   unsigned int speed,
					   unsigned int bus_mhz)
{
	unsigned int cntl;
	unsigned int hcnt, lcnt;
	unsigned int ena;
	int i2c_spd;

	if (speed >= I2C_MAX_SPEED)
		i2c_spd = IC_SPEED_MODE_MAX;
	else if (speed >= I2C_FAST_SPEED)
		i2c_spd = IC_SPEED_MODE_FAST;
	else
		i2c_spd = IC_SPEED_MODE_STANDARD;

	/* Get enable setting for restore later */
	ena = readl(&i2c_base->ic_enable) & IC_ENABLE_0B;

	/* to set speed cltr must be disabled */
	dw_i2c_enable(i2c_base, false);

	cntl = (readl(&i2c_base->ic_con) & (~IC_CON_SPD_MSK));

	switch (i2c_spd) {
	case IC_SPEED_MODE_MAX:
		cntl |= IC_CON_SPD_SS;
		if (scl_sda_cfg) {
			hcnt = scl_sda_cfg->fs_hcnt;
			lcnt = scl_sda_cfg->fs_lcnt;
		} else {
			hcnt = (bus_mhz * MIN_HS_SCL_HIGHTIME) / NANO_TO_MICRO;
			lcnt = (bus_mhz * MIN_HS_SCL_LOWTIME) / NANO_TO_MICRO;
		}
		writel(hcnt, &i2c_base->ic_hs_scl_hcnt);
		writel(lcnt, &i2c_base->ic_hs_scl_lcnt);
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		if (scl_sda_cfg) {
			hcnt = scl_sda_cfg->ss_hcnt;
			lcnt = scl_sda_cfg->ss_lcnt;
		} else {
			hcnt = (bus_mhz * MIN_SS_SCL_HIGHTIME) / NANO_TO_MICRO;
			lcnt = (bus_mhz * MIN_SS_SCL_LOWTIME) / NANO_TO_MICRO;
		}
		writel(hcnt, &i2c_base->ic_ss_scl_hcnt);
		writel(lcnt, &i2c_base->ic_ss_scl_lcnt);
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		if (scl_sda_cfg) {
			hcnt = scl_sda_cfg->fs_hcnt;
			lcnt = scl_sda_cfg->fs_lcnt;
		} else {
			hcnt = (bus_mhz * MIN_FS_SCL_HIGHTIME) / NANO_TO_MICRO;
			lcnt = (bus_mhz * MIN_FS_SCL_LOWTIME) / NANO_TO_MICRO;
		}
		writel(hcnt, &i2c_base->ic_fs_scl_hcnt);
		writel(lcnt, &i2c_base->ic_fs_scl_lcnt);
		break;
	}

	writel(cntl, &i2c_base->ic_con);

	/* Configure SDA Hold Time if required */
	if (scl_sda_cfg)
		writel(scl_sda_cfg->sda_hold, &i2c_base->ic_sda_hold);

	/* Restore back i2c now speed set */
	if (ena == IC_ENABLE_0B)
		dw_i2c_enable(i2c_base, true);

	return 0;
}

/*
 * i2c_setaddress - Sets the target slave address
 * @i2c_addr:	target i2c address
 *
 * Sets the target slave address.
 */
static void i2c_setaddress(struct i2c_regs *i2c_base, unsigned int i2c_addr)
{
	u32 value;
	/* Disable i2c */
	dw_i2c_enable(i2c_base, false);

	value = readl(&i2c_base->ic_con);
	if (i2c_addr & (~0x7f))
		value |= IC_CON_10BITADDRMASTER;
	else
		value &= ~IC_CON_10BITADDRMASTER;
	writel(value, &i2c_base->ic_con);
	
	writel(i2c_addr, &i2c_base->ic_tar);

	/* Enable i2c */
	dw_i2c_enable(i2c_base, true);
}

/*
 * i2c_flush_rxfifo - Flushes the i2c RX FIFO
 *
 * Flushes the i2c RX FIFO
 */
static void i2c_flush_rxfifo(struct i2c_regs *i2c_base)
{
	while (readl(&i2c_base->ic_status) & IC_STATUS_RFNE)
		readl(&i2c_base->ic_cmd_data);
}

/*
 * i2c_wait_for_bb - Waits for bus busy
 *
 * Waits for bus busy
 */
static int i2c_wait_for_bb(struct i2c_regs *i2c_base)
{
	//unsigned long start_time_bb = get_timer(0);
    int time_bb = 1000;

	while ((readl(&i2c_base->ic_status) & IC_STATUS_MA) ||
	       !(readl(&i2c_base->ic_status) & IC_STATUS_TFE)) {

		/* Evaluate timeout */
		if (time_bb-- < 0)
			return 1;
	}

	return 0;
}

static int i2c_xfer_init(struct i2c_regs *i2c_base, uchar chip, uint addr,
			 int alen)
{
	if (i2c_wait_for_bb(i2c_base))
		return 1;

	i2c_setaddress(i2c_base, chip);
	while (alen) {
		alen--;
		/* high byte address going out first */
		writel((addr >> (alen * 8)) & 0xff,
		       &i2c_base->ic_cmd_data);
	}
	return 0;
}

static int i2c_xfer_finish(struct i2c_regs *i2c_base)
{
	//ulong start_stop_det = get_timer(0);
    int start_stop = 1000;

	while (1) {
		if ((readl(&i2c_base->ic_raw_intr_stat) & IC_STOP_DET)) {
			readl(&i2c_base->ic_clr_stop_det);
			break;
		} else if (start_stop < 0) {
			break;
		}
        start_stop--;
	}

	if (i2c_wait_for_bb(i2c_base)) {
		printf("Timed out waiting for bus\n");
		return 1;
	}

	i2c_flush_rxfifo(i2c_base);

	return 0;
}

/*
 * i2c_read - Read from i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
static int __dw_i2c_read(struct i2c_regs *i2c_base, u8 dev, uint addr,
			 int alen, u8 *buffer, int len)
{
	int start_time_rx = 1000;
	unsigned int active = 0;

	if (i2c_xfer_init(i2c_base, dev, addr, alen))
		return 1;

	//start_time_rx = get_timer(0);
	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			if (len == 1)
				writel(IC_CMD | IC_STOP, &i2c_base->ic_cmd_data);
			else
				writel(IC_CMD, &i2c_base->ic_cmd_data);
			active = 1;
		}

		if (readl(&i2c_base->ic_status) & IC_STATUS_RFNE) {
			*buffer++ = (uchar)readl(&i2c_base->ic_cmd_data);
			len--;
			active = 0;
            start_time_rx = 1000;
		} else if (start_time_rx < 0) {
			return 1;
		}
        start_time_rx--;
	}

	return i2c_xfer_finish(i2c_base);
}

/*
 * i2c_write - Write to i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Write to i2c memory.
 */
static int __dw_i2c_write(struct i2c_regs *i2c_base, u8 dev, uint addr,
			  int alen, u8 *buffer, int len)
{
	int start_time_tx = 1000;

	if (i2c_xfer_init(i2c_base, dev, addr, alen))
		return 1;

	//start_time_tx = get_timer(0);
	while (len) {
		if (readl(&i2c_base->ic_status) & IC_STATUS_TFNF) {
			if (--len == 0) {
				writel(*buffer | IC_STOP,
				       &i2c_base->ic_cmd_data);
			} else {
				writel(*buffer, &i2c_base->ic_cmd_data);
			}
			buffer++;
			start_time_tx = 1000;

		} else if (start_time_tx < 0) {
				printf("Timed out. i2c write Failed\n");
				return 1;
		}
	}

	return i2c_xfer_finish(i2c_base);
}

/*
 * __dw_i2c_init - Init function
 * @speed:	required i2c speed
 * @slaveaddr:	slave address for the device
 *
 * Initialization function.
 */
static int __dw_i2c_init(struct i2c_regs *i2c_base, int speed, int slaveaddr)
{
	int ret;

	/* Disable i2c */
	ret = dw_i2c_enable(i2c_base, false);
	if (ret)
		return ret;

	writel(IC_CON_SD | IC_CON_RE | IC_CON_SPD_FS | IC_CON_MM,
	       &i2c_base->ic_con);
	writel(IC_RX_TL, &i2c_base->ic_rx_tl);
	writel(IC_TX_TL, &i2c_base->ic_tx_tl);
	writel(IC_STOP_DET, &i2c_base->ic_intr_mask);

	__dw_i2c_set_bus_speed(i2c_base, NULL, speed, IC_CLK);
	writel(slaveaddr, &i2c_base->ic_sar);

	/* Enable i2c */
	ret = dw_i2c_enable(i2c_base, true);
	if (ret)
		return ret;

	return 0;
}

/*
 * The legacy I2C functions. These need to get removed once
 * all users of this driver are converted to DM.
 */
static struct i2c_regs *i2c_get_base(struct i2c_adapter *adap)
{
	switch (adap->hwadapnr) {
#if CONFIG_I2C_MAX_BUS_NUM >= 6
	case 5:
		return (struct i2c_regs *)I2C5_BASE_ADDR;
#endif
#if CONFIG_I2C_MAX_BUS_NUM >= 5
	case 4:
		return (struct i2c_regs *)I2C4_BASE_ADDR;
#endif
#if CONFIG_I2C_MAX_BUS_NUM >= 4
	case 3:
		return (struct i2c_regs *)I2C3_BASE_ADDR;
#endif
#if CONFIG_I2C_MAX_BUS_NUM >= 3
	case 2:
		return (struct i2c_regs *)I2C2_BASE_ADDR;
#endif
#if CONFIG_I2C_MAX_BUS_NUM >= 2
	case 1:
		return (struct i2c_regs *)I2C1_BASE_ADDR;
#endif
	case 0:
		return (struct i2c_regs *)I2C0_BASE_ADDR;
	default:
		printf("Wrong I2C-adapter number %d\n", adap->hwadapnr);
	}

	return NULL;
}

static unsigned int dw_i2c_set_bus_speed(struct i2c_adapter *adap,
					 unsigned int speed)
{
	adap->speed = speed;
	return __dw_i2c_set_bus_speed(i2c_get_base(adap), NULL, speed, IC_CLK);
}

static void dw_i2c_init(struct i2c_adapter *adap, int speed, int slaveaddr)
{
	__dw_i2c_init(i2c_get_base(adap), speed, slaveaddr);
}

static int dw_i2c_read(struct i2c_adapter *adap, u8 dev, uint addr,
		       int alen, u8 *buffer, int len)
{
	return __dw_i2c_read(i2c_get_base(adap), dev, addr, alen, buffer, len);
}

static int dw_i2c_write(struct i2c_adapter *adap, u8 dev, uint addr,
			int alen, u8 *buffer, int len)
{
	return __dw_i2c_write(i2c_get_base(adap), dev, addr, alen, buffer, len);
}

/* dw_i2c_probe - Probe the i2c chip */
static int dw_i2c_probe(struct i2c_adapter *adap, u8 dev)
{
	struct i2c_regs *i2c_base = i2c_get_base(adap);
	u32 tmp;
	int ret;

	/*
	 * Try to read the first location of the chip.
	 */
	ret = __dw_i2c_read(i2c_base, dev, 0, 1, (uchar *)&tmp, 1);
	if (ret)
		dw_i2c_init(adap, adap->speed, adap->slaveaddr);

	return ret;
}

#define PINMUX_REG_BASE AON_PMM_REG_BASE_ADDR
#define AON_REG_0 0x0
#define REG_0_MASK 0x1fffffff
#define PINMUX_I2C1_SCL (0 << 31)
#define PINMUX_I2C0_SDA (0 << 30)
#define PINMUX_I2C0_SCL (0 << 29)

#define AON_REG_1 0x04
#define REG_1_MASK 0xfffffe00
#define PINMUX_I2C5_SDA (0 << 8)
#define PINMUX_I2C5_SCL (0 << 7)
#define PINMUX_I2C4_SDA (0 << 6)
#define PINMUX_I2C4_SCL (0 << 5)
#define PINMUX_I2C3_SDA (0 << 4)
#define PINMUX_I2C3_SCL (0 << 3)
#define PINMUX_I2C2_SDA (0 << 2)
#define PINMUX_I2C2_SCL (0 << 1)
#define PINMUX_I2C1_SDA (0 << 0)

static void pinmux_reg_init(void)
{
	u32 value;
	value = readl(PINMUX_REG_BASE + AON_REG_0);
	writel(value & (REG_0_MASK | PINMUX_I2C0_SDA | PINMUX_I2C0_SCL |
			PINMUX_I2C1_SCL),
	       PINMUX_REG_BASE + AON_REG_0);
	value = readl(PINMUX_REG_BASE + AON_REG_1);
	writel(value & (REG_1_MASK | PINMUX_I2C1_SDA | PINMUX_I2C2_SCL |
			PINMUX_I2C2_SDA | PINMUX_I2C3_SCL | PINMUX_I2C3_SDA |
			PINMUX_I2C4_SCL | PINMUX_I2C4_SDA | PINMUX_I2C5_SCL |
			PINMUX_I2C5_SDA),
	       PINMUX_REG_BASE + AON_REG_1);
}

struct i2c_adapter adap = {
    .init = dw_i2c_init,
    .probe = dw_i2c_probe,
    .read = dw_i2c_read,
    .write = dw_i2c_write,
    .set_bus_speed = dw_i2c_set_bus_speed,
    .name = "Bst i2c adapter",
    .hwadapnr = 0,
    .hwadapmax = CONFIG_I2C_MAX_BUS_NUM,
	.speed = 100000,
};

void i2c_init(void)
{
    int i;

    pinmux_reg_init();
    for (i = 0; i < adap.hwadapmax; i++) {
        adap.hwadapnr = i;
        adap.init(&adap, 100000, 0x55);
    }
    adap.hwadapnr = 0;
}
