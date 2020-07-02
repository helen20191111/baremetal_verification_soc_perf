/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _UTILITY_LITE_H
#define _UTILITY_LITE_H

typedef void (*F_postFn)(void);

typedef void (*F_cmdFn)(int para);

typedef struct cmd_struct {
	F_cmdFn cmdFn;
	int para;
	unsigned long attr;
	const char *cmdName;
	const char *description;
} T_Cmd_list;

#define CMD_ATTR_AUTO_TEST 0X1

#define CMD_DEFINE(__cmdName, __cmdFn, __para, __description, __attr)          \
	__attribute__((section(".section_test_cmd_table"))) struct cmd_struct  \
		_section_item_##__cmdName##_cmd = {                            \
			.cmdFn = __cmdFn,                                      \
			.para = __para,                                        \
			.attr = __attr,                                        \
			.cmdName = #__cmdName,                                 \
			.description = __description,                          \
		}

/*soft delay*/
void delaySoft(unsigned long loopcnt);

/*dump mem info by input*/
void dumpMem(int para);

/*set mem by 32 bit*/
void setMem32(int para);

/*show all cmd support*/
void helpCmd(int para);

/*get string(cmd)*/
int getString(char *cmd_buf, int len);

/*get a hex value*/
int getHex(unsigned long *value);

/*
4 bit hex to char
*/
int putHex(unsigned int value);

/*
char buffer, 4 bit hex to char
*/
void putStr(unsigned char *ch, unsigned int length);

/*
memset by byte
*/
void memSet8(void *buf, unsigned char val, unsigned int length);

/*
set register32 from buffer
*/
void setReg32FromBuf(unsigned int dstReg, unsigned int *buf,
		     unsigned int length);

int memCmp8(void *buf1, void *buf2, unsigned int length);

/*
memory  compare,return err cnt
*/
void *memCpy8(void *srcBuf, void *dstBuf, unsigned int length);

unsigned int *memCpy32(unsigned int *dest, unsigned int *src,
		       unsigned int count);

/*
dump value by word(int)
*/
void dumpIntVal(unsigned int *pBuf, unsigned int len32);

/*
change hex string to char
*/
int str2ValStartTail(char *str, unsigned int *val, int valLength8);

int str2ValStartHead(char *str, unsigned int *val, int valLength8);

/*run Cmd we support*/
int runCmd(const char *cmdBuf);

#define BigtoLittle32(A)                                                       \
	((((unsigned int)(A)&0xff000000) >> 24) |                              \
	 (((unsigned int)(A)&0x00ff0000) >> 8) |                               \
	 (((unsigned int)(A)&0x0000ff00) << 8) |                               \
	 (((unsigned int)(A)&0x000000ff) << 24))

/*
buffer bit to lit by word (int)
*/
void Buffer_Big2Lit32(unsigned int *pBuf, unsigned int len32);
void cmd_SetPostCb(F_postFn fCb);
void cmd_main(void);

#endif /* _UTILITY_LITE_H */
