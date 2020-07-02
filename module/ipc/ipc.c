/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */
#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <utility_lite.h>
#include <test_common.h>
#include <int_num.h>
#include <module/ipc.h>

static void A55_getSemaphore(void)
{
	unsigned long val;

	val = REG32(REG_SMPH_A55_BANK0_ID0);
	hprintf(TSENV, "\r\nREG_SMPH_A55_BANK0_ID0 0x%x", val);
	val = REG32(REG_SMPH_A55_BANK1_ID0 + 4);
	hprintf(TSENV, "\r\nREG_SMPH_A55_BANK1_ID0 0x%x", val);
	val = REG32(REG_SMPH_A55_BANK2_ID0 + 8);
	hprintf(TSENV, "\r\nREG_SMPH_A55_BANK2_ID0 0x%x", val);
	val = REG32(REG_SMPH_A55_BANK3_ID0 + 0xc);
	hprintf(TSENV, "\r\nREG_SMPH_A55_BANK3_ID0 0x%x", val);
}

static void A55_releaseSemaphore(void)
{
	putc('0');
	REG32(REG_SMPH_A55_BANK0_ID0) = 1;
	putc('1');
	REG32(REG_SMPH_A55_BANK1_ID0 + 4) = 1;
	putc('2');
	REG32(REG_SMPH_A55_BANK2_ID0 + 8) = 1;
	putc('3');
	REG32(REG_SMPH_A55_BANK3_ID0 + 0xc) = 1;
	putc('4');
}

static void SF_getSemaphore(void)
{
	unsigned long val;

	val = REG32(REG_SMPH_SF_BANK0_ID0);
	hprintf(TSENV, "\r\nREG_SMPH_SF_BANK0_ID0 0x%x", val);
	val = REG32(REG_SMPH_SF_BANK1_ID0 + 4);
	hprintf(TSENV, "\r\nREG_SMPH_SF_BANK1_ID1 0x%x", val);
	val = REG32(REG_SMPH_SF_BANK2_ID0 + 8);
	hprintf(TSENV, "\r\nREG_SMPH_SF_BANK2_ID2 0x%x", val);
	val = REG32(REG_SMPH_SF_BANK3_ID0 + 0xc);
	hprintf(TSENV, "\r\nREG_SMPH_SF_BANK3_ID3 0x%x", val);
}

static void SF_releaseSemaphore(void)
{
	putc('0');
	REG32(REG_SMPH_SF_BANK0_ID0) = 1;
	putc('1');
	REG32(REG_SMPH_SF_BANK1_ID0 + 4) = 1;
	putc('2');
	REG32(REG_SMPH_SF_BANK2_ID0 + 8) = 1;
	putc('3');
	REG32(REG_SMPH_SF_BANK3_ID0 + 0xc) = 1;
	putc('4');
}

static void SC_getSemaphore(void)
{
	unsigned long val;

	val = REG32(REG_SMPH_SC_BANK0_ID0);
	hprintf(TSENV, "\r\nREG_SMPH_SC_BANK0_ID0 0x%x", val);
	val = REG32(REG_SMPH_SC_BANK1_ID0 + 4);
	hprintf(TSENV, "\r\nREG_SMPH_SC_BANK1_ID1 0x%x", val);
	val = REG32(REG_SMPH_SC_BANK2_ID0 + 8);
	hprintf(TSENV, "\r\nREG_SMPH_SC_BANK2_ID2 0x%x", val);
	val = REG32(REG_SMPH_SC_BANK3_ID0 + 0xc);
	hprintf(TSENV, "\r\nREG_SMPH_SC_BANK3_ID3 0x%x", val);
}

static void SC_releaseSemaphore(void)
{
	putc('0');
	REG32(REG_SMPH_SC_BANK0_ID0) = 1;
	putc('1');
	REG32(REG_SMPH_SC_BANK1_ID0 + 4) = 1;
	putc('2');
	REG32(REG_SMPH_SC_BANK2_ID0 + 8) = 1;
	putc('3');
	REG32(REG_SMPH_SC_BANK3_ID0 + 0xc) = 1;
	putc('4');
}

