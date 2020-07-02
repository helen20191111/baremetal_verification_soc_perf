/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <linux/types.h>
#include <printf_inc.h> //use related path
#include <utility_lite.h>
#include <linux/string.h>

/////////////////////////////////////putc  getc for printf////////////////////////////////////////
#include <a1000/sysreg_base.h>
#include <RegTest.h>
#include "memtester.h"

const char progress[] = "-\\|/";
#define PROGRESSLEN 4
#define PROGRESSOFTEN 2500
#define ONE 0x00000001L

const int use_phys = 1;

off_t physaddrbase = 0;

static ul rand_next = 158433;

struct mem_test mem_tests[] = {

	{ "read Write", test_rw_comparison },
	{ "Block Sequential", test_blockseq_comparison },
	{ "Random Value", test_random_value },
	{ "Compare XOR", test_xor_comparison },
	{ "Compare SUB", test_sub_comparison },
	{ "Compare MUL", test_mul_comparison },
	{ "Compare DIV", test_div_comparison },
	{ "Compare OR", test_or_comparison },
	{ "Compare AND", test_and_comparison },
	{ "Sequential Increment", test_seqinc_comparison },
	{ "Solid Bits", test_solidbits_comparison },
	//    { "Block Sequential", test_blockseq_comparison },
	{ "Checkerboard", test_checkerboard_comparison },
	{ "Bit Spread", test_bitspread_comparison },
	{ "Bit Flip", test_bitflip_comparison },
	{ "Walking Ones", test_walkbits1_comparison },
	{ "Walking Zeroes", test_walkbits0_comparison },
	{ NULL, NULL }
};

ul my_rand(void)
{
	rand_next = rand_next * 1103515245 + 12345;
	return ((unsigned)(rand_next / 65536) % 32768);
}

ul my_randul(void)
{
	rand_next = rand_next * 1103515245 + 12345;
	return rand_next;
}

void my_rand_seed(ul seed) //��srand���Ĺ�������ֵͨ����ʽ����������Ϊ���ֵ
{
	rand_next = seed;
}

int compare_regions(ulv *bufa, ulv *bufb, size_t count)
{
	int r = 0;
	size_t i;
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	off_t physaddr;

	for (i = 0; i < count; i++, p1++, p2++) {
		if (*p1 != *p2) {
			if (use_phys) {
				physaddr = physaddrbase + (i * sizeof(ul));
				printf("FAILURE: 0x%08x != 0x%08x at physical address "
				       "0x%08x.\n",
				       (ul)*p1, (ul)*p2, physaddr);
			} else {
				printf("FAILURE: 0x%08x != 0x%08x at offset 0x%08x.\n",
				       (ul)*p1, (ul)*p2, (ul)(i * sizeof(ul)));
			}
			/* printf("Skipping to next test..."); */
			r = -1;
		}
	}
	return r;
}

int compare_regions_ul(ulv *bufa, ulv val, size_t count)
{
	int r = 0;
	size_t i;
	ulv *p1 = bufa;
	off_t physaddr;

	for (i = 0; i < count; i++, p1++) {
		if (*p1 != val) {
			if (use_phys) {
				physaddr = physaddrbase + (i * sizeof(ul));
				printf("FAILURE: 0x%08x != 0x%08x at physical address "
				       "0x%08x.\n",
				       (ul)*p1, val, physaddr);
			} else {
				printf("FAILURE: 0x%08x != 0x%08x at offset 0x%08x.\n",
				       (ul)*p1, val, (ul)(i * sizeof(ul)));
			}
			/* printf("Skipping to next test..."); */
			r = -1;
		}
	}
	return r;
}

int test_rw_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	size_t i;

	for (i = 0; i < count * 2; i++) {
		*p1++ = 0x55aaaa55;
	}
	return compare_regions_ul(bufa, 0x55aaaa55, count);
}

int test_random_value(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	ul j = 0;
	size_t i;

	putchar(' ');
	for (i = 0; i < count; i++) {
		*p1++ = *p2++ = my_rand();
		if (!(i % PROGRESSOFTEN)) {
			putchar('\b');
			putchar(progress[++j % PROGRESSLEN]);
		}
	}
	printf("\b \b");
	return compare_regions(bufa, bufb, count);
}

