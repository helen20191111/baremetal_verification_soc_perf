#include "global.h"
// #include "Uart.hh"
//#include "XTOR/UART.M-2017.03-2/include/Uart.hh"
// #include "/home/shares/zebu_ip/XTOR/UART.M-2017.03-2/include/Uart.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/UART.N-2017.12-SP1-1/include/Uart.hh"
#include "xtor/xtor_uart.h"

#define DATA_LEN 32
//#define DATA_LEN 256

//using namespace UART;
using namespace UART;
Uart * glUart2 = NULL;
pthread_t glUart2treadID = 0;
void uart_data_single(void* argv);
void uart_afc(void* atgv);
int uart2_cfg(Uart *ut,const char *name,int d,int s,int p);
void uart_rev_datas(void* argv);

int Uart2Init(Board * board)
{
	if(glUart2){
		printf("UART2 has be inited. please recheck it.");
		return -1;
	}
	glUart2 = new Uart;
	glUart2->init(board,UART2_TRAS_DEV); 
	if(uart2_cfg(glUart2,UART2_TRAS_ALIAS,8,1,0)){
		throw (UART2_TRAS_ALIAS " uart2  cfg fail!."); 
	}
    return 0;
}

int Uart2Run(void)
{
	printf("UART2 ....run\r\n");
	//pthread_create(&glUart2treadID,NULL,(void* (*)(void*))uart_data_single,(void*)glUart2 );//test 6,7,8
	pthread_create(&glUart2treadID,NULL,(void* (*)(void*))uart_afc,(void*)glUart2 );//test13(uart 5) (#define DATA_LEN 256)  or(uart 7) /test10 received  (#define DATA_LEN 32)  /interrupt
	//pthread_create(&glUart2treadID,NULL,(void* (*)(void*))uart_rev_datas,(void*)glUart2 );//test10  transmit (uart 6)

     return 0;
}

int uart2_cfg(Uart *ut,const char *name,int d,int s,int p)
{
	bool ok;
	
	if (name != NULL){
		printf("****************************************************************************\r\n");
		printf("uart config (%10s):%d(%5s):%d(%7s):%d\r\n","data bits",d,"stop",s,"parity",p);	
		printf("****************************************************************************\r\n");
	}
	switch(d){
		case 8:
		ok = ut->setWidth(8);
		break;
		case 7:
		ok = ut->setWidth(7);
		break;
		case 6:
		ok = ut->setWidth(6);
		break;
		case 5:
		ok = ut->setWidth(5);
		break;
		default:
		ok = ut->setWidth(8);
		break;	
	}
	switch(s){
		case 1:
		ok &= ut->setStopBit(OneStopBit);
		break;
		case 2:
		ok &= ut->setStopBit(TwoStopBit);
		break;
		default:
		ok &= ut->setStopBit(OneStopBit);
		break;	
	}
	switch(p){
		case 0:
		ok &= ut->setParity(NoParity);
		break;
		case 1:
		ok &= ut->setParity(OddParity);
		break;
		case 2:
		ok &= ut->setParity(EvenParity);
		break;
		default:
		ok &= ut->setParity(NoParity);
		break;	
	}

	ok &= ut->setRatio(16);
	ok &= ut->config(); 
	if(!ok) { 
		printf ("Uart[%s] Could not configure UART",name);
		return -1;
	}
	return 0;
}

void uart_data_single(void* argv)
{
	printf("enter data bits thread...\r\n");
	uint8_t tmpData = 0; 
	Uart * uart = (Uart*)argv;
	printf("data word length..0x%x\r\n",uart->getWidth());
	printf("data parity setting..0x%x\r\n",uart->getParity());
	printf("data stop bits..0x%x\r\n",uart->getStopBit());
	while(1){
		if(uart->receive(&tmpData,true) == 1){
			printf("uart_data_single : rev data ...0x%x\r\n",tmpData);
			
			if (tmpData == 0x81){
				printf("receied  8 bits. 1 stop odd ... ok\r\n");
				// break;	
			}
			if (tmpData == 0x82){
				printf("receied  8 bits. 2 stop odd... ok\r\n");
				// break;	
			}
			if (tmpData == 0xaa){
				printf("receied  8 bits. 1 stop even... ok\r\n");
				// break;	
			}
			if (tmpData == 0x55){
				printf("receied  8 bits. 1 stop stick... ok\r\n");
				// break;	
			}
			if (tmpData == 0xee){
				printf("receied  8 bits. 1 stop no... ok\r\n");
				// break;	
			}
			if (tmpData == 0x11){
				printf("receied  5 bits. 1 stop odd ... ok\r\n");
				// break;	
			}
			if (tmpData == 0x21){
				printf("receied  6 bits. 1 stop odd ... ok\r\n");
				// break;	
			}
			if (tmpData == 0x12){
				printf("receied  5 bits. 2 stop odd... ok\r\n");
				// break;	
			}

			if (tmpData == 0x71){
				printf("receied  7 bits. 1 stop odd... ok\r\n");
				// break;	
			}

		}
	}

	return ;	
}

void uart_afc(void* argv)
{
	printf("enter send data  thread...\r\n");
	int i; 
	Uart * uart = (Uart*)argv;
	uint8_t tmpData = 0; 
	printf("data word length..0x%x\r\n",uart->getWidth());
	printf("data parity setting..0x%x\r\n",uart->getParity());
	printf("data stop bits..0x%x\r\n",uart->getStopBit());
	uint8_t data[DATA_LEN];//={0x10,0x11};
	for(i = 0;i< DATA_LEN;i++){
		data[i] = i;
	}
	while (1){
		while(1){
			if(uart->receive(&tmpData,true)){
				printf("rev data ...0x%x\r\n",tmpData);
				if (tmpData == 0xFF){
					printf("receied  0xff start to send datas ... ok\r\n");
					break;	
				}
			}	
		}

		printf("start to send data\r\n");
		uart->setTimeout(10);
		for(i=0;i< DATA_LEN;i++){
			if(uart->send(data[i],true)){
				printf("send date succeed..0x%x\r\n",data[i]);
			}else{
				printf("failed to send\r\n");
			}
		}
	}
	return;
}

void uart_rev_datas(void* argv)
{
	printf("enter receiced datas  thread...\r\n");
	uint8_t tmpData = 0; 
	uint16_t i = 0;
	Uart * uart = (Uart*)argv;
	printf("data word length..0x%x\r\n",uart->getWidth());
	printf("data parity setting..0x%x\r\n",uart->getParity());
	printf("data stop bits..0x%x\r\n",uart->getStopBit());

	while(1){
		if(uart->receive(&tmpData,true)){
			i++;
			printf("rev data %d...0x%x\r\n", i, tmpData);
		}
	}
	if(i >= 32)
	{
		printf("DMA rev data ...OK\r\n");
	}
	return;
}
