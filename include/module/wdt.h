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

#ifndef _WDT_H
#define _WDT_H

#define WDT_CR 0x0
#define WDT_TORR 0x4
#define WDT_CCVR 0x8
#define WDT_CRR 0xC
#define WDT_STAT 0x10
#define WDT_EOI 0x14
#define WDT_PROT_LEVEL 0x1C
#define WDT_COMP_PARAM_5 0XE4
#define WDT_COMP_PARAM_4 0XE8
#define WDT_COMP_PARAM_3 0XEC
#define WDT_COMP_PARAM_2 0XF0
#define WDT_COMP_PARAM_1 0XF4
#define WDT_COMP_VERSI 0XF8
#define WDT_COMP_TYPE 0XFC

#define WDT_KICK_MAGIC 0X76

#define WDT_TEST_KICK_DOG_CNT 100

typedef enum { WDT_RMODE_RESET = 0, WDT_RMODE_INTR } WDT_RMODE_E;

typedef enum {
	WDT_RPL_2_CYCLES = 0,
	WDT_RPL_4_CYCLES,
	WDT_RPL_8_CYCLES,
	WDT_RPL_16_CYCLES,
	WDT_RPL_32_CYCLES,
	WDT_RPL_64_CYCLES,
	WDT_RPL_128_CYCLES,
	WDT_RPL_256_CYCLES
} WDT_RPL_E;

#endif
