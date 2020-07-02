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

#include "Usb3XUrbCommon.hh"
#include "usbstring.hh"

using namespace std;

static int utf8_to_utf16le(const char *s, uint16_t *cp, unsigned len)
{
  int  count = 0;
  uint8_t  c;
  uint16_t  uchar;

  /* this insists on correct encodings, though not minimal ones.
   * BUT it currently rejects legit 4-byte UTF-8 code points,
   * which need surrogate pairs.  (Unicode 3.1 can use them.)
   */
  while (len != 0 && (c = (uint8_t) *s++) != 0) {
    if ((c & 0x80) != 0) {
      // 2-byte sequence:
      // 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
      if ((c & 0xe0) == 0xc0) {
        uchar = (c & 0x1f) << 6;

        c = (uint8_t) *s++;
        if ((c & 0xc0) != 0xc0)
          goto fail;
        c &= 0x3f;
        uchar |= c;

      // 3-byte sequence (most CJKV characters):
      // zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
      } else if ((c & 0xf0) == 0xe0) {
        uchar = (c & 0x0f) << 12;

        c = (uint8_t) *s++;
        if ((c & 0xc0) != 0xc0)
          goto fail;
        c &= 0x3f;
        uchar |= c << 6;

        c = (uint8_t) *s++;
        if ((c & 0xc0) != 0xc0)
          goto fail;
        c &= 0x3f;
        uchar |= c;

        /* no bogus surrogates */
        if (0xd800 <= uchar && uchar <= 0xdfff)
          goto fail;

      // 4-byte sequence (surrogate pairs, currently rare):
      // 11101110wwwwzzzzyy + 110111yyyyxxxxxx
      //     = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
      // (uuuuu = wwww + 1)
      // FIXME accept the surrogate code points (only)

      } else
        goto fail;
    } else
      uchar = c;
    *(cp++) = uchar;
    count++;
    len--;
  }
  return count;
fail:
  return -1;
}

/**
 * usb_gadget_get_string - fill out a string descriptor 
 */
int
usb_gadget_get_string (struct zusb_GadgetStrings *table, int id, uint8_t *buf)
{
  struct zusb_String  *s;
  int      len;

  /* descriptor 0 has the language id */
  if (id == 0) {
    buf [0] = 4;
    buf [1] = ZUSB_DT_STRING;
    buf [2] = (uint8_t) table->language;
    buf [3] = (uint8_t) (table->language >> 8);
    return 4;
  }
  for (s = table->strings; s && s->s; s++)
    if (s->id == id)
      break;

  /* unrecognized: stall. */
  if (!s || !s->s)
    return -1;

  /* string descriptors have length, tag, then UTF16-LE text */
  len = min ((size_t) 126, strlen (s->s));
  memset (buf + 2, 0, 2 * len);  /* zero all the bytes */
  len = utf8_to_utf16le(s->s, (uint16_t *)&buf[2], len);
  if (len < 0)
    return -1;
  buf [0] = (len + 1) * 2;
  buf [1] = ZUSB_DT_STRING;
  return buf [0];
}



/**
 * usb_descriptor_fillbuf - fill buffer with descriptors
 */
int
usb_descriptor_fillbuf(void *buf, unsigned buflen,
    const struct zusb_DescriptorHeader **src)
{
  uint8_t  *dest = (uint8_t*)buf;

  if (!src)
    return -1;

  /* fill buffer from src[] until null descriptor ptr */
  for (; 0 != *src; src++) {
    unsigned    len = (*src)->bLength;

    if (len > buflen)
      return -1;
    memcpy(dest, *src, len);
    buflen -= len;
    dest += len;
  }
  return dest - (uint8_t *)buf;
}


/**
 * usb_gadget_config_buf - builts a complete configuration descriptor
 */
int usb_gadget_config_buf(
  const struct zusb_ConfigDescriptor  *config,
  void          *buf,
  unsigned        length,
  const struct zusb_DescriptorHeader  **desc
)
{
  struct zusb_ConfigDescriptor    *cp = (zusb_ConfigDescriptor*)buf;
  int          len;

  /* config descriptor first */
  if (length < ZUSB_DT_CONFIG_SIZE || !desc)
    return -1;
  *cp = *config; 

  /* then interface/endpoint/class/vendor/... */
  len = usb_descriptor_fillbuf(ZUSB_DT_CONFIG_SIZE + (uint8_t*)buf,
      length - ZUSB_DT_CONFIG_SIZE, desc);
  if (len < 0)
    return len;
  len += ZUSB_DT_CONFIG_SIZE;
  if (len > 0xffff)
    return -1;

  /* patch up the config descriptor */
  cp->bLength = ZUSB_DT_CONFIG_SIZE;
  cp->bDescriptorType = ZUSB_DT_CONFIG;
  cp->wTotalLength = len;
  cp->bmAttributes |= ZUSB_CONFIG_ATT_ONE;
  return len;
}