int test_stuck_address(ulv *bufa, size_t count)
{
	ulv *p1 = bufa;
	unsigned int j;
	size_t i;
	off_t physaddr;

	printf("           ");

	for (j = 0; j < 16; j++) {
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		p1 = (ulv *)bufa;
		printf("setting %3u", j);

		for (i = 0; i < count; i++) {
			*p1 = ((j + i) % 2) == 0 ? (ul)p1 : ~((ul)p1);
			*p1++;
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		printf("testing %3u", j);

		p1 = (ulv *)bufa;
		for (i = 0; i < count; i++, p1++) {
			if (*p1 != (((j + i) % 2) == 0 ? (ul)p1 : ~((ul)p1))) {
				if (use_phys) {
					physaddr =
						physaddrbase + (i * sizeof(ul));
					printf("FAILURE: possible bad address line at physical "
					       "address 0x%08x.\n",
					       physaddr);
				} else {
					printf("FAILURE: possible bad address line at offset "
					       "0x%08x.\n",
					       (ul)(i * sizeof(ul)));
				}
				printf("Skipping to next test...\n");

				return -1;
			}
		}
	}
	printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");

	return 0;
}

int test_xor_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	size_t i;
	ul q = my_rand();

	for (i = 0; i < count; i++) {
		*p1++ ^= q;
		*p2++ ^= q;
	}
	return compare_regions(bufa, bufb, count);
}

int test_sub_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	size_t i;
	ul q = my_rand();

	for (i = 0; i < count; i++) {
		*p1++ -= q;
		*p2++ -= q;
	}
	return compare_regions(bufa, bufb, count);
}
int test_seqinc_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	size_t i;
	ul q = my_rand();

	for (i = 0; i < count; i++) {
		*p1++ = *p2++ = (i + q);
	}
	return compare_regions(bufa, bufb, count);
}

int test_mul_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	size_t i;
	ul q = my_rand();

	for (i = 0; i < count; i++) {
		*p1++ *= q;
		*p2++ *= q;
	}
	return compare_regions(bufa, bufb, count);
}

int test_div_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	size_t i;
	ul q = my_rand();

	for (i = 0; i < count; i++) {
		if (!q) {
			q++;
		}
		*p1++ /= q;
		*p2++ /= q;
	}
	return compare_regions(bufa, bufb, count);
}

int test_or_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	size_t i;
	ul q = my_rand();

	for (i = 0; i < count; i++) {
		*p1++ |= q;
		*p2++ |= q;
	}
	return compare_regions(bufa, bufb, count);
}

int test_and_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	size_t i;
	ul q = my_rand();

	for (i = 0; i < count; i++) {
		*p1++ &= q;
		*p2++ &= q;
	}
	return compare_regions(bufa, bufb, count);
}

int test_solidbits_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	unsigned int j;
	ul q;
	size_t i;

	printf("           ");

	for (j = 0; j < 10; j++) {
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		q = (j % 2) == 0 ? UL_ONEBITS : 0;
		printf("setting %3u", j);

		p1 = (ulv *)bufa;
		p2 = (ulv *)bufb;
		for (i = 0; i < count; i++) {
			*p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		printf("testing %3u", j);

		if (compare_regions(bufa, bufb, count)) {
			return -1;
		}
	}
	printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");

	return 0;
}

int test_checkerboard_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	unsigned int j;
	ul q;
	size_t i;

	printf("           ");

	for (j = 0; j < 64; j++) {
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		q = (j % 2) == 0 ? CHECKERBOARD1 : CHECKERBOARD2;
		printf("setting %3u", j);

		p1 = (ulv *)bufa;
		p2 = (ulv *)bufb;
		for (i = 0; i < count; i++) {
			*p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		printf("testing %3u", j);

		if (compare_regions(bufa, bufb, count)) {
			return -1;
		}
	}
	printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");

	return 0;
}

int test_blockseq_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	unsigned int j;
	size_t i;

	printf("           ");

	for (j = 0; j < 256; j++) {
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		p1 = (ulv *)bufa;
		p2 = (ulv *)bufb;
		printf("setting %3u", j);

		for (i = 0; i < count; i++) {
			*p1++ = *p2++ = (ul)UL_BYTE(j);
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		printf("testing %3u", j);

		if (compare_regions(bufa, bufb, count)) {
			return -1;
		}
	}
	printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");

	return 0;
}

int test_walkbits0_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	unsigned int j;
	size_t i;

	printf("           ");

	for (j = 0; j < UL_LEN * 2; j++) {
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		p1 = (ulv *)bufa;
		p2 = (ulv *)bufb;
		printf("setting %3u", j);

		for (i = 0; i < count; i++) {
			if (j < UL_LEN) { /* Walk it up. */
				*p1++ = *p2++ = ONE << j;
			} else { /* Walk it back down. */
				*p1++ = *p2++ = ONE << (UL_LEN * 2 - j - 1);
			}
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		printf("testing %3u", j);

		if (compare_regions(bufa, bufb, count)) {
			return -1;
		}
	}
	printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");

	return 0;
}

