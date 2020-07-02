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

//#define ZEBUWORK "./zebu.work"
//#define DESIGNFILE "./designFeatures"
//kun add end

using namespace std;
using namespace ZEBU;
using namespace ZEBU_IP;
using namespace USB_URB;
using namespace ZEBU_MP;

//UsbHost *glusbHst  = NULL;

#define DEBUG

static const int s_pktsz = 512; // endpoint max packet size
static const uint32_t s_SelectedInterface = 0;
static const uint32_t s_AltSetting = 1;
static uint32_t may_Isoc = 0;
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

UsbHost *usbHst;

// DATA input and output files
FILE *inFile  = NULL;
FILE *outFile = NULL;
FILE *dbgFile = NULL;

uint32_t totalXferSize = 20480;
uint8_t*  buff;
uint32_t xfersize;
int32_t totalTransfered;
uint32_t xfer_written;
uint8_t* ptBuff;
uint32_t rcvBuffSize;
uint8_t*  rcvbuff;
int actual_length = 0;
//uint32_t xfersizeOUT = 1024;
//uint32_t xfersizeIN = 1024;
uint32_t xfersizeOUT = 4;
uint32_t xfersizeIN = 4;
int currXfer = 0;
ZebuUrb* myurb = 0;
ctxStruct myStruct;

uint32_t iso_size = 4096;
uint8_t *iso_buffer_out;
uint8_t *iso_buffer_in;

int USB2DevInit(Board* board ) 
{  


	usbHst = new UsbHost;

	//////////////////////////////////////////////////////
	//         Initialize testbench files               //
	//////////////////////////////////////////////////////

	inFile  = fopen("test.in","r");
	outFile = fopen("test.out","w");
	dbgFile = fopen("trace_host.log","w");

	if (inFile == NULL || outFile == NULL) {
		if (inFile == NULL) {
			printf("Could not open input file\n");
		}
		if (outFile == NULL) {
			printf("Could not open output file\n");
		}
		return 1;
	}


	buff = UsbHost::AllocBuffer(totalXferSize+3);
	xfersize = fread((void *)buff, 1, totalXferSize, inFile);
	totalTransfered = xfersize;

	ptBuff = buff;
	rcvBuffSize = ((xfersize + s_pktsz - 1) / s_pktsz) * s_pktsz;
	rcvbuff = UsbHost::AllocBuffer(rcvBuffSize);


	myStruct.complete_val = 0;
	myStruct.xtor = usbHst;

	//////////////////////////////////////////////////////
	//         Host transactor initialization           //
	//////////////////////////////////////////////////////

	usbHst->SetLogPrefix("Host");

	printf("Opening ZeBu board\n");

#if SEM == 1
	usbHst->simulationMode(true);
#endif
	usbHst->Init(board, "A1000_tb.u_usb2_driver_Utmi_Host", "A1000_tb.usb2_host_xtor_clk");
	usbHst->RegisterCallBack(&service_loop_CB, (void *)("usb_callback"));
	usbHst->SetLog(dbgFile, true);
	usbHst->SetDebugLevel((logMask_t)0xFFFFFFFF);
	usbHst->RegisterTimeOutCB(&timeout_CB, &usbHst);


}

