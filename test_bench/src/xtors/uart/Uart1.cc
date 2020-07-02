#include "global.h"
#include "/home/shares/zebu_ip_2017.12/XTOR/UART.N-2017.12-SP1-1/include/Uart.hh"
#include "xtor/xtor_uart.h"

using namespace UART;

UartTerm * glUart1 = NULL;
extern int uartterm_cfg(UartTerm *ut,const char *name);
extern void uart_thread(void* argv);

int Uart1Init(Board * board)
{
	if(glUart1){
		printf("UART1 has be inited. please recheck it.");
		return -1;
	}
	glUart1 = new UartTerm;
	glUart1->init(board,UART1_TRAS_DEV); 
	
	if(uartterm_cfg(glUart1,UART1_TRAS_ALIAS)){
		throw (UART1_TRAS_ALIAS" uart term cfg fail!."); 
	}

    return 0;
}

#ifdef CONFIG_UART1_RUN
pthread_t glUart1treadID = 0;
int Uart1Run(void)
{
	printf("UART1 run.");
	if(pthread_create(&glUart1treadID,NULL,(void* (*)(void*))uart_thread, (void*)glUart1)){
		throw (UART1_TRAS_ALIAS"Could not creat Uart pthread"); 
	}
     return 0;
}
#endif

