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

#define I2C_TEST_WR_LEN 12
static unsigned int data = 0;

int I2cMasterTask(void *para)
{
    int i;
    unsigned char tx_buf[I2C_TEST_WR_LEN];
	char sync_str[] = "sync";
    HS_I2cTransEvt result = TrI2cError;
    HS_I2cStatus_t BusStatus;
    HS_I2cEvt Cpl;

    HS_I2c *i2c_xtor = (HS_I2c *)para;

    printf("\n"); fflush(stdout);
    for (i = 0; i < I2C_TEST_WR_LEN; i++) {
        tx_buf[i] = (data + i)%256;
        printf("%x ", tx_buf[i]); fflush(stdout);
    }
    printf("\n generate data ok!\n"); fflush(stdout);

    i2c_xtor->getI2cStatus(&BusStatus);
    while (BusStatus.b.BUS_BUSY == 1);
    printf("\n bus status is free now!\n"); fflush(stdout);

    while (1) {
        printf("\n wait for time sync\n"); fflush(stdout);
        xtor_dut_sync_read(sync_str);
        printf("\ntime sync..ed success!\n"); fflush(stdout);
        i2c_xtor->writeNByte(false, I2C_SLAVE_ADDR, tx_buf, I2C_TEST_WR_LEN);

        while (1) {
            result = i2c_xtor->send();
            if (result == TrI2cDone) break;
        }
        printf("\nsend 12 data success!\n"); fflush(stdout);

        i2c_xtor->getI2cStatus(&BusStatus);
        while (BusStatus.b.BUS_BUSY == 1);
        printf("\n bus status is free again\n");

        i2c_xtor->readNByte(false, I2C_SLAVE_ADDR, I2C_TEST_WR_LEN);
        while (i2c_xtor->send() != TrI2cDone);

        for (i = 0; i < I2C_TEST_WR_LEN; i++) {
            while (!i2c_xtor->getRcvEvt(Cpl));
            if (Cpl.AnsReq) {
                printf ("i: %d  read Byte %d is %x\n", i, Cpl.ByteId, Cpl.Byte);
                fflush(stdout);
            }
        }
    }
    return 0;
}

