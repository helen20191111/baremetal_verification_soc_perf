#include <global.h>
#include <unistd.h>

#include <xtor/xtor_uart.h>
#include <xtor/xtor_i2c.h>

int main(int argc, char *argv[])
{
	Board *board = NULL;
	board = BoardOpen();

	Uart0Init(board);
	Uart1Init(board);
	Uart2Init(board);
	Uart3Init(board);
	UartSyncInit();
	
	I2cSlaveInit(board);
	I2cMasterInit(board);

	BoardInit();

	Uart0Run();
	Uart1Run();
	UartSyncRun();

	I2cSlaveRun();
	I2cMasterRun();

	printf("\n\n zebu transctor  finish\n");
	while (1) {
		sleep(1000);
	}
	return 0;
}

