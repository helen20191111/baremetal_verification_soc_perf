/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __MEMTESTER_H__
#define __MEMTESTER_H__

#define EXIT_FAIL_NONSTARTER 0x01
#define EXIT_FAIL_ADDRESSLINES 0x02
#define EXIT_FAIL_OTHERTEST 0x04

typedef unsigned long ul;
typedef unsigned long long ull;
typedef unsigned long volatile ulv;
typedef unsigned char volatile u8v;
typedef unsigned short volatile u16v;

typedef ul off_t;

#define UL_ONEBITS 0xffffffff
#define UL_LEN 32
#define CHECKERBOARD1 0x55555555
#define CHECKERBOARD2 0xaaaaaaaa
#define UL_BYTE(x) ((x | x << 8 | x << 16 | x << 24))

struct mem_test {
	char *name;
	int (*fp)(ulv *bufa, ulv *bufb, size_t count);
};

union {
	unsigned char bytes[UL_LEN / 8];
	ul val;
} mword8;

union {
	unsigned short u16s[UL_LEN / 16];
	ul val;
} mword16;

int test_stuck_address(unsigned long volatile *bufa, size_t count);
int test_random_value(ulv *bufa, ulv *bufb, size_t count);
int test_rw_comparison(ulv *bufa, ulv *bufb, size_t count);
int test_xor_comparison(unsigned long volatile *bufa,
			unsigned long volatile *bufb, size_t count);
int test_sub_comparison(unsigned long volatile *bufa,
			unsigned long volatile *bufb, size_t count);
int test_mul_comparison(unsigned long volatile *bufa,
			unsigned long volatile *bufb, size_t count);
int test_div_comparison(unsigned long volatile *bufa,
			unsigned long volatile *bufb, size_t count);
int test_or_comparison(unsigned long volatile *bufa,
		       unsigned long volatile *bufb, size_t count);
int test_and_comparison(unsigned long volatile *bufa,
			unsigned long volatile *bufb, size_t count);
int test_seqinc_comparison(unsigned long volatile *bufa,
			   unsigned long volatile *bufb, size_t count);
int test_solidbits_comparison(unsigned long volatile *bufa,
			      unsigned long volatile *bufb, size_t count);
int test_checkerboard_comparison(unsigned long volatile *bufa,
				 unsigned long volatile *bufb, size_t count);
int test_blockseq_comparison(unsigned long volatile *bufa,
			     unsigned long volatile *bufb, size_t count);
int test_walkbits0_comparison(unsigned long volatile *bufa,
			      unsigned long volatile *bufb, size_t count);
int test_walkbits1_comparison(unsigned long volatile *bufa,
			      unsigned long volatile *bufb, size_t count);
int test_bitspread_comparison(unsigned long volatile *bufa,
			      unsigned long volatile *bufb, size_t count);
int test_bitflip_comparison(unsigned long volatile *bufa,
			    unsigned long volatile *bufb, size_t count);

#endif
