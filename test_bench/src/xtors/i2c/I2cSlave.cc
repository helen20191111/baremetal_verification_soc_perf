#include "global.h"
#include "include/HS_I2c.hh"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "xtor/xtor_i2c.h"

using namespace I2C;

#ifdef CONFIG_I2C_SLAVE_GENERAL
static void i2cConfig(zHS_I2cConfig *i2cCfg)
{
	i2cCfg->mode = Slave;
	i2cCfg->speed = StandardI2c; // StandardI2c   FastI2c   HighSpeedI2c
	i2cCfg->slave_address = I2C_SLAVE_ADDR;
	i2cCfg->hs_master_code = 0;
	i2cCfg->smbus_mode = false;
	i2cCfg->smbus_arp_enable = false;
#ifdef CONFIG_I2C_ADDR_7BIT
	i2cCfg->slave_10b = false;
#else
	i2cCfg->slave_10b = true;
#endif
	i2cCfg->ack_general_call = true;
	i2cCfg->smbus_quick_slave = false;
	i2cCfg->smbus_arp_slv_udid = 0;
	i2cCfg->disable_smbus_reserved_addr_check = false;
}
#endif

#ifdef CONFIG_I2C0_SLAVE_INIT
static HS_I2c *gli2c0Slave = NULL;
static void I2c0SlaveInit(Board *board)
{
	zHS_I2cConfig i2c0Cfg;
	gli2c0Slave = new HS_I2c;
	gli2c0Slave->init(board, I2C0_TRAS_DEV);
	
	i2cConfig(&i2c0Cfg);
    
	gli2c0Slave->enableWatchDog(false);

	while (!gli2c0Slave->config(i2c0Cfg));
	printf("i2c0 slave init bfm\n"); fflush(stdout);
	while (!gli2c0Slave->initBFM());
	gli2c0Slave->connect();
	gli2c0Slave->disableSequencer();
	printf("=============== i2c0 slave init finished\n"); fflush(stdout);
}
#else
static inline void I2c0SlaveInit(Board *board) {}
#endif

#ifdef CONFIG_I2C1_SLAVE_INIT
static HS_I2c *gli2c1Slave = NULL;
static void I2c1SlaveInit(Board *board)
{
	zHS_I2cConfig i2c1Cfg;
	gli2c1Slave = new HS_I2c;
	gli2c1Slave->init(board, I2C1_TRAS_DEV);
	
	i2cConfig(&i2c1Cfg);
    
	gli2c1Slave->enableWatchDog(false);

	while (!gli2c1Slave->config(i2c1Cfg));
	printf("i2c1 slave init bfm\n"); fflush(stdout);
	while (!gli2c1Slave->initBFM());
	gli2c1Slave->connect();
	gli2c1Slave->disableSequencer();
	printf("=============== i2c1 slave init finished\n"); fflush(stdout);
}
#else
static inline void I2c1SlaveInit(Board *board) {}
#endif

#ifdef CONFIG_I2C2_SLAVE_INIT
static HS_I2c *gli2c2Slave = NULL;
static void I2c2SlaveInit(Board *board)
{
	zHS_I2cConfig i2c2Cfg;
	gli2c2Slave = new HS_I2c;
	gli2c2Slave->init(board, I2C2_TRAS_DEV);
	
	i2cConfig(&i2c2Cfg);
    
	gli2c2Slave->enableWatchDog(false);

	while (!gli2c2Slave->config(i2c2Cfg));
	printf("i2c0 slave init bfm\n"); fflush(stdout);
	while (!gli2c2Slave->initBFM());
	gli2c2Slave->connect();
	gli2c2Slave->disableSequencer();
	printf("=============== i2c2 slave init finished\n"); fflush(stdout);
}
#else
static inline void I2c2SlaveInit(Board *board) {}
#endif

#ifdef CONFIG_I2C3_SLAVE_INIT
static HS_I2c *gli2c3Slave = NULL;
static void I2c3SlaveInit(Board *board)
{
	zHS_I2cConfig i2c3Cfg;
	gli2c3Slave = new HS_I2c;
	gli2c3Slave->init(board, I2C3_TRAS_DEV);
	
	i2cConfig(&i2c3Cfg);
    
	gli2c3Slave->enableWatchDog(false);

	while (!gli2c3Slave->config(i2c3Cfg));
	printf("i2c3 slave init bfm\n"); fflush(stdout);
	while (!gli2c3Slave->initBFM());
	gli2c3Slave->connect();
	gli2c3Slave->disableSequencer();
	printf("=============== i2c3 slave init finished\n"); fflush(stdout);
}
#else
static inline void I2c3SlaveInit(Board *board) {}
#endif

