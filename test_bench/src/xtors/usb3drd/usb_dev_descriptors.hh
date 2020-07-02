// -----------------------------------------------
// usb_dev_descriptors.hh -- USB descriptors
//                  for USB URB API example
// 
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



#ifndef __DEMO_USB_DEVICE_H
#define __DEMO_USB_DEVICE_H

#include "Usb3XUrbCommon.hh"

using namespace std;
using namespace ZEBU;
using namespace ZEBU_IP;
using namespace USB3X;
       
#define ZUSB_BUFSIZ  512
#define SS


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      DEVICE DESCRIPTION                                   //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*-------------------------------------------------------------------------*/

static const char manufacturer[] = "EVE ZeBu Gadget";
static const char product [] = "ZeBu Gadget Demo Design";
static const char serial [] = "Demo Version";
/*-------------------------------------------------------------------------*/

#define STRING_MANUFACTURER 1
#define STRING_PRODUCT      2
#define STRING_SERIAL       3

/* static strings, in UTF-8 */
static struct zusb_String    strings [] = {
  { STRING_MANUFACTURER, manufacturer, },
  { STRING_PRODUCT, product, },
  { STRING_SERIAL, serial, },
  {  }      /* end of list */
};

static struct zusb_GadgetStrings  stringtab = {
  language  : 0x0409,  /* en-us */
  strings  : strings
};


static struct zusb_DeviceDescriptor
device_desc = {
bLength :    ZUSB_DT_DEVICE_SIZE,
  bDescriptorType :  ZUSB_DT_DEVICE,
#if defined(SS) || defined(SSP)
  bcdUSB             :  0x0310, //USB3.1
#else	
  bcdUSB             :  0x0210, //USB2.1
#endif	
  bDeviceClass :    ZUSB_CLASS_PER_INTERFACE,
  bDeviceSubClass :      0,
  bDeviceProtocol :      0,
#if defined(SS) || defined(SSP)
  bMaxPacketSize0    :  9,
#else	
  bMaxPacketSize0    :  64,
#endif
  idVendor :    0x0405,
  idProduct :  0x3002,
  bcdDevice :            0x0100,
  iManufacturer :  STRING_MANUFACTURER,
  iProduct :    STRING_PRODUCT,
  iSerialNumber :  STRING_SERIAL,
  bNumConfigurations :  1
};

static const struct zusb_ConfigDescriptor
z_config = {
bLength :    ZUSB_DT_CONFIG_SIZE,
  bDescriptorType :  ZUSB_DT_CONFIG,

  wTotalLength :  0,
  /* compute wTotalLength on the fly */
  bNumInterfaces :  1,
  bConfigurationValue :  1,
  iConfiguration :  0,
  bmAttributes :    0x40,
  bMaxPower :    0  /* self-powered */
};


static struct zusb_QualifierDescriptor
dev_qualifier = {
bLength :    10, // size of qualifier
  bDescriptorType :  ZUSB_DT_DEVICE_QUALIFIER,
#if defined(SS) || defined(SSP)
  bcdUSB             :  0x0310, //USB3.1
#else	
  bcdUSB             :  0x0210, //USB2.1
#endif	
  bDeviceClass :    ZUSB_CLASS_VENDOR_SPEC,
  bDeviceSubClass :    0,
  bDeviceProtocol :    0,
  bMaxPacketSize0 :    0,
  bNumConfigurations :  1,
  bRESERVED :    0,
};

static struct zusb_BOSDescriptor
BOS_desc = {
  bLength         : ZUSB_DT_BOS_SIZE,
  bDescriptorType : ZUSB_DT_BOS, 
  wTotalLength    : (ZUSB_DT_BOS_SIZE + ZUSB_DT_USB_EXT_CAP_SIZE + ZUSB_DT_USB_SS_CAP_SIZE + 0x14),
  bNumDeviceCaps  : 3
 };

static struct zusb_USB2ExtensionDescriptor
dev_usb2_capability =
{
  bLength : ZUSB_DT_USB_EXT_CAP_SIZE,
  bDescriptorType : ZUSB_DT_DEVICE_CAPABILITY,
  bDevCapabilityType : ZUSB_CAP_TYPE_EXT,
  bmAttributes : 2
 
 };

