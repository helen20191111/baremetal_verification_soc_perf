
#include "global.h"
#if 0
#include "/home/shares/zebu_ip/XTOR/UART.M-2017.03-2/include/Uart.hh"
#include "/home/shares/zebu_ip/XTOR/I2C.M-2017.03-2/include/HS_I2c.hh"
#include "/home/shares/zebu_ip/XTOR/ZLPDDR4_XTOR.M-2017.03-2/include/ZLPDDR4Xtor.hh"
#include "/home/shares/zebu_ip/XTOR/I2S.M-2017.03-2/include/Audio.hh" 
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/UsbStruct.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/HostChannel.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/DevEndpoint.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/Usb.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/ZFSDB.hh"
#include "libZebuZEMI3.hh"
#include "ZEMI3Manager.hh"
#define THREADING svt_pthread_threading
#include "ZEMI3Xtor.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_zebu_platform.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_c_runtime_cfg.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_pthread_threading.hh"
#endif

svt_c_runtime_cfg* runtime = NULL; 
ZEMI3Manager *glZemi3 = NULL;
Board* glboard;
svt_report      *glreporter;
svt_c_threading *glzemi3_threading;

typedef struct XtorEXLibsS{
	char* ExLibPath;
	char* EXLibName;
}XtorEXLibsT;

XtorEXLibsT ExDynLibsTab[]={
#ifdef BST_USB2_DEV_INIT
	{"./zebu.work/usb_monitor.so","usb_monitor"},
#endif

#ifdef BST_CANFD_INIT
	{"./zebu.work/can_fd.so","can_fd"},
#endif
};


int Zemi3AddExLib(ZEMI3Manager *glZemi3)
{
	int i;
	for(i = 0;i < sizeof(ExDynLibsTab)/sizeof(ExDynLibsTab[0]);i++){
		glZemi3->setXtorDynLib(ExDynLibsTab[i].ExLibPath, ExDynLibsTab[i].EXLibName);
	}
}

Board* Zemi3BoardOpen() {
  
	glreporter  = new svt_report_default("Default SVT Reporter");
	glzemi3_threading = new THREADING();

	runtime = new svt_c_runtime_cfg();
	runtime->set_report_api(glreporter);
	std::cout <<"DDEBUG_SW tb_top, runtime_cfg " << runtime << std::endl;
	std::cout <<"DDEBUG_SW tb_top, glreporter " << glreporter << std::endl;
	runtime->set_threading_api(glzemi3_threading);
	
	//////////////////////////////////////////////////////
	//         Host transactor initialization           //
	//////////////////////////////////////////////////////

	printf("Opening ZEMI3 ZeBu board\n");
  

	char *zebuWork       = (char *)(ZEBUWORK_DIR);
	char *designFeatures = (char *)(DESIGNFILE_DIR); 

	glZemi3 = ZEMI3Manager::open(ZEBUWORK_DIR);
	glboard = glZemi3->getBoard();
	glZemi3->buildXtorList(XTOR_DPI_LST); // manually add the xtor or use buildXtorList
	Zemi3AddExLib(glZemi3);
	runtime->set_platform(new svt_zebu_platform(glboard, 1));
	svt_c_runtime_cfg::set_default(runtime);


	if (glboard == NULL) {
		printf("Could not open Zebu.");
		return glboard;
	}

	return glboard;
}

int BoardInit()
{
	glZemi3->init();	  //Using glZemi3->init() instead of board->init(NULL)
	glZemi3->start();   //This will start the glZemi3 service loop
	return 0;
}

Board* BoardOpen()
{
	return Zemi3BoardOpen();
}

