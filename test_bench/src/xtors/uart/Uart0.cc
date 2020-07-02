#include "global.h"
#include "/home/shares/zebu_ip_2017.12/XTOR/UART.N-2017.12-SP1-1/include/Uart.hh"
#include "xtor/xtor_uart.h"

using namespace UART;

UartTerm * glUart0 = NULL;
extern int uartterm_cfg(UartTerm *ut,const char *name);
extern void uart_thread(void* argv);

int Uart0Init(Board * board)
{
	if(glUart0) {
		printf("UART0 has be inited. please recheck it.");
		return -1;
	}

	glUart0 = new UartTerm;
	
	glUart0->init(board,UART0_TRAS_DEV);

	if(uartterm_cfg(glUart0, UART0_TRAS_ALIAS)){
		throw (UART0_TRAS_ALIAS " uart term cfg fail!."); 
	}
    return 0;
}

#ifdef CONFIG_UART0_RUN
pthread_t glUart0treadID = 0;
int Uart0Run(void)
{
	printf("UART0 run.");
	if(pthread_create(&glUart0treadID,NULL,(void* (*)(void*))uart_thread, (void*)glUart0)){
		throw ("Could not creat uartTerm pthread"); 
	}	
	
     return 0;
}
#endif

