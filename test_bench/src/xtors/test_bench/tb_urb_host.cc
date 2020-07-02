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
#include "/home/shares/ZeBu_tools/zebu/M-2017.03-SP1-5/include/libZebuZEMI3.hh"
#include "/home/shares/ZeBu_tools/zebu/M-2017.03-SP1-5/include/ZEMI3Manager.hh"
#define THREADING svt_pthread_threading
#include "/home/shares/ZeBu_tools/zebu/M-2017.03-SP1-5/include/ZEMI3Xtor.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_zebu_platform.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_c_runtime_cfg.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_pthread_threading.hh"
#include "etheravb.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/ETHERAVB.N-2017.12-SP1-1/include/ethPtpFrame.hh"

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
using namespace ZEBU_ETHERAVB;
using namespace ETHERAVBFRAME;

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

int frame_rx = 0;
int frame_rx_len = 0;

int next_step_cnt = 0;

void read_rxd_frame_cb(void* xtor_a, etheravbFrame* frame_ptr, int queue_num)
{
	static int cell_count =0;
	etheravb* xtor = (etheravb*)xtor_a;
	const char* xtor_name = xtor->getHdlPath();

    printf("[%s] Frame received on queue %0d \n",xtor_name, queue_num);
    xtor->dumpFrame(*frame_ptr);
	frame_rx += 1;
	frame_rx_len = 0;//frame_ptr->getFrameSize();

    printf( "cell received by now %d, frame_rx ID:%d, recv-len:%d \n", cell_count++, frame_rx, frame_rx_len);
}

void read_rxd_status_cb(void* xtor_a, uint32_t* status_ptr, uint32_t array_size, int queue_num)
{
	printf( "read rxd status start! \n");

	etheravb* xtor = (etheravb*)xtor_a;
	const char* xtor_name = xtor->getHdlPath();
	printf( "read rxd status done! \n");
}

void fill_frame(packetStruct * ptrpacket, unsigned char * payload, int dlen, unsigned char start)
{
	int i;
	
	ptrpacket->dest_addr         =    0x776655443322;
	ptrpacket->src_addr          =    0x203040506070;
	
	ptrpacket->outer_tag_enable  =    0;//ptrpacket->outer_tag         =    0x81005678;
	ptrpacket->inner_tag_enable  =    0;//ptrpacket->inner_tag         =    0x81001246;
	ptrpacket->ether_type_length =    0x0800;
	ptrpacket->payload_length    =    dlen;
	
	//fill frame data
	for (i = 0; i<dlen; i++) {
		payload[i] = (unsigned char)((start+i/0x10)&0xff);
	}
	ptrpacket->payload = payload;
}

void fill_frame_vlan(packetStruct * ptrpacket, unsigned char * payload, int vlanid, int dlen, unsigned char start)
{
	int i;
	
	ptrpacket->dest_addr         =    0x776655443322;
	ptrpacket->src_addr          =    0x203040506070;
	
	ptrpacket->outer_tag_enable  =    1; ptrpacket->outer_tag = 0x81000000 | (vlanid&0xffff);
	ptrpacket->inner_tag_enable  =    0; ptrpacket->inner_tag = 0x81000000 | (vlanid&0xffff);
	ptrpacket->ether_type_length =    0x0800;
	ptrpacket->payload_length    =    dlen;
	
	//fill frame data
	for (i = 0; i<dlen; i++) {
		payload[i] = (unsigned char)((start+i/0x10)&0xff) ;
	}
	ptrpacket->payload = payload;
}

void xtor_sendframe(etheravb * etheravb, etheravbFrame * TX_Frame, zetheravbCommonConfig * m_cfg)
{  
	FrameReturn_t   frame_sent_status;
	int tx_cnt = 0;
		
	frame_sent_status = etheravb->send(*TX_Frame);
	while(frame_sent_status ==  FRAME_RETRY){
		frame_sent_status = etheravb->send(*TX_Frame);
		printf("%d retry time tx\n", ++tx_cnt);fflush(stdout);
	}
	if(frame_sent_status ==  FRAME_SENT){
		printf("send successfully \n");fflush(stdout);
	}
	etheravb->printConfig(m_cfg);
}


