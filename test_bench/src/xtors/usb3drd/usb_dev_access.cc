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
// usb_dev_access.cc -- Enpoints accesses functions
//                  for USB URB API example

#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "Usb3X.hh"
#include "usbstring.hh"
#include "usb_dev_descriptors.hh"
#include "usb_dev_access.hh"

using namespace std;
using namespace ZEBU;
using namespace ZEBU_IP;
using namespace USB3X;
       
#define ZUSB_BUFSIZ  512

#if defined(SS) || defined(SSP)
const int g_length = 512;
#else
const int g_length = 64;
#endif
int numTransfer = 0;

static int int_xfer_len;
static char rxbuf[512];

// Instantiation of the device description
example_device_description usb_device_example;

int config_buf (uint8_t *buf, uint8_t type, unsigned index)
{
  int len;
  const struct zusb_DescriptorHeader **function;
  
  function = z_function;
  len = usb_gadget_config_buf (&z_config, buf, ZUSB_BUFSIZ, function);
  if (len < 0)
    return len;
  ((struct zusb_ConfigDescriptor *) buf)->bDescriptorType = type;
  return len;
}



void 
int_in_complete (DeviceEP*  ep,
                  ZebuRequest* req)
{
  struct example_device_description  *dev = (struct example_device_description*)ep->GetPrivateData();
  int    status = req->status;
  int i, j;
  uint32_t len = g_length;

  if(status == 0) {       /* normal completion? */
    req->length = len;
    //printf("\n ---------------> intin %d writing %d bytes\n", numTransfer, req->length);
    ((uint8_t*)req->buf)[0] = numTransfer;
    //printf("%c", ((uint8_t*)req->buf)[0]);
    ++numTransfer;
    printf("\n\n");
  }
  else {
    if(status == ZUSB_TRANSFER_CANCELLED) {
      printf("Request cancelled for endpoint number %u, ignoring data\n", ep->GetNumber());
      return;
    }
    else {
      printf("Endpoint number %u stopped, status %d unexpected, exit\n", ep->GetNumber(), status);
      exit(1);
    }
  }

  status = ep->QueueEP(req);
  if (status) {
    exit(1);
  }
}

