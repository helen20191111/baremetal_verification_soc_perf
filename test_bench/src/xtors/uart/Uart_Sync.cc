#include "global.h"
#include "/home/shares/zebu_ip_2017.12/XTOR/UART.N-2017.12-SP1-1/include/Uart.hh"
#include "xtor/xtor_uart.h"

using namespace UART;

#define SYNC_BUF_LENGTH		(8)

extern Uart * glUart3;
static Uart * UartSync = NULL;
pthread_t SyncUarttreadID = 0;

volatile unsigned char sbuf[SYNC_BUF_LENGTH];

int head;
int tail;

void sync_uart_thread(void* argv)
{
	unsigned char data;
	int ret;

	while(1) {
		UartSync->UARTServiceLoop();
		ret = UartSync->receive(&data, true);
		if(1 == ret) {
			sbuf[head++] = data;
			printf("rx data: 0x%x\n", data);
			if(head >= SYNC_BUF_LENGTH) {
				head = 0; //just assume head will not exceed tail
			}
		}
	}
}

int UartSyncInit()
{
	printf("Enter uart sync init.\n");
	UartSync = glUart3;
	printf("UART3 use for XTOR<->DUT sync.\n");

	UartSync->setTimeout(60); //set blocking tx/rx data timeout seconds
	head = 0;
	tail = 0;

	return 0;
}

void short_delay(int count)
{
	int i;
	for(i = 0; i < count; i++);
}

int UartSyncRun()
{

	printf("Enter uart run.\n");
	if(pthread_create(&SyncUarttreadID,NULL,(void* (*)(void*))sync_uart_thread, 0)){
		throw ("Could not creat uartTerm pthread"); 
	}
	return 0;
}

int buf_valid_data()
{

	static int head_bak = 0;
	static int tail_bak = 0;
	if((head != head_bak) || (tail != tail_bak)) {
		head_bak = head;
		tail_bak = tail;
		printf("sbuf:%x,%x,%x,%x, %x,%x,%x,%x\n", sbuf[0],sbuf[1],sbuf[2],sbuf[3],sbuf[4],sbuf[5],sbuf[6],sbuf[7]);
	}

	if(head >= tail) {
		return (head-tail);
	} else {
		return (head+SYNC_BUF_LENGTH-tail);
	}
}

unsigned int xtor_dut_sync_read(char *str)
{
	unsigned int syncval = 0, syncval_read = 0, i;
	char *ptr = str;

	if(NULL == ptr) {
		printf("parameter invalid.\n");
		return 0;
	}

	if(strlen(ptr) > 100) {
		printf("string is too long.\n");
		return 0;
	}

	while(*ptr) {
		syncval += *(ptr++);
	}

	printf("read str:%s\n", ptr);
	printf("expect syncval:%x\n", syncval&0xFFFFFFFF);

	while(1) {
		if(buf_valid_data() >= 4) {
			for(i = 0, syncval_read = 0; i < 4; i++) {
				syncval_read <<= 8;
				syncval_read |= sbuf[tail++];
				if(tail >= SYNC_BUF_LENGTH) {
					tail = 0;
				}
			}
			printf("syncval read:%x\n", syncval_read&0xFFFFFFFF);
			if(syncval_read == syncval) {
				break;	
			}
		}
	}

	return (syncval_read&0xFFFFFFFF);
}

void xtor_dut_sync_write(char *str)
{
	unsigned int syncval = 0;
	char *ptr = str;

	if(NULL == ptr) {
		printf("parameter invalid.\n");
		return;
	}

	if(strlen(ptr) > 100) {
		printf("string is too long.\n");
		return;
	}

	while(*ptr) {
		syncval += *(ptr++);
	}

	printf("write str:%s\n", ptr);
	printf("write syncval:%x\n", syncval&0xFFFFFFFF);
	UartSync->send( (syncval&0xFF000000)>>24 , true);
	UartSync->send( (syncval&0x00FF0000)>>16 , true);
	UartSync->send( (syncval&0x0000FF00)>>8  , true);
	UartSync->send( (syncval&0x000000FF)>>0  , true);
}

