#ifndef __I2C_CORE_H_
#define __I2C_CORE_H_

#include <linux/types.h>

struct i2c_adapter {
	void		(*init)(struct i2c_adapter *adap, int speed,
				int slaveaddr);
	int		(*probe)(struct i2c_adapter *adap, uint8_t chip);
	int		(*read)(struct i2c_adapter *adap, uint8_t chip,
				uint addr, int alen, uint8_t *buffer,
				int len);
	int		(*write)(struct i2c_adapter *adap, uint8_t chip,
				uint addr, int alen, uint8_t *buffer,
				int len);
	uint		(*set_bus_speed)(struct i2c_adapter *adap,
				uint speed);
	int		speed;
	int		waitdelay;
	int		slaveaddr;
	int		init_done;
	int		hwadapnr;
	int     hwadapmax;
	char		*name;
};

extern struct i2c_adapter adap;
extern void i2c_init(void);

static inline int i2c_get_cur_bus(void)
{
	return adap.hwadapnr;
}

static inline int i2c_set_cur_bus(int bus_num)
{
	if (bus_num < adap.hwadapmax) {
		adap.hwadapnr = bus_num;
		return 0;
	}
	return -EINVAL;
}

static inline int i2c_read(u8 dev, u32 addr, int alen, u8 *buffer, int len)
{
	return adap.read(&adap, dev, addr, alen, buffer, len);
}

static inline int i2c_write(u8 dev, u32 addr, int alen, u8 *buffer, int len)
{
	return adap.write(&adap, dev, addr, alen, buffer, len);
}

static inline char *i2c_get_adap_name(void)
{
	return adap.name;
}

static inline int i2c_get_adap_speed(void)
{
	return adap.speed;
}

static inline int i2c_probe(u8 dev)
{
	return adap.probe(&adap, dev);
}

static inline int i2c_set_bus_speed(u32 speed)
{
	return adap.set_bus_speed(&adap, speed);
}

#endif

