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
//#include "usb_host_desc.hh"


#include "libZebu.hh"
#include "/home/shares/zebu_ip_2017.12/include/ZFSDB.hh"


#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/UsbStruct.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/HostChannel.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/DevEndpoint.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/Usb.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/ZFSDB.hh"
#include "libZebuZEMI3.hh"
#include "ZEMI3Manager.hh"

#if 1
#include "/home/shares/zebu_ip_2017.12/XTOR/USB3.N-2017.12-SP1-1/include/Usb3X.hh"
#include "usb_dev_descriptors.hh"
#include "usb_dev_access.hh"
#include "usb_host_desc.hh"
#endif

using namespace std;
using namespace ZEBU;
using namespace ZEBU_IP;
using namespace USB3X;
//using namespace USB_URB;


#define ZEBUWORK "./zebu.work"
#define DESIGNFILE "./designFeatures"


#define DEBUG

FILE* USB3X_Device = NULL;
Usb3XDRD * usbDev = NULL;
FastWaveformCapture* waveform;

int USB3HostInit(Board* board )
{
	usbDev = new Usb3XDRD(DRD_Device);

	USB3X_Device = fopen ("USB3X_Device.log","w");
	usbDev->setDebugLevel(ZEBU_IP::USB3X::logPcd);
	usbDev->setLogPrefix("Device ");
	usbDev->setLog(USB3X_Device, true); //No log - mask define in file UsbStruct.hh 

	printf("[Log] Init USB device Xactor\n");
	usbDev->init(board, "A1000_tb.core_top_inst.lb_usb30_top_inst.lb_usb30_top_pre.USB3_DRD_DRIVER", "A1000_tb.clk_48m");  //the last parameter is the name of a clock for debug purpose

	return 0;
}

int USB3HostRun(void)
{
	zusb_DeviceSpeed speed  = ZUSB_SPEED_SUPER;
	usb_device_example.ctrl_ep = 0;
	usb_device_example.in_ep = 0;
	usb_device_example.out_ep = 0;
	//usb_device_example.isoc_out_ep = 0;
	usb_device_example.ctrl_req = 0;
	usb_device_example.in_req = 0;
	usb_device_example.out_req = 0;
	//usb_device_example.isoc_out_req = 0;
	usb_device_example.xtor = usbDev;
	FastWaveformCapture* waveform;
	int cnt = 0;

	#if SEM == 1
		usbDev->simulationMode(true);
	#endif
	usbDev->config();
	cerr << "TB DEV : InitBFM Start \n" ;

	if(usbDev->initBFM(speed, false)) {
		printf("Gadget Initialization failed.");
		return 0;
	} 
	cerr << "TB DEV : InitBFM End \n";

	bool disconnect_detected = false;
	do {
		disconnect_detected = (~usbDev->readUserIO()) & 1;
	} while(disconnect_detected);
	cerr << "TB DEV : Disconnect Detected \n";


	printf ("Enter usbDev->loop() \n"); fflush(stdout);
	{	int cnt = 0;
		while(1) {
			usbDev->loop();
			cnt++;
			printf("cnt = %d\r", cnt);
			if(cnt >= 180000)
				break;
		}
	}


	//////////////////////////////////////////////////////
	//         Connecting device after 2 ms             //
	//////////////////////////////////////////////////////

	if(usbDev->USBPlug(5,0)) {

		printf("TB DEV : Cannot connect the device.");
		return 0;
	} 
	printf ("TB DEV : USB Plug \n"); fflush(stdout);



	//////////////////////////////////////////////////////
	//       Control Enpoint 0 initialization           //
	//////////////////////////////////////////////////////

	usb_device_example.ctrl_ep = usbDev->getEndpoint(0);
	usb_device_example.ctrl_req = usb_device_example.ctrl_ep->AllocRequest(ZUSB_BUFSIZ);
	usb_device_example.ctrl_req->complete = ctrl_complete; // no complete function
	usbDev->registerControlSetupCB(usb_device_example.ctrl_req, my_dev_setup_cb, (void*)(&usb_device_example));

	device_desc.bMaxPacketSize0 = usb_device_example.ctrl_ep->GetMaxPacket();
	dev_qualifier.bMaxPacketSize0 = usb_device_example.ctrl_ep->GetMaxPacket();

	#if 0
	waveform = new FastWaveformCapture;
	waveform -> initialize(board);
	waveform -> add("qiwc_wholeDesign");
	waveform -> dumpFile("./verdi_dir/kun_fwc.ztdb");
	#endif

	do {
		usbDev->loop();
		cnt ++;
		if((cnt%1000) == 0) printf ("cnt = %d \n", cnt); fflush(stdout);
		#if 0
		if(cnt == 8200) {
		waveform = new FastWaveformCapture;
		waveform -> initialize(board);
		waveform -> add("qiwc_wholeDesign");
		waveform -> dumpFile("./verdi_dir/kun_fwc.ztdb");
		}
		#endif
		if(cnt >= 16000) {
			break; //exit test
		}
	}
	while(1);
	printf ("usb host test done. \n"); fflush(stdout);
	while(1) {usbDev->loop();}

	return 0;
}