// case 1 :  tx&rx eth-II frame
// case 2 :  tx&rx direct VLAN frame
// case 3 :  tx&rx broadcast frame
// case 4 :  tx&rx multicast frame
// case 5 :  SA replace SA frame
// case 6 :  SA insert SA frame
// case 7 :  SA filter drop
// case 8 :  SA filter PASS
// case 7+:  DA filter drop
// case 8+:  DA filter PASS
// case 9 :  multi-queue send
// case 10 : VLAN-tag replace
// case 11 : VLAN-tag insert
// case 12 : vlan filter drop
// case 13 : vlan filter pass



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
//yanfeng   zemi3->setXtorDynLib("./zebu.work/usb_monitor.so", "usb_monitor");
//yanfeng 0513-2019
//yanfeng   zemi3->setXtorDynLib("./zebu.work/etheravb.so", "etheravb");

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

  zemi3->init();    //Using zemi3->init() instead of board->init(NULL)

  usbHst->RegisterCallBack(&service_loop_CB, (void *)("usb_callback"));


  usbHst->SetLog(dbgFile, true);
  //usbHst->SetDebugLevel(logInfo);
  usbHst->SetDebugLevel((logMask_t)0xFFFFFFFF); 
  
  usbHst->RegisterTimeOutCB(&timeout_CB, &usbHst);

  Signal *flag = board->getSignal("A1000_tb.core_top_inst.lb_usb20_top_inst.lb_usb20_top_pre.usb2_controller.U_DWC_usb3_noclkrst.U_DWC_usb3_pwrm.U_DWC_usb3_pwrm_csr.dctl_run_stop_d");


	//===============================================================================
	//start etheravb xtor
	//--------------------------------------------//
	//for test GMAC
	etheravb *etheravb1,*etheravb2;
	{
		etheravb *etheravb;
		zetheravbCommonConfig *m_cfg_1;
		etheravbFrame       *TX_Frame1;
		int i, icnt;
		
		int             pktSize;          
		uint8_t*        pktPayload1;   
		packetStruct    packet1;
		int   exp_queue_num = 0; 
	
		bool rsl;
		int tx_cnt = 0;
		frameStats_s frame_stats1;
		
		
		printf("=========================================================\n");
		printf("Gmac test start...\n");
		printf("=========================================================\n");
		
		m_cfg_1= new zetheravbCommonConfig();
		pktPayload1 = new uint8_t[2048]; 
		TX_Frame1 =  new etheravbFrame[1];
		
		/*etheravb1 = etheravb::get("A1000_tb.etheravb_xtor_1.etheravb_i", runtime);  
		if (etheravb1 == NULL) {
			printf("Invalid path to etheravb_1 transactor specified");
			return 1;
		}*/
		etheravb1 = etheravb::get("A1000_tb.etheravb_xtor_2.etheravb_i", runtime);  
		if (etheravb1 == NULL) {
			printf("Invalid path to etheravb_2 transactor specified");
			return 1;
		}
		
		if(etheravb1->runUntilReset())
			printf("etheravb_1 transactor specified ready");
		else
			printf("etheravb_1 transactor specified stays in RESET");
		
		m_cfg_1->enable_tx =1;
		m_cfg_1->enable_rx =1;
		m_cfg_1->set_tx_q_mode[0] = Q_MODE_ENABLE;
		m_cfg_1->set_rx_q_mode[0] = Q_MODE_ENABLE;
		m_cfg_1->mac_packet_filter.b.RA=1;
		//m_cfg_1->rx_flow_control_en = true;
		
		m_cfg_1->mac_speed_mode = ETHERAVB_1000Mbps;
		m_cfg_1->mac_addr0_high = 0x00002020;
		m_cfg_1->mac_addr0_low  = 0x40406060;
		
		
		rsl = etheravb1->checkConfigValidity(m_cfg_1);
		printf("value of checkConfigValidity is %d \n", rsl);
		etheravb1->config(m_cfg_1);
		etheravb1->setDebugFile("etheravb1_wyc0621.log",0); /* if run too long time will cause disk full! */
		etheravb1->setMacAddr(0x00003030, 0x40406060);    
		
		//regist rx callback
		etheravb1->registerCallbackRxStatus(&read_rxd_status_cb, etheravb1);
		etheravb1->registerCallbackRxdPacket(&read_rxd_frame_cb, etheravb1);
		etheravb1->setDebugLevel(4);
			
		//board->init(NULL);
		//zemi3->init();    //Using zemi3->init() instead of board->init(NULL)
		zemi3->start();   //This will start the Zemi3 service loop
		printf("Initial etheravb xtor complete\n");fflush(stdout);
		
		//----------------------------------------------//
		//start test gmac
		{			
#if 1
			//=====================================================================
			//-----------------------------------------case 1
			next_step_cnt += 1;
			//test rx & tx 802.2
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test rx & tx 802.2 \n, waiting...");fflush(stdout);
			while(frame_rx < next_step_cnt){
				//printf(".");fflush(stdout);
				etheravb1->runClk(200);
				etheravb1->getFrameCount(frame_stats1);
				printf("Stats for etheravb1 are, Transmitted Frames :: %d, Received Frames :: %d \n", frame_stats1.txFrameCount, frame_stats1.rxFrameCount);
				fflush(stdout);
				
				sleep(1);
			}
			printf("---will do reback send case 1---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 90, 0x10);
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
#endif


#if 1			
			//-----------------------------------------case 2
			next_step_cnt += 1;
			//test direct vlan frame
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test rx & tx direct vlan frame \n, waiting...");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				etheravb1->getFrameCount(frame_stats1);
				
				sleep(1);
			}
			printf("---will do reback send case 2---- \n, waiting..");fflush(stdout);
			fill_frame_vlan(&packet1, pktPayload1, 10, 92, 0x11);
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case 3
			next_step_cnt += 1;
			//test rx & tx broadcast
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test rx & tx broadcast \n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			printf("---will do reback send case 3---- \n");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 94, 0x20);
			packet1.dest_addr         =  0xffffffffffff;
			packet1.src_addr          =  0x203040506070;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case 4
			next_step_cnt += 1;
			//test rx & tx multicast
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test rx & tx multicast \n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			printf("---will do reback send case 4---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 96, 0x20);
			packet1.dest_addr         =   0xe01020304050;
			packet1.src_addr          =   0xe03040506070;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);

			//-----------------------------------------case 5
			next_step_cnt += 1;
			//test SA replace
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test SA replace \n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			printf("---will do reback send case 5---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 98, 0x20);
			packet1.dest_addr         =   0x605040302010;
			packet1.src_addr          =   0x706050403020;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case 6
			next_step_cnt += 1;
			//test SA insert
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test SA insert \n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			printf("---will do reback send case 6---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 100, 0x20);
			packet1.dest_addr         =   0x605040302010;
			packet1.src_addr          =   0x706050403020;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
#endif
			
#if 1
			//=====================================================================
			//------------------------------------------case -7
			//test SA filter
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test SA filter (filted)\n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			
			printf("---will do reback send case -7 ---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 102, 0x20);
			packet1.dest_addr         =   0x886655222222;
			packet1.src_addr          =   0x706050403010;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case -8
			//test SA filter
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test SA filter (pass?)\n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			
			printf("---will do reback send case -8 ---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 102, 0x20);
			packet1.dest_addr         =   0x776655443322;
			packet1.src_addr          =   0x706050202020;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case -9
			//test SA filter
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test SA filter (pass？)\n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			
			printf("---will do reback send case -9 ---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 102, 0x20);
			packet1.dest_addr         =   0x776655442222;
			packet1.src_addr          =   0x706050403020;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case -10
			//test SA filter
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test SA filter (pass!)\n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			
			printf("---will do reback send case -10 ---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 102, 0x20);
			packet1.dest_addr         =   0x776655443322;
			packet1.src_addr          =   0x706050403020;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
#endif
			
#if 1
			//=====================================================================
			//------------------------------------------case 7
			//test DA filter
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test DA filter (filted)\n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			
			printf("---will do reback send case 7 ---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 102, 0x20);
			packet1.dest_addr         =   0x886655222222;
			packet1.src_addr          =   0x706050403020;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case 8
			//test DA filter
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			printf("---test DA filter (pass)\n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			
			printf("---will do reback send case 8 ---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 102, 0x20);
			packet1.dest_addr         =   0x776655443322;
			packet1.src_addr          =   0x706050403020;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
#endif

#if 1
			/*next_step_cnt += 1;
			printf("---test multi-queue \n, waiting..");fflush(stdout);
			{
				int cnt = 300;
				while(frame_rx < next_step_cnt){
					printf(".");fflush(stdout);
					etheravb1->runClk(200);
					sleep(1);
					if(--cnt < 0)
						break;
				}
			}			
			//start dump wave
			waveform = new FastWaveformCapture;
			waveform -> initialize(board);
			waveform -> add("qiwc_wholeDesign");
			waveform -> dumpFile("./gmac_vlanfilter.ztdb");
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);*/
			
			next_step_cnt += 4;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			//=====================================================================
			//-----------------------------------------case 9
			//test multi queue 
			printf("---test multi-queue \n, waiting..");fflush(stdout);
			{
				int cnt = 12;
				while(frame_rx < next_step_cnt){
					printf(".");fflush(stdout);
					etheravb1->runClk(200);
					sleep(1);
					if(--cnt < 0)
						break;
				}
			}
			
			/*sleep(1);
			waveform -> disable();
			waveform -> closeFile();*/
			
			printf("---will do reback send case 9---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 102, 0x20);
			packet1.dest_addr         =   0x776655443322;
			packet1.src_addr          =   0x706050403020;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			printf("====================================================");fflush(stdout);
#endif
			
#if 1
			//VLAN TEST
			//=====================================================================
			//-----------------------------------------case 10
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			//test rx & tx replace vlan tag
			printf("---test rx & tx replace vlan tag \n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			printf("---will do reback send case 10---- \n");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 106, 0x20);
			packet1.dest_addr         =   0x605040302010;
			packet1.src_addr          =   0x706050403020;
			packet1.outer_tag_enable  =   0;
			//packet1.outer_tag         =   0x81000020;
			packet1.inner_tag_enable  =   1;
			packet1.inner_tag         =   0x81000012;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case 11
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			//test rx & tx insert vlan tag
			printf("---test rx & tx insert vlan tag \n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			printf("---will do reback send case 11---- \n, waiting..");fflush(stdout);
			fill_frame(&packet1, pktPayload1, 104, 0x20);
			packet1.dest_addr         =   0x605040302010;
			packet1.src_addr          =   0x706050403020;
			packet1.outer_tag_enable  =   0;
			//packet1.outer_tag         =   0x81000010;
			packet1.inner_tag_enable  =   1;
			packet1.inner_tag         =   0x81000015;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
#endif
			
#if 1
			//-----------------------------------------case 12
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			//test rx & tx GMAC vlan-filter, diff ID
			printf("---test rx & tx GMAC vlan-filter, diff ID \n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			printf("---will do reback send case 12---- \n");fflush(stdout);
			fill_frame_vlan(&packet1, pktPayload1, 0x20, 106, 0x30);
			packet1.dest_addr         =   0x504030201005;
			packet1.src_addr          =   0x807060504030;
			packet1.ether_type_length =   0x0800;
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			//-----------------------------------------case 13
			next_step_cnt += 1;
			printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
			//test rx & tx GMAC vlan-filter, same ID
			printf("---test rx & tx GMAC vlan-filter, same ID \n, waiting..");fflush(stdout);
			while(frame_rx < next_step_cnt){
				printf(".");fflush(stdout);
				etheravb1->runClk(200);
				sleep(1);
			}
			printf("---will do reback send case 13---- \n");fflush(stdout);
			fill_frame_vlan(&packet1, pktPayload1, 0x12, 106, 0x40);
			/*packet1.dest_addr         =   0x605040302010;
			packet1.src_addr          =   0x807060504030;
			packet1.ether_type_length =   0x0800;*/
			etheravb1->createFrame(packet1, TX_Frame1);
			etheravb1->dumpFrame(*TX_Frame1);
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			/*/start dump wave
			waveform = new FastWaveformCapture;
			waveform -> initialize(board);
			waveform -> add("qiwc_wholeDesign");
			waveform -> dumpFile("./gmac_vlanfilter.ztdb");
			xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			
			sleep(1);
			waveform -> disable();
			waveform -> closeFile();*/
#endif
#if 1
			//=====================================================================
            for (i = 0; i < 8; i++) {
				//-----------------------------------------case rx tx
				next_step_cnt += 1;
				//test rx & tx 802.2
				printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
				printf("---test rx tx counter %d\n, waiting...", i+1);fflush(stdout);
				while(frame_rx < next_step_cnt){
					printf(".");fflush(stdout);
					etheravb1->runClk(200);
					etheravb1->getFrameCount(frame_stats1);
					fflush(stdout);
					
					sleep(1);
				}
				printf("---will do reback send case 1---- \n, waiting..");fflush(stdout);
				fill_frame(&packet1, pktPayload1, 90, 0x10);
				etheravb1->createFrame(packet1, TX_Frame1);
				etheravb1->dumpFrame(*TX_Frame1);
				xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
			}
#endif
#if 1
			//=====================================================================
            while (1) {
				next_step_cnt += 1;
				printf("frame_rx: %d, next_step_cnt: %d\n", frame_rx, next_step_cnt);
				printf("---test \n, waiting...");fflush(stdout);
				while(frame_rx < next_step_cnt){
					printf(".");fflush(stdout);
					etheravb1->runClk(200);
					etheravb1->getFrameCount(frame_stats1);
					fflush(stdout);
					
					sleep(1);
				}
				printf("---will do reback send case x---- \n, waiting..");fflush(stdout);
				fill_frame(&packet1, pktPayload1, 90, 0x10);
				etheravb1->createFrame(packet1, TX_Frame1);
				etheravb1->dumpFrame(*TX_Frame1);
				xtor_sendframe(etheravb1, TX_Frame1, m_cfg_1);
		}
#endif
		}
		
		printf("Gmac test complete\n");
		printf("====================================\n");
		delete pktPayload1;
		delete m_cfg_1;
		delete []TX_Frame1;
	}


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
#endif



  delete usbHst;
  printf("Closing ZeBu board\n");
  //board->close();
  zemi3->terminate();
  zemi3->close(); //Using in place of board->close("testbench : ZeBu correctly closed.\n")

  return 0;
}
    
