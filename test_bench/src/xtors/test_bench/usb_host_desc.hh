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
// usb_host_desc.hh -- USB descriptors parser
//                  for USB URB API example

#ifndef __DEMO_DESC_PARSE_H
#define __DEMO_DESC_PARSE_H

using namespace ZEBU_IP;
using namespace USB_URB;

extern int32_t parse_usb_config(const uint8_t* confBuf,
                 zusb_EndpointDescriptor** epBulkIn,
                 zusb_EndpointDescriptor** epBulkOut,
                 zusb_EndpointDescriptor** epIsocOut,
                 const uint32_t           configNum,
                 const uint32_t           interfaceNum,
                 const uint32_t           altSetting);
#endif
