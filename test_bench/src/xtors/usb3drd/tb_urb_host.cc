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
#include "libZebu.hh"
#include "libZebuZEMI3.hh"
#include "/home/shares/zebu_ip_2017.12/include/ZFSDB.hh"

#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/UsbStruct.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/HostChannel.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/DevEndpoint.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/Usb.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/ZFSDB.hh"

#include "/home/shares/zebu_ip_2017.12/XTOR/USB3.N-2017.12-SP1-1/include/Usb3X.hh"
#include "usb_dev_descriptors.hh"
#include "usb_dev_access.hh"
#include "usb_host_desc.hh"

using namespace std;
using namespace ZEBU;
using namespace ZEBU_IP;
using namespace USB3X;

// Structure passed as context for completion functions
struct ctxStruct {
  uint32_t   complete_val;
  Usb3XDRD * xtor;
};

Usb3XDRD * usbHst = NULL;
ctxStruct myStruct;
FILE* USB3X_Host = NULL;

#define BULK_PKT_NUM		(10)
#define BULK_PKT_SIZE		(1024)

///////////////////////////////////////////////////////////////////////////////
//                  ZebuURB  COMPLETION FUNCTIONS                            //
///////////////////////////////////////////////////////////////////////////////

static void bulk_complete_fct(ZebuUrb* zurb) {
	printf("bulk complete callback\n");
	ctxStruct* theStruct = static_cast<ctxStruct*>(zurb->context);
	theStruct->complete_val = 1;
	printf("bulk complete callback exit\n");
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

int USB3DevInit(Board* board )
{
	usbHst = new Usb3XDRD(DRD_Host);

	USB3X_Host = fopen ("USB3X_Host.log","w");
	usbHst->setLogPrefix("Host   ");
	usbHst->setDebugLevel((logMask_t)0xFFFFFFFF); 
	usbHst->setLog(USB3X_Host, true);

	printf("[Log] Init USB host Xactor\n");
	//init() the last parameter is the name of a clock for debug purpose
	usbHst->init(board, "A1000_tb.core_top_inst.lb_usb30_top_inst.lb_usb30_top_pre.USB3_DRD_DRIVER", "A1000_tb.clk_48m"); 

	return 0;
}

int USB3DevRun(void)
{
	uint8_t*  txbuff = Usb3XDRD::allocBuffer(BULK_PKT_NUM*2*BULK_PKT_SIZE);
	uint8_t*  rxbuff = Usb3XDRD::allocBuffer(BULK_PKT_NUM*2*BULK_PKT_SIZE);
	int actual_length = 0;
	ZebuUrb* myurb_out = 0;
	ZebuUrb* myurb_in = 0;
	int32_t xfer_len;
	int i, m, n, idx;
	int32_t ret;

	myStruct.complete_val = 0;
	myStruct.xtor = usbHst;

///////////////////////////////////////////////////////////////////////////////
//                  Choose the USB device address                            //
///////////////////////////////////////////////////////////////////////////////

	if(usbHst->setDeviceAddress(9)) {
		printf("Could not set a new address to device\n");
		return 1;
	}
  

///////////////////////////////////////////////////////////////////////////////
//                  Initialize hardware transactor                           //
///////////////////////////////////////////////////////////////////////////////
	usbHst->config();

	if(usbHst->initBFM(1/*super_speed*/ , false)) {
		printf("Transactor initialization failed.");
		return 1;
	} 

///////////////////////////////////////////////////////////////////////////////
//                            Pluging device                                 //
///////////////////////////////////////////////////////////////////////////////
	if(usbHst->USBPlug(0,0)) {
		printf("Transactor plug failed.");
		return 1;
	}
	printf ("TB HOST : USB Plug \n"); fflush(stdout);
 
///////////////////////////////////////////////////////////////////////////////
//                             Device discovering                            //
///////////////////////////////////////////////////////////////////////////////
	printf("\n\n ZSIM --waiting dev connections \n"); fflush(stdout);
	printf("Waiting device connection\n"); fflush(stdout);
	int32_t st = ZUSB_STATUS_NO_DEVICE;
	do {
		st = usbHst->discoverDevice();
		printf("discoverDevice() result:%d\n", st);
	}while(st == ZUSB_STATUS_NO_DEVICE);

	printf("\nDevice connected\n");

	if(usbHst->getDeviceSpeed() == ZUSB_SPEED_HIGH) {
		printf("High Speed device detected\n");
	}
	else if (usbHst->getDeviceSpeed() == ZUSB_SPEED_SUPER) {
		printf("Super Speed device detected\n");
	}
	else if (usbHst->getDeviceSpeed() == ZUSB_SPEED_SUPER_PLUS) {
		printf("Super Speed Plus device detected\n");
	}
	else
	printf("Full Speed device detected\n");

	// usbHst->loop() to wait DUT run to bulk test stage
	for(i = 0; i < 3000; i++) { 
		usbHst->loop();
	}

	// tx buffer init
	for(m = 0, idx = 0; m < BULK_PKT_NUM*2; m++) {
		if((BULK_PKT_NUM*2-1) == m) {
			for(n = 0; n < (BULK_PKT_SIZE-1); n++) {
				txbuff[idx++] = (m+n)&0xFF;
			}
		} else {
			for(n = 0; n < BULK_PKT_SIZE; n++) {
				txbuff[idx++] = (m+n)&0xFF;
			}		
		}
	}

	/*
	 * First bulk out test
	 * total 10 packets: packet 0~ packet 9 all packets size are BULK_PKT_SIZE 
	 */
	printf("Bulk out test start\n");fflush(stdout);
	xfer_len = BULK_PKT_NUM*BULK_PKT_SIZE;
	printf("xfer_len:%d\n", xfer_len);fflush(stdout);
	myurb_out = Usb3XDRD::allocUrb(0);
	usbHst->fillBulkUrb (myurb_out, 1, ZUSB_DIR_OUT, txbuff, xfer_len, bulk_complete_fct, &myStruct);

	myStruct.complete_val = 0;
	myurb_out->transfer_flags = 0;
	ret = usbHst->submitUrb(myurb_out);
	if(ret != 0) {
		printf("Submit failed with code = %d\n", ret);fflush(stdout);
	}

	while(!myStruct.complete_val) {
		if(ret = usbHst->loop()) {
			if(ret == ZUSB_STATUS_NO_DEVICE) {
				printf("Device disconnected, exit!\n");fflush(stdout);
			}
			else {
				printf("Device error detected, exit!\n");fflush(stdout);
			}
		}
	}

	if(myurb_out->status != 0) {
		printf("End bulk out transfers status = %d\n", myurb_out->status);fflush(stdout);
	}

	actual_length = myurb_out->actual_length;
	printf("END BULK OUT, actual length = %d\n", actual_length);fflush(stdout);


	/*
	 * Second bulk out test
	 * total 10 packets: packet 0~ packet 8 all packets size are BULK_PKT_SIZE 
	 * packet 9 size is BULK_PKT_SIZE-1
	 */
	printf("Bulk out test start\n");fflush(stdout);
	xfer_len = BULK_PKT_SIZE*BULK_PKT_NUM-1;
	printf("xfer_len:%d\n", xfer_len);fflush(stdout);
	usbHst->fillBulkUrb (myurb_out, 1, ZUSB_DIR_OUT, &txbuff[BULK_PKT_SIZE*BULK_PKT_NUM], xfer_len, bulk_complete_fct, &myStruct);

	myStruct.complete_val = 0;
	myurb_out->transfer_flags = 0;
	ret = usbHst->submitUrb(myurb_out);
	if(ret != 0) {
		printf("Submit failed with code = %d\n", ret);fflush(stdout);
	}

	while(!myStruct.complete_val) {
		if(ret = usbHst->loop()) {
			if(ret == ZUSB_STATUS_NO_DEVICE) {
				printf("Device disconnected, exit!\n");fflush(stdout);
			}
			else {
				printf("Device error detected, exit!\n");fflush(stdout);
			}
		}
	}

	if(myurb_out->status != 0) {
		printf("End bulk out transfers status = %d\n", myurb_out->status);fflush(stdout);
	}

	actual_length = myurb_out->actual_length;
	Usb3XDRD::freeUrb(myurb_out);
	printf("END BULK OUT, actual length = %d\n", actual_length);fflush(stdout);

	//small loop to wait DUT prepare bulk in transfer
	for(i = 0; i < 100; i++) {
		usbHst->loop();
	}


	myurb_in = Usb3XDRD::allocUrb(0);

	xfer_len = BULK_PKT_SIZE*BULK_PKT_NUM;
	printf("bulk in transfer len: %d\n", xfer_len);
	usbHst->fillBulkUrb (myurb_in, 1, ZUSB_DIR_IN, rxbuff, xfer_len, bulk_complete_fct, &myStruct);
	myurb_in->transfer_flags = 0;
	myStruct.complete_val = 0;
	ret = usbHst->submitUrb(myurb_in);

	if(ret != 0) {
		printf("Submit failed with code = %d\n", ret);
	}
	while(!myStruct.complete_val) {
		if(ret = usbHst->loop()) {
			if(ret == ZUSB_STATUS_NO_DEVICE) {
				printf("Device disconnected, exit!\n");
			}
			else {
				printf("Device error detected, exit!\n");
			}
			break;
		}
	}

	if(myurb_in->status != 0) {
		printf("End bulk in transfers status = %d\n", myurb_in->status);
	}

	xfer_len = BULK_PKT_SIZE*BULK_PKT_NUM-1;
	printf("bulk in transfer len: %d\n", xfer_len);
	myStruct.complete_val = 0;
	usbHst->fillBulkUrb (myurb_in, 1, ZUSB_DIR_IN, &(rxbuff[1024*10]), xfer_len, bulk_complete_fct, &myStruct);
	myurb_in->transfer_flags = 0;
	ret = usbHst->submitUrb(myurb_in);

	if(ret != 0) {
		printf("Submit failed with code = %d\n", ret);
		//return 1;
	}

	while(!myStruct.complete_val) {
		if(ret = usbHst->loop()) {
			if(ret == ZUSB_STATUS_NO_DEVICE) {
				printf("Device disconnected, exit!\n");
			}
			else {
				printf("Device error detected, exit!\n");
			}
			break;
		}
		printf("usbHst->loop\n");
	}

	if(myurb_in->status != 0) {
		printf("End bulk in transfers status = %d\n", myurb_in->status);
	}

	/*
	 * check bulk in data
	 * packet 0: 0x00, 0x01, 0x02, 0x03, 0x04, ... (total 1024 bytes)
	 * packet 1: 0x01, 0x02, 0x03, 0x04, 0x05, ... (total 1024 bytes)
	 * packet 2: 0x02, 0x03, 0x04, 0x05, 0x06, ... (total 1024 bytes)
	 *  ... ...
	 * packet 19: ...                              (total 1023 bytes)
	 */
	ret = 0;
	for(m = 0, idx = 0; m < BULK_PKT_NUM*2; m++) {
		printf("m = %d\n", m);fflush(stdout);
		if((BULK_PKT_NUM*2-1) == m) {
			for(n = 0; n < (BULK_PKT_SIZE-1); n++) {
				if(rxbuff[idx++] != ((m+n)&0xFF)) {
					ret = 1;
					printf("data error: expect(0x%x), real(0x%x)\n", rxbuff[idx-1], (m+n)&0xFF);fflush(stdout);
				}
			}
		} else {
			for(n = 0; n < BULK_PKT_SIZE; n++) {
				if(rxbuff[idx++] != ((m+n)&0xFF)) {
					ret = 1;
					printf("data error: expect(0x%x), real(0x%x)\n", rxbuff[idx-1], (m+n)&0xFF);fflush(stdout);
				}
			}		
		}
	}
	printf("data check finished.\n");fflush(stdout);
	Usb3XDRD::freeUrb(myurb_in);

	if(! ret) {
		printf("USB3 device bulk in data check pass.\n");fflush(stdout);
	} else {
		printf("USB3 device bulk in data check fail.\n");fflush(stdout);
	}
	
	//do not remove this loop, otherwise DUT clock will stuck and DUT data check can not finish
	for(i = 0; i < 2000; i++) {
		usbHst->loop();
	}

	//delete usbHst;
	fclose(USB3X_Host);
	printf("USB3 device test done.\n");fflush(stdout);
	return 0;
}
