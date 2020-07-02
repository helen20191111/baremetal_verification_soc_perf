/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <linux/string.h>
#include <linux/io.h>
#include <utility_lite.h>
#include <printf_inc.h>
#include <printf.h>
#include <RegTest.h>

T_Cmd_list *glb_cmd_list = NULL;
int glb_cmd_cnt = 0;
static int is_quit_flag = 0;
extern unsigned long __section_test_cnd_table_start;
extern unsigned long __section_test_cnd_table_end;

int str2hex(const char *str, unsigned long *value)
{
	unsigned long temp = 0;

	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
	}

	while (*str != '\0') {
		if ((*str >= '0') && (*str <= '9')) {
			temp = temp * 16 + (*str - '0');
			str++;
		} else if ((*str >= 'a') && (*str <= 'f')) {
			temp = temp * 16 + (*str - 'a' + 10);
			str++;
		} else if ((*str >= 'A') && (*str <= 'F')) {
			temp = temp * 16 + (*str - 'A' + 10);
			str++;
		} else {
			putc('e');
			putc(*str);
			return -1;
		}
	}

	*value = temp;
	return 0;
}

void delaySoft(unsigned long loopcnt)
{
	while (loopcnt-- > 0) {
		nop();
	}
}

int getHex(unsigned long *value)
{
	char cBuf[19];

	getString(cBuf, 19);
	if (str2hex(cBuf, value)) {
		hprintf(TSENV, "wrong input\r\n");
		return -1;
	}

	return 0;
}

void dumpMem(int para)
{
	unsigned long addr, size;

	unsigned int i;

	hprintf(TSENV, "\r\ninput addr(hex)\r\n");
	if (getHex(&addr)) {
		return;
	}

	hprintf(TSENV, "\r\ninput dump size(hex)\r\n");
	if (getHex(&size)) {
		size = 4;
	}

	addr &= ~0x3;
	for (i = 0; i < size; i += 4) {
		if (!(i & 0xf)) {
			hprintf(TSENV, "\n\rADDR 0x%08x: ", addr + i);
		}
		hprintf(TSENV, "0x%08x ", REG32_READ(addr + i));
	}
	hprintf(TSENV, "\n");
}

void setMem32(int para)
{
	unsigned long addr;
	unsigned long value;

	hprintf(TSENV, "input addr(hex)\r\n");
	if (getHex(&addr)) {
		return;
	}

	//while(1) {
	hprintf(TSENV, "0x%lx : ? (hex)", addr);
	if (getHex(&value)) {
		return;
	}
	hprintf(TSENV, "\r\n");
	REG32_WRITE(addr, value);
	/*    
		addr = addr + 4;
	}
    */
	hprintf(TSENV, "addr 0x%x = 0x%x\r\n", addr, REG32_READ(addr));
}

void setReg32FromBuf(unsigned int dstReg, unsigned int *buf,
		     unsigned int length)
{
	int i = 0;

	for (i = 0; i < length; i++) {
		REG32_WRITE(dstReg + i * 4, buf[i]);
	}
}

extern T_Cmd_list CmdList[];

void helpCmd(int para)
{
	int i = 0;
	T_Cmd_list *pCmdList = (T_Cmd_list *)glb_cmd_list;

	hprintf(TSENV, "\r\n");

	for (i = 0; i < glb_cmd_cnt; i++) {
		//	while(pCmdList[i].cmdFn != NULL){
		hprintf(TSENV, pCmdList[i].cmdName);
		hprintf(TSENV, ".........");
		hprintf(TSENV, pCmdList[i].description);
		hprintf(TSENV, "\r\n");
	}
}

void quitCmd(int para)
{
	if (!is_quit_flag)
		is_quit_flag = 1;
}

/*get string(cmd)*/
int getString(char *cmd_buf, int len)
{
	int i = 0;
	unsigned char c;

	while (1) {
		if (i > len) {
			i = len;
			break;
		}

		c = getc();
		if ((c == 0x0d) || (c == 0x0a)) {
			cmd_buf[i] = '\0';
			break;
		} else if (c == '\b') {
			if (i > 0)
				i--;

			putc(c);
			putc(0x7f);
		} else {
			putc(c);
			cmd_buf[i++] = c;
		}
	}

	return i;
}

