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

static unsigned int data = 0;

static int I2cSlaveMemoryCallBack(void *ptr, int para)
{
    HS_I2c* interface = (HS_I2c*)ptr;
    HS_I2cEvt access;

    while (interface->getRcvEvt(access)) {
        interface->displayXtorStatus();
        if (access.Start) {
            printf("\nI2c slave addr:%x\n", access.SlvId); fflush(stdout);
        } else if (access.Stop) {
            printf("\nI2c Catch Stop evt\n"); fflush(stdout);
        } else if (access.WriteReq) {
            printf("\ni2c revice data: %x\n", access.Byte); fflush(stdout);
        } else if (access.ReadReq) {
            interface->answerReadRequest(((++data)%256));
            printf("\ni2c send data: %x\n", data%256); fflush(stdout);
        } else 
            printf("\nCatch other evt\n"); fflush(stdout);
    }
    return 0;
}

int I2cSlaveTask(void *para)
{
    HS_I2c *i2c_xtor = (HS_I2c *)para;
    while (1) {
        i2c_xtor->serviceLoop(I2cSlaveMemoryCallBack, i2c_xtor);
    }
    return 0;
}

