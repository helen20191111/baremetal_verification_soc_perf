#include "global.h"
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string>
#include <vector>
//#include "include/Uart.hh"
#include "/home/shares/zebu_ip/XTOR/UART.M-2017.03-2/include/Uart.hh"
#include "xtor/xtor_uart.h"

using namespace UART;
using namespace std;
void gettime(char *buf,bool withms=true);
int uart_cfg(Uart *ut,const char *name)
{
	bool ok;
	
	if (name != NULL){
		printf("uartTerm config %s\n",name);	
	}

	// UART configuration		 
	ok = ut->setWidth(8);
	ok &= ut->setParity(NoParity);
	ok &= ut->setStopBit(OneStopBit);
	ok &= ut->setRatio(16);
	//ok &= ut->setTermName(name);
	ok &= ut->config(); 
	
	if(!ok) { 
		printf ("Uart[%s] Could not configure UART",name);
		return -1;
	}
	return 0;
}

int uartterm_cfg(UartTerm *ut,const char *name)
{
	bool ok;
	
	if (name != NULL){
		printf("uartTerm config %s\n",name);	
	}

	// UART configuration		 
	ok = ut->setWidth(8);
	ok &= ut->setParity(NoParity);
	ok &= ut->setStopBit(OneStopBit);
	ok &= ut->setRatio(16);
	//ut->setTermName(name);
	//set debugLevel2
	ut->setDebugLevel(2);//enable log
	//set Uartterm logfile
	std::string src1=name;
        char timenow[80] ={0};
        gettime(timenow,false);
	std::string src2=timenow;
        std::string src3=".log";
	std::string log_name=src1+src2+src3;
	char* log_filename=NULL;
	log_filename=(char*)(log_name.c_str());
	ut->setLog(log_filename,false);
	ok &= ut->config(); 
	
	if(!ok) { 
		printf ("Uart[%s] Could not configure UART",name);
		return -1;
	}
	return 0;
}

void uart_thread(void* argv)
{
	UartTerm * uart = (UartTerm*)argv;
	//printf("UART[%s] thread Go!\n",uart->getTermName());
	while (uart && uart->isAlive()) {
		uart->UARTServiceLoop();
	}
	uart->closeDumpFile();
	//printf("UART[%s]  close!\n",uart->getTermName());
	if (uart != NULL) { 
		delete uart; 
		uart = NULL;
	}
}

