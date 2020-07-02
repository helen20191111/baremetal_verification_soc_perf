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

// USB gadget definitions
#ifndef __DEMO_USB_GADGET_STRING_H
#define __DEMO_USB_GADGET_STRING_H

using namespace ZEBU_IP;
using namespace USB3X;

extern int usb_gadget_get_string (struct zusb_GadgetStrings *table, int id, uint8_t *buf);
extern int usb_gadget_config_buf(
  const struct zusb_ConfigDescriptor  *config,
  void          *buf,
  unsigned        length,
  const struct zusb_DescriptorHeader  **desc
);
extern int usb_descriptor_fillbuf(void *buf, unsigned buflen,
    const struct zusb_DescriptorHeader **src);
#endif
