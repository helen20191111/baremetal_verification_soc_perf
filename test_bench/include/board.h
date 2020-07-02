#ifndef __BOARD_H_
#define __BOARD_H_

#define ZEBUWORK_DIR "./zebu.work"
#define DESIGNFILE_DIR "./designFeatures"
#define XTOR_DPI_LST "./zebu.work/xtor_dpi.lst"

/* UART */
#define UART0_TRAS_ALIAS "UART0"
#define UART0_TRAS_DEV "A1000_tb.uart_driver_0"

#define UART1_TRAS_ALIAS "UART1"
#define UART1_TRAS_DEV "A1000_tb.uart_driver_1"

#define UART2_TRAS_ALIAS "UART2"
#define UART2_TRAS_DEV "A1000_tb.uart_driver_2"

#define UART3_TRAS_ALIAS "UART3"
#define UART3_TRAS_DEV "A1000_tb.uart_driver_3"

/* I2C */
#define I2C0_TRAS_DEV "A1000_tb.i2c_xtor0"
#define I2C1_TRAS_DEV "A1000_tb.i2c_xtor1"
#define I2C2_TRAS_DEV "A1000_tb.i2c_xtor2"
#define I2C3_TRAS_DEV "A1000_tb.i2c_xtor3"
#define I2C4_TRAS_DEV "A1000_tb.i2c_xtor4"
#define I2C5_TRAS_DEV "A1000_tb.i2c_xtor5"

/* I2S */
#define I2S0_TRAS_DEV "A1000_tb.i2s_xactor_0" //XTOR i2s0 is slave
#define I2S1_TRAS_DEV "A1000_tb.i2s_xactor_1" //XTOR i2s1 is master

/* CAN-FD */
#define CanFD_TRAS_DEV "A1000_tb.can_fd_i_1"

#endif

