// -----------------------------------------------
// usb_dev_access.hh -- Enpoints accesses functions
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

#ifndef __DEMO_USB_ACCESS_H
#define __DEMO_USB_ACCESS_H

using namespace std;
using namespace ZEBU;
using namespace ZEBU_IP;
using namespace USB3X;
       
#define RBUF_LEN 32768
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                Structure representing the device                          //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct example_device_description {
  // Control endpoint structures
  ZebuRequest* ctrl_req;         /* for control responses */
  DeviceEP*    ctrl_ep;         /* Controle EP0 */

  // Other endpoints structures
  DeviceEP*    in_ep;       /* IN INT EP */
  DeviceEP*    out_ep;      /* OUT INT EP */
  ZebuRequest* in_req;      /* IN INT REQUEST */
  ZebuRequest* out_req;     /* OUT INT REQUEST */

  // Pointer to the USB transactor
  Usb3XDRD*      xtor;        /* Pointer to xtor */
};

// Instantiation of the device
extern example_device_description usb_device_example;

extern const int g_length;
extern int numTransfer;

extern int config_buf (uint8_t *buf, uint8_t type, unsigned index);

extern void ctrl_complete(DeviceEP* ep, ZebuRequest* req);
extern void int_in_complete (DeviceEP*  ep, ZebuRequest* req);
extern void int_out_complete (DeviceEP*  ep, ZebuRequest* req);

extern ZebuRequest* start_int_in_ep (DeviceEP* ep);
extern ZebuRequest* start_int_out_ep (DeviceEP* ep);

extern int my_dev_setconfig (struct example_device_description* dev, uint32_t num, uint32_t altnum);
extern int32_t my_dev_setup_cb ( ZebuRequest* req, zusb_CtrlRequest* ctrl, void* context);

#endif
