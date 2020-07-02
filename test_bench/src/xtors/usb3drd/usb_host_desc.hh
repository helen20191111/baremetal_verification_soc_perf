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



#ifndef __DEMO_DESC_PARSE_H
#define __DEMO_DESC_PARSE_H

using namespace ZEBU_IP;
using namespace USB3X;

extern int32_t parse_usb_config(const uint8_t* confBuf,
                 zusb_EndpointDescriptor** epIntIn,
                 zusb_EndpointDescriptor** epIntOut,
                 const uint32_t configNum);
#endif