int test_walkbits1_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	unsigned int j;
	size_t i;

	printf("           ");

	for (j = 0; j < UL_LEN * 2; j++) {
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		p1 = (ulv *)bufa;
		p2 = (ulv *)bufb;
		printf("setting %3u", j);

		for (i = 0; i < count; i++) {
			if (j < UL_LEN) { /* Walk it up. */
				*p1++ = *p2++ = UL_ONEBITS ^ (ONE << j);
			} else { /* Walk it back down. */
				*p1++ = *p2++ = UL_ONEBITS ^
						(ONE << (UL_LEN * 2 - j - 1));
			}
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		printf("testing %3u", j);

		if (compare_regions(bufa, bufb, count)) {
			return -1;
		}
	}
	printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");

	return 0;
}

int test_bitspread_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	unsigned int j;
	size_t i;

	printf("           ");

	for (j = 0; j < UL_LEN * 2; j++) {
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		p1 = (ulv *)bufa;
		p2 = (ulv *)bufb;
		printf("setting %3u", j);

		for (i = 0; i < count; i++) {
			if (j < UL_LEN) { /* Walk it up. */
				*p1++ = *p2++ =
					(i % 2 == 0) ?
						(ONE << j) | (ONE << (j + 2)) :
						UL_ONEBITS ^ ((ONE << j) |
							      (ONE << (j + 2)));
			} else { /* Walk it back down. */
				*p1++ = *p2++ =
					(i % 2 == 0) ?
						(ONE << (UL_LEN * 2 - 1 - j)) |
							(ONE << (UL_LEN * 2 +
								 1 - j)) :
						UL_ONEBITS ^
							(ONE << (UL_LEN * 2 -
								 1 - j) |
							 (ONE << (UL_LEN * 2 +
								  1 - j)));
			}
		}
		printf("\b\b\b\b\b\b\b\b\b\b\b");
		printf("testing %3u", j);

		if (compare_regions(bufa, bufb, count)) {
			return -1;
		}
	}
	printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");

	return 0;
}

int test_bitflip_comparison(ulv *bufa, ulv *bufb, size_t count)
{
	ulv *p1 = bufa;
	ulv *p2 = bufb;
	unsigned int j, k;
	ul q;
	size_t i;

	printf("           ");

	for (k = 0; k < UL_LEN; k++) {
		q = ONE << k;
		for (j = 0; j < 8; j++) {
			printf("\b\b\b\b\b\b\b\b\b\b\b");
			q = ~q;
			printf("setting %3u", k * 8 + j);

			p1 = (ulv *)bufa;
			p2 = (ulv *)bufb;
			for (i = 0; i < count; i++) {
				*p1++ = *p2++ = (i % 2) == 0 ? q : ~q;
			}
			printf("\b\b\b\b\b\b\b\b\b\b\b");
			printf("testing %3u", k * 8 + j);

			if (compare_regions(bufa, bufb, count)) {
				return -1;
			}
		}
	}
	printf("\b\b\b\b\b\b\b\b\b\b\b           \b\b\b\b\b\b\b\b\b\b\b");

	return 0;
}

int memtest_main(ul addr, ul size, int cnt)
{
	ul i, j;
	ulv *bufa, *bufb;
	ul halflen, count;
	ul error_cnt = 0;
	struct mem_test *test_entry;
	halflen = size / 2;
	bufa = (ulv *)addr;
	bufb = (ulv *)(addr + size / 2);
	count = halflen / sizeof(ul);
	for (j = 0; j < cnt; j++) {
		my_rand_seed(j * 62587421);
		printf("Mem Test LOOP:[%d]\n\r", j);
		for (i = 0; i < sizeof(mem_tests) / sizeof(mem_tests[0]); i++) {
			test_entry = &mem_tests[i];
			if (test_entry && test_entry->fp) {
				printf("Start test:[%s]:(0x%08x,%x)\n\r",
				       test_entry->name, addr, size);
				error_cnt -= test_entry->fp(bufa, bufb, count);
				printf("End test:[%s] Error Cnt:%d\n\r",
				       test_entry->name, error_cnt);
			} else {
				break;
			}
		}
	}
	return 0;
}
