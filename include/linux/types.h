///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           //
//  This file contains proprietary information that is the sole intellectual property        //
//  of Black Sesame Technologies, Inc. and its affiliates.  No portions of this material     //
//  may be reproduced in any form without the written permission of:                         //
//                                                                                           //
//  Black Sesame Technologies, Inc. and its affiliates                                       //
//  2255 Martin Ave. Suite D                                                                 //
//  Santa Clara, CA  95050                                                                   //
//  Copyright \@2018: all right reserved.                                                    //
//                                                                                           //
//  Notice:                                                                                  //
//                                                                                           //
//  You are running an EVALUATION distribution of the neural network tools provided by       //
//  Black Sesame Technologies, Inc. under NDA.                                               //
//  This copy may NOT be used in production or distributed to any third party.               //
//  For distribution or production, further Software License Agreement is required.          //
//                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TPYES_H
#define _TPYES_H

#include <stdbool.h>
#include <linux/stddef.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _ASM_GENERIC_INT_LL64_H
#define _ASM_GENERIC_INT_LL64_H

#ifndef __ASSEMBLY__
/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#ifdef __GNUC__
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#else
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

typedef __s8 s8;
typedef __u8 u8;
typedef __s16 s16;
typedef __u16 u16;
typedef __s32 s32;
typedef __u32 u32;
typedef __s64 s64;
typedef __u64 u64;

#endif /* __ASSEMBLY__ */

#endif /* _ASM_GENERIC_INT_LL64_H */

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

#ifndef __BIT_TYPES_DEFINED__
#define __BIT_TYPES_DEFINED__

typedef u8			u_int8_t;
typedef s8			int8_t;
typedef u16			u_int16_t;
typedef s16			int16_t;
typedef u32			u_int32_t;
typedef s32			int32_t;

#endif /* !(__BIT_TYPES_DEFINED__) */

typedef u8			uint8_t;
typedef u16			uint16_t;
typedef u32			uint32_t;

#if defined(__GNUC__)
typedef u64			uint64_t;
typedef u64			u_int64_t;
typedef s64			int64_t;
#endif

/* this is a special 64bit data type that is 8-byte aligned */
#define aligned_u64		__aligned_u64
#define aligned_be64		__aligned_be64
#define aligned_le64		__aligned_le64


#ifndef _SIZE_T
#define _SIZE_T
#ifdef __LP64__
typedef u64 size_t;
#else
typedef u32 size_t;
#endif

#endif /* _SIZE_T */

#ifdef __LP64__
typedef u64 dma_addr_t;
#else
typedef u32 dma_addr_t;
#endif

typedef enum {
	HVTE_FALSE,
	HVTE_TRUE,
} bool_enum;

typedef void (*breset_call)(void);
typedef u32 (*funcp)(u32 flags, void *para);

typedef struct _test_struct {
	funcp fp;
	u32 flags;
	void *para;
	char *name;
	char *description;
} test_struct;

typedef struct _test_static {
	u32 total_test_case;
	u32 total_pass_case;
	u32 total_fail_case;
	u32 total_test_ip;
	u32 total_pass_ip;
	u32 total_fail_ip;
	u32 po_fail_flag; //power on test fail flag
	u32 ip_fail_flag;
} test_static;

#endif /* _TPYES_H */