void A55_IpcTst(int para)
{
	unsigned long val;

	hprintf(TSENV, "\r\na55 send to Secure event:");
	if (getHex(&val)) {
		val = 2;
	}
	REG32(REG_SRC_A550_TO_SC) = val;

	hprintf(TSENV, "\r\nsend to Safety0+1 event:");
	if (getHex(&val)) {
		val = 2;
	}
	REG32(REG_SRC_A550_TO_SF0) = val;
	REG32(REG_SRC_A550_TO_SF1) = val;

	hprintf(TSENV,
		"\r\n press anykey to continue test  A55 release semaphore");
	getc();
	A55_getSemaphore();

	hprintf(TSENV, "\r\n...pending......");
	SF_getSemaphore();
	SC_getSemaphore();

	hprintf(TSENV, "\r\n...release......");
	A55_releaseSemaphore();
}

void Secure_IpcTst(int para)
{
	unsigned long val;

	hprintf(TSENV, "\r\nSecure send to a55 event:");
	if (getHex(&val)) {
		val = 2;
	}
	REG32(REG_SRC_SC_TO_A550) = val;
	REG32(REG_SRC_SC_TO_A551) = val;
	REG32(REG_SRC_SC_TO_A552) = val;
	REG32(REG_SRC_SC_TO_A553) = val;
	REG32(REG_SRC_SC_TO_A554) = val;
	REG32(REG_SRC_SC_TO_A555) = val;
	REG32(REG_SRC_SC_TO_A556) = val;
	REG32(REG_SRC_SC_TO_A557) = val;

	hprintf(TSENV, "\r\nsend to Safety0+1 event:");
	if (getHex(&val)) {
		val = 2;
	}
	REG32(REG_SRC_SC_TO_SF0) = val;
	REG32(REG_SRC_SC_TO_SF1) = val;

	hprintf(TSENV,
		"\r\n press anykey to continue test Secure release semaphore");
	getc();
	SC_getSemaphore();

	hprintf(TSENV, "\r\n...pending......");
	SF_getSemaphore();
	A55_getSemaphore();

	hprintf(TSENV, "\r\n...release......");
	SC_releaseSemaphore();
}

void Safety0_IpcTst(int para)
{
	unsigned long val;

	hprintf(TSENV, "\r\nSafety send to a55 event:");
	if (getHex(&val)) {
		val = 2;
	}
	REG32(REG_SRC_SF0_TO_A550) = val;
	REG32(REG_SRC_SF0_TO_A551) = val;
	REG32(REG_SRC_SF0_TO_A552) = val;
	REG32(REG_SRC_SF0_TO_A553) = val;
	REG32(REG_SRC_SF0_TO_A554) = val;
	REG32(REG_SRC_SF0_TO_A555) = val;
	REG32(REG_SRC_SF0_TO_A556) = val;
	REG32(REG_SRC_SF0_TO_A557) = val;

	hprintf(TSENV, "\r\nsend to Sec event:");
	if (getHex(&val)) {
		val = 2;
	}
	REG32(REG_SRC_SF0_TO_SC) = val;

	hprintf(TSENV,
		"\r\n press anykey to continue test SF release semaphore");
	getc();
	SF_getSemaphore();

	hprintf(TSENV, "\r\n...pending......");
	SC_getSemaphore();
	A55_getSemaphore();

	hprintf(TSENV, "\r\n...release......");
	SF_releaseSemaphore();
}

#if defined(CONFIG_A55)
CMD_DEFINE(a55ipc, A55_IpcTst, 0, "A55 to sec+safety ipc tst", 0);
#endif

#if !defined(CONFIG_A55)
CMD_DEFINE(a55ipc, A55_IpcTst, 0, "A55 to sec+safety ipc tst", 0);
CMD_DEFINE(scipc, Secure_IpcTst, 0, "Secure to A55+safety ipc tst", 0);
CMD_DEFINE(sfipc, Safety0_IpcTst, 0, "sf0 to sec+A55 ipc tst", 0);
#endif
