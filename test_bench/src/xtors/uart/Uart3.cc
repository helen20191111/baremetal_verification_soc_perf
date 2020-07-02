#include "global.h"
#include "/home/shares/zebu_ip_2017.12/XTOR/UART.N-2017.12-SP1-1/include/Uart.hh"
#include "xtor/xtor_uart.h"

using namespace UART;

extern int uart_cfg(Uart *ut,const char *name);

#ifdef CONFIG_UART2_INIT
Uart * glUart2 = NULL;
int Uart2Init(Board * board)
{
	if(glUart2) {
		printf("UART2 has be inited. please recheck it.");
		return -1;
	}

	glUart2 = new Uart;
	
	glUart2->init(board,UART2_TRAS_DEV);

	if(uart_cfg(glUart2, UART2_TRAS_ALIAS)){
		throw (UART2_TRAS_ALIAS " uart2 cfg fail!."); 
	}
    return 0;
}
#endif

#ifdef CONFIG_UART3_INIT
Uart * glUart3 = NULL;
int Uart3Init(Board * board)
{
	if(glUart3) {
		printf("UART3 has be inited. please recheck it.");
		return -1;
	}

	glUart3 = new Uart;
	
	glUart3->init(board,UART3_TRAS_DEV);

	if(uart_cfg(glUart3, UART3_TRAS_ALIAS)){
		throw (UART3_TRAS_ALIAS " uart3 cfg fail!."); 
	}
    return 0;
}
#endif

#ifdef CONFIG_UART3_RUN
pthread_t glUart3treadID = 0;
int Uart3Run(void)
{
	printf("UART3 run.");
	if(pthread_create(&glUart3treadID,NULL,(void* (*)(void*))uart_thread, (void*)glUart3)){
		throw ("Could not creat uartTerm pthread"); 
	}	
	
     return 0;
}
#endif