int runCmd(const char *cmdBuf)
{
	const char *pCmd;
	int i = 0;
	T_Cmd_list *pCmdList = (T_Cmd_list *)glb_cmd_list;

	for (i = 0; i < glb_cmd_cnt; i++) {
		//	while(cmdList[i].cmdFn != NULL){
		pCmd = pCmdList[i].cmdName;
		if (!strcmp(pCmd, cmdBuf)) {
			pCmdList[i].cmdFn(pCmdList[i].para);
			return 0;
		}
	}
	hprintf(TSENV, "%s is wrong command\r\n", cmdBuf);

	return i;
}

unsigned char ascii2Byte(char High4bit, char Low4bit)
{
	unsigned char value = 0;

	if ((High4bit >= '0') && (High4bit <= '9')) {
		value = (High4bit - '0') << 4;
	} else if ((High4bit >= 'a') && (High4bit <= 'f')) {
		value = (High4bit - 'a' + 10) << 4;
	} else if ((High4bit >= 'A') && (High4bit <= 'F')) {
		value = (High4bit - 'A' + 10) << 4;
	}

	if ((Low4bit >= '0') && (Low4bit <= '9')) {
		value += (Low4bit - '0');
	} else if ((Low4bit >= 'a') && (Low4bit <= 'f')) {
		value += (Low4bit - 'a' + 10);
	} else if ((Low4bit >= 'A') && (Low4bit <= 'F')) {
		value += (Low4bit - 'A' + 10);
	}

	return value;
}

int str2ValStartTail(char *str, unsigned int *val, int valLength8)
{
	unsigned int length;
	unsigned char *pVal;
	char *pStr, asiicLow, asiicHigh;

	length = strlen(str);
	if (length > 2 * valLength8) {
		hprintf(DEBUG, "\r\nstring %d too long, error", length);
		return -1;
	}
	pStr = str + length - 1;
	pVal = (unsigned char *)val;
	do {
		asiicLow = *pStr--;

		if (pStr >= str)
			asiicHigh = *pStr--;
		else
			asiicHigh = '0';

		*pVal = ascii2Byte(asiicHigh, asiicLow);
		pVal++;
	} while (pStr >= str);
	return 0;
}

int str2ValStartHead(char *str, unsigned int *val, int valLength8)
{
	unsigned int length;
	unsigned char *pVal;
	char *pStr, asiicLow, asiicHigh;

	length = strlen(str);
	if (length > 2 * valLength8) {
		hprintf(DEBUG, "\r\nstring is too long, error");
		return -1;
	}
	if (length % 2) {
		hprintf(DEBUG, "\r\nlength is odd, error, must even");
		return -1;
	}
	pStr = str + length - 1;
	pVal = (unsigned char *)val;
	do {
		asiicHigh = *str++;
		asiicLow = *str++;

		*pVal = ascii2Byte(asiicHigh, asiicLow);
		pVal++;
	} while (pStr >= str);
	return 0;
}

/*
4 bit hex to char
*/
char hex2Char(unsigned int val)
{
	if ((val >= 0) && (val <= 9))
		return val + 0x30;
	else if ((val >= 10) && (val <= 15))
		return val - 10 + 'A';

	return 0;
}

int putHex(unsigned int value)
{
	putc('\r');
	putc('\n');

	putc(hex2Char((value >> 4) & 0xf));
	putc(hex2Char((value >> 0) & 0xf));
	putc(hex2Char((value >> 12) & 0xf));
	putc(hex2Char((value >> 8) & 0xf));
	putc(hex2Char((value >> 20) & 0xf));
	putc(hex2Char((value >> 16) & 0xf));
	putc(hex2Char((value >> 28) & 0xf));
	putc(hex2Char((value >> 24) & 0xf));

	return 0;
}

void putStr(unsigned char *ch, unsigned int length)
{
	int i = 0;

	for (i = 0; i < length; i++) {
		if (i % 4 == 0) {
			hprintf(DEBUG, "\r\n");
		}
		putc(hex2Char((ch[i] >> 4) & 0xf));
		putc(hex2Char((ch[i] >> 0) & 0xf));
	}
}

/*
memset by byte
*/
void memSet8(void *buf, unsigned char val, unsigned int length)
{
	unsigned char *p = (unsigned char *)buf;

	int i = 0;
	for (i = 0; i < length; i++) {
		p[i] = val;
	}
}

