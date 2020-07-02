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

#ifdef CONFIG_I2C_MASTER_GENERAL
static void i2cConfig(zHS_I2cConfig *i2cCfg)
{
    i2cCfg->mode = Master;
    i2cCfg->speed = StandardI2c; // StandardI2c   FastI2c   HighSpeedI2c
    i2cCfg->ack_general_call = true;
    i2cCfg->hs_master_code = 1;
#ifdef CONFIG_I2C_ADDR_7BIT
    i2cCfg->slave_10b = false;
#else
    i2cCfg->slave_10b = true;
#endif
    i2cCfg->sda_setup_cnt = 8;
}
#endif

#ifdef CONFIG_I2C0_MASTER_INIT
static HS_I2c *gli2c0Master = NULL;
static void I2c0MasterInit(Board *board)
{
    zHS_I2cConfig i2c0Cfg;

    gli2c0Master = new HS_I2c;
    gli2c0Master->init(board, I2C0_TRAS_DEV);
	
	i2cConfig(&i2c0Cfg);

	gli2c0Master->enableWatchDog(false);
	
    while (!gli2c0Master->config(i2c0Cfg));
    printf("\ni2c0 master init bfm\n"); fflush(stdout);
    while (!gli2c0Master->initBFM());
    gli2c0Master->connect();
    gli2c0Master->disableSequencer();
    printf("=============  i2c0 master init finished\n"); fflush(stdout);
}
#else
static inline void I2c0MasterInit(Board *board) {}
#endif

#ifdef CONFIG_I2C1_MASTER_INIT
static HS_I2c *gli2c1Master = NULL;
static void I2c1MasterInit(Board *board)
{
    zHS_I2cConfig i2c1Cfg;

    gli2c1Master = new HS_I2c;
    gli2c1Master->init(board, I2C1_TRAS_DEV);
	
	i2cConfig(&i2c1Cfg);

	gli2c1Master->enableWatchDog(false);
	
    while (!gli2c1Master->config(i2c1Cfg));
    printf("\ni2c1 master init bfm\n"); fflush(stdout);
    while (!gli2c1Master->initBFM());
    gli2c1Master->connect();
    gli2c1Master->disableSequencer();
    printf("=============  i2c1 master init finished\n"); fflush(stdout);
}
#else
static inline void I2c1MasterInit(Board *board) {}
#endif

#ifdef CONFIG_I2C2_MASTER_INIT
static HS_I2c *gli2c2Master = NULL;
static void I2c2MasterInit(Board *board)
{
    zHS_I2cConfig i2c2Cfg;

    gli2c2Master = new HS_I2c;
    gli2c2Master->init(board, I2C2_TRAS_DEV);
	
	i2cConfig(&i2c2Cfg);

	gli2c2Master->enableWatchDog(false);
	
    while (!gli2c2Master->config(i2c2Cfg));
    printf("\ni2c2 master init bfm\n"); fflush(stdout);
    while (!gli2c2Master->initBFM());
    gli2c2Master->connect();
    gli2c2Master->disableSequencer();
    printf("=============  i2c2 master init finished\n"); fflush(stdout);
}
#else
static inline void I2c2MasterInit(Board *board) {}
#endif

#ifdef CONFIG_I2C3_MASTER_INIT
static HS_I2c *gli2c3Master = NULL;
static void I2c3MasterInit(Board *board)
{
    zHS_I2cConfig i2c3Cfg;

    gli2c3Master = new HS_I2c;
    gli2c3Master->init(board, I2C3_TRAS_DEV);
	
	i2cConfig(&i2c3Cfg);

	gli2c3Master->enableWatchDog(false);
	
    while (!gli2c3Master->config(i2c3Cfg));
    printf("\ni2c3 master init bfm\n"); fflush(stdout);
    while (!gli2c3Master->initBFM());
    gli2c3Master->connect();
    gli2c3Master->disableSequencer();
    printf("=============  i2c3 master init finished\n"); fflush(stdout);
}
#else
static inline void I2c3MasterInit(Board *board) {}
#endif

#ifdef CONFIG_I2C4_MASTER_INIT
static HS_I2c *gli2c4Master = NULL;
static void I2c4MasterInit(Board *board)
{
    zHS_I2cConfig i2c4Cfg;

    gli2c4Master = new HS_I2c;
    gli2c4Master->init(board, I2C4_TRAS_DEV);
	
	i2cConfig(&i2c4Cfg);

	gli2c4Master->enableWatchDog(false);
	
    while (!gli2c4Master->config(i2c4Cfg));
    printf("\ni2c4 master init bfm\n"); fflush(stdout);
    while (!gli2c4Master->initBFM());
    gli2c4Master->connect();
    gli2c4Master->disableSequencer();
    printf("=============  i2c4 master init finished\n"); fflush(stdout);
}
#else
static inline void I2c4MasterInit(Board *board) {}
#endif


