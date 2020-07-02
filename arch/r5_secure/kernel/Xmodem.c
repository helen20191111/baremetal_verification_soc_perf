/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <Xmodem.h>
#include <Common.h>
#include <utility_lite.h>
#include <printf_inc.h>

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CTRLZ 0x1A
#define DLY_1S SOFT_LOOP_1S
#define MAXRETRANS 25

static int last_error = 0;

void port_outbyte(unsigned char trychar)
{
	putc(trychar);
}

unsigned char port_inbyte(unsigned int time_out)
{
	unsigned char ch;
	last_error = 0;
	if (getc_nb(&ch, time_out) == 0)
		return ch;
	last_error = 1;
	return ch;
}

static int check(int crc, const unsigned char *buf, int sz)
{
	//not use crc now
	/*
	if (crc){
        unsigned short crc = crc16_ccitt(buf, sz);
        unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];

        if (crc == tcrc)
            return 1;
	}else{
	*/
	int i;
	unsigned char cks = 0;

	for (i = 0; i < sz; ++i) {
		cks += buf[i];
	}

	if (cks == buf[sz])
		return 1;
	//}

	return 0;
}

static void flushinput(void)
{
	//while (port_inbyte(((DLY_1S)*3)>>1) >= 0)
	//;
}

int xmodemReceive(unsigned char *dest, int destsz)
{
	unsigned char xbuff[1030];

	unsigned char *p;
	int bufsz, crc = 0;
	//unsigned char trychar = 'C';
	unsigned char trychar = NAK;

	unsigned char packetno = 1;
	int i, c, len = 0;
	int retry, retrans = MAXRETRANS;

	last_error = 0;

	for (;;) {
		for (retry = 0; retry < 60; ++retry) {
			if (trychar)
				port_outbyte(trychar);

			c = port_inbyte((DLY_1S)*3);

			if (last_error == 0) {
				switch (c) {
				case SOH:
					bufsz = 128;
					goto start_recv;

				case STX:
					bufsz = 1024;
					goto start_recv;

				case EOT:
					flushinput();
					port_outbyte(ACK);
					return len;

				case CAN:
					c = port_inbyte(DLY_1S);
					if (c == CAN) {
						flushinput();
						port_outbyte(ACK);
						return -1;
					}
					break;

				default:
					break;
				}
			}
		}

		if (trychar == 'C') {
			trychar = NAK;
			continue;
		}

		flushinput();
		port_outbyte(CAN);
		port_outbyte(CAN);
		port_outbyte(CAN);
		return -2;

	start_recv:
		//if (trychar == 'C') crc = 1;
		trychar = 0;

		p = xbuff;
		*p++ = c;

		for (i = 0; i < (bufsz + (crc ? 1 : 0) + 3); ++i) {
			c = port_inbyte(DLY_1S);
			if (last_error != 0)
				goto reject;
			*p++ = c;
		}

		if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
		    (xbuff[1] == packetno ||
		     xbuff[1] == (unsigned char)packetno - 1) &&
		    check(crc, &xbuff[3], bufsz)) {
			if (xbuff[1] == packetno) {
				int count = destsz - len;
				if (count > bufsz)
					count = bufsz;

				if (count > 0) {
					memCpy8(&dest[len], &xbuff[3], count);
					len += count;
				}

				++packetno;
				retrans = MAXRETRANS + 1;
			}

			if (--retrans <= 0) {
				flushinput();
				port_outbyte(CAN);
				port_outbyte(CAN);
				port_outbyte(CAN);
				return -3;
			}

			port_outbyte(ACK);
			continue;
		}

	reject:

		flushinput();

		port_outbyte(NAK);
	}
}

/*
 binary send
 */
int bstdef_Receive(unsigned char *dest, int destsz)
{
	unsigned int idx;
	unsigned char ch;
	unsigned char exitMagic[9] = { 'b', 's', 't', 'd', 'o',
				       'w', 'n', 'o', 'k' };
	unsigned int magicChkIdx;

	idx = 0;
	magicChkIdx = 0;
	while (destsz--) {
		//if(getc_nb(&ch, 100000)!= RET_OK)
		//return ERR_N1;
		ch = getc();
		dest[idx++] = ch;

		if (idx % 1024 == 0) {
			putc('K');
		}

		if (ch == exitMagic[magicChkIdx]) {
			magicChkIdx++;
		} else {
			magicChkIdx = 0;
		}

		if (magicChkIdx >= 9) {
			break;
		}
	}

	return idx;
}
