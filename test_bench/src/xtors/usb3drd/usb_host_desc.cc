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



#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "Usb3XUrbCommon.hh"
#include "usb_host_desc.hh"

using namespace ZEBU_IP;
using namespace USB3X;

int32_t parse_usb_config(const uint8_t*           confBuf,
                 zusb_EndpointDescriptor** epIntIn,
                 zusb_EndpointDescriptor** epIntOut,
                 const uint32_t           configNum) {
  // Parsing descriptors

  zusb_DescriptorHeader headerDesc;
  zusb_ConfigDescriptor configDesc;
  zusb_InterfaceDescriptor itfDesc;
  zusb_EndpointDescriptor epDesc;

  memcpy(&headerDesc, confBuf, ZUSB_DESC_HEADER_SIZE );
 while(headerDesc.bLength != 0) {
    if(headerDesc.bDescriptorType == ZUSB_DT_ENDPOINT) {
      memcpy(&epDesc, confBuf, ZUSB_DT_ENDPOINT_SIZE);
      printf("Reading Endpoint Descriptor:\n");
      printf("  Endpoint address %d\n", (epDesc.bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK);
      if((epDesc.bEndpointAddress & ZUSB_ENDPOINT_DIR_MASK) == ZUSB_DIR_IN) {
        if((epDesc.bmAttributes & ZUSB_ENDPOINT_XFERTYPE_MASK) == ZUSB_ENDPOINT_XFER_INT) {
          *epIntIn = new zusb_EndpointDescriptor;
          memcpy(*epIntIn, &epDesc, ZUSB_DT_ENDPOINT_SIZE);
          printf("  INTERRUPT IN\n");
        }
      }
      else {
        if((epDesc.bmAttributes & ZUSB_ENDPOINT_XFERTYPE_MASK) == ZUSB_ENDPOINT_XFER_INT) {
          *epIntOut = new zusb_EndpointDescriptor;
          memcpy(*epIntOut, &epDesc, ZUSB_DT_ENDPOINT_SIZE);
          printf("  INTERRUPT OUT\n");
        }
      }
    }
    else if(headerDesc.bDescriptorType == ZUSB_DT_CONFIG) {
      memcpy(&configDesc, confBuf, ZUSB_DT_CONFIG_SIZE);
      if(configDesc.bConfigurationValue != configNum) {
        printf("Error, bad configuration number\n");
        return 1;
      }
      printf("Reading Configuration Descriptor:\n");
      printf("  Configuration number = %d\n", configDesc.bConfigurationValue);
      printf("  Number of interfaces = %d\n", configDesc.bNumInterfaces);
    }
    else if(headerDesc.bDescriptorType == ZUSB_DT_INTERFACE) {
      memcpy(&itfDesc, confBuf, ZUSB_DT_INTERFACE_SIZE);
      printf("Reading Interface Descriptor:\n");
      printf("  Interface number %d\n", itfDesc.bInterfaceNumber);
      printf("  Alternate Setting %d\n", itfDesc.bAlternateSetting);
      printf("  Number of endpoints = %d\n", itfDesc.bNumEndpoints);
    }
    confBuf = confBuf + headerDesc.bLength;
    memcpy(&headerDesc, confBuf, ZUSB_DESC_HEADER_SIZE);
  }

  return 0;
}
