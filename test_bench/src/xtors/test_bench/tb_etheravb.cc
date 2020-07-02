//---------------------------------------------------------------------  
// COPYRIGHT (C) 2019 SYNOPSYS INC.       
// This software and the associated documentation are confidential and   
// proprietary to Synopsys, Inc. Your use or disclosure of this software 
// is subject to the terms and conditions of a written license agreement 
// between you, or your company, and Synopsys, Inc. In the event of      
// publications, the following notice is applicable:                     
//                                                                       
// ALL RIGHTS RESERVED                                                   
//                                                                       
// The entire notice above must be reproduced on all authorized copies.  
//---------------------------------------------------------------------- 
// tb_urb_host.cc -- Testbench example of ZeBu USB
//                  URB API 

#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <zlib.h>

#include "UsbUrbCommon.hh"
#include "UsbUrbHost.hh"
#include "usb_host_desc.hh"


#include "libZebu.hh"
#include "libZebuZEMI3.hh"
//#include "ZFSDB.hh"
#include "/home/shares/zebu_ip_2017.12/include/ZFSDB.hh"

//kun add
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
using namespace UART;
using namespace I2C;
using namespace DRAM_SW;
using namespace I2S;

#define ZEBUWORK "./zebu.work"
#define DESIGNFILE "./designFeatures"
//kun add end

using namespace std;
using namespace ZEBU;
using namespace ZEBU_IP;
using namespace USB_URB;
using namespace ZEBU_MP;

#define DEBUG

Board     *board = 0;

static const int s_pktsz = 512; // endpoint max packet size
static const uint32_t s_SelectedInterface = 0;
static const uint32_t s_AltSetting = 1;


//kun add
typedef enum {
    eNORM_UART_ONLY,
    eSAFETY_UART_ONLY,
    eDUAL_UART,
	eNO_UART
}T_EUN_TERM_CFG;

UartTerm * urtTm = NULL;
HS_I2c *i2c0    = NULL;
UartTerm * urtTmSafety = NULL;
Audio  *i2s0    = NULL;
Audio  *i2s1    = NULL;
svt_c_runtime_cfg* runtime = NULL; 
//svt_c_runtime_cfg* runtime = NULL; 


int uartTerm_cfg(UartTerm *ut,const char *name)
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
	ok &= ut->setTermName(name);
	ok &= ut->config(); 
	
	if(!ok) { 
		printf ("uartTerm Could not configure UART");
		return -1;
	}
	
	return 0;
}


void uartTerm_thread()
{
	printf("uartTerm thread go\n");
/*
	urtTm->useZebuServiceLoop();
	// Processing UART data
	while (urtTm->isAlive()) {board->serviceLoop();}
*/

	while (urtTm->isAlive()) {
		urtTm->UARTServiceLoop();
	}
	
	urtTm->closeDumpFile();
	
out:	
	printf("uartTerm close\n");
	if (urtTm != NULL) { delete urtTm; urtTm = NULL;}
}

void uartTermSafety_thread()
{
	printf("uartTermSafety_thread go\n");
/*
	urtTm->useZebuServiceLoop();
	// Processing UART data
	while (urtTm->isAlive()) {board->serviceLoop();}
*/

	while (urtTm->isAlive()) {
		urtTm->UARTServiceLoop();
	}
	
	urtTm->closeDumpFile();
	
out:	
	printf("uartTermSafety_thread close\n");
	if (urtTm != NULL) { delete urtTm; urtTm = NULL;}
}



