/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _REGTEST_H
#define _REGTEST_H
#include <linux/types.h>

#define REG32_READ(addr) (*((volatile u32 *)((unsigned long)(addr))))
#define REG32_WRITE(addr, val)                                                 \
	(*((volatile u32 *)((unsigned long)(addr))) = ((u32)(val)))
#define REG64_READ(addr) (*((volatile u64 *)((unsigned long)(addr))))
#define REG64_WRITE(addr, val)                                                 \
	(*((volatile u64 *)((unsigned long)(addr))) = ((u64)(val)))
#define REG8_READ(addr) (*((volatile u8 *)((unsigned long)addr)))
#define REG8_WRITE(addr, val)                                                  \
	(*((volatile u8 *)((unsigned long)(addr))) = ((u8)(val)))
#define REG16_READ(addr) (*((volatile u16 *)((unsigned long)(addr))))
#define REG16_WRITE(addr, val)                                                 \
	(*((volatile u16 *)((unsigned long)(addr))) = ((u16)(val)))

#define REG32(addr) (*((volatile u32 *)((unsigned long)(addr))))
#define REG8(addr) (*((volatile u8 *)((unsigned long)(addr))))

#define REG_RW_TEST(addr, bit_strobe)                                          \
	do {                                                                   \
		u32 _i;                                                        \
		REG32_WRITE(addr, 0x0 & (bit_strobe));                         \
		HCOMP(0x0 & (bit_strobe), REG32_READ(addr) & (bit_strobe));    \
		REG32_WRITE(addr, 0xFFFFFFFF & (bit_strobe));                  \
		HCOMP(0xFFFFFFFF & (bit_strobe),                               \
		      REG32_READ(addr) & (bit_strobe));                        \
		REG32_WRITE(addr, 0x0 & (bit_strobe));                         \
		HCOMP(0x0 & (bit_strobe), REG32_READ(addr) & (bit_strobe));    \
		REG32_WRITE(addr, 0x55555555 & (bit_strobe));                  \
		HCOMP(0x55555555 & (bit_strobe),                               \
		      REG32_READ(addr) & (bit_strobe));                        \
		REG32_WRITE(addr, 0xAAAAAAAA & bit_strobe);                    \
		HCOMP(0xAAAAAAAA & (bit_strobe),                               \
		      REG32_READ(addr) & (bit_strobe));                        \
		for (_i = 0; _i < 32; _i++) {                                  \
			if ((0x1 << _i) & (bit_strobe)) {                      \
				REG32_WRITE(addr, (0x0 << _i));                \
				HCOMP(0x0 << _i,                               \
				      REG32_READ(addr) & (0x1 << _i));         \
				REG32_WRITE(addr, (0x1 << _i));                \
				HCOMP(0x1 << _i,                               \
				      REG32_READ(addr) & (0x1 << _i));         \
				REG32_WRITE(addr, (0x0 << _i));                \
				HCOMP(0x0 << _i,                               \
				      REG32_READ(addr) & (0x1 << _i));         \
			}                                                      \
		}                                                              \
	} while (0)

#define REG_RO_TEST(addr, bit_strobe)                                          \
	do {                                                                   \
		u32 _i, _val;                                                  \
		_val = REG32_READ(addr);                                       \
		REG32_WRITE(addr, 0x0 & bit_strobe);                           \
		HCOMP(_val &bit_strobe, REG32_READ(addr) & bit_strobe);        \
		REG32_WRITE(addr, 0xFFFFFFFF & bit_strobe);                    \
		HCOMP(_val &bit_strobe, REG32_READ(addr) & bit_strobe);        \
		REG32_WRITE(addr, 0x55555555 & bit_strobe);                    \
		HCOMP(_val &bit_strobe, REG32_READ(addr) & bit_strobe);        \
		REG32_WRITE(addr, 0xAAAAAAAA & bit_strobe);                    \
		HCOMP(_val &bit_strobe, REG32_READ(addr) & bit_strobe);        \
		for (_i = 0; _i < 32; _i++) {                                  \
			if ((0x1 << _i) & bit_strobe) {                        \
				REG32_WRITE(addr, (0x0 << _i));                \
				HCOMP(_val &(0x1 << _i),                       \
				      REG32_READ(addr) & (0x1 << _i));         \
				REG32_WRITE(addr, (0x1 << _i));                \
				HCOMP(_val &(0x1 << _i),                       \
				      REG32_READ(addr) & (0x1 << _i));         \
			}                                                      \
		}                                                              \
	} while (0)

typedef struct _reg_struct {
	u32 addr;
	u32 default_val;
	u32 bit_strobe;
	s8 *name;
} reg_struct;

#endif