#ifdef CONFIG_I2C5_MASTER_INIT
static HS_I2c *gli2c5Master = NULL;
static void I2c5MasterInit(Board *board)
{
    zHS_I2cConfig i2c5Cfg;

    gli2c5Master = new HS_I2c;
    gli2c5Master->init(board, I2C5_TRAS_DEV);
	
	i2cConfig(&i2c5Cfg);

	gli2c5Master->enableWatchDog(false);
	
    while (!gli2c5Master->config(i2c5Cfg));
    printf("\ni2c5 master init bfm\n"); fflush(stdout);
    while (!gli2c5Master->initBFM());
    gli2c5Master->connect();
    gli2c5Master->disableSequencer();
    printf("=============  i2c5 master init finished\n"); fflush(stdout);
}
#else
static inline void I2c5MasterInit(Board *board) {}
#endif

int I2cMasterInit(Board *board)
{
	I2c0MasterInit(board);
	I2c1MasterInit(board);
	I2c2MasterInit(board);
	I2c3MasterInit(board);
	I2c4MasterInit(board);
	I2c5MasterInit(board);
	return 0;
}

#ifdef CONFIG_I2C0_MASTER_RUN
static pthread_t i2c0_master_pid;
static int I2c0MasterRun(void)
{
    if (!gli2c0Master) {
        printf("\nI2c0 master not inited\n"); fflush(stdout);
        return -1;
    }
    pthread_create(&i2c0_master_pid, NULL, (void *(*)(void *))I2cMasterTask, (void *)gli2c0Master);
    printf("\nI2c0 master working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c0MasterRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C1_MASTER_RUN
static pthread_t i2c1_master_pid;
static int I2c1MasterRun(void)
{
    if (!gli2c1Master) {
        printf("\nI2c1 master not inited\n"); fflush(stdout);
        return -1;
    }
    pthread_create(&i2c1_master_pid, NULL, (void *(*)(void *))I2cMasterTask, (void *)gli2c1Master);
    printf("\nI2c1 master working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c1MasterRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C2_MASTER_RUN
static pthread_t i2c2_master_pid;
static int I2c2MasterRun(void)
{
    if (!gli2c2Master) {
        printf("\nI2c2 master not inited\n"); fflush(stdout);
        return -1;
    }
    pthread_create(&i2c2_master_pid, NULL, (void *(*)(void *))I2cMasterTask, (void *)gli2c2Master);
    printf("\nI2c2 master working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c2MasterRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C3_MASTER_RUN
static pthread_t i2c3_master_pid;
static int I2c3MasterRun(void)
{
    if (!gli2c3Master) {
        printf("\nI2c3 master not inited\n"); fflush(stdout);
        return -1;
    }
    pthread_create(&i2c3_master_pid, NULL, (void *(*)(void *))I2cMasterTask, (void *)gli2c3Master);
    printf("\nI2c3 master working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c3MasterRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C4_MASTER_RUN
static pthread_t i2c4_master_pid;
static int I2c4MasterRun(void)
{
    if (!gli2c4Master) {
        printf("\nI2c4 master not inited\n"); fflush(stdout);
        return -1;
    }
    pthread_create(&i2c4_master_pid, NULL, (void *(*)(void *))I2cMasterTask, (void *)gli2c4Master);
    printf("\nI2c4 master working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c4MasterRun(void) {return 0;}
#endif

#ifdef CONFIG_I2C5_MASTER_RUN
static pthread_t i2c5_master_pid;
static int I2c5MasterRun(void)
{
    if (!gli2c5Master) {
        printf("\nI2c5 master not inited\n"); fflush(stdout);
        return -1;
    }
    pthread_create(&i2c5_master_pid, NULL, (void *(*)(void *))I2cMasterTask, (void *)gli2c5Master);
    printf("\nI2c5 master working\n"); fflush(stdout);
	return 0;
}
#else
static inline int I2c5MasterRun(void) {return 0;}
#endif

int I2cMasterRun(void)
{
	int ret = 0;
	ret = I2c0MasterRun();
	ret = I2c1MasterRun();
	ret = I2c2MasterRun();
	ret = I2c3MasterRun();
	ret = I2c4MasterRun();
	ret = I2c5MasterRun();
	return ret;
}