#ifdef CONFIG_I2C4_SLAVE_INIT
static HS_I2c *gli2c4Slave = NULL;
static void I2c4SlaveInit(Board *board)
{
	zHS_I2cConfig i2c4Cfg;
	gli2c4Slave = new HS_I2c;
	gli2c4Slave->init(board, I2C4_TRAS_DEV);
	
	i2cConfig(&i2c4Cfg);
    
	gli2c4Slave->enableWatchDog(false);

	while (!gli2c4Slave->config(i2c4Cfg));
	printf("i2c4 slave init bfm\n"); fflush(stdout);
	while (!gli2c4Slave->initBFM());
	gli2c4Slave->connect();
	gli2c4Slave->disableSequencer();
	printf("=============== i2c4 slave init finished\n"); fflush(stdout);
}
#else
static inline void I2c4SlaveInit(Board *board) {}
#endif

#ifdef CONFIG_I2C5_SLAVE_INIT
static HS_I2c *gli2c5Slave = NULL;
static void I2c5SlaveInit(Board *board)
{
	zHS_I2cConfig i2c5Cfg;
	gli2c5Slave = new HS_I2c;
	gli2c5Slave->init(board, I2C5_TRAS_DEV);
	
	i2cConfig(&i2c5Cfg);
    
	gli2c5Slave->enableWatchDog(false);

	while (!gli2c5Slave->config(i2c5Cfg));
	printf("i2c5 slave init bfm\n"); fflush(stdout);
	while (!gli2c5Slave->initBFM());
	gli2c5Slave->connect();
	gli2c5Slave->disableSequencer();
	printf("=============== i2c5 slave init finished\n"); fflush(stdout);
}
#else
static inline void I2c5SlaveInit(Board *board) {}
#endif

int I2cSlaveInit(Board *board)
{
	I2c0SlaveInit(board);
	I2c1SlaveInit(board);
	I2c2SlaveInit(board);
	I2c3SlaveInit(board);
	I2c4SlaveInit(board);
	I2c5SlaveInit(board);
	return 0;
}

#ifdef CONFIG_I2C0_SLAVE_RUN
static pthread_t i2c0_slave_pid;
static int I2c0SlaveRun(void)
{
	if (!gli2c0Slave) {
		printf("\nI2c0 slave not inited\n"); fflush(stdout);
		return -1;
	}
	pthread_create(&i2c0_slave_pid, NULL, (void *(*)(void *))I2cSlaveTask, (void *)gli2c0Slave);
	printf("\nI2c0 slave working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c0SlaveRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C1_SLAVE_RUN
static pthread_t i2c1_slave_pid;
static int I2c1SlaveRun(void)
{
	if (!gli2c1Slave) {
		printf("\nI2c1 slave not inited\n"); fflush(stdout);
		return -1;
	}
	pthread_create(&i2c1_slave_pid, NULL, (void *(*)(void *))I2cSlaveTask, (void *)gli2c1Slave);
	printf("\nI2c1 slave working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c1SlaveRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C2_SLAVE_RUN
static pthread_t i2c2_slave_pid;
static int I2c2SlaveRun(void)
{
	if (!gli2c2Slave) {
		printf("\nI2c2 slave not inited\n"); fflush(stdout);
		return -1;
	}
	pthread_create(&i2c2_slave_pid, NULL, (void *(*)(void *))I2cSlaveTask, (void *)gli2c2Slave);
	printf("\nI2c2 slave working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c2SlaveRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C3_SLAVE_RUN
static pthread_t i2c3_slave_pid;
static int I2c3SlaveRun(void)
{
	if (!gli2c3Slave) {
		printf("\nI2c3 slave not inited\n"); fflush(stdout);
		return -1;
	}
	pthread_create(&i2c3_slave_pid, NULL, (void *(*)(void *))I2cSlaveTask, (void *)gli2c3Slave);
	printf("\nI2c3 slave working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c3SlaveRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C4_SLAVE_RUN
static pthread_t i2c4_slave_pid;
static int I2c4SlaveRun(void)
{
	if (!gli2c4Slave) {
		printf("\nI2c4 slave not inited\n"); fflush(stdout);
		return -1;
	}
	pthread_create(&i2c4_slave_pid, NULL, (void *(*)(void *))I2cSlaveTask, (void *)gli2c4Slave);
	printf("\nI2c4 slave working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c4SlaveRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C5_SLAVE_RUN
static pthread_t i2c5_slave_pid;
static int I2c5SlaveRun(void)
{
	if (!gli2c5Slave) {
		printf("\nI2c5 slave not inited\n"); fflush(stdout);
		return -1;
	}
	pthread_create(&i2c5_slave_pid, NULL, (void *(*)(void *))I2cSlaveTask, (void *)gli2c5Slave);
	printf("\nI2c5 slave working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c5SlaveRun(void) {return 0;}
#endif

int I2cSlaveRun(void)
{
	int ret = 0;
	ret = I2c0SlaveRun();
	ret = I2c1SlaveRun();
	ret = I2c2SlaveRun();
	ret = I2c3SlaveRun();
	ret = I2c4SlaveRun();
	ret = I2c5SlaveRun();
	return ret;
}