void 
int_out_complete(DeviceEP*  ep,
                       ZebuRequest* req)
{
  struct example_device_description  *dev = (struct example_device_description*)ep->GetPrivateData();
  int    status = req->status;
  int i, j;

  if(status == 0) {       /* normal completion? */
    if(req->buf[0] >= 30) {
      // End of int out transfers
      printf("\n----------> end int out detected, starting in int sequence\n");
      numTransfer = 0;
      if (start_int_in_ep (dev->in_ep) != 0) {
        return;
      }
    }
    else {
      printf("\n ---------------> intout %d reading %d bytes\n", numTransfer, req->actual);
      //printf("value = %u", ((uint8_t*)req->buf)[0]);
      int_xfer_len = req->actual;
      ++numTransfer;
      for(i = 0; i < req->actual; i++) {
        printf("%x ", req->buf[i]);
	rxbuf[i] = req->buf[i];
      }
      printf("\n");
      printf ("start_int_in_ep. \n"); fflush(stdout);
      start_int_in_ep (dev->in_ep);
      printf ("start_int_in_ep done. \n"); fflush(stdout);
#ifdef DEBUG
      printf("\n\n");
#endif
    }
  }
  else {
    if(status == ZUSB_TRANSFER_CANCELLED) {
      printf("Request cancelled for endpoint number %u, ignoring data\n", ep->GetNumber());
      return;
    }
    else {
      printf("Endpoint number %u stopped, status %d unexpected, exit\n", ep->GetNumber(), status);
      exit(1);
    }
  }

  status = ep->QueueEP(req);
  if (status) {
    exit(1);
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    REQUESTS CREATION FUNCTIONS                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    REQUESTS CREATION FUNCTIONS                            //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


ZebuRequest*
start_int_in_ep (DeviceEP* ep)
{
  int      status;
  int i,j;
  uint32_t len = int_xfer_len;

  usb_device_example.in_req = ep->AllocRequest(len);
  if (!usb_device_example.in_req)
    return NULL;

  for( int i = 0; i < len; ++i) {
    //usb_device_example.in_req->buf[i] = 'B';
    usb_device_example.in_req->buf[i] = rxbuf[i];
#ifdef DEBUG 
  printf("%c", ((uint8_t*)usb_device_example.in_req->buf)[i]);
#endif
  }

  usb_device_example.in_req->complete = int_in_complete;

  usb_device_example.in_req->length = len;
  printf("\n ---------------> intin %d writing %d bytes\n", numTransfer, usb_device_example.in_req->length);
  //((uint8_t*)usb_device_example.in_req->buf)[0] = numTransfer;
  ++numTransfer;
  printf("\n\n");

  status = ep->QueueEP(usb_device_example.in_req);
  if (status) {
    exit(1);
  }

  return usb_device_example.in_req;
}

ZebuRequest*
start_int_out_ep (DeviceEP* ep)
{
  int      status;
  int i;

  usb_device_example.out_req = ep->AllocRequest(g_length);
  if (!usb_device_example.out_req)
    return NULL;

  usb_device_example.out_req->complete = int_out_complete;

  status = ep->QueueEP(usb_device_example.out_req);
  if (status) {
    exit(1);
  }

  return usb_device_example.out_req;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Endpoints Initialization                              //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int my_dev_setconfig (struct example_device_description* dev, uint32_t num, uint32_t altnum)
{
  int result = 0;
  const struct zusb_EndpointDescriptor  *d;

// Disable all current installed EP
  if(dev->in_ep) {
    dev->in_ep->DisableEP();
    dev->in_ep = 0;
  }
  if(dev->out_ep) {
    dev->out_ep->DisableEP();
    dev->out_ep = 0;
  }

  d = &z_interrupt_in; 
//  dev->in_ep = dev->xtor->getEndpoint((d->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK);
  dev->in_ep = dev->xtor->getEndpoint((d->bEndpointAddress)/* & ZUSB_ENDPOINT_NUMBER_MASK*/);
  result = dev->in_ep->EnableEP (d);
  if (result != 0) {
    dev->in_ep->DisableEP();
    exit(1);
  }
  dev->in_ep->SetPrivateData(dev);

  d = &z_interrupt_out; 
//  dev->out_ep = dev->xtor->getEndpoint((d->bEndpointAddress) & ZUSB_ENDPOINT_NUMBER_MASK);
  dev->out_ep = dev->xtor->getEndpoint((d->bEndpointAddress)/* & ZUSB_ENDPOINT_NUMBER_MASK*/);
  result = dev->out_ep->EnableEP (d);
  if (result != 0) {
    dev->out_ep->DisableEP();
    exit(1);
  }
  dev->out_ep->SetPrivateData(dev);
  printf ("start_int_out_ep. \n"); fflush(stdout);
  start_int_out_ep(dev->out_ep);
  printf ("start_int_out_ep done. \n"); fflush(stdout);
  
  return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     CONTROL REQUESTS CALLBACK                             //
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// This function implements all control setup action that are not handled
// by the Peripheral driver inside the transactor
int32_t my_dev_setup_cb ( ZebuRequest* req,
                  zusb_CtrlRequest* ctrl,
                  void*                context)
{
  struct example_device_description    *dev = (struct example_device_description*)context;
  int     value = -1;

  /* usually this stores reply data in the pre-allocated ctrl_ep buffer,
   * but config change events will reconfigure hardware.
   */
  req->zero = 0;
  switch (ctrl->bRequest) {

  case ZUSB_REQ_GET_DESCRIPTOR:

    switch (ctrl->wValue >> 8) {
    case ZUSB_DT_DEVICE:
      value = min (ctrl->wLength, (uint16_t) sizeof device_desc);
      memcpy (req->buf, &device_desc, value);
      break;
    case ZUSB_DT_DEVICE_QUALIFIER:
      value = min (ctrl->wLength, (uint16_t) sizeof dev_qualifier);
      memcpy (req->buf, &dev_qualifier, value);
      break;
    case ZUSB_DT_CONFIG:
      value = config_buf ((uint8_t*)req->buf,
          ctrl->wValue >> 8,
          ctrl->wValue & 0xff);
      if (value >= 0)
        value = min (ctrl->wLength, (uint16_t) value);
      break;
    case ZUSB_DT_BOS:
      value = ctrl->wLength;
      memcpy (req->buf, &BOS_desc, (uint16_t) sizeof BOS_desc );
      if ( ctrl->wLength == BOS_desc.wTotalLength) {
        memcpy (req->buf + (uint16_t) sizeof BOS_desc, &dev_usb2_capability,  (uint16_t) sizeof dev_usb2_capability);
        memcpy (req->buf + (uint16_t) sizeof BOS_desc + (uint16_t) sizeof dev_usb2_capability, &dev_ss_capability,  (uint16_t) sizeof dev_ss_capability);
        memcpy (req->buf + (uint16_t) sizeof BOS_desc +(uint16_t) sizeof dev_usb2_capability + (uint16_t) sizeof dev_ss_capability, &dev_ssp_capability, (uint16_t) sizeof dev_ssp_capability);
      }

      break;

    case ZUSB_DT_STRING:
      value = usb_gadget_get_string (&stringtab,
          ctrl->wValue & 0xff, (uint8_t*)req->buf);
      if (value >= 0) {
        value = min (ctrl->wLength, (uint16_t) value);
      }
      break;
    }
    break;

  case ZUSB_REQ_SET_CONFIGURATION:
    if (ctrl->bRequestType != 0)
      goto unknown;

    value = my_dev_setconfig (dev, ctrl->wValue, 0);
    break;
  case ZUSB_REQ_GET_CONFIGURATION:
    if (ctrl->bRequestType != ZUSB_DIR_IN)
      goto unknown;
    *(uint8_t *)req->buf = 0;
    value = min (ctrl->wLength, (uint16_t) 1);
    break;

  case ZUSB_REQ_SET_INTERFACE:

    if (ctrl->bRequestType != ZUSB_RECIP_INTERFACE)
      goto unknown;
      my_dev_setconfig (dev, 0, ctrl->wValue);
      value = 0;
    break;
  case ZUSB_REQ_GET_INTERFACE:
    if ((ctrl->bRequestType == 0x21) && (ctrl->wIndex == 0x02)) {
      value = ctrl->wLength;
      break;
    }
    else {
      if (ctrl->bRequestType != (ZUSB_DIR_IN|ZUSB_RECIP_INTERFACE))
        goto unknown;
      if (ctrl->wIndex != 0) {
        value = -1;
        break;
      }
      *(uint8_t *)req->buf = 0;
      value = min (ctrl->wLength, (uint16_t) 1);
    }
    break;

  default:
unknown:
    printf( "  Unknown Setup pkt: ReqType: 0x%02x\n",ctrl->bRequestType);
    printf( "                Req: 0x%02x\n", ctrl->bRequest);
    printf( "              Value: 0x%04x\n",ctrl->wValue);
    printf( "              Index: 0x%04x\n",ctrl->wIndex);
    printf( "             Length: 0x%04x\n",ctrl->wLength);
  }

  /* respond with data transfer before status phase */
  if (value >= 0) {
    req->length = value;
    req->zero = value < ctrl->wLength
        && (value % dev->ctrl_ep->GetMaxPacket()) == 0;
    value = dev->ctrl_ep->QueueEP(req);
    if (value < 0) {
      printf("QueueEP error status %d\n", value);
      req->status = 0;
    }
  }

  /* device either stalls (value < 0) or reports success */
  return value;
}


void ctrl_complete(DeviceEP*  ep,
             ZebuRequest* req)
{
  if (req->status || req->actual != req->length) {
     printf("Control Setup complete status %d\n", req->status);
  }
}