int i2c_hsCfg()
{
	zHS_I2cConfig i2cCfg;
	HS_I2cStatus_t HS_I2cStatus;
	BfmStatus_t BfmStatus;
	
	printf("i2c_hsCfg  run\n");
	
	i2c0->enableWatchdog(0); //disable watchdog
	
	i2cCfg.mode=Slave;
	i2cCfg.speed=StandardI2c;
	//i2cCfg.speed=HighSpeedI2c;
	i2cCfg.slave_10b=false;
	i2cCfg.slave_address=0x01;
	
	while(!i2c0->config(i2cCfg));
	printf("i2c_hsCfg  init bfm\n");
	while(!i2c0->initBFM());
	//assert(i2c0->runBFM (RunNcyc,1024));
	
	//i2c0->displayRxTrn(true,Full);
	i2c0->displayRxTrn(false,Full);
	
	/*
	while (!i2c0->waitBFMStatusChange (BfmStatus));
    assert (BfmStatus.b.Stopped == 1);
    assert (BfmStatus.b.RunNcycDone == 1);
	*/
	//get and clear
	//i2c0->getI2cStatus(&HS_I2cStatus,true);
	//i2c0->clearI2cStatus();	
	
	
	printf("i2c_hsCfg  connect\n");	
	i2c0->connect();
	
	printf("i2c_hsCfg slave disableSequencer\n");	
	i2c0->disableSequencer ();
	
	printf("i2c_hsCfg  done\n");
	return 0;
}


void i2cSlave_loop()
{
	HS_I2cStatus_t HS_I2cStatus;
	HS_I2cEvt RcvEvt;
	uint8_t addr = 0;
	bool end = false;
	bool ret;
	
	printf("i2c slave loop  run\n");
	
	while(1){
		//ret=i2c0->runBFM (RunNcycIdle,0xFFFFFFFF);
		//printf("run runBFM ret %d\n",ret);
		
		i2c0->getI2cStatus(&HS_I2cStatus,true);		
		printf("i2c status 0x%x\n",HS_I2cStatus);
		end = false;
		printf("i2c slave run get RcvEvt\n");
		while (!end) {
            if (i2c0->getRcvEvt(RcvEvt)) {
				printf("............i2c getRcvEvt\n");
				
                if(RcvEvt.Start){
					addr = RcvEvt.SlvId;
					printf("i2c start,addr %d\n",addr);
				}else if(RcvEvt.WriteReq) {
                    printf("i2c write request,value is %d\n",RcvEvt.Byte);
				}else if(RcvEvt.ReadReq){
					printf("i2c read request\n");
				}else if(RcvEvt.Stop){	
					printf("i2c stop\n");
					addr = 0;
					end = true;
				}
            }else{
				printf(".");				
				
				sleep(3);
			}
        }
		
		i2c0->clearI2cStatus();		
	}

	
out:	
	printf("i2c slave loop  close\n");
	if (i2c0 != NULL) { delete i2c0; i2c0 = NULL;}
}
//kun add end



// Structure passed as context for completion functions
struct ctxStruct {
  uint32_t complete_val;
  UsbHost* xtor;
};

///////////////////////////////////////////////////////////////////////////////
//                  ZebuURB  COMPLETION FUNCTIONS                            //
///////////////////////////////////////////////////////////////////////////////

static void bulk_complete_fct(ZebuUrb* zurb) {
  ctxStruct* theStruct = static_cast<ctxStruct*>(zurb->context);
  theStruct->complete_val = 1;
}

///////////////////////////////////////////////////////////////////////////////
//                  Zebu Xtor Timeout Callback                               //
///////////////////////////////////////////////////////////////////////////////