int USB2DevRun(void) 
{
	
	int i, j;
	///////////////////////////////////////////////////////////////////////////////
	//                  Choose the USB device address                            //
	///////////////////////////////////////////////////////////////////////////////

	if(usbHst->SetDeviceAddress(9)) {
		printf("Could not set a new address to device\n");
		return 1;
	}


	///////////////////////////////////////////////////////////////////////////////
	//                  Initialize hardware transactor                           //
	///////////////////////////////////////////////////////////////////////////////

	if(usbHst->InitBFM(1/*high_speed*/)) {
		printf("Transactor initialization failed.");
		return 1;
	} 

#if 0//USE_MONITOR
	// Creating a bus activity monitor file called usb_bus_activity.bmn
	usbHst->SetBusMonFileName("usb_bus_activity");

	// Starting monitor and dump datapayload information also
	usbHst->StartBusMonitor(MonitorData);
#endif

	// Creating an URB monitor file called host_urb_dump.umn
	usbHst->SetUrbMonFileName("host_urb_dump");
	// Starting URB dump
	usbHst->StartUrbMonitor();

	///////////////////////////////////////////////////////////////////////////////
	//                            Pluging device                                 //
	///////////////////////////////////////////////////////////////////////////////

	if(usbHst->USBPlug(0,0)) {
		printf("Transactor plug failed.");
		return 1;
	}


	///////////////////////////////////////////////////////////////////////////////
	//                             Device discovering                            //
	///////////////////////////////////////////////////////////////////////////////
	printf("Waiting device connection\n");
	fflush(stdout);
	int32_t st = ZUSB_STATUS_NO_DEVICE;
	do {
		st = usbHst->DiscoverDevice();
	}while(st == ZUSB_STATUS_NO_DEVICE);

	printf("\nDevice connected\n");
	fflush(stdout);

	if(usbHst->GetDeviceSpeed() == ZUSB_SPEED_HIGH) {
		printf("High speed device detected\n");
		fflush(stdout);
	}
	else {
		printf("Full speed device detected\n");
		fflush(stdout);
	}

	///////////////////////////////////////////////////////////////////////////////
	//                        Parsing device configuration                       //
	///////////////////////////////////////////////////////////////////////////////

	if(s_AltSetting != 0) {
		printf("Setting alternative interface : %d \n", s_AltSetting);
		fflush(stdout);
		if(usbHst->SetDeviceInterface( s_SelectedInterface, s_AltSetting) != ZUSB_STATUS_SUCCESS) {
			printf("Error : Unable to set interface\n");
			fflush(stdout);
			return 1;
		}
	}

	printf("Parsing configuration : \n");
	uint32_t num = usbHst->GetDeviceConfig();
	printf("usbHst->GetDeviceConfig() is %d\n", num);
	fflush(stdout);
	const uint8_t* confBuf = usbHst->GetRawConfiguration(usbHst->GetDeviceConfig());
	const uint8_t* ptBuf = confBuf;
	if (confBuf == NULL) {
		printf("confBuf is NULL, maybe Isoc\n");
		fflush(stdout);
		may_Isoc = 1;
	}
	// Selecting alternate setting 1

//	if(s_AltSetting != 0) {
//		printf("Setting alternative interface : %d \n", s_AltSetting);
//		fflush(stdout);
//		if(usbHst->SetDeviceInterface( s_SelectedInterface, s_AltSetting) != ZUSB_STATUS_SUCCESS) {
//			printf("Error : Unable to set interface\n");
//			fflush(stdout);
//			return 1;
//		}
//	}
// Parsing descriptors
	printf("Parsing configuration...\n");
	fflush(stdout);
	zusb_EndpointDescriptor* epBulkOut = 0;
	zusb_EndpointDescriptor* epBulkIn = 0;
	zusb_EndpointDescriptor* epIsocIn = 0;
	zusb_EndpointDescriptor* epIsocOut = 0;
	zusb_EndpointDescriptor* epIntIn = 0;
	zusb_EndpointDescriptor* epIntOut = 0;

	if (!may_Isoc) {
		if(parse_usb_config(confBuf,
					&epBulkIn,
					&epBulkOut,
					&epIsocIn,
					&epIsocOut,
					&epIntIn,
					&epIntOut,
					usbHst->GetDeviceConfig(),
					s_SelectedInterface,
					s_AltSetting)) {
			printf("Error during descriptors parsing.\n");
			fflush(stdout);
			return 1;
		}
	}
	printf("altsetting....%d \n", __LINE__);
	fflush(stdout);
	int32_t altsetting = usbHst->GetDeviceAltInterface( s_SelectedInterface );
	if(altsetting != ZUSB_STATUS_NOT_SUPPORTED) {
		printf("Checking selected interface : %d alt : %d\n", s_SelectedInterface, altsetting);
		fflush(stdout);
		if( (uint32_t)altsetting != s_AltSetting) {
			printf("Error : Alternate setting is not correctly set\n");
			fflush(stdout);
			return 1;
		}
	}
	else {
		printf("Error : Cannot read interface alternate setting\n");
		fflush(stdout);
		return 1;
	}

	printf("#####################################################\n");
	printf("################   Starting DEMO  ###################\n");
	printf("#####################################################\n");
	fflush(stdout);
	// BULK OUT

	int32_t ret = 0;

	uint32_t  numTransfer = 0;
	if(epBulkOut != 0) {
		printf("********start epBulkOut test!!!*********\n");
		fflush(stdout);
		while(totalTransfered > 0) {
			myStruct.complete_val = 0;
			if(totalTransfered < xfersizeOUT) {
				currXfer = totalTransfered;
			}
			else {
				currXfer = xfersizeOUT;
			}

			printf("BULK OUT %u USING SUBMIT\n", numTransfer);
			fflush(stdout);
			myurb = UsbHost::AllocUrb(0); 
			usbHst->FillBulkUrb (myurb, (epBulkOut->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK, ZUSB_DIR_OUT, ptBuff, currXfer,
					bulk_complete_fct, &myStruct);
			myurb->transfer_flags = 0;
			int32_t ret = usbHst->SubmitUrb(myurb);
			if(ret != 0) {
				printf("Submit failed with code = %d\n", ret);
				fflush(stdout);
				return 1;
			}
			while(!myStruct.complete_val) {
				if(ret = usbHst->Loop()) {
					if(ret == ZUSB_STATUS_NO_DEVICE) {
						printf("Device disconnected, exit!\n");
						fflush(stdout);
					}
					else {
						printf("Device error detected, exit!\n");
						fflush(stdout);
					}
					return 1;
				}
			}
			if(myurb->status != 0) {
				printf("End bulk out transfers status = %d\n", myurb->status);
				fflush(stdout);
				return 1;
			}
			actual_length = myurb->actual_length;
			UsbHost::FreeUrb(myurb);
			printf("END BULK OUT, actual length = %d\n", actual_length);
			fflush(stdout);
			++numTransfer;
			totalTransfered -= actual_length;
			ptBuff += actual_length;
		}
		// 0 transfer to indicate device to switch to IN bulk sequence
		printf("END BULK OUT transfer sequence\n");

		printf("EMPTY BULK OUT to indicate device the end of OUT transfers\n");
		fflush(stdout);
		ret = usbHst->SendBulkMessage( (epBulkOut->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK, ZUSB_DIR_OUT, ptBuff, 0, &actual_length, 0);
		if(ret != 0) {
			printf("End sending bulk out transfers status = %d\n", ret);
			fflush(stdout);
			return 1;
		}
	}
	if(epBulkIn != 0) {
		printf("********start epBulkIn test!!!*********\n");
		fflush(stdout);
		printf("STARTING BULK IN transfers\n");
		fflush(stdout);
		totalTransfered = xfersize;
		// BULK IN
		numTransfer = 0;
		while(totalTransfered > 0) {
			if(xfersizeIN > totalTransfered) {
				xfersizeIN = s_pktsz; 
			}
			myStruct.complete_val = 0;
			printf("BULK IN %u USING SEND BULK\n", numTransfer);
			fflush(stdout);
			ret = usbHst->SendBulkMessage((epBulkIn->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK, ZUSB_DIR_IN, rcvbuff, xfersizeIN, &actual_length, 0);
			if(ret != 0) {
				printf("End sending bulk in transfers status = %d\n", ret);
				fflush(stdout);
				return 1;
			}
			printf("END BULK IN, actual length = %d\n", actual_length);
			fflush(stdout);
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
	}
	if(epIntOut != 0) {
		printf("********start epIntOut test!!!*********\n");
		fflush(stdout);
		numTransfer = 0;
		totalTransfered = xfersize;
		ptBuff = buff;
		while(totalTransfered > 0) {
			myStruct.complete_val = 0;
			if(totalTransfered < xfersizeOUT) {
				currXfer = totalTransfered;
			}
			else {
				currXfer = xfersizeOUT;
			}

			printf("INT OUT %u USING SUBMIT\n", numTransfer);
			fflush(stdout);
			myurb = UsbHost::AllocUrb(0); 
			usbHst->FillIntUrb (myurb, (epIntOut->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK, ZUSB_DIR_OUT, ptBuff, currXfer,
					bulk_complete_fct, &myStruct, epIntOut->bInterval);
			myurb->transfer_flags = 0;
			int32_t ret = usbHst->SubmitUrb(myurb);
			if(ret != 0) {
				printf("Submit failed with code = %d\n", ret);
				fflush(stdout);
				return 1;
			}
			while(!myStruct.complete_val) {
				if(ret = usbHst->Loop()) {
					if(ret == ZUSB_STATUS_NO_DEVICE) {
						printf("Device disconnected, exit!\n");
						fflush(stdout);
					}
					else {
						printf("Device error detected, exit!\n");
						fflush(stdout);
					}
					return 1;
				}
			}
			if(myurb->status != 0) {
				printf("End int out transfers status = %d\n", myurb->status);
				fflush(stdout);
				return 1;
			}
			actual_length = myurb->actual_length;
			UsbHost::FreeUrb(myurb);
			printf("END INT OUT, actual length = %d\n", actual_length);
			fflush(stdout);
			++numTransfer;
			totalTransfered -= actual_length;
			ptBuff += actual_length;
		}
		// 0 transfer to indicate device to switch to IN bulk sequence
		printf("END INT OUT transfer sequence\n");

		printf("EMPTY INT OUT to indicate device the end of OUT transfers\n");
		fflush(stdout);
	}
	if(epIntIn != 0) {
		printf("********start epIntIn test!!!*********\n");
		fflush(stdout);
		printf("STARTING INT IN transfers\n");
		fflush(stdout);
		totalTransfered = xfersize;
		// INT IN
		numTransfer = 0;
		myStruct.complete_val = 0;
		printf("INT IN %u USING SEND BULK\n", numTransfer);
		fflush(stdout);
		myurb = UsbHost::AllocUrb(0); 
		usbHst->FillIntUrb (myurb, (epIntIn->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK, ZUSB_DIR_IN, rcvbuff, 4,
				bulk_complete_fct, &myStruct, epIntIn->bInterval);
		myurb->transfer_flags = 0;
		int32_t ret = usbHst->SubmitUrb(myurb);
		if(ret != 0) {
			printf("Submit failed with code = %d\n", ret);
			fflush(stdout);
			return 1;
		}
		while(!myStruct.complete_val) {
			if(ret = usbHst->Loop()) {
				if(ret == ZUSB_STATUS_NO_DEVICE) {
					printf("Device disconnected, exit!\n");
					fflush(stdout);
				}
				else {
					printf("Device error detected, exit!\n");
					fflush(stdout);
				}
				return 1;
			}
		}
		if(myurb->status != 0) {
			printf("End int in transfers status = %d\n", myurb->status);
			fflush(stdout);
			return 1;
		}
		printf("END INT IN, actual length = %d\n", myurb->actual_length);
		fflush(stdout);

		xfer_written = fwrite((void*)rcvbuff, 4, 1, outFile);
		fflush(outFile);
		printf("END INT IN transfer sequence\n");
	}
	if (may_Isoc) {
		printf("********start epIoscOut test!!!*********\n");
		fflush(stdout);
		iso_buffer_out = UsbHost::AllocBuffer(iso_size);
		iso_buffer_in = UsbHost::AllocBuffer(iso_size);
		for (i = 0; i < iso_size; i++) {
			int ii = i/256;
			switch (ii) {
			case 0:
				iso_buffer_out[i] = 2;
				break;
			case 1:
				iso_buffer_out[i] = 4;
				break;
			case 2:
				iso_buffer_out[i] = 6;
				break;
			case 3:
				iso_buffer_out[i] = 8;
				break;
			case 4:
				iso_buffer_out[i] = 10;
				break;
			case 5:
				iso_buffer_out[i] = 12;
				break;
			case 6:
				iso_buffer_out[i] = 14;
				break;
			case 7:
				iso_buffer_out[i] = 16;
				break;
			default:
				iso_buffer_out[i] = 18;
				break;
				
			}
		}
		myurb = UsbHost::AllocUrb(4);
		myStruct.complete_val = 0;
		usbHst->FillIsoUrb(myurb, 2, ZUSB_DIR_OUT, iso_buffer_out, 512 * 4, 4, bulk_complete_fct, &myStruct, 4);
		myurb->transfer_flags = ZURB_ISO_ASAP;
		for (i =j=0; i < 4; i++, j += 512) {
			usbHst->FillIsoPacket(myurb, i, j, 512);
		}
		printf("Start send isoc packet\n");
		fflush(stdout);
		myurb->actual_length = 0;
		ret = usbHst->SubmitUrb(myurb);
		if (ret != 0) {
			printf("Submit failed with code = %d\n", ret);
			fflush(stdout);
			return 1;
		}
		while (!myStruct.complete_val) {
			if (ret = usbHst->Loop()) {
				if (ret == ZUSB_STATUS_NO_DEVICE) {
					printf("Device disconnected, exit!\n");
					fflush(stdout);
				} else {
					printf("Device error detected, exit!\n");
					fflush(stdout);
				}
				return 1;
			}
		}
		if(myurb->status != 0) {
			printf("End isoc out transfers status = %d\n", myurb->status);
			fflush(stdout);
			return 1;
		}

		
		printf("********start epIoscIn test!!!*********\n");
		fflush(stdout);
		memset(iso_buffer_in, 0x1, 4096);
		UsbHost::FreeUrb(myurb);
		myurb = UsbHost::AllocUrb(4);
		myStruct.complete_val = 0;
		usbHst->FillIsoUrb(myurb, 2, ZUSB_DIR_IN, iso_buffer_in, 1024 * 4, 4, bulk_complete_fct, &myStruct, 4);
		for (i =j=0; i < 4; i++, j += 512) {
			usbHst->FillIsoPacket(myurb, i, j, 1024);
		}
		printf("Start Recive isoc packet\n");
		fflush(stdout);
		myurb->actual_length = 0;
		ret = usbHst->SubmitUrb(myurb);
		if (ret != 0) {
			printf("Submit failed with code = %d\n", ret);
			fflush(stdout);
			return 1;
		}
		while (!myStruct.complete_val) {
			if (ret = usbHst->Loop()) {
				if (ret == ZUSB_STATUS_NO_DEVICE) {
					printf("Device disconnected, exit!\n");
					fflush(stdout);
				} else {
					printf("Device error detected, exit!\n");
					fflush(stdout);
				}
				return 1;
			}

		}
		if(myurb->status != 0) {
			printf("End isoc in transfers status = %d\n", myurb->status);
			fflush(stdout);
			return 1;
		}

		xfer_written = fwrite((void*)iso_buffer_in, 1024, 4, outFile);
		fflush(outFile);
		printf("END INT IN transfer sequence\n");
		
	}
	printf("HOST TB DONE\n");
	fflush(stdout);
	while (1) {
		if(ret = usbHst->Loop()) {
			if(ret == ZUSB_STATUS_NO_DEVICE) {
				printf("Device disconnected, exit!\n");
				fflush(stdout);
			}
			else {
				printf("Device error detected, exit!\n");
				fflush(stdout);
			}
			return 1;
		}
	}
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

	delete usbHst;

	printf("Closing ZeBu board\n");
	// board->close();
	//zemi3->terminate();
	//zemi3->close(); //Using in place of board->close("testbench : ZeBu correctly closed.\n")

	return 0;
}
    
