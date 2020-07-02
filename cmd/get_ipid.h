#ifndef __GET_IPID_H_
#define __GET_IPID_H_

#define I2C0 0
#define I2C1 1
#define I2C2 2
#define I2C3 3
#define I2C4 4
#define I2C5 5

#define UART0 6
#define UART1 7
#define UART2 8
#define UART3 9

#define SPI0 10
#define SPI1 11

#define QSPI0 12
#define QSPI1 13

#define I2S0 14
#define I2S1 15

#define GPIO0 16
#define GPIO1 17

#define CANFD0 18
#define CANFD1 19

#define TIMER0 20
#define TIMER1 21
#define TIMER2 22
#define TIMER3 23

#define WDT0 24
#define WDT1 25
#define WDT2 26
#define WDT3 27

#define USB2 28

#define PCIE_4X 29
#define PCIE_2X 30
#define PCIE_PHY 31

#define SD_EMMC0 32
#define SD_EMMC1 33

#define GIC 34

#define DDRC0 35
#define DDRC1 36

#define GPU 37

#define A55_WDT0 38
#define A55_WDT1 39
#define A55_WDT2 40
#define A55_WDT3 41
#define A55_WDT4 42
#define A55_WDT5 43
#define A55_WDT6 44
#define A55_WDT7 45

#define A55_TIMER 46

#define USB3 47

#define GMAC0 48
#define GMAC1 49

#define GDMA 50
#define SDMA 51

#define IP_MAX 52

#define MODULE_IP(ip, name, base, offset)	{name, base, offset}

struct module_ip_struct {
    char *name;
    u32 base;
    u32 offset;
};

#endif