static bool timeout_CB (void* ctx) {
  printf("TIMEOUT detected\n");
  // Return 1 to force application to exit
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//                  Zebu Xtor Service Loop Callback                          //
///////////////////////////////////////////////////////////////////////////////

static void service_loop_CB (void *param){
  // Nothing special in this example
}

int main(int argc, char *argv[]) {
  // DATA input and output files
  FILE *inFile  = NULL;
  FILE *outFile = NULL;
  FILE *dbgFile = NULL;
svt_report      *reporter  = new svt_report_default("Default SVT Reporter");
svt_c_threading *threading = new THREADING();

 
  
  //////////////////////////////////////////////////////
  //         Initialize testbench files               //
  //////////////////////////////////////////////////////

  inFile  = fopen("test_bench/test.in","r");
  outFile = fopen("test_bench/test.out","w");
  dbgFile = fopen("test_bench/trace_host.log","w");

  if (inFile == NULL || outFile == NULL) {
    if (inFile == NULL) {
      printf("Could not open input file\n");
    }
    if (outFile == NULL) {
      printf("Could not open output file\n");
    }
    return 1;
  }
 
  uint32_t totalXferSize = 20480;
  uint8_t*  buff = UsbHost::AllocBuffer(totalXferSize+3);
  uint32_t xfersize = fread((void *)buff, 1, totalXferSize, inFile);
  int32_t totalTransfered = xfersize;
  
  uint32_t xfer_written;
  uint8_t* ptBuff = buff;
  uint32_t rcvBuffSize = ((xfersize + s_pktsz - 1) / s_pktsz) * s_pktsz;
  uint8_t*  rcvbuff = UsbHost::AllocBuffer(rcvBuffSize);
  int actual_length = 0;
  uint32_t xfersizeOUT = 1024;
  uint32_t xfersizeIN = 1024;
  int currXfer = 0;
  ZebuUrb* myurb = 0;
  T_EUN_TERM_CFG termCfg = eDUAL_UART;

  ctxStruct myStruct;

//kun add
  FastWaveformCapture* waveform;
	runtime = new svt_c_runtime_cfg();
	runtime->set_report_api(reporter);
	runtime->set_threading_api(threading);
//kun add end

  //////////////////////////////////////////////////////
  //         Host transactor initialization           //
  //////////////////////////////////////////////////////


  printf("Opening ZeBu board\n");
  
   //ZEMI3Manager *zemi3 = ZEMI3Manager::open(ZWORK, "designFeatures", "usb_driver");
//kun add
	char *zebuWork       = (char *)(ZEBUWORK);
	char *designFeatures = (char *)(DESIGNFILE); 
//kun add end
   //ZEMI3Manager *zemi3 = ZEMI3Manager::open(zebuWork, designFeatures, "uart");
   ZEMI3Manager *zemi3 = ZEMI3Manager::open("./zebu.work");
   board = zemi3->getBoard();
   zemi3->buildXtorList("./zebu.work/xtor_dpi.lst"); // manually add the xtor or use buildXtorList
   zemi3->setXtorDynLib("./zebu.work/usb_monitor.so", "usb_monitor");

   runtime->set_platform(new svt_zebu_platform(board, 1));
   svt_c_runtime_cfg::set_default(runtime);

// #if SEM == 1
//   board = Board::openWithSimulationMode(ZWORK, "designFeatures", "usb_driver") ;
// #else
//   board = Board::open(ZWORK, "designFeatures", "usb_driver");
// #endif

  if (board == NULL) {
    printf("Could not open Zebu.");
    return 1;
  }

#if 0
	waveform = new FastWaveformCapture;
	waveform -> initialize(board);
	waveform -> add("fwc_essential_signal");
	waveform -> dumpFile("fwc.ztdb");
#endif

#if 0
//kun add
	 if (termCfg == eNORM_UART_ONLY){
		urtTm = new UartTerm;
		urtTm->init(board,"A1000_tb.uart_driver_0"); 
	 }else if(termCfg == eSAFETY_UART_ONLY){
		urtTmSafety = new UartTerm;
		urtTmSafety->init(board,"A1000_tb.uart_driver_1"); 	
	 }else if(termCfg == eDUAL_UART){
		urtTm = new UartTerm;
		urtTm->init(board,"A1000_tb.uart_driver_0"); 
		urtTmSafety = new UartTerm;
		urtTmSafety->init(board,"A1000_tb.uart_driver_1"); 	
	 }
	 
	 i2c0 = new HS_I2c;
	 i2c0->init( board, "A1000_tb.i2c_xtor0");	


	 i2s0 = new Audio();
	 i2s0->init(board,"A1000_tb.i2s_xactor_0",1, 1, 32);
//kun add end
//kun add
		 //open xterm thread
		 pthread_t uartTerm_id = 0;
		 pthread_t uartTermSafety_id = 0;
		 
		 if (termCfg == eNORM_UART_ONLY){
			if(uartTerm_cfg(urtTm,"norm")){throw ("norm uart term cfg fail");}
				if(pthread_create(&uartTerm_id,NULL,(void* (*)(void*))uartTerm_thread,NULL)){
				throw ("Could not creat uartTerm pthread"); 
			}		
			
		 }else if (termCfg == eSAFETY_UART_ONLY){
			if(uartTerm_cfg(urtTmSafety,"safety")){throw ("safety uart term cfg fail");} 			
			if(pthread_create(&uartTermSafety_id,NULL,(void* (*)(void*))uartTermSafety_thread,NULL)){
				throw ("Could not creat uartTermSafety_thread pthread"); 
			}			
		 }else if (termCfg == eDUAL_UART){
			if(uartTerm_cfg(urtTm,"norm")){throw ("norm uart term cfg fail"); }
			if(uartTerm_cfg(urtTmSafety,"safety")){throw ("safety uart term cfg fail"); }				
	
			if(pthread_create(&uartTerm_id,NULL,(void* (*)(void*))uartTerm_thread,NULL)){
				throw ("Could not creat uartTerm pthread"); 
			}	
			if(pthread_create(&uartTermSafety_id,NULL,(void* (*)(void*))uartTermSafety_thread,NULL)){
				throw ("Could not creat uartTermSafety_thread pthread"); 
			}
		 }
		 
		 if(i2c_hsCfg()){throw ("i2c cfg fail"); }
#endif

  UsbHost *usbHst = new UsbHost;
  usbHst->SetLogPrefix("Host");
  myStruct.complete_val = 0;
  myStruct.xtor = usbHst;

  

  
#if SEM == 1
      usbHst->simulationMode(true);
#endif

// Warning: the monitor is used, the third parameter bellow MUST be the xtor clock
  //usbHst->Init(board, "utf_wrapper.u_usb_driver_Utmi_Host", "utf_wrapper.host_xtor_clk");
  usbHst->Init(board, "A1000_tb.u_usb2_driver_Utmi_Host", "A1000_tb.usb2_host_xtor_clk");

  usbHst->RegisterCallBack(&service_loop_CB, (void *)("usb_callback"));

  usbHst->SetLog(dbgFile, true);
  //usbHst->SetDebugLevel(logInfo);
  usbHst->SetDebugLevel((logMask_t)0xFFFFFFFF); 
  
  usbHst->RegisterTimeOutCB(&timeout_CB, &usbHst);

  Signal *flag = board->getSignal("A1000_tb.core_top_inst.lb_usb20_top_inst.lb_usb20_top_pre.usb2_controller.U_DWC_usb3_noclkrst.U_DWC_usb3_pwrm.U_DWC_usb3_pwrm_csr.dctl_run_stop_d");
  //board->init(NULL);
  zemi3->init();    //Using zemi3->init() instead of board->init(NULL)
  zemi3->start();   //This will start the Zemi3 service loop




        //waiting for device init done
//kun add end

///////////////////////////////////////////////////////////////////////////////
//                  Choose the USB device address                            //
///////////////////////////////////////////////////////////////////////////////

printf("usbHst->SetDeviceAddress(9)\n");
  if(usbHst->SetDeviceAddress(9)) {
    printf("Could not set a new address to device\n");
    return 1;
  }

printf("\n\n ZSIM --wait ltssm enabled- \n"); fflush(stdout);
//  while(!flag->get(0)) {
//     board->invalidateCaches(); 
//     printf("\n\n ZSIM --wait loop\n"); fflush(stdout);
//  	//usbHst->Loop();
//	board->serviceLoop();
//     sleep(1);   
//  }
printf("\n\n ZSIM --wait dut init- \n"); fflush(stdout);

///////////////////////////////////////////////////////////////////////////////
//                  Initialize hardware transactor                           //
///////////////////////////////////////////////////////////////////////////////
printf("usbHst->InitBFM.");
  if(usbHst->InitBFM(1, 0 /*1:high_speed*/)) {
//  if(usbHst->InitBFM(1 /*1:high_speed*/)) {
    printf("Transactor initialization failed.");
    return 1;
  } 

#if 0
#ifdef USE_MONITOR
printf("usbHst->SetBusMonFileName.");
// Creating a bus activity monitor file called usb_bus_activity.bmn
  usbHst->SetBusMonFileName("usb_bus_activity");

// Starting monitor and dump datapayload information also
  usbHst->StartBusMonitor(MonitorData);
#endif
#endif

printf("usbHst->SetUrbMonFileName.");
// Creating an URB monitor file called host_urb_dump.umn
  usbHst->SetUrbMonFileName("host_urb_dump");
// Starting URB dump
printf("usbHst->StartUrbMonitor.");
  usbHst->StartUrbMonitor();


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
printf("start sleep.\n");
//sleep(20);
printf("usbHst->USBPlug.");
  if(usbHst->USBPlug(0,0)) {
    printf("Transactor plug failed.");
    return 1;
  }


///////////////////////////////////////////////////////////////////////////////
//                             Device discovering                            //
///////////////////////////////////////////////////////////////////////////////
printf("start sleep.\n");
//sleep(5);
  printf("Waiting device connection\n");
  int32_t st = ZUSB_STATUS_NO_DEVICE;
  do {
    st = usbHst->DiscoverDevice();
	sleep(1);
	printf("discover result: %d\n", st);
  }while(st == ZUSB_STATUS_NO_DEVICE);

  printf("\nDevice connected\n");
  
  if(usbHst->GetDeviceSpeed() == ZUSB_SPEED_HIGH) {
    printf("High speed device detected\n");
  }
  else {
    printf("Full speed device detected\n");
  }

///////////////////////////////////////////////////////////////////////////////
//                        Parsing device configuration                       //
///////////////////////////////////////////////////////////////////////////////

  printf("Parsing configuration : \n");
  const uint8_t* confBuf = usbHst->GetRawConfiguration(usbHst->GetDeviceConfig());
  const uint8_t* ptBuf = confBuf;
  
  // Selecting alternate setting 1

  if(s_AltSetting != 0) {
    printf("Setting alternative interface : %d \n", s_AltSetting);
    if(usbHst->SetDeviceInterface( s_SelectedInterface, s_AltSetting) != ZUSB_STATUS_SUCCESS) {
      printf("Error : Unable to set interface\n");
      return 1;
    }
  }

  // Parsing descriptors

  zusb_EndpointDescriptor* epBulkOut = 0;
  zusb_EndpointDescriptor* epBulkIn = 0;
  zusb_EndpointDescriptor* epIsocOut = 0;

  if(parse_usb_config(confBuf,
                   &epBulkIn,
                   &epBulkOut,
                   &epIsocOut,
                   usbHst->GetDeviceConfig(),
                   s_SelectedInterface,
                   s_AltSetting)) {
      printf("Error during descriptors parsing.\n");
      return 1;
  }

  int32_t altsetting = usbHst->GetDeviceAltInterface( s_SelectedInterface );
  if(altsetting != ZUSB_STATUS_NOT_SUPPORTED) {
    printf("Checking selected interface : %d alt : %d\n", s_SelectedInterface, altsetting);
    if( (uint32_t)altsetting != s_AltSetting) {
      printf("Error : Alternate setting is not correctly set\n");
      return 1;
    }
  }
  else {
    printf("Error : Cannot read interface alternate setting\n");
    return 1;
  }

  printf("#####################################################\n");
  printf("################   Starting DEMO  ###################\n");
  printf("#####################################################\n");
  // BULK OUT

  int32_t ret = 0;
  int j = 0;
  int k = 0;

  uint32_t  numTransfer = 0;

  if(epBulkOut == 0) {
      printf("Error : Cannot TEST BULK OUT transfers, Endpoint not found in USB device configuration\n");
      return 1;
  }
  while(totalTransfered > 0) {
    myStruct.complete_val = 0;
    if(totalTransfered < xfersizeOUT) {
      currXfer = totalTransfered;
    }
    else {
      currXfer = xfersizeOUT;
    }

    printf("BULK OUT %u USING SUBMIT\n", numTransfer);
    myurb = UsbHost::AllocUrb(0); 
    usbHst->FillBulkUrb (myurb, (epBulkOut->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK, ZUSB_DIR_OUT, ptBuff, currXfer,
        bulk_complete_fct, &myStruct);
    myurb->transfer_flags = 0;
    int32_t ret = usbHst->SubmitUrb(myurb);
    if(ret != 0) {
      printf("Submit failed with code = %d\n", ret);
      return 1;
    }
    while(!myStruct.complete_val) {
      if(ret = usbHst->Loop()) {
        if(ret == ZUSB_STATUS_NO_DEVICE) {
          printf("Device disconnected, exit!\n");
        }
        else {
          printf("Device error detected, exit!\n");
        }
        return 1;
      }
    }
    if(myurb->status != 0) {
      printf("End bulk out transfers status = %d\n", myurb->status);
      return 1;
    }
    actual_length = myurb->actual_length;
    UsbHost::FreeUrb(myurb);
    printf("END BULK OUT, actual length = %d\n", actual_length);
    ++numTransfer;
    totalTransfered -= actual_length;
    ptBuff += actual_length;
  }

  // 0 transfer to indicate device to switch to IN bulk sequence
  printf("END BULK OUT transfer sequence\n");
  
  printf("EMPTY BULK OUT to indicate device the end of OUT transfers\n");
  ret = usbHst->SendBulkMessage( (epBulkOut->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK, ZUSB_DIR_OUT, ptBuff, 0, &actual_length, 0);
  if(ret != 0) {
    printf("End sending bulk out transfers status = %d\n", ret);
    return 1;
  }

  if(epBulkIn == 0) {
    printf("Cannot TEST BULK IN transfers, Endpoint not found in USB device configuration\n");
    return 1;
  }

  printf("STARTING BULK IN transfers\n");
  totalTransfered = xfersize;
  // BULK IN
  numTransfer = 0;
  while(totalTransfered > 0) {
    if(xfersizeIN > totalTransfered) {
      xfersizeIN = s_pktsz; 
    }
    myStruct.complete_val = 0;
    printf("BULK IN %u USING SEND BULK\n", numTransfer);
    ret = usbHst->SendBulkMessage((epBulkIn->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK, ZUSB_DIR_IN, rcvbuff, xfersizeIN, &actual_length, 0);
    if(ret != 0) {
      printf("End sending bulk in transfers status = %d\n", ret);
      return 1;
    }
    printf("END BULK IN, actual length = %d\n", actual_length);
    ++numTransfer;
    totalTransfered -= actual_length;
    int toWrite = actual_length;
    if(totalTransfered < 0) {
      toWrite = xfersize % xfersizeIN;
    }
    xfer_written = fwrite((void*)rcvbuff, 1, toWrite, outFile);
    fflush(outFile);
  }
  printf("END BULK IN transfer sequence\n");

  printf("HOST TB DONE\n");

#ifdef USE_MONITOR
// Stopping USB activity dump and close file
  usbHst->StopBusMonitor();
#endif

// Stopping URB dump and close file
  usbHst->StopUrbMonitor();

  UsbHost::FreeBuffer(rcvbuff);
  UsbHost::FreeBuffer(buff);
  fclose(inFile);
  fclose(outFile);
  fclose(dbgFile);

  delete epBulkOut;
  delete epBulkIn;
  delete epIsocOut;

  waveform -> disable();
  waveform -> closeFile();

  delete usbHst;
  printf("Closing ZeBu board\n");
  //board->close();
  zemi3->terminate();
  zemi3->close(); //Using in place of board->close("testbench : ZeBu correctly closed.\n")

  return 0;
}
    
