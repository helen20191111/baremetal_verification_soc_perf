#include <stdexcept>
#include <exception>
#include <queue>
#include <sys/time.h>
#include <libZebu.hh>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "/home/shares/zebu_ip/XTOR/UART.M-2017.03-2/include/Uart.hh"
#include "/home/shares/zebu_ip/XTOR/I2C.M-2017.03-2/include/HS_I2c.hh"
#include "/home/shares/zebu_ip/XTOR/I2S.M-2017.03-2/include/Audio.hh" 

using namespace ZEBU;
using namespace ZEBU_IP;
using namespace UART;
using namespace I2C;
using namespace I2S;

using namespace std;
//#################################
// main
//#################################


#define ZEBUWORK "./zebu.work"
#define DESIGNFILE "./designFeatures"

Board * board = NULL;
UartTerm * urtTm = NULL;
Uart *uart1 = NULL;
Audio  *i2s0    = NULL;
Audio  *i2s1    = NULL;

//#define UART_HTREAD

#ifdef UART_HTREAD
void uartTerm0_thread()
{
	printf("uartTerm0_thread go\n");

	while (urtTm->isAlive()) {
		urtTm->UARTServiceLoop();
	}
	
	urtTm->closeDumpFile();
	
out:	
	printf("uartTermSafety_thread close\n");
	if (urtTm != NULL) { delete urtTm; urtTm = NULL;}
}

void uartTerm1_thread()
{
	printf("uartTerm1_thread go\n");

	while (uart1->isAlive()) {
		uart1->UARTServiceLoop();
	}
	
	uart1->closeDumpFile();
	
out:	
	printf("uartTermSafety_thread close\n");
	if (uart1 != NULL) { delete uart1; uart1 = NULL;}
}
#endif


int main (int argc, char *argv[]) {	
	int ret = 0;
	HS_I2c *i2c0    = NULL;
	zHS_I2cConfig i2cCfg;
	HS_I2cEvt RcvEvt;
	HS_I2cStatus_t HS_I2cStatus;
	BfmStatus_t BfmStatus;
	bool ok;
	char *zebuWork       = (char *)(ZEBUWORK);
	char *designFeatures = (char *)(DESIGNFILE); 

#ifdef UART_HTREAD
	pthread_t uartTerm0 = 0;
	pthread_t uartTerm1 = 0;
#endif
 
	//RpCtxt replierCtxt;
	try {
		//open ZeBu
		printf("ZSIM opening ZEBU...\n");
		board = Board::open(zebuWork,designFeatures,"test_uart");
		if (board==NULL) { throw ("Could not open Zebu"); }

		printf("ZSIM opening uart Term...\n");	

		urtTm = new UartTerm;
		uart1 = new Uart;
		i2s0 = new Audio();
		//i2s1 = new Audio();

		i2c0 = new HS_I2c;
		i2cCfg.mode=Slave;
		i2cCfg.speed=StandardI2c;
		i2cCfg.slave_address=0x01;
		urtTm->init(board,"A1000_tb.uart_driver_0");
		uart1->init(board,"A1000_tb.uart_driver_1");
		i2c0->init( board, "A1000_tb.i2c_xtor0");

		printf("\n\n ZSIM -- init board -- \n"); fflush(stdout);
		board->init(NULL);
		i2c0->enableWatchdog(0); //disable watchdog
		while(!i2c0->config(i2cCfg));
		while(!i2c0->initBFM());
		assert(i2c0->runBFM (RunNcyc,0xFFFFFFFF));
		i2c0->getI2cStatus(&HS_I2cStatus,true);
		i2c0->connect();

		i2s0->init(board,"A1000_tb.i2s_xactor_0",1, 1, 32);
		//i2s1->init(board,"A1000_tb.i2s_xactor_1",0, 1, 32);

		// UART configuration		 
		ok = urtTm->setWidth(8);
		ok &= urtTm->setParity(NoParity);
		ok &= urtTm->setStopBit(OneStopBit);
		ok &= urtTm->setRatio(16);
		ok &= urtTm->config(); 

		ok = uart1->setWidth(8);
		ok &= uart1->setParity(NoParity);
		ok &= uart1->setStopBit(OneStopBit);
		ok &= uart1->setRatio(16);
		ok &= uart1->config(); 
		if(!ok) { throw ("Could not configure UART");}

		urtTm->useZebuServiceLoop();
		printf("\n\n ZSIM-- Starting testbench -- \n"); fflush(stdout);

		uart1->useZebuServiceLoop();

#ifdef UART_HTREAD
		if(pthread_create(&uartTerm0,NULL,(void* (*)(void*))uartTerm0_thread,NULL)){
			throw ("Could not creat uartTerm pthread"); 
		}	
#if 0
		if(pthread_create(&uartTerm1,NULL,(void* (*)(void*))uartTerm1_thread,NULL)){
			throw ("Could not creat uartTermSafety_thread pthread"); 
		}
#endif
#else

		while (urtTm->isAlive()) {board->serviceLoop();}
#endif
	}
	 catch (const char *err) {
		ret = 1; 
		fprintf(stderr,"Testbench error: %s\n", err);
	}
 
	if (urtTm != NULL) { delete urtTm; }
	if (board != NULL) { board->close((ret ==0)?"OK":"KO"); }
	printf("Test %s\n",(ret==0)?"OK":"KO");
	return ret;
}