static struct zusb_SSDeviceCapabilitiesDescriptor
dev_ss_capability = {
  bLength                : ZUSB_DT_USB_SS_CAP_SIZE,
  bDescriptorType        : ZUSB_DT_DEVICE_CAPABILITY,
  bDevCapabilityType     : ZUSB_SS_CAP_TYPE,
  bmAttributes           : 0,
  wSpeedsSupported       : (ZUSB_FULL_SPEED_OPERATION | ZUSB_HIGH_SPEED_OPERATION | ZUSB_5GBPS_OPERATION),
  bFunctionnalitySupport : ZUSB_FULL_SPEED_OPERATION,
  bU1DevExitLat          : 10,
  bU2DevExitLat          : 256
 };

static const struct zusb_SSPDeviceCapabilitiesDescriptor 
dev_ssp_capability = {
  bLength                 : USB_DT_USB_SSP_CAP_SIZE(1),
  bDescriptorType         : ZUSB_DT_DEVICE_CAPABILITY,
  bDevCapabilityType      : ZUSB_SSP_CAP_TYPE,
  bReserved               : 0x00,
  bmAttributes            : 0x01,
  wFunctionalitySupport   : (1 << 8) | (1 << 12),
  wReserved               : 0x0000,
  bmSublinkSpeedAttr      : {((3 << 4) | (1 << 14) | (0xa << 16)), ((3 << 4) | (1 << 14) | (0xa << 16) | (1 << 7))}
};

///////////////////////////////////////////////////////////////////////////////
//                      INTERFACES DESCRIPTION                               //
// This device is composed of one inferface with an alternate settings       //
///////////////////////////////////////////////////////////////////////////////

static const struct zusb_InterfaceDescriptor
z_itf0_desc = {
bLength :    ZUSB_DT_INTERFACE_SIZE,
  bDescriptorType :  ZUSB_DT_INTERFACE,
  bInterfaceNumber : 0,
  bAlternateSetting : 0,
  bNumEndpoints : 2,
  bInterfaceClass : ZUSB_CLASS_VENDOR_SPEC,
  bInterfaceSubClass : 0x1,
  bInterfaceProtocol : 0,
  iInterface : 0,
};

///////////////////////////////////////////////////////////////////////////////
//                  ENDPOINTS DESCRIPTION ALT SETTING 0                      //
///////////////////////////////////////////////////////////////////////////////

static const struct zusb_EndpointDescriptor 
z_interrupt_in = {
bLength : 0x07,
  bDescriptorType : ZUSB_DT_ENDPOINT,
  bEndpointAddress : 0x81,
  bmAttributes : 0x03,
#ifdef HS	
  wMaxPacketSize : 0x0040,
#else
#ifdef FS	
  wMaxPacketSize : 0x0040,
#else	
  wMaxPacketSize : 0x0200,
#endif	
#endif	
  bInterval : 0x01
};

#if defined(SS) || defined(SSP)
static const struct zusb_SSEndpointCompDescriptor
z_interrupt_in_ep_comp = {
  bLength         : ZUSB_DT_SS_EP_COMP_SIZE,
  bDescriptorType : ZUSB_DT_SS_ENDPOINT_COMP,
  bMaxBurst       : 0x00,
  bmAttributes    : 0x0,
  wBytesPerInterval : 0x100
 };
#endif

static const struct zusb_EndpointDescriptor 
z_interrupt_out = {
bLength : 0x07,
  bDescriptorType : ZUSB_DT_ENDPOINT,
  bEndpointAddress : 0x02,
  bmAttributes : 0x03,
#ifdef HS	
  wMaxPacketSize : 0x0040,
#else
#ifdef FS	
  wMaxPacketSize : 0x0040,
#else	
  wMaxPacketSize : 0x0400,
#endif	
#endif	
  bInterval : 0x01
};

#if defined(SS) || defined(SSP)
static const struct zusb_SSEndpointCompDescriptor
z_interrupt_out_ep_comp = {
  bLength         : ZUSB_DT_SS_EP_COMP_SIZE,
  bDescriptorType : ZUSB_DT_SS_ENDPOINT_COMP,
  bMaxBurst       : 0x00,
  bmAttributes    : 0x0,
  wBytesPerInterval : 0x100
 };
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static const struct zusb_DescriptorHeader *z_function [] = {
  (struct zusb_DescriptorHeader *) &z_itf0_desc,
  (struct zusb_DescriptorHeader *) &z_interrupt_in,
#if defined(SS) || defined(SSP)
  (struct zusb_DescriptorHeader *) &z_interrupt_in_ep_comp,
#endif	
  (struct zusb_DescriptorHeader *) &z_interrupt_out,
#if defined(SS) || defined(SSP)
  (struct zusb_DescriptorHeader *) &z_interrupt_out_ep_comp,
#endif	
  NULL,
};

#endif
