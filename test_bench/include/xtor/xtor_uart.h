#ifndef __XTOR_UART_H_
#define __XTOR_UART_H_

#ifdef CONFIG_UART0_INIT
extern int Uart0Init(Board *board);
#else
static inline int Uart0Init(Board *board) {return 0;}
#endif

#ifdef CONFIG_UART0_RUN
extern int Uart0Run(void);
#else
static inline int Uart0Run(void){return 0;}
#endif

#ifdef CONFIG_UART1_INIT
extern int Uart1Init(Board *board);
#else
static inline int Uart1Init(Board *board) {return 0;}
#endif
#ifdef CONFIG_UART1_RUN
extern int Uart1Run(void);
#else
static inline int Uart1Run(void){return 0;}
#endif

#ifdef CONFIG_UART2_INIT
extern int Uart2Init(Board *board);
#else
static inline int Uart2Init(Board *board) {return 0;}
#endif

#ifdef CONFIG_UART3_INIT
extern int Uart3Init(Board *board);
#else
static inline int Uart3Init(Board *board) {return 0;}
#endif


#ifdef CONFIG_UART3_SYNC
extern int UartSyncInit(void);
extern int UartSyncRun(void);
extern unsigned int xtor_dut_sync_read(char *str);
extern void xtor_dut_sync_write(char *str);
#else
static inline int UartSyncInit(void) {return 0;}
static inline int UartSyncRun(void) {return 0;}
static inline unsigned int xtor_dut_sync_read(char *str){return 0;}
static inline void xtor_dut_sync_write(char *str){}
#endif

#endif
