#ifndef __XTOR_I2C_H_
#define __XTOR_I2C_H_
#include "global.h"
#include "include/HS_I2c.hh"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "xtor/xtor_uart.h"

#ifdef CONFIG_XTOR_I2C
extern int I2cSlaveInit(Board *board);
extern int I2cSlaveRun(void);
extern int I2cMasterInit(Board *board);
extern int I2cMasterRun(void);
#else
static inline int I2cSlaveInit(Board *board) {return 0;}
static inline int I2cSlaveRun(void) {return 0;}
static inline int I2cMasterInit(Board *board) {return 0;}
static inline int I2cMasterRun(void) {return 0;}
#endif



#ifdef CONFIG_I2C_SLAVE_GENERAL
extern int I2cSlaveTask(void *para);
#else
static inline int I2cSlaveTask(void *pard) {return 0;}
#endif

#ifdef CONFIG_I2C_MASTER_GENERAL
extern int I2cMasterTask(void *para);
#else
static inline int I2cMasterTask(void *para) {return 0;}
#endif

#ifdef CONFIG_I2C_ADDR_7BIT
#define I2C_SLAVE_ADDR 0x55
#else
#define I2C_SLAVE_ADDR 0x2aa
#endif

#endif