/*
memory  compare,return err cnt
*/
int memCmp8(void *buf1, void *buf2, unsigned int length)
{
	int i = 0, ret = 0;
	unsigned char *p1 = (unsigned char *)buf1;
	unsigned char *p2 = (unsigned char *)buf2;

	for (i = 0; i < length; i++) {
		if (p1[i] != p2[i]) {
			ret++;
		}
	}

	return ret;
}

/*
memory  copy, care ! not check length ,return dst addr
*/
void *memCpy8(void *dstBuf, void *srcBuf, unsigned int length)
{
	int i = 0;
	unsigned char *src = (unsigned char *)srcBuf;
	unsigned char *dst = (unsigned char *)dstBuf;

	for (i = 0; i < length; i++) {
		dst[i] = src[i];
	}

	return dstBuf;
}

unsigned int *memCpy32(unsigned int *dest, unsigned int *src,
		       unsigned int count)
{
	//if (NULL == dest || NULL == src || count <= 0)
	//	return NULL;

	while (count--)
		*(unsigned int *)dest++ = *(unsigned int *)src++;

	return dest;
}

/*
dump value by word(int)
*/
void dumpIntVal(unsigned int *pBuf, unsigned int len32)
{
	unsigned int i;

	for (i = 0; i < len32; i++) {
		if (i % 4 == 0) {
			hprintf(DEBUG, "\r\n0x%08x->", i * 4);
		}
		hprintf(DEBUG, "\t0x%08x", pBuf[i]);
	}
}

/*
buffer bit to lit by word (int)
*/
void Buffer_Big2Lit32(unsigned int *pBuf, unsigned int len32)
{
	unsigned int i;

	//putStr(pTmp,tAesObj.msgLen);
	for (i = 0; i < len32; i++) {
		pBuf[i] = BigtoLittle32(pBuf[i]);
	}
}

void auto_test_cmd(int para)
{
	int i = 0;

	T_Cmd_list *pCmdList = (T_Cmd_list *)glb_cmd_list;

	for (i = 0; i < glb_cmd_cnt; i++) {
		//	while(cmdList[i].cmdFn != NULL){
		if (pCmdList[i].attr & CMD_ATTR_AUTO_TEST) {
			pCmdList[i].cmdFn(1);
			//return 0;
		}
	}
	hprintf(TSENV, "Auto test Done.\r\n");

	//return i;
}

void cmd_init(void)
{
	int i = 0;
	T_Cmd_list *pCmdList;
	glb_cmd_list = (T_Cmd_list *)&__section_test_cnd_table_start;
	//glb_cmd_cnt = (unsigned int)((&__section_test_cnd_table_end - &__section_test_cnd_table_start)/sizeof(T_Cmd_list));
	pCmdList = (T_Cmd_list *)&__section_test_cnd_table_start;
	do {
		i++;
		pCmdList++;
	} while (pCmdList < (T_Cmd_list *)&__section_test_cnd_table_end);
	glb_cmd_cnt = i;
	//	printf("[CMD_INIT]Start:%08x , End:%08x size:%08x\n\r",&__section_test_cnd_table_start,&__section_test_cnd_table_end,sizeof(T_Cmd_list));
	return;
}

extern void prj_logo(void);

static F_postFn fPostCb = NULL;
void cmd_SetPostCb(F_postFn fCb)
{
	fPostCb = fCb;
}

void cmd_main(void)
{
	int len;
	char cmdBuf[128] = { 0 };
	cmd_init();

	//some cmd must run after cmd init
	if (fPostCb != NULL)
		fPostCb();

	//    printf("[CMD_INIT]cmd_list Satar:%08x ,cmd_cnt:%d.\n\r",glb_cmd_list,glb_cmd_cnt);
	//    helpCmd(0);
	while (1) {
		prj_logo();
		len = getString(cmdBuf, 128);
		putc('\r');
		putc('\n');

		if (len > 2) {
			runCmd((const char *)cmdBuf);
		}

		if (is_quit_flag) {
			is_quit_flag = 0;
			break;
		}
	}
}

CMD_DEFINE(help, helpCmd, 0, "show all cmd", 0);
CMD_DEFINE(dump, dumpMem, 0, "dump reg or memory", 0);
CMD_DEFINE(set32, setMem32, 0, "set reg or memory by 32 bit", 0);
CMD_DEFINE(autotest, auto_test_cmd, 0, "auto test all", 0);
CMD_DEFINE(quit, quitCmd, 0, "quit utility test cli", 0);
