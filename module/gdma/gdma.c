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
#include <uart_sync.h>
#include <ut_test.h>
#include <int_num.h>
#include <module/gdma.h>
#include <module/dmac.h>
#include <linux/types.h>

struct dma_test_struct;

static int flag_dma_trans_done = 0;
static int flag_block_trans_done = 0;
static int flag_dma_src_done_int = 0;
static int flag_dma_dst_done_int = 0;
static int flag_dma_src_decode_err_int = 0;
static int flag_dma_dst_decode_err_int = 0;
static int flag_dma_lli_shadow_invalid = 0;

#define DATA_LEN 1024 // 1KB
#define DATA_WIDTH 32

void sdma_irq_init(void);
//enable all sdma interrupt
/*
 *  fine. I know this is "SILLY", 
 *  but this is what we need to do in order not to flush data out of the cache.
 */
u32 buf[DATA_LEN] = {
	1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,
	13,   14,   15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
	25,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,
	37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,   48,
	49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,
	61,   62,   63,   64,   65,   66,   67,   68,   69,   70,   71,   72,
	73,   74,   75,   76,   77,   78,   79,   80,   81,   82,   83,   84,
	85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95,   96,
	97,   98,   99,   100,  101,  102,  103,  104,  105,  106,  107,  108,
	109,  110,  111,  112,  113,  114,  115,  116,  117,  118,  119,  120,
	121,  122,  123,  124,  125,  126,  127,  128,  129,  130,  131,  132,
	133,  134,  135,  136,  137,  138,  139,  140,  141,  142,  143,  144,
	145,  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,  156,
	157,  158,  159,  160,  161,  162,  163,  164,  165,  166,  167,  168,
	169,  170,  171,  172,  173,  174,  175,  176,  177,  178,  179,  180,
	181,  182,  183,  184,  185,  186,  187,  188,  189,  190,  191,  192,
	193,  194,  195,  196,  197,  198,  199,  200,  201,  202,  203,  204,
	205,  206,  207,  208,  209,  210,  211,  212,  213,  214,  215,  216,
	217,  218,  219,  220,  221,  222,  223,  224,  225,  226,  227,  228,
	229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239,  240,
	241,  242,  243,  244,  245,  246,  247,  248,  249,  250,  251,  252,
	253,  254,  255,  256,  257,  258,  259,  260,  261,  262,  263,  264,
	265,  266,  267,  268,  269,  270,  271,  272,  273,  274,  275,  276,
	277,  278,  279,  280,  281,  282,  283,  284,  285,  286,  287,  288,
	289,  290,  291,  292,  293,  294,  295,  296,  297,  298,  299,  300,
	301,  302,  303,  304,  305,  306,  307,  308,  309,  310,  311,  312,
	313,  314,  315,  316,  317,  318,  319,  320,  321,  322,  323,  324,
	325,  326,  327,  328,  329,  330,  331,  332,  333,  334,  335,  336,
	337,  338,  339,  340,  341,  342,  343,  344,  345,  346,  347,  348,
	349,  350,  351,  352,  353,  354,  355,  356,  357,  358,  359,  360,
	361,  362,  363,  364,  365,  366,  367,  368,  369,  370,  371,  372,
	373,  374,  375,  376,  377,  378,  379,  380,  381,  382,  383,  384,
	385,  386,  387,  388,  389,  390,  391,  392,  393,  394,  395,  396,
	397,  398,  399,  400,  401,  402,  403,  404,  405,  406,  407,  408,
	409,  410,  411,  412,  413,  414,  415,  416,  417,  418,  419,  420,
	421,  422,  423,  424,  425,  426,  427,  428,  429,  430,  431,  432,
	433,  434,  435,  436,  437,  438,  439,  440,  441,  442,  443,  444,
	445,  446,  447,  448,  449,  450,  451,  452,  453,  454,  455,  456,
	457,  458,  459,  460,  461,  462,  463,  464,  465,  466,  467,  468,
	469,  470,  471,  472,  473,  474,  475,  476,  477,  478,  479,  480,
	481,  482,  483,  484,  485,  486,  487,  488,  489,  490,  491,  492,
	493,  494,  495,  496,  497,  498,  499,  500,  501,  502,  503,  504,
	505,  506,  507,  508,  509,  510,  511,  512,  513,  514,  515,  516,
	517,  518,  519,  520,  521,  522,  523,  524,  525,  526,  527,  528,
	529,  530,  531,  532,  533,  534,  535,  536,  537,  538,  539,  540,
	541,  542,  543,  544,  545,  546,  547,  548,  549,  550,  551,  552,
	553,  554,  555,  556,  557,  558,  559,  560,  561,  562,  563,  564,
	565,  566,  567,  568,  569,  570,  571,  572,  573,  574,  575,  576,
	577,  578,  579,  580,  581,  582,  583,  584,  585,  586,  587,  588,
	589,  590,  591,  592,  593,  594,  595,  596,  597,  598,  599,  600,
	601,  602,  603,  604,  605,  606,  607,  608,  609,  610,  611,  612,
	613,  614,  615,  616,  617,  618,  619,  620,  621,  622,  623,  624,
	625,  626,  627,  628,  629,  630,  631,  632,  633,  634,  635,  636,
	637,  638,  639,  640,  641,  642,  643,  644,  645,  646,  647,  648,
	649,  650,  651,  652,  653,  654,  655,  656,  657,  658,  659,  660,
	661,  662,  663,  664,  665,  666,  667,  668,  669,  670,  671,  672,
	673,  674,  675,  676,  677,  678,  679,  680,  681,  682,  683,  684,
	685,  686,  687,  688,  689,  690,  691,  692,  693,  694,  695,  696,
	697,  698,  699,  700,  701,  702,  703,  704,  705,  706,  707,  708,
	709,  710,  711,  712,  713,  714,  715,  716,  717,  718,  719,  720,
	721,  722,  723,  724,  725,  726,  727,  728,  729,  730,  731,  732,
	733,  734,  735,  736,  737,  738,  739,  740,  741,  742,  743,  744,
	745,  746,  747,  748,  749,  750,  751,  752,  753,  754,  755,  756,
	757,  758,  759,  760,  761,  762,  763,  764,  765,  766,  767,  768,
	769,  770,  771,  772,  773,  774,  775,  776,  777,  778,  779,  780,
	781,  782,  783,  784,  785,  786,  787,  788,  789,  790,  791,  792,
	793,  794,  795,  796,  797,  798,  799,  800,  801,  802,  803,  804,
	805,  806,  807,  808,  809,  810,  811,  812,  813,  814,  815,  816,
	817,  818,  819,  820,  821,  822,  823,  824,  825,  826,  827,  828,
	829,  830,  831,  832,  833,  834,  835,  836,  837,  838,  839,  840,
	841,  842,  843,  844,  845,  846,  847,  848,  849,  850,  851,  852,
	853,  854,  855,  856,  857,  858,  859,  860,  861,  862,  863,  864,
	865,  866,  867,  868,  869,  870,  871,  872,  873,  874,  875,  876,
	877,  878,  879,  880,  881,  882,  883,  884,  885,  886,  887,  888,
	889,  890,  891,  892,  893,  894,  895,  896,  897,  898,  899,  900,
	901,  902,  903,  904,  905,  906,  907,  908,  909,  910,  911,  912,
	913,  914,  915,  916,  917,  918,  919,  920,  921,  922,  923,  924,
	925,  926,  927,  928,  929,  930,  931,  932,  933,  934,  935,  936,
	937,  938,  939,  940,  941,  942,  943,  944,  945,  946,  947,  948,
	949,  950,  951,  952,  953,  954,  955,  956,  957,  958,  959,  960,
	961,  962,  963,  964,  965,  966,  967,  968,  969,  970,  971,  972,
	973,  974,  975,  976,  977,  978,  979,  980,  981,  982,  983,  984,
	985,  986,  987,  988,  989,  990,  991,  992,  993,  994,  995,  996,
	997,  998,  999,  1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008,
	1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020,
	1021, 1022, 1023, 1024
};

#define comp_one_reg(__base, __offset, realval_H, realval_L)                                  \
	do {                                                                                  \
		u32 read_value = REG32_READ(__base + __offset);                               \
		pr_debug(                                                               \
			"REG_Addr: %08X  NAME: %-20s  read_value: %08X  expect_value: %08X ", \
			__base + __offset, #__offset, read_value, realval_L);                 \
		if (read_value != realval_L) {                                                \
			pr_debug("  test result:   FAILED! \r\n ");                     \
		} else {                                                                      \
			pr_debug("   test result:   SUCCESS! \r\n");                    \
		}                                                                             \
		read_value = REG32_READ(__base + __offset + 0x04);                            \
		pr_debug(                                                               \
			"high-addr read_value: %08X  expect_value: %08X ",                    \
			read_value, realval_H);                                               \
		if (read_value != realval_H) {                                                \
			pr_debug("  test result:   FAILED! \r\n ");                     \
		} else {                                                                      \
			pr_debug("   test result:   SUCCESS! \r\n");                    \
		}                                                                             \
	} while (0)

static int gdmac_test_registers_dump(zebu_test_mod_t *mod, void *para)
{
	unsigned int reg_base;
	reg_base = mod->reg_base;
	/*comp_one_reg(reg_base, DMAC_IDREG,                         0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_COMPVERREG,			   0x00000000, 0x3130332a);
	comp_one_reg(reg_base, DMAC_CFGREG,                        0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHENREG,                       0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_INTSTATUSREG,                  0x00000000, 0x00000000);
//	comp_one_reg(reg_base, DMAC_COMMONREG_INTCLEARREG,         0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_COMMONREG_INTSTATUS_ENABLEREG, 0xffffffff, 0xffffffff);
	comp_one_reg(reg_base, DMAC_COMMONREG_INTSIGNAL_ENABLEREG, 0xffffffff, 0xffffffff);
	comp_one_reg(reg_base, DMAC_COMMONREG_INTSTATUSREG,        0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_RESETREG,                      0x00000000, 0x00000000);
	//comp_one_reg(reg_base, DMAC_LOWPOWER_CFGREG,               0x00040404, 0x0000000b);

	for ( i=1; i<GDMA_CHANNEL_CNT+1; i++ ){

	comp_one_reg(reg_base, DMAC_CHX_SAR(i),         	         0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_DAR(i),                    0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_BLOCK_TS(i),               0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_CTL(i),               	   0x00000000, 0x00001200);
	comp_one_reg(reg_base, DMAC_CHX_CFG(i),               	   0x00000018 | ((GDMA_CHANNEL_CNT-i)<<17), 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_LLP(i),               	   0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_STATUSREG(i),              0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_SWHSSRCREG(i),             0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_SWHSDSTREG(i),             0x00000000, 0x00000000);
	//comp_one_reg(reg_base, DMAC_CHX_BLK_TFR_RESUMEREQREG(i),   0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_AXI_IDREG(i),              0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_AXI_QOSREG(i),             0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_INTSTATUS_ENABLEREG(i),    0xffffffff, 0xffffffff);
	comp_one_reg(reg_base, DMAC_CHX_INTSTATUS(i),              0x00000000, 0x00000000);
	comp_one_reg(reg_base, DMAC_CHX_INTSIGNAL_ENABLEREG(i),    0xffffffff, 0xffffffff);
	//comp_one_reg(reg_base, DMAC_CHX_INTCLEARREG(i),            0x00000000, 0x00000000);
		
	}*/

	return 0;
}

#define wr_one_reg_test(__base, __offset, __set_h, __exp_h, __set_l, __exp_l)                         \
	do {                                                                                          \
		u32 __temp;                                                                           \
		u32 __errcnt;                                                                         \
		u32 __init_value = REG32_READ(__base + __offset);                                     \
		REG32_WRITE(__base + __offset, __set_l);                                              \
		__temp = (u32)REG32_READ(__base + __offset);                                          \
		__errcnt = ((((u32)__temp) != ((u32)__exp_l)) ? 1 : 0);                               \
		pr_debug(                                                                       \
			"REG:%08X(%-20s) W/R Set-Read(Exp):%08X - %08X(%08X):     result -- %s \r\n", \
			__base + __offset, #__offset, __set_l, __temp,                                \
			__exp_l, (__errcnt ? "Nok" : "ok"));                                          \
		REG32_WRITE(__base + __offset, __init_value);                                         \
		__init_value = REG32_READ(__base + __offset + 0x04);                                  \
		REG32_WRITE(__base + __offset + 0x04, __set_h);                                       \
		__temp = (u32)REG32_READ(__base + __offset + 0x04);                                   \
		__errcnt = ((((u32)__temp) != ((u32)__exp_h)) ? 1 : 0);                               \
		pr_debug(                                                                       \
			"high-addr W/R Set-Read(Exp):%08X - %08X(%08X):     result -- %s \r\n",       \
			__set_h, __temp, __exp_h, (__errcnt ? "Nok" : "ok"));                         \
	} while (0)

void print_address(u64 *address, u32 is_32)
{
	int i;
	if (is_32)
		//for (i = 0; i < DATA_LEN; i++) {
		for (i = 0; i < 20; i++) {
			pr_debug("%08x ", REG32_READ(address + 4 * i));
		}
	else
		//for (i = 0; i < DATA_LEN/2; i++) {
		for (i = 0; i < 20; i++) {
			pr_debug("%08x ", REG64_READ(address + 8 * i));
		}
}

static int gdmac_test_registers_wr(zebu_test_mod_t *mod, void *para)
{
	u32 result = 0;
	int reg_base;
	u32 ret = 0;
	reg_base = mod->reg_base;
	// set_h   &  ~ init_h       // mask_h  &  ~ init_h	  // setl_l  &  ~ init_l       // mask_l  &  ~ init_l
	wr_one_reg_test(reg_base, DMAC_IDREG, 0xffffffff & (~0x00000000),
			0x00000000 & (~0x00000000), 0xffffffff & (~0x00000000),
			0x00000000);
	wr_one_reg_test(reg_base, DMAC_COMPVERREG, 0xffffffff & (~0x00000000),
			0x00000000 & (~0x00000000), 0xffffffff & (~0x3130332a),
			0x3130332a);
	wr_one_reg_test(reg_base, DMAC_CFGREG, 0xffffffff & (~0x00000000),
			0x00000000 & (~0x00000000), 0xffffffff & (~0x00000000),
			0x00000003 & (~0x00000000));
	wr_one_reg_test(reg_base, DMAC_CHENREG, 0xffffffff & (~0x00000000),
			0x00000000, 0xffffffff & (~0x00000000), 0x00000000);
	/*wr_one_reg_test(reg_base, DMAC_INTSTATUSREG,			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) );
	//wr_one_reg_test(reg_base, DMAC_COMMONREG_INTCLEARREG,		0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_COMMONREG_INTSTATUS_ENABLEREG,	0xffffffff & (~ 0xffffffff), 0x00000000 & (~ 0xffffffff), 0xffffffff & (~ 0xffffffff), 0x0000010f & (~ 0xffffffff) );
	wr_one_reg_test(reg_base, DMAC_COMMONREG_INTSIGNAL_ENABLEREG,	0xffffffff & (~ 0xffffffff), 0x00000000 & (~ 0xffffffff), 0xffffffff & (~ 0xffffffff), 0x0000010f & (~ 0xffffffff) );
	wr_one_reg_test(reg_base, DMAC_COMMONREG_INTSTATUSREG,		0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000002 );
	wr_one_reg_test(reg_base, DMAC_RESETREG,				0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 );
	//wr_one_reg_test(reg_base, DMAC_LOWPOWER_CFGREG,			0xffffffff & (~ 0x00040404), 0x00ffffff & (~ 0x00040404), 0xffffffff & (~ 0x0000000b), 0x0000000f & (~ 0x0000000b) );

	for ( i=1; i<GDMA_CHANNEL_CNT+1; i++ ){

	wr_one_reg_test(reg_base, DMAC_CHX_SAR(i),				0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_DAR(i),				0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_BLOCK_TS(i),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x0000001f );
	wr_one_reg_test(reg_base, DMAC_CHX_CTL(i),				0xffffffff & (~ 0x00000000), 0xc7ffffff & (~ 0x00000000), 0xffffffff & (~ 0x00001200), 0x7fc89b50 );
	wr_one_reg_test(reg_base, DMAC_CHX_CFG(i),				0xffffffff & (~ (0x0000001b | ((GDMA_CHANNEL_CNT-i)<<17))), 0x7f8e739f & (~ (0x0000001b | ((GDMA_CHANNEL_CNT-i)<<17))), 0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_LLP(i),				0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffc0 & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_STATUSREG(0),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00010000 );
	wr_one_reg_test(reg_base, DMAC_CHX_SWHSSRCREG(i),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 );
	wr_one_reg_test(reg_base, DMAC_CHX_SWHSDSTREG(i),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 );
	//wr_one_reg_test(reg_base, DMAC_CHX_BLK_TFR_RESUMEREQREG(i),	0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_AXI_IDREG(i),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00070007 );
	wr_one_reg_test(reg_base, DMAC_CHX_AXI_QOSREG(i),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x000000ff & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_SSTAT(i),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_DSTAT(i),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_SSTATAR(i),			0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_DSTATAR(i),			0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0xffffffff & (~ 0x00000000) );
	wr_one_reg_test(reg_base, DMAC_CHX_INTSTATUS_ENABLEREG(i),	0xffffffff & (~ 0xffffffff), 0x00000000 & (~ 0xffffffff), 0xffffffff & (~ 0xffffffff), 0xf83f7ffb & (~ 0xffffffff) );
	wr_one_reg_test(reg_base, DMAC_CHX_INTSTATUS(i),			0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00020000 );
	wr_one_reg_test(reg_base, DMAC_CHX_INTSIGNAL_ENABLEREG(i),	0xffffffff & (~ 0xffffffff), 0x00000000 & (~ 0xffffffff), 0xffffffff & (~ 0xffffffff), 0xf83f7ffb & (~ 0xffffffff) );
	//wr_one_reg_test(reg_base, DMAC_CHX_INTCLEARREG(i),		0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000), 0xffffffff & (~ 0x00000000), 0x00000000 & (~ 0x00000000) );
		
	}*/
	pr_debug("RESET test...\n");
	REG32_WRITE(reg_base + DMAC_RESETREG, 0x1);
	while (1) {
		result = REG32_READ(reg_base + DMAC_RESETREG);
		if ((result & (0x1 << 0)) == 0) {
			pr_debug("software RESET ok...\n");
			break;
		}
		pr_debug("wait RESET... %08x\n", result);
	}
	return ret;
}

static void disable_channel_enable_dma(zebu_test_mod_t *mod)
{
	int reg_base = mod->reg_base;
	u32 reg_data;

	// dmac enable config
	REG32_WRITE(reg_base + DMAC_CFGREG,
		    (REG32_READ(reg_base + DMAC_CFGREG) & 0xfffffffe) |
			    (0x1 << 0)); //enable dma
	REG32_WRITE(reg_base + DMAC_CFGREG,
		    (REG32_READ(reg_base + DMAC_CFGREG) & 0xfffffffd) |
			    (0x1 << 1)); //enable dmac interrupt

	// channel select
	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data =
		((reg_data & (~(0x1 << 0))) |
		 (0x0
		  << 0)); //disable CH1_EN, this bit auto clear after transfer done
	reg_data = (reg_data & (~(0x1 << 8))) |
		   (0x1 << 8); //enable CH1_EN_WE, write only bit
	reg_data =
		(reg_data & (~(0x1 << 1))) |
		(0x0
		 << 1); //disable CH2_EN, this bit auto clear after transfer done
	reg_data = (reg_data & (~(0x1 << 9))) |
		   (0x1 << 9); //enable CH2_EN_WE, write only bit
	reg_data =
		(reg_data & (~(0x1 << 2))) |
		(0x0
		 << 2); //disable CH3_EN, this bit auto clear after transfer done
	reg_data = (reg_data & (~(0x1 << 10))) |
		   (0x1 << 10); //enable CH3_EN_WE, write only bit
	reg_data =
		(reg_data & (~(0x1 << 3))) |
		(0x0
		 << 3); //disable CH4_EN, this bit auto clear after transfer done
	reg_data = (reg_data & (~(0x1 << 11))) |
		   (0x1 << 11); //enable CH4_EN_WE, write only bit
	reg_data =
		(reg_data & (~(0x1 << 5))) |
		(0x0
		 << 5); //disable CH5_EN, this bit auto clear after transfer done
	reg_data = (reg_data & (~(0x1 << 12))) |
		   (0x1 << 12); //enable CH5_EN_WE, write only bit
	reg_data =
		(reg_data & (~(0x1 << 6))) |
		(0x0
		 << 6); //disable CH6_EN, this bit auto clear after transfer done
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); //enable CH6_EN_WE, write only bit
	reg_data =
		(reg_data & (~(0x1 << 7))) |
		(0x0
		 << 7); //disable CH7_EN, this bit auto clear after transfer done
	reg_data = (reg_data & (~(0x1 << 14))) |
		   (0x1 << 14); //enable CH7_EN_WE, write only bit
	reg_data =
		(reg_data & (~(0x1 << 8))) |
		(0x0
		 << 8); //disable CH8_EN, this bit auto clear after transfer done
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); //enable CH8_EN_WE, write only bit
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);
}

static void single_test(int channel, int reg_base, u64 *src, u64 *dst,
			u32 is_32)
{
	u32 reg_data;
	if ((src != SRAM_A55_OUTSIDE) && (src != ERROR_BASE_ADDRESS))
		print_address(src, is_32 & 0x1);

	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel));
	reg_data = (reg_data & (~(0x3 << 0))) |
		   (0x0 << 0); /* src set contiugous */
	reg_data = (reg_data & (~(0x3 << 2))) |
		   (0x0 << 2); /* dst set contiugous */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel) + 0x4, reg_data);

	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel),
		    ((u64)src & 0xffffffff)); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel) + 0x4,
		    ((u64)src >> 32) & 0xffffffff); /* write src address */

	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel),
		    (u64)dst & 0xffffffff); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel) + 0x4,
		    ((u64)dst >> 32) & 0xffffffff); /* write dst address */

	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel),
		    DATA_LEN - 1); /* write block size 1024(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel) + 0x4,
		    0x0); /* write block size 1024(byte) */

	reg_data = REG32_READ(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 30); /* last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0X4, reg_data);

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
					      channel)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(channel), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", channel);

	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data = (reg_data & (~(1 << (channel - 1)))) |
		   (0x1 << (channel -
			    1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (channel + 7)))) |
		   (0x1 << (channel + 7)); /* enable ch_we */
	/*
	 * ***NOTE!***
	 * 		Activation of the channel may result
	 * 	    in SLVIF_WRONCHEN_ERR_IntStat interruption, for unknown reasons.
	 *      DMAC_CHX_INTSTATUS = 0x80000
	 */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);

	while (1) {
		if (flag_block_trans_done != 0)
			pr_debug("block trans done: %d\n",
				flag_block_trans_done);

		if (flag_dma_dst_decode_err_int != 0) {
			pr_debug(
				"*****happen  dst err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_decode_err_int != 0) {
			pr_debug(
				"*****happen  src err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_done_int != 0)
			pr_debug("src trans done: %d\n",
				flag_dma_src_done_int);
		if (flag_dma_dst_done_int != 0)
			pr_debug("dst trans done: %d\n",
				flag_dma_dst_done_int);
		if (flag_dma_trans_done != 0) {
			pr_debug("dma trans done: %d\n",
				flag_dma_trans_done);
			break;
		}
	}
	print_address(dst, is_32 & 0x2);

end:
	pr_debug("\nwait for ch_%d_en auto clear....\n", channel);
	while ((REG32_READ(reg_base + DMAC_CHENREG) & (0x1 << (channel - 1))) !=
	       0x0)
		;
	pr_debug(" ok!\n", channel);
}

static int sdmac_single_test(zebu_test_mod_t *mod, void *para)
{
	int i, blk_num = 3;
	int channel;
	int reg_base = mod->reg_base;

	disable_channel_enable_dma(mod);

	for (channel = 1; channel < GDMA_CHANNEL_CNT + 1; channel++) {
		pr_debug("\n test GDMA channel %d\n", channel);

		/* clear sram and ddr */
		/* cache problem, so... */
		for (i = 0; i < DATA_LEN * blk_num; i++) {
			REG32_WRITE(SRAM_A55_BASE + i * 4, 0x0);
			REG32_WRITE(SRAM_A55_SENCOND + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_BASE + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_SECOND + i * 4, 0x0);
			REG64_WRITE(DDRC1_A55_BASE + i * 8, 0x0);
			REG64_WRITE(DDRC1_A55_SECOND + i * 8, 0x0);
		}

		/* process buf to ddr0 */
		pr_debug("\n****channel %d, start buf to sram base****\n",
			channel);
		single_test(channel, reg_base, (u64 *)buf, SRAM_A55_BASE, 0x3);

		/* process sram to sram */
		pr_debug(
			"\n****channel %d, start sram base to sram second****\n",
			channel);
		single_test(channel, reg_base, SRAM_A55_BASE, SRAM_A55_SENCOND,
			    0x3);

		/* process sram to ddr0 */
		pr_debug(
			"\n****channel %d, start sram second to ddr0 base****\n",
			channel);
		single_test(channel, reg_base, SRAM_A55_SENCOND, DDRC0_A55_BASE,
			    0x3);

		/* process ddr0 to ddr0 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr0 sencond****\n",
			channel);
		single_test(channel, reg_base, DDRC0_A55_BASE, DDRC0_A55_SECOND,
			    0x3);

		/* process ddr0 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr1 base****\n",
			channel);
		single_test(channel, reg_base, DDRC0_A55_BASE, DDRC1_A55_BASE,
			    0x1);

		/* process ddr1 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr1 base  to ddr1 second****\n",
			channel);
		single_test(channel, reg_base, DDRC1_A55_BASE, DDRC1_A55_SECOND,
			    0x0);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start ddr0 base to SRAM outside****\n",
			channel);
		single_test(channel, reg_base, SRAM_A55_BASE, SRAM_A55_OUTSIDE,
			    0x3);

		/* process ddr1 to error addr */
		pr_debug(
			"\n****channel %d, start ddr0 base to error address****\n",
			channel);
		single_test(channel, reg_base, DDRC0_A55_BASE,
			    ERROR_BASE_ADDRESS, 0x3);

		/* process error addr to ddr0 */
		pr_debug(
			"\n****channel %d, start error address to ddr0 base****\n",
			channel);
		single_test(channel, reg_base, ERROR_BASE_ADDRESS,
			    DDRC0_A55_BASE, 0x3);

		/* process ddr0  to error ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base address to ddr1 outside****\n",
			channel);
		single_test(channel, reg_base, DDRC0_A55_BASE, DDR1_A55_OUTSIDE,
			    0x1);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start SRAM outside to  ddr0****\n",
			channel);
		single_test(channel, reg_base, SRAM_A55_OUTSIDE, DDRC0_A55_BASE,
			    0x3);
	}
	pr_debug("\nsingle test finish\n");
	return 0;
}

void print_address_mulit(u64 *address, u32 is_32)
{
	int i;
	if (is_32)
		//for (i = 0; i < DATA_LEN*3; i++) {
		for (i = 0; i < 20; i++) {
			pr_debug("%08x ", REG32_READ(address + 4 * i));
		}
	else
		//for (i = 0; i < (DATA_LEN*3)/2; i++) {
		for (i = 0; i < 20; i++) {
			pr_debug("%08x ", REG64_READ(address + 8 * i));
		}
}

void contiguous_test(int channel, int reg_base, u64 *src, u64 *dst, u32 is_32)
{
	int i, blk_num = 3;
	u32 reg_data;

	if ((src != SRAM_A55_OUTSIDE) && (src != ERROR_BASE_ADDRESS))
		print_address(src, is_32 & 0x1);
	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel));
	/*
	 *  this mode feature:
	 *      CHx_SAR loaded from shadoow
	 *      CHx_DAR contiguous
	 *      CHx_CTL loaded from shadow
	 *      CHx_BLOCK_TS loaded from shadow
	 */
	reg_data = (reg_data & (~(0x3 << 0))) | (0x2 << 0); /* src set shadow */
	reg_data = (reg_data & (~(0x3 << 2))) |
		   (0x0 << 2); /* dst set contiguous */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel) + 0x4, reg_data);

	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel),
		    ((u64)src & 0xffffffff)); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel) + 0x4,
		    ((u64)src >> 32) & 0xffffffff); /* write src address */

	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel),
		    (u64)dst & 0xffffffff); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel) + 0x4,
		    ((u64)dst >> 32) & 0xffffffff); /* write dst address */

	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel),
		    DATA_LEN - 1); /* write block size 16(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel) + 0x4,
		    0x0); /* write block size 16(byte) */

	reg_data = REG32_READ(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data =
		(reg_data & (~(0x1 << 30))) | (0x0 << 30); /* not last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0X4, reg_data);

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
					      channel)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(channel), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", channel);

	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data = (reg_data & (~(1 << (channel - 1)))) |
		   (0x1 << (channel -
			    1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (channel + 7)))) |
		   (0x1 << (channel + 7)); /* enable ch_we */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);

	i = 2;
	while (1) {
		reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) + 0x4);
		if ((reg_data & (0x1 << 31)) == 0x0) {
			pr_debug("\nstart config %d block\n", i);
			REG32_WRITE(reg_base + DMAC_CHX_SAR(channel),
				    ((u64)src &
				     0xffffffff)); /* write src address */
			REG32_WRITE(reg_base + DMAC_CHX_SAR(channel) + 0x4,
				    ((u64)src >> 32) &
					    0xffffffff); /* write src address */
			//REG32_WRITE(reg_base + DMAC_CHX_DAR(channel), dst + DATA_LEN*4*(i-1)); // write dst address
			//REG32_WRITE(reg_base + DMAC_CHX_DAR(channel) + 0x4, 0x0); // write dst address
			REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel),
				    DATA_LEN -
					    1); /* write block size 16(byte) */

			reg_data = REG32_READ(
				reg_base +
				DMAC_CHX_CTL(channel)); // config channel ctl
			reg_data = (reg_data & (~(0x1 << 4))) |
				   (0x0 << 4); /* src address increment */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x0 << 6); /* dst address increment */
			reg_data = (reg_data & (~(0x7 << 8))) |
				   (0x2 << 8); /* src width 32 bit */
			reg_data = (reg_data & (~(0x7 << 11))) |
				   (0x2 << 11); /* dst width 32 bit */
			reg_data = (reg_data & (~(0xf << 14))) |
				   (0x0 << 14); /* src msize 8 byte */
			reg_data = (reg_data & (~(0xf << 18))) |
				   (0x0 << 18); /* dst msize 8 byte */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
			reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
					      0x4); /* config channel ctl */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x1 << 6); /* src burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
			reg_data = (reg_data & (~(0x1 << 15))) |
				   (0x1 << 15); /* dst burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
			reg_data =
				(reg_data & (~(0x1 << 26))) |
				(0x1
				 << 26); /* enable shadow blk done interrupt */
			reg_data = (reg_data & (~(0x1 << 30))) |
				   (0x0 << 30); /* not last block */
			if (i == blk_num)
				reg_data = (reg_data & (~(0x1 << 30))) |
					   (0x1 << 30); /* last block */

			reg_data = (reg_data & (~(0x1 << 31))) |
				   (0x1 << 31); /* shadow register enable */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0x4,
				    reg_data);
			if (flag_dma_lli_shadow_invalid != 0) {
				flag_dma_lli_shadow_invalid = 0;
				pr_debug(
					"\nhappen  shadow or lli invalid, set BLK_TFR_RESUMEREQ*****\n");
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							channel),
					0x1);
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							channel) +
						0x4,
					0x0);
				pr_debug("\nfinish resume channel...\n");
			}

			pr_debug("end config %d block\n", i);
			i++;
		}
		if (flag_block_trans_done != 0)
			pr_debug("block trans done: %d\n",
				flag_block_trans_done);
		if (flag_dma_dst_decode_err_int != 0) {
			pr_debug(
				"*****happen  dst err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_decode_err_int != 0) {
			pr_debug(
				"*****happen  src err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_done_int != 0)
			pr_debug("src trans done: %d\n",
				flag_dma_src_done_int);
		if (flag_dma_dst_done_int != 0)
			pr_debug("dst trans done: %d\n",
				flag_dma_dst_done_int);
		if (flag_dma_trans_done != 0) {
			pr_debug("dma trans done: %d\n",
				flag_dma_trans_done);
			break;
		}
	}
	print_address_mulit(src, is_32 & 0x2);

end:
	pr_debug("\nwait for ch_%d_en auto clear....\n", channel);
	while ((REG32_READ(reg_base + DMAC_CHENREG) & (0x1 << (channel - 1))) !=
	       0x0)
		;
	pr_debug(" ok!\n", channel);
}

/*
 *  **NOTE!**
 *  	contiguous mode cannot be used for both source and destination addresses.
 *      src: shadow        dst: contiguous 
 */

static int sdmac_multi_blk_contiguous_address_test(zebu_test_mod_t *mod,
						   void *para)
{
	int i, blk_num = 3;
	int channel;
	int reg_base = mod->reg_base;

	disable_channel_enable_dma(mod);

	for (channel = 1; channel < GDMA_CHANNEL_CNT + 1; channel++) {
		pr_debug("\n test GDMA channel %d\n", channel);

		/* clear sram and ddr */
		for (i = 0; i < DATA_LEN * blk_num; i++) {
			REG32_WRITE(SRAM_A55_BASE + i * 4, 0x0);
			REG32_WRITE(SRAM_A55_SENCOND + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_BASE + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_SECOND + i * 4, 0x0);
			REG64_WRITE(DDRC1_A55_BASE + i * 8, 0x0);
			REG64_WRITE(DDRC1_A55_SECOND + i * 8, 0x0);
		}

		/* process buf to ddr0 */
		pr_debug("\n****channel %d, start buf to sram base****\n",
			channel);
		contiguous_test(channel, reg_base, (u64 *)buf, SRAM_A55_BASE,
				0x3);

		/* process sram to sram */
		pr_debug(
			"\n****channel %d, start sram base to sram second****\n",
			channel);
		contiguous_test(channel, reg_base, SRAM_A55_BASE,
				SRAM_A55_SENCOND, 0x3);

		/* process sram to ddr0 */
		pr_debug(
			"\n****channel %d, start sram second to ddr0 base****\n",
			channel);
		contiguous_test(channel, reg_base, SRAM_A55_SENCOND,
				DDRC0_A55_BASE, 0x3);

		/* process ddr0 to ddr0 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr0 sencond****\n",
			channel);
		contiguous_test(channel, reg_base, DDRC0_A55_BASE,
				DDRC0_A55_SECOND, 0x3);

		/* process ddr0 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr1 base****\n",
			channel);
		contiguous_test(channel, reg_base, DDRC0_A55_BASE,
				DDRC1_A55_BASE, 0x1);

		/* process ddr1 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr1 base  to ddr1 second****\n",
			channel);
		contiguous_test(channel, reg_base, DDRC1_A55_BASE,
				DDRC1_A55_SECOND, 0x0);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start ddr0 base to SRAM outside****\n",
			channel);
		contiguous_test(channel, reg_base, SRAM_A55_BASE,
				SRAM_A55_OUTSIDE, 0x3);

		/* process ddr1 to error addr */
		pr_debug(
			"\n****channel %d, start ddr0 base to error address****\n",
			channel);
		contiguous_test(channel, reg_base, DDRC0_A55_BASE,
				ERROR_BASE_ADDRESS, 0x3);

		/* process error addr to ddr0 */
		pr_debug(
			"\n****channel %d, start error address to ddr0 base****\n",
			channel);
		contiguous_test(channel, reg_base, ERROR_BASE_ADDRESS,
				DDRC0_A55_BASE, 0x3);

		/* process ddr0  to error ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base address to ddr1 outside****\n",
			channel);
		single_test(channel, reg_base, DDRC0_A55_BASE, DDR1_A55_OUTSIDE,
			    0x1);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start SRAM outside to  ddr0****\n",
			channel);
		single_test(channel, reg_base, SRAM_A55_OUTSIDE, DDRC0_A55_BASE,
			    0x3);
	}
	pr_debug("\ncontiguous test finish\n");
	return 0;
}

static void auto_reload_test(int channel, int reg_base, u64 *src, u64 *dst,
			     u32 is_32)
{
	int i, blk_num = 3;
	u32 reg_data;
	if ((src != SRAM_A55_OUTSIDE) && (src != ERROR_BASE_ADDRESS))
		print_address(src, is_32 & 0x1);
	/* channel config */
	/*
	 *  this mode feature:
	 *      CHx_SAR loaded from auto init value
	 *      CHx_DAR shadow from shadow
	 *      CHx_CTL loaded from shadow
	 *      CHx_BLOCK_TS loaded shadow
	 */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel));
	reg_data = (reg_data & (~(0x3 << 0))) |
		   (0x1 << 0); /* src set auto-reload */
	reg_data = (reg_data & (~(0x3 << 2))) | (0x2 << 2); /* dst shadow */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel) + 0x4, reg_data);

	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel),
		    ((u64)src & 0xffffffff)); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel) + 0x4,
		    ((u64)src >> 32) & 0xffffffff); /* write src address */

	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel),
		    (u64)dst & 0xffffffff); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel) + 0x4,
		    ((u64)dst >> 32) & 0xffffffff); /* write dst address */

	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel),
		    DATA_LEN - 1); /* write block size 16(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel) + 0x4,
		    0x0); /* write block size 16(byte) */

	reg_data = REG32_READ(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data =
		(reg_data & (~(0x1 << 30))) | (0x0 << 30); /* not last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0X4, reg_data);

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
					      channel)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(channel), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", channel);

	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data = (reg_data & (~(1 << (channel - 1)))) |
		   (0x1 << (channel -
			    1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (channel + 7)))) |
		   (0x1 << (channel + 7)); /* enable ch_we */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);

	i = 2;
	while (1) {
		reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) + 0x4);
		if ((reg_data & (0x1 << 31)) == 0x0) {
			pr_debug("\nfinish block trans is %d\n",
				flag_block_trans_done);

			//REG32_WRITE(reg_base + DMAC_CHX_SAR(channel), src); /* write src address */
			//REG32_WRITE(reg_base + DMAC_CHX_SAR(channel) + 0x4, 0x0); /* write src address */
			REG32_WRITE(
				reg_base + DMAC_CHX_DAR(channel),
				((u64)dst & 0xffffffff) +
					DATA_LEN * 4 *
						(i -
						 1)); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_DAR(channel) + 0x4,
				    ((u64)dst >> 32) &
					    0xffffffff); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel),
				    DATA_LEN -
					    1); /* write block size 16(byte) */

			reg_data = REG32_READ(
				reg_base +
				DMAC_CHX_CTL(channel)); // config channel ctl
			reg_data = (reg_data & (~(0x1 << 4))) |
				   (0x0 << 4); /* src address increment */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x0 << 6); /* dst address increment */
			reg_data = (reg_data & (~(0x7 << 8))) |
				   (0x2 << 8); /* src width 32 bit */
			reg_data = (reg_data & (~(0x7 << 11))) |
				   (0x2 << 11); /* dst width 32 bit */
			reg_data = (reg_data & (~(0xf << 14))) |
				   (0x0 << 14); /* src msize 8 byte */
			reg_data = (reg_data & (~(0xf << 18))) |
				   (0x0 << 18); /* dst msize 8 byte */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
			reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
					      0x4); /* config channel ctl */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x1 << 6); /* src burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
			reg_data = (reg_data & (~(0x1 << 15))) |
				   (0x1 << 15); /* dst burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
			reg_data =
				(reg_data & (~(0x1 << 26))) |
				(0x1
				 << 26); /* enable shadow blk done interrupt */
			reg_data = (reg_data & (~(0x1 << 30))) |
				   (0x0 << 30); /* not last block */
			if (i == blk_num)
				reg_data = (reg_data & (~(0x1 << 30))) |
					   (0x1 << 30); /* last block */

			reg_data = (reg_data & (~(0x1 << 31))) |
				   (0x1 << 31); /* shadow register enable */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0x4,
				    reg_data);
			reg_data = REG32_READ(reg_base +
					      DMAC_CHX_INTSTATUS(channel));
			if (flag_dma_lli_shadow_invalid != 0) {
				flag_dma_lli_shadow_invalid = 0;
				pr_debug(
					"\nhappen  shadow or lli invalid, set BLK_TFR_RESUMEREQ*****\n");
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							channel),
					0x1);
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							channel) +
						0x4,
					0x0);
				pr_debug("\nfinish resume channel...\n");
			}

			pr_debug("end config %d block\n", i);
			i++;
		}
		if (flag_block_trans_done != 0)
			pr_debug("block trans done: %d\n",
				flag_block_trans_done);
		if (flag_dma_dst_decode_err_int != 0) {
			pr_debug(
				"*****happen  dst err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_decode_err_int != 0) {
			pr_debug(
				"*****happen  src err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_done_int != 0)
			pr_debug("src trans done: %d\n",
				flag_dma_src_done_int);
		if (flag_dma_dst_done_int != 0)
			pr_debug("dst trans done: %d\n",
				flag_dma_dst_done_int);
		if (flag_dma_trans_done != 0) {
			pr_debug("dma trans done: %d\n",
				flag_dma_trans_done);
			break;
		}
	}
	print_address_mulit(src, is_32 & 0x2);

end:
	pr_debug("\nwait for ch_%d_en auto clear....\n", channel);
	while ((REG32_READ(reg_base + DMAC_CHENREG) & (0x1 << (channel - 1))) !=
	       0x0)
		;
	pr_debug(" ok!\n", channel);
}

static int sdmac_multi_blk_auto_reloading_test(zebu_test_mod_t *mod, void *para)
{
	int i, blk_num = 3;
	int channel;
	int reg_base = mod->reg_base;

	disable_channel_enable_dma(mod);

	for (channel = 1; channel < GDMA_CHANNEL_CNT + 1; channel++) {
		pr_debug("\n test GDMA channel %d\n", channel);

		/* clear sram and ddr */
		for (i = 0; i < DATA_LEN * blk_num; i++) {
			REG32_WRITE(SRAM_A55_BASE + i * 4, 0x0);
			REG32_WRITE(SRAM_A55_SENCOND + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_BASE + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_SECOND + i * 4, 0x0);
			REG64_WRITE(DDRC1_A55_BASE + i * 8, 0x0);
			REG64_WRITE(DDRC1_A55_SECOND + i * 8, 0x0);
		}
		/* process buf to ddr0 */
		pr_debug("\n****channel %d, start buf to sram base****\n",
			channel);
		auto_reload_test(channel, reg_base, (u64 *)buf, SRAM_A55_BASE,
				 0x3);

		/* process sram to sram */
		pr_debug(
			"\n****channel %d, start sram base to sram second****\n",
			channel);
		auto_reload_test(channel, reg_base, SRAM_A55_BASE,
				 SRAM_A55_SENCOND, 0x3);

		/* process sram to ddr0 */
		pr_debug(
			"\n****channel %d, start sram second to ddr0 base****\n",
			channel);
		auto_reload_test(channel, reg_base, SRAM_A55_SENCOND,
				 DDRC0_A55_BASE, 0x3);

		/* process ddr0 to ddr0 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr0 sencond****\n",
			channel);
		auto_reload_test(channel, reg_base, DDRC0_A55_BASE,
				 DDRC0_A55_SECOND, 0x3);

		/* process ddr0 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr1 base****\n",
			channel);
		auto_reload_test(channel, reg_base, DDRC0_A55_BASE,
				 DDRC1_A55_BASE, 0x1);

		/* process ddr1 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr1 base  to ddr1 second****\n",
			channel);
		auto_reload_test(channel, reg_base, DDRC1_A55_BASE,
				 DDRC1_A55_SECOND, 0x0);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start ddr0 base to SRAM outside****\n",
			channel);
		auto_reload_test(channel, reg_base, SRAM_A55_BASE,
				 SRAM_A55_OUTSIDE, 0x3);

		/* process ddr1 to error addr */
		pr_debug(
			"\n****channel %d, start ddr0 base to error address****\n",
			channel);
		auto_reload_test(channel, reg_base, DDRC0_A55_BASE,
				 ERROR_BASE_ADDRESS, 0x3);

		/* process error addr to ddr0 */
		pr_debug(
			"\n****channel %d, start error address to ddr0 base****\n",
			channel);
		auto_reload_test(channel, reg_base, ERROR_BASE_ADDRESS,
				 DDRC0_A55_BASE, 0x3);

		/* process ddr0  to error ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base address to ddr1 outside****\n",
			channel);
		single_test(channel, reg_base, DDRC0_A55_BASE, DDR1_A55_OUTSIDE,
			    0x1);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start SRAM outside to  ddr0****\n",
			channel);
		single_test(channel, reg_base, SRAM_A55_OUTSIDE, DDRC0_A55_BASE,
			    0x3);
	}
	pr_debug("\nauto relaod test finish\n");
	return 0;
}

static void shadow_test(int channel, int reg_base, u64 *src, u64 *dst,
			u32 is_32)
{
	int i, blk_num = 3;
	u32 reg_data;
	if ((src != SRAM_A55_OUTSIDE) && (src != ERROR_BASE_ADDRESS))
		print_address(src, is_32 & 0x1);
	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel));
	reg_data = (reg_data & (~(0x3 << 0))) | (0x2 << 0); /* src set shadow */
	reg_data = (reg_data & (~(0x3 << 2))) | (0x2 << 2); /* dst set shadow */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel) + 0x4, reg_data);

	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel),
		    ((u64)src & 0xffffffff)); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel) + 0x4,
		    ((u64)src >> 32) & 0xffffffff); /* write src address */

	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel),
		    (u64)dst & 0xffffffff); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel) + 0x4,
		    ((u64)dst >> 32) & 0xffffffff); /* write dst address */

	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel),
		    DATA_LEN - 1); /* write block size 16(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel) + 0x4,
		    0x0); /* write block size 16(byte) */

	reg_data = REG32_READ(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data =
		(reg_data & (~(0x1 << 30))) | (0x0 << 30); /* not last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0X4, reg_data);

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
					      channel)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(channel), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", channel);

	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data = (reg_data & (~(1 << (channel - 1)))) |
		   (0x1 << (channel -
			    1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (channel + 7)))) |
		   (0x1 << (channel + 7)); /* enable ch_we */
	/*
	 * ***NOTE!***
	 * 		Activation of the channel may result
	 * 	    in SLVIF_WRONCHEN_ERR_IntStat interruption, for unknown reasons.
	 *      DMAC_CHX_INTSTATUS = 0x80000
	 */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);

	i = 2;
	while (1) {
		reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) + 0x4);
		if ((reg_data & (0x1 << 31)) == 0x0) {
			pr_debug("\nstart config %d block\n", i);
			REG32_WRITE(reg_base + DMAC_CHX_SAR(channel),
				    ((u64)src &
				     0xffffffff)); /* write src address */
			REG32_WRITE(reg_base + DMAC_CHX_SAR(channel) + 0x4,
				    ((u64)src >> 32) &
					    0xffffffff); /* write src address */
			REG32_WRITE(
				reg_base + DMAC_CHX_DAR(channel),
				((u64)dst & 0xffffffff) +
					DATA_LEN * 4 *
						(i -
						 1)); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_DAR(channel) + 0x4,
				    ((u64)dst >> 32) &
					    0xffffffff); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel),
				    DATA_LEN -
					    1); /* write block size 16(byte) */

			reg_data = REG32_READ(
				reg_base +
				DMAC_CHX_CTL(channel)); // config channel ctl
			reg_data = (reg_data & (~(0x1 << 4))) |
				   (0x0 << 4); /* src address increment */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x0 << 6); /* dst address increment */
			reg_data = (reg_data & (~(0x7 << 8))) |
				   (0x2 << 8); /* src width 32 bit */
			reg_data = (reg_data & (~(0x7 << 11))) |
				   (0x2 << 11); /* dst width 32 bit */
			reg_data = (reg_data & (~(0xf << 14))) |
				   (0x0 << 14); /* src msize 8 byte */
			reg_data = (reg_data & (~(0xf << 18))) |
				   (0x0 << 18); /* dst msize 8 byte */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
			reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
					      0x4); /* config channel ctl */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x1 << 6); /* src burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
			reg_data = (reg_data & (~(0x1 << 15))) |
				   (0x1 << 15); /* dst burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
			reg_data =
				(reg_data & (~(0x1 << 26))) |
				(0x1
				 << 26); /* enable shadow blk done interrupt */
			reg_data = (reg_data & (~(0x1 << 30))) |
				   (0x0 << 30); /* not last block */
			if (i == blk_num)
				reg_data = (reg_data & (~(0x1 << 30))) |
					   (0x1 << 30); /* last block */

			reg_data = (reg_data & (~(0x1 << 31))) |
				   (0x1 << 31); /* shadow register enable */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0x4,
				    reg_data);
			if (flag_dma_lli_shadow_invalid != 0) {
				flag_dma_lli_shadow_invalid = 0;
				pr_debug(
					"\nhappen  shadow or lli invalid, set BLK_TFR_RESUMEREQ*****\n");
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							channel),
					0x1);
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							channel) +
						0x4,
					0x0);
				pr_debug("\nfinish resume channel...\n");
			}

			pr_debug("end config %d block\n", i);
			i++;
		}
		if (flag_block_trans_done != 0)
			pr_debug("block trans done: %d\n",
				flag_block_trans_done);
		if (flag_dma_dst_decode_err_int != 0) {
			pr_debug(
				"*****happen  dst err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_decode_err_int != 0) {
			pr_debug(
				"*****happen  src err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_done_int != 0)
			pr_debug("src trans done: %d\n",
				flag_dma_src_done_int);
		if (flag_dma_dst_done_int != 0)
			pr_debug("dst trans done: %d\n",
				flag_dma_dst_done_int);
		if (flag_dma_trans_done != 0) {
			pr_debug("dma trans done: %d\n",
				flag_dma_trans_done);
			break;
		}
	}
	print_address_mulit(src, is_32 & 0x2);

end:
	pr_debug("\nwait for ch_%d_en auto clear....\n", channel);
	while ((REG32_READ(reg_base + DMAC_CHENREG) & (0x1 << (channel - 1))) !=
	       0x0)
		;
	pr_debug(" ok!\n", channel);
}

static int sdmac_multi_blk_shadow_test(zebu_test_mod_t *mod, void *para)
{
	int i, blk_num = 3;
	int channel;
	int reg_base = mod->reg_base;

	disable_channel_enable_dma(mod);

	for (channel = 1; channel < GDMA_CHANNEL_CNT + 1; channel++) {
		pr_debug("\n test GDMA channel %d\n", channel);

		/* clear sram and ddr */
		for (i = 0; i < DATA_LEN * blk_num; i++) {
			REG32_WRITE(SRAM_A55_BASE + i * 4, 0x0);
			REG32_WRITE(SRAM_A55_SENCOND + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_BASE + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_SECOND + i * 4, 0x0);
			REG64_WRITE(DDRC1_A55_BASE + i * 8, 0x0);
			REG64_WRITE(DDRC1_A55_SECOND + i * 8, 0x0);
		}
		/* process buf to ddr0 */
		pr_debug("\n****channel %d, start buf to sram base****\n",
			channel);
		shadow_test(channel, reg_base, (u64 *)buf, SRAM_A55_BASE, 0x3);

		/* process sram to sram */
		pr_debug(
			"\n****channel %d, start sram base to sram second****\n",
			channel);
		shadow_test(channel, reg_base, SRAM_A55_BASE, SRAM_A55_SENCOND,
			    0x3);

		/* process sram to ddr0 */
		pr_debug(
			"\n****channel %d, start sram second to ddr0 base****\n",
			channel);
		shadow_test(channel, reg_base, SRAM_A55_SENCOND, DDRC0_A55_BASE,
			    0x3);

		/* process ddr0 to ddr0 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr0 sencond****\n",
			channel);
		shadow_test(channel, reg_base, DDRC0_A55_BASE, DDRC0_A55_SECOND,
			    0x3);

		/* process ddr0 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr1 base****\n",
			channel);
		shadow_test(channel, reg_base, DDRC0_A55_BASE, DDRC1_A55_BASE,
			    0x1);

		/* process ddr1 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr1 base  to ddr1 second****\n",
			channel);
		shadow_test(channel, reg_base, DDRC1_A55_BASE, DDRC1_A55_SECOND,
			    0x0);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start ddr0 base to SRAM outside****\n",
			channel);
		shadow_test(channel, reg_base, SRAM_A55_BASE, SRAM_A55_OUTSIDE,
			    0x3);

		/* process ddr1 to error addr */
		pr_debug(
			"\n****channel %d, start ddr0 base to error address****\n",
			channel);
		shadow_test(channel, reg_base, DDRC0_A55_BASE,
			    ERROR_BASE_ADDRESS, 0x3);

		/* process error addr to ddr0 */
		pr_debug(
			"\n****channel %d, start error address to ddr0 base****\n",
			channel);
		shadow_test(channel, reg_base, ERROR_BASE_ADDRESS,
			    DDRC0_A55_BASE, 0x3);

		/* process ddr0  to error ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base address to ddr1 outside****\n",
			channel);
		single_test(channel, reg_base, DDRC0_A55_BASE, DDR1_A55_OUTSIDE,
			    0x1);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start SRAM outside to  ddr0****\n",
			channel);
		single_test(channel, reg_base, SRAM_A55_OUTSIDE, DDRC0_A55_BASE,
			    0x3);
	}
	pr_debug("\nshadow test finish\n");
	return 0;
}

struct lli_struct lli __attribute__((aligned(64)));
struct lli_struct lli_next __attribute__((aligned(64)));

static void ll_test(int channel, int reg_base, u64 *src, u64 *dst, u32 is_32)
{
	int i;
	u32 reg_data;

	memset(&lli, 0, sizeof(lli));
	memset(&lli_next, 0, sizeof(lli));
	if ((src != SRAM_A55_OUTSIDE) && (src != ERROR_BASE_ADDRESS))
		print_address(src, is_32 & 0x1);

	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel));
	reg_data = (reg_data & (~(0x3 << 0))) | (0x3 << 0); /* src set ll */
	reg_data = (reg_data & (~(0x3 << 2))) | (0x3 << 2); /* dst set ll */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel) + 0x4, reg_data);

	lli.chx_sar_l = ((u64)src & 0xffffffff); /* write src address */
	lli.chx_sar_h = ((u64)src >> 32) & 0xffffffff; /* write src address */
	lli.chx_dar_l = (u64)dst & 0xffffffff; /* write dst address */
	lli.chx_dar_h = ((u64)dst >> 32) & 0xffffffff; /* write dst address */

	lli.chx_block_ts = DATA_LEN - 1; /* write block size 1024(byte) */
	reg_data = REG32_READ(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	//REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
	lli.chx_ctl_l = reg_data;
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);

	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data =
		(reg_data & (~(0x1 << 30))) | (0x0 << 30); /* not last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	//REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0x4, reg_data);
	lli.chx_ctl_h = reg_data;

	lli.chx_llp_l = ((u64)(&lli_next) & 0xffffffff);
	lli.chx_llp_h = (((u64)(&lli_next) >> 32) & 0xffffffff);
	REG32_WRITE(reg_base + DMAC_CHX_LLP(channel),
		    ((u64)(&lli) & 0xffffffff));
	REG32_WRITE(reg_base + DMAC_CHX_LLP(channel) + 0x4,
		    (((u64)(&lli) >> 32) & 0xffffffff));

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
					      channel)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(channel), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", channel);

	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data = (reg_data & (~(1 << (channel - 1)))) |
		   (0x1 << (channel -
			    1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (channel + 7)))) |
		   (0x1 << (channel + 7)); /* enable ch_we */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);

	i = 2;
	while (1) {
		reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) + 0x4);
		if ((reg_data & (0x1 << 31)) == 0x0) {
			pr_debug("\nstart config %d block...\n", i);
			if (i == 2) {
				memset(&lli, 0, sizeof(lli));
				lli_next.chx_sar_l =
					((u64)src &
					 0xffffffff); /* write src address */
				lli_next.chx_sar_h =
					((u64)src >> 32) &
					0xffffffff; /* write src address */
				lli_next.chx_dar_l =
					((u64)dst & 0xffffffff) +
					DATA_LEN *
						(i - 1); /* write dst address */
				lli_next.chx_dar_h =
					((u64)dst >> 32) &
					0xffffffff; /* write dst address */
				lli_next.chx_block_ts =
					DATA_LEN -
					1; /* write block size 16(byte) */
				lli_next.chx_llp_l = ((u64)(&lli) & 0xffffffff);
				lli_next.chx_llp_h =
					(((u64)(&lli) >> 32) & 0xffffffff);
				reg_data = REG32_READ(reg_base +
						      DMAC_CHX_CTL(channel));
				reg_data =
					(reg_data & (~(0x1 << 4))) |
					(0x0 << 4); /* src address increment */
				reg_data =
					(reg_data & (~(0x1 << 6))) |
					(0x0 << 6); /* dst address increment */
				reg_data = (reg_data & (~(0x7 << 8))) |
					   (0x2 << 8); /* src width 32 bit */
				reg_data = (reg_data & (~(0x7 << 11))) |
					   (0x2 << 11); /* dst width 32 bit */
				reg_data = (reg_data & (~(0xf << 14))) |
					   (0x0 << 14); /* src msize 8 byte */
				reg_data = (reg_data & (~(0xf << 18))) |
					   (0x0 << 18); /* dst msize 8 byte */
				//REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
				lli_next.chx_ctl_l = reg_data;
				reg_data = REG32_READ(
					reg_base + DMAC_CHX_CTL(channel) +
					0x4); /* config channel ctl */
				reg_data =
					(reg_data & (~(0x1 << 6))) |
					(0x1 << 6); /* src burst len enable */
				/* 
				* TODO src burst length AXI
				*/
				reg_data = (reg_data & (~(0xff << 7))) |
					   (0x1 << 7);
				reg_data =
					(reg_data & (~(0x1 << 15))) |
					(0x1 << 15); /* dst burst len enable */
				/* 
				* TODO src burst length AXI
				*/
				reg_data = (reg_data & (~(0xff << 16))) |
					   (0x1 << 16);
				reg_data =
					(reg_data & (~(0x1 << 26))) |
					(0x1
					 << 26); /* enable shadow blk done interrupt */
				reg_data = (reg_data & (~(0x1 << 30))) |
					   (0x0 << 30); /* not last block */

				reg_data = (reg_data & (~(0x1 << 31))) |
					   (0x1
					    << 31); /* shadow register enable */
				//REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0x4, reg_data);
				lli_next.chx_ctl_h = reg_data;
			} else if (i == 3) {
				memset(&lli_next, 0, sizeof(lli));
				lli.chx_sar_l =
					((u64)src &
					 0xffffffff); /* write src address */
				lli.chx_sar_h =
					((u64)src >> 32) &
					0xffffffff; /* write src address */
				lli.chx_dar_l =
					((u64)dst & 0xffffffff) +
					DATA_LEN *
						(i - 1); /* write dst address */
				lli.chx_dar_h =
					((u64)dst >> 32) &
					0xffffffff; /* write dst address */
				lli.chx_block_ts =
					DATA_LEN -
					1; /* write block size 16(byte) */
				lli.chx_llp_l = ((u64)(&lli_next) & 0xffffffff);
				lli.chx_llp_h =
					(((u64)(&lli_next) >> 32) & 0xffffffff);
				reg_data = REG32_READ(reg_base +
						      DMAC_CHX_CTL(channel));
				reg_data =
					(reg_data & (~(0x1 << 4))) |
					(0x0 << 4); /* src address increment */
				reg_data =
					(reg_data & (~(0x1 << 6))) |
					(0x0 << 6); /* dst address increment */
				reg_data = (reg_data & (~(0x7 << 8))) |
					   (0x2 << 8); /* src width 32 bit */
				reg_data = (reg_data & (~(0x7 << 11))) |
					   (0x2 << 11); /* dst width 32 bit */
				reg_data = (reg_data & (~(0xf << 14))) |
					   (0x0 << 14); /* src msize 8 byte */
				reg_data = (reg_data & (~(0xf << 18))) |
					   (0x0 << 18); /* dst msize 8 byte */
				//REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
				lli.chx_ctl_l = reg_data;
				reg_data = REG32_READ(
					reg_base + DMAC_CHX_CTL(channel) +
					0x4); /* config channel ctl */
				reg_data =
					(reg_data & (~(0x1 << 6))) |
					(0x1 << 6); /* src burst len enable */
				/* 
				* TODO src burst length AXI
				*/
				reg_data = (reg_data & (~(0xff << 7))) |
					   (0x1 << 7);
				reg_data =
					(reg_data & (~(0x1 << 15))) |
					(0x1 << 15); /* dst burst len enable */
				/* 
				* TODO src burst length AXI
				*/
				reg_data = (reg_data & (~(0xff << 16))) |
					   (0x1 << 16);
				reg_data =
					(reg_data & (~(0x1 << 26))) |
					(0x1
					 << 26); /* enable shadow blk done interrupt */
				reg_data = (reg_data & (~(0x1 << 30))) |
					   (0x1 << 30); /* last block */
				reg_data = (reg_data & (~(0x1 << 31))) |
					   (0x1
					    << 31); /* shadow register enable */
				//REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0x4, reg_data);
				lli.chx_ctl_h = reg_data;
			}
			if (flag_dma_lli_shadow_invalid != 0) {
				flag_dma_lli_shadow_invalid = 0;
				pr_debug(
					"\nhappen  shadow or lli invalid, set BLK_TFR_RESUMEREQ*****\n");
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							channel),
					0x1);
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							channel) +
						0x4,
					0x0);
				pr_debug("\nfinish resume channel...\n");
			}
			pr_debug("end config %d block\n", i);
			i++;
		}
		if (flag_block_trans_done != 0)
			pr_debug("block trans done: %d\n",
				flag_block_trans_done);
		if (flag_dma_dst_decode_err_int != 0) {
			pr_debug(
				"*****happen  dst err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_decode_err_int != 0) {
			pr_debug(
				"*****happen  src err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_done_int != 0)
			pr_debug("src trans done: %d\n",
				flag_dma_src_done_int);
		if (flag_dma_dst_done_int != 0)
			pr_debug("dst trans done: %d\n",
				flag_dma_dst_done_int);
		if (flag_dma_trans_done != 0) {
			pr_debug("dma trans done: %d\n",
				flag_dma_trans_done);
			break;
		}
	}
	print_address_mulit(src, is_32 & 0x2);

end:
	pr_debug("\nwait for ch_%d_en auto clear....\n", channel);
	while ((REG32_READ(reg_base + DMAC_CHENREG) & (0x1 << (channel - 1))) !=
	       0x0)
		;
	pr_debug(" ok!\n", channel);
}

static int sdmac_multi_blk_ll_test(zebu_test_mod_t *mod, void *para)
{
	int i, blk_num = 3;
	int channel;
	int reg_base = mod->reg_base;

	disable_channel_enable_dma(mod);

	for (channel = 1; channel < GDMA_CHANNEL_CNT + 1; channel++) {
		pr_debug("\n test GDMA channel %d\n", channel);

		/* clear sram and ddr */
		for (i = 0; i < DATA_LEN * blk_num; i++) {
			REG32_WRITE(SRAM_A55_BASE + i * 4, 0x0);
			REG32_WRITE(SRAM_A55_SENCOND + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_BASE + i * 4, 0x0);
			REG32_WRITE(DDRC0_A55_SECOND + i * 4, 0x0);
			REG64_WRITE(DDRC1_A55_BASE + i * 8, 0x0);
			REG64_WRITE(DDRC1_A55_SECOND + i * 8, 0x0);
		}
		/* process buf to ddr0 */
		pr_debug("\n****channel %d, start buf to sram base****\n",
			channel);
		ll_test(channel, reg_base, (u64 *)buf, SRAM_A55_BASE, 0x3);

		/* process sram to sram */
		pr_debug(
			"\n****channel %d, start sram base to sram second****\n",
			channel);
		ll_test(channel, reg_base, SRAM_A55_BASE, SRAM_A55_SENCOND,
			0x3);

		/* process sram to ddr0 */
		pr_debug(
			"\n****channel %d, start sram second to ddr0 base****\n",
			channel);
		ll_test(channel, reg_base, SRAM_A55_SENCOND, DDRC0_A55_BASE,
			0x3);

		/* process ddr0 to ddr0 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr0 sencond****\n",
			channel);
		ll_test(channel, reg_base, DDRC0_A55_BASE, DDRC0_A55_SECOND,
			0x3);

		/* process ddr0 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base to ddr1 base****\n",
			channel);
		ll_test(channel, reg_base, DDRC0_A55_BASE, DDRC1_A55_BASE, 0x1);

		/* process ddr1 to ddr1 */
		pr_debug(
			"\n****channel %d, start ddr1 base  to ddr1 second****\n",
			channel);
		ll_test(channel, reg_base, DDRC1_A55_BASE, DDRC1_A55_SECOND,
			0x0);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start ddr0 base to SRAM outside****\n",
			channel);
		ll_test(channel, reg_base, SRAM_A55_BASE, SRAM_A55_OUTSIDE,
			0x3);

		/* process ddr1 to error addr */
		pr_debug(
			"\n****channel %d, start ddr0 base to error address****\n",
			channel);
		ll_test(channel, reg_base, DDRC0_A55_BASE, ERROR_BASE_ADDRESS,
			0x3);

		/* process error addr to ddr0 */
		pr_debug(
			"\n****channel %d, start error address to ddr0 base****\n",
			channel);
		ll_test(channel, reg_base, ERROR_BASE_ADDRESS, DDRC0_A55_BASE,
			0x3);

		/* process ddr0  to error ddr1 */
		pr_debug(
			"\n****channel %d, start ddr0 base address to ddr1 outside****\n",
			channel);
		single_test(channel, reg_base, DDRC0_A55_BASE, DDR1_A55_OUTSIDE,
			    0x1);

		/* process ddr1 to outside */
		pr_debug(
			"\n****channel %d, start SRAM outside to  ddr0****\n",
			channel);
		single_test(channel, reg_base, SRAM_A55_OUTSIDE, DDRC0_A55_BASE,
			    0x3);
	}

	pr_debug("\nll test finish\n");
	return 0;
}

static int sdmac_dma_channel_lock_test(zebu_test_mod_t *mod, void *para)
{
	int i, blk_num = 3;
	u32 reg_data;
	int reg_base = mod->reg_base;

	disable_channel_enable_dma(mod);
	pr_debug("start channel lock test...\n");

	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(1));
	reg_data = (reg_data & (~(0x3 << 0))) | (0x2 << 0); /* src set shadow */
	reg_data = (reg_data & (~(0x3 << 2))) | (0x2 << 2); /* dst set shadow */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(1), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(1) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x1 << 20); /* en ch lock */
	reg_data =
		(reg_data & (~(0xf << 27))) | (0x0 << 21); /* select dma lock */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */

	REG32_WRITE(reg_base + DMAC_CHX_CFG(1) + 0x4, reg_data);

	REG32_WRITE(reg_base + DMAC_CHX_SAR(1),
		    (u64)buf); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(1) + 0x4,
		    0x0); /* write src address */

	REG32_WRITE(reg_base + DMAC_CHX_DAR(1),
		    (u64)DDRC0_A55_BASE); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(1) + 0x4,
		    0x0); /* write dst address */

	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(1),
		    DATA_LEN - 1); /* write block size 16(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(1) + 0x4,
		    0x0); /* write block size 16(byte) */

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_CTL(1)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(1), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(1) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data =
		(reg_data & (~(0x1 << 30))) | (0x0 << 30); /* not last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(1) + 0X4, reg_data);

	reg_data = REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
						 1)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(1), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", 1);

	/* channel 2*/
	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(2));
	reg_data = (reg_data & (~(0x3 << 0))) |
		   (0x0 << 0); /* src set contiugous */
	reg_data = (reg_data & (~(0x3 << 2))) |
		   (0x0 << 2); /* dst set contiugous */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(2), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(2) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(2) + 0x4, reg_data);

	REG32_WRITE(reg_base + DMAC_CHX_SAR(2),
		    (u64)(DDRC0_A55_BASE + 0x4000)); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(2) + 0x4,
		    0x0); /* write src address */

	REG32_WRITE(reg_base + DMAC_CHX_DAR(2),
		    (u64)DDRC0_A55_SECOND); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(2) + 0x4,
		    0x0); /* write dst address */

	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(2),
		    DATA_LEN - 1); /* write block size 1024(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(2) + 0x4,
		    0x0); /* write block size 1024(byte) */

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_CTL(2)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(2), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(2) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 30); /* last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(2) + 0X4, reg_data);

	reg_data = REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
						 2)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(2), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", 2);

	/*
	 * ***NOTE!***
	 * 		Activation of the channel may result
	 * 	    in SLVIF_WRONCHEN_ERR_IntStat interruption, for unknown reasons.
	 *      DMAC_CHX_INTSTATUS = 0x80000
	 */
	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data =
		(reg_data & (~(1 << (1 - 1)))) |
		(0x1 << (1 - 1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (1 + 7)))) |
		   (0x1 << (1 + 7)); /* enable ch_we */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);
	reg_data =
		(reg_data & (~(1 << (2 - 1)))) |
		(0x1 << (2 - 1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (2 + 7)))) |
		   (0x1 << (2 + 7)); /* enable ch_we */
	/*
	 * ***NOTE!***
	 * 		Activation of the channel may result
	 * 	    in SLVIF_WRONCHEN_ERR_IntStat interruption, for unknown reasons.
	 *      DMAC_CHX_INTSTATUS = 0x80000
	 */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);

	i = 2;
	while (1) {
		reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(1) + 0x4);
		if ((reg_data & (0x1 << 31)) == 0x0) {
			pr_debug("\nstart config %d block\n", i);
			REG32_WRITE(reg_base + DMAC_CHX_SAR(1),
				    (u64)buf); /* write src address */
			REG32_WRITE(reg_base + DMAC_CHX_SAR(1) + 0x4,
				    0x0); /* write src address */
			REG32_WRITE(reg_base + DMAC_CHX_DAR(1),
				    (u64)DDRC0_A55_BASE + 0x8000); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_DAR(1) + 0x4,
				    0x0); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(1),
				    DATA_LEN -
					    1); /* write block size 16(byte) */

			reg_data = REG32_READ(
				reg_base +
				DMAC_CHX_CTL(1)); // config channel ctl
			reg_data = (reg_data & (~(0x1 << 4))) |
				   (0x0 << 4); /* src address increment */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x0 << 6); /* dst address increment */
			reg_data = (reg_data & (~(0x7 << 8))) |
				   (0x2 << 8); /* src width 32 bit */
			reg_data = (reg_data & (~(0x7 << 11))) |
				   (0x2 << 11); /* dst width 32 bit */
			reg_data = (reg_data & (~(0xf << 14))) |
				   (0x0 << 14); /* src msize 8 byte */
			reg_data = (reg_data & (~(0xf << 18))) |
				   (0x0 << 18); /* dst msize 8 byte */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(1), reg_data);
			reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(1) +
					      0x4); /* config channel ctl */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x1 << 6); /* src burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
			reg_data = (reg_data & (~(0x1 << 15))) |
				   (0x1 << 15); /* dst burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
			reg_data =
				(reg_data & (~(0x1 << 26))) |
				(0x1
				 << 26); /* enable shadow blk done interrupt */
			reg_data = (reg_data & (~(0x1 << 30))) |
				   (0x0 << 30); /* not last block */
			if (i == blk_num)
				reg_data = (reg_data & (~(0x1 << 30))) |
					   (0x1 << 30); /* last block */

			reg_data = (reg_data & (~(0x1 << 31))) |
				   (0x1 << 31); /* shadow register enable */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(1) + 0x4, reg_data);
			if (flag_dma_lli_shadow_invalid != 0) {
				flag_dma_lli_shadow_invalid = 0;
				pr_debug(
					"\nhappen  shadow or lli invalid, set BLK_TFR_RESUMEREQ*****\n");
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							1),
					0x1);
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							1) +
						0x4,
					0x0);
				pr_debug("\nfinish resume channel...\n");
			}

			pr_debug("end config %d block\n", i);
			i++;
		}
		if (flag_block_trans_done != 0)
			pr_debug("block trans done: %d\n",
				flag_block_trans_done);
		if (flag_dma_dst_decode_err_int != 0) {
			pr_debug(
				"*****happen  dst err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_decode_err_int != 0) {
			pr_debug(
				"*****happen  src err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_done_int != 0)
			pr_debug("src trans done: %d\n",
				flag_dma_src_done_int);
		if (flag_dma_dst_done_int != 0)
			pr_debug("dst trans done: %d\n",
				flag_dma_dst_done_int);
		if (flag_dma_trans_done != 0) {
			pr_debug("dma trans done: %d\n",
				flag_dma_trans_done);
			break;
		}
	}

end:
	pr_debug("\nwait for ch_%d_en auto clear....\n", 1);
	while ((REG32_READ(reg_base + DMAC_CHENREG) & (0x1 << (1 - 1))) != 0x0)
		;
	pr_debug(" ok!\n", 1);
	return 0;
}

static int sdmac_block_channel_lock_test(zebu_test_mod_t *mod, void *para)
{
	int i, blk_num = 3;
	u32 reg_data;
	int reg_base = mod->reg_base;

	disable_channel_enable_dma(mod);
	pr_debug("start channel lock test...\n");

	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(1));
	reg_data = (reg_data & (~(0x3 << 0))) | (0x2 << 0); /* src set shadow */
	reg_data = (reg_data & (~(0x3 << 2))) | (0x2 << 2); /* dst set shadow */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(1), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(1) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x1 << 20); /* en ch lock */
	reg_data = (reg_data & (~(0xf << 27))) |
		   (0x1 << 21); /* select block lock */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */

	REG32_WRITE(reg_base + DMAC_CHX_CFG(1) + 0x4, reg_data);

	REG32_WRITE(reg_base + DMAC_CHX_SAR(1),
		    (u64)(buf)); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(1) + 0x4,
		    0x0); /* write src address */

	REG32_WRITE(reg_base + DMAC_CHX_DAR(1),
		    (u64)DDRC0_A55_BASE); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(1) + 0x4,
		    0x0); /* write dst address */

	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(1),
		    DATA_LEN - 1); /* write block size 16(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(1) + 0x4,
		    0x0); /* write block size 16(byte) */

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_CTL(1)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(1), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(1) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data =
		(reg_data & (~(0x1 << 30))) | (0x0 << 30); /* not last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(1) + 0X4, reg_data);

	reg_data = REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
						 1)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(1), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", 1);

	/* channel 2*/
	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(2));
	reg_data = (reg_data & (~(0x3 << 0))) |
		   (0x0 << 0); /* src set contiugous */
	reg_data = (reg_data & (~(0x3 << 2))) |
		   (0x0 << 2); /* dst set contiugous */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(2), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(2) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(2) + 0x4, reg_data);

	REG32_WRITE(reg_base + DMAC_CHX_SAR(2),
		    (u64)(DDRC0_A55_BASE + 0x40000)); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(2) + 0x4,
		    0x0); /* write src address */

	REG32_WRITE(reg_base + DMAC_CHX_DAR(2),
		    (u64)DDRC0_A55_SECOND); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(2) + 0x4,
		    0x0); /* write dst address */

	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(2),
		    DATA_LEN - 1); /* write block size 1024(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(2) + 0x4,
		    0x0); /* write block size 1024(byte) */

	reg_data =
		REG32_READ(reg_base + DMAC_CHX_CTL(2)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(2), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(2) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 30); /* last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(2) + 0X4, reg_data);

	reg_data = REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
						 2)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(2), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", 2);

	/*
	 * ***NOTE!***
	 * 		Activation of the channel may result
	 * 	    in SLVIF_WRONCHEN_ERR_IntStat interruption, for unknown reasons.
	 *      DMAC_CHX_INTSTATUS = 0x80000
	 */
	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data =
		(reg_data & (~(1 << (1 - 1)))) |
		(0x1 << (1 - 1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (1 + 7)))) |
		   (0x1 << (1 + 7)); /* enable ch_we */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);
	reg_data =
		(reg_data & (~(1 << (2 - 1)))) |
		(0x1 << (2 - 1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (2 + 7)))) |
		   (0x1 << (2 + 7)); /* enable ch_we */
	/*
	 * ***NOTE!***
	 * 		Activation of the channel may result
	 * 	    in SLVIF_WRONCHEN_ERR_IntStat interruption, for unknown reasons.
	 *      DMAC_CHX_INTSTATUS = 0x80000
	 */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);

	i = 2;
	while (1) {
		reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(1) + 0x4);
		if ((reg_data & (0x1 << 31)) == 0x0) {
			pr_debug("\nstart config %d block\n", i);
			REG32_WRITE(reg_base + DMAC_CHX_SAR(1),
				    (u64)buf); /* write src address */
			REG32_WRITE(reg_base + DMAC_CHX_SAR(1) + 0x4,
				    0x0); /* write src address */
			REG32_WRITE(reg_base + DMAC_CHX_DAR(1),
				    (u64)DDRC0_A55_BASE+ 0x8000); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_DAR(1) + 0x4,
				    0x0); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(1),
				    DATA_LEN -
					    1); /* write block size 16(byte) */

			reg_data = REG32_READ(
				reg_base +
				DMAC_CHX_CTL(1)); // config channel ctl
			reg_data = (reg_data & (~(0x1 << 4))) |
				   (0x0 << 4); /* src address increment */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x0 << 6); /* dst address increment */
			reg_data = (reg_data & (~(0x7 << 8))) |
				   (0x2 << 8); /* src width 32 bit */
			reg_data = (reg_data & (~(0x7 << 11))) |
				   (0x2 << 11); /* dst width 32 bit */
			reg_data = (reg_data & (~(0xf << 14))) |
				   (0x0 << 14); /* src msize 8 byte */
			reg_data = (reg_data & (~(0xf << 18))) |
				   (0x0 << 18); /* dst msize 8 byte */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(1), reg_data);
			reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(1) +
					      0x4); /* config channel ctl */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x1 << 6); /* src burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
			reg_data = (reg_data & (~(0x1 << 15))) |
				   (0x1 << 15); /* dst burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
			reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
			reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
			reg_data =
				(reg_data & (~(0x1 << 26))) |
				(0x1
				 << 26); /* enable shadow blk done interrupt */
			reg_data = (reg_data & (~(0x1 << 30))) |
				   (0x0 << 30); /* not last block */
			if (i == blk_num)
				reg_data = (reg_data & (~(0x1 << 30))) |
					   (0x1 << 30); /* last block */

			reg_data = (reg_data & (~(0x1 << 31))) |
				   (0x1 << 31); /* shadow register enable */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(1) + 0x4, reg_data);
			if (flag_dma_lli_shadow_invalid != 0) {
				flag_dma_lli_shadow_invalid = 0;
				pr_debug(
					"\nhappen  shadow or lli invalid, set BLK_TFR_RESUMEREQ*****\n");
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							1),
					0x1);
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							1) +
						0x4,
					0x0);
				pr_debug("\nfinish resume channel...\n");
			}

			pr_debug("end config %d block\n", i);
			i++;
		}
		if (flag_block_trans_done != 0)
			pr_debug("block trans done: %d\n",
				flag_block_trans_done);
		if (flag_dma_dst_decode_err_int != 0) {
			pr_debug(
				"*****happen  dst err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_decode_err_int != 0) {
			pr_debug(
				"*****happen  src err interrupt! exit...*****\n");
			goto end;
		}
		if (flag_dma_src_done_int != 0)
			pr_debug("src trans done: %d\n",
				flag_dma_src_done_int);
		if (flag_dma_dst_done_int != 0)
			pr_debug("dst trans done: %d\n",
				flag_dma_dst_done_int);
		if (flag_dma_trans_done != 0) {
			pr_debug("dma trans done: %d\n",
				flag_dma_trans_done);
			if (i > 3)
				break;
		}
	}

end:
	pr_debug("\nwait for ch_%d_en auto clear....\n", 1);
	while ((REG32_READ(reg_base + DMAC_CHENREG) & (0x1 << (1 - 1))) != 0x0)
		;
	pr_debug(" ok!\n", 1);
	return 0;
}

#define DATA_BURST_LEN (DATA_LEN * 30)
#define SRAM_CPU_USE_BASE (SRAM_A55_BASE + DATA_BURST_LEN * 4 * 2)

extern void dcache_disable(void);
extern void dcache_enable(void);

static void shadow_burst_test(int channel, int reg_base, u64 *src, u64 *dst)
{
	int i, j;
	u32 reg_data;
	dcache_disable();
	pr_debug("src addr is %p\n", src);
	for (i = 0; i < DATA_BURST_LEN / (1024); i++) {
		pr_debug("%08x ", REG32_READ(src + 4 * i));
	}
	pr_debug("\ndst addr is %p\n", dst);
	for (i = 0; i < DATA_BURST_LEN / (1024); i++) {
		pr_debug("%08x ", REG32_READ(dst + 4 * i));
	}
	pr_debug("\nSRAM_CPU_USE_BASE addr is %p\n", SRAM_CPU_USE_BASE);
	for (i = 0; i < DATA_BURST_LEN / (1024); i++) {
		pr_debug("%08x ", REG32_READ(SRAM_CPU_USE_BASE + 4 * i));
	}

	/* channel config */
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel));
	reg_data = (reg_data & (~(0x3 << 0))) | (0x2 << 0); /* src set shadow */
	reg_data = (reg_data & (~(0x3 << 2))) | (0x2 << 2); /* dst set shadow */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CFG(channel) + 0x4);
	reg_data = (reg_data & (~(0x7 << 0))) |
		   (0x0 << 0); /* mem to mem dmac flow */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x0 << 3); /* src hardware handshaking */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* dst hardware handshaking */
	reg_data = (reg_data & (~(0xf << 23))) | (0x0 << 23); /* src osr lmt */
	reg_data = (reg_data & (~(0xf << 27))) | (0x0 << 27); /* dst osr lmt */
	REG32_WRITE(reg_base + DMAC_CHX_CFG(channel) + 0x4, reg_data);
	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel),
		    ((u64)src & 0xffffffff)); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_SAR(channel) + 0x4,
		    ((u64)src >> 32) & 0xffffffff); /* write src address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel),
		    (u64)dst & 0xffffffff); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_DAR(channel) + 0x4,
		    ((u64)dst >> 32) & 0xffffffff); /* write dst address */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel),
		    DATA_BURST_LEN - 1); /* write block size 16(byte) */
	REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(channel) + 0x4,
		    0x0); /* write block size 16(byte) */
	reg_data = REG32_READ(reg_base +
			      DMAC_CHX_CTL(channel)); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x0 << 4); /* src address increment */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x0 << 6); /* dst address increment */
	reg_data =
		(reg_data & (~(0x7 << 8))) | (0x2 << 8); /* src width 32 bit */
	reg_data = (reg_data & (~(0x7 << 11))) |
		   (0x2 << 11); /* dst width 32 bit */
	reg_data = (reg_data & (~(0xf << 14))) |
		   (0x0 << 14); /* src msize 8 byte */
	reg_data = (reg_data & (~(0xf << 18))) |
		   (0x0 << 18); /* dst msize 8 byte */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel), reg_data);
	reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(channel) +
			      0x4); /* config channel ctl */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* src burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
	reg_data = (reg_data & (~(0x1 << 15))) |
		   (0x1 << 15); /* dst burst len enable */
	/* 
	 * TODO src burst length AXI
	 */
	reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
	reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
	reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
	reg_data = (reg_data & (~(0x1 << 26))) |
		   (0x1 << 26); /* enable shadow blk done interrupt */
	reg_data =
		(reg_data & (~(0x1 << 30))) | (0x0 << 30); /* not last block */
	reg_data = (reg_data & (~(0x1 << 31))) |
		   (0x1 << 31); /* shadow register enable */
	REG32_WRITE(reg_base + DMAC_CHX_CTL(channel) + 0X4, reg_data);
	reg_data =
		REG32_READ(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(
					      channel)); /* config interrupt */
	reg_data = (reg_data & (~(0x1 << 0))) |
		   (0x1 << 0); /* enable block done interrupt */
	reg_data = (reg_data & (~(0x1 << 1))) |
		   (0x1 << 1); /* enable dma done interrupt */
	reg_data = (reg_data & (~(0x1 << 3))) |
		   (0x1 << 3); /* enable src done interrupt */
	reg_data = (reg_data & (~(0x1 << 4))) |
		   (0x1 << 4); /* enable dst done interrupt */
	reg_data = (reg_data & (~(0x1 << 5))) |
		   (0x1 << 5); /* enable src decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 6))) |
		   (0x1 << 6); /* enable dst decode status interrupt */
	reg_data = (reg_data & (~(0x1 << 13))) |
		   (0x1 << 13); /* enable shadow invalid err interrupt */
	REG32_WRITE(reg_base + DMAC_CHX_INTSTATUS_ENABLEREG(channel), reg_data);

	flag_dma_trans_done = 0;
	flag_block_trans_done = 0;
	flag_dma_src_done_int = 0;
	flag_dma_dst_done_int = 0;
	flag_dma_src_decode_err_int = 0;
	flag_dma_dst_decode_err_int = 0;
	flag_dma_lli_shadow_invalid = 0;

	pr_debug("\nch_%d first config done!\n", channel);

	reg_data = REG32_READ(reg_base + DMAC_CHENREG);
	reg_data = (reg_data & (~(1 << (channel - 1)))) |
		   (0x1 << (channel -
			    1)); /* enable ch,auto clear after transfer done */
	reg_data = (reg_data & (~(1 << (channel + 7)))) |
		   (0x1 << (channel + 7)); /* enable ch_we */
	/*
	 * ***NOTE!***
	 * 		Activation of the channel may result
	 * 	    in SLVIF_WRONCHEN_ERR_IntStat interruption, for unknown reasons.
	 *      DMAC_CHX_INTSTATUS = 0x80000
	 */
	REG32_WRITE(reg_base + DMAC_CHENREG, reg_data);
	j = 2;
	for (i = 0; i < DATA_BURST_LEN; i++) {
		reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(1) + 0x4);
		if ((reg_data & (0x1 << 31)) == 0x0) {
			pr_debug("\nstart config %d block\n", j);
			REG32_WRITE(reg_base + DMAC_CHX_SAR(1),
				    (u64)src); /* write src address */
			REG32_WRITE(reg_base + DMAC_CHX_SAR(1) + 0x4,
				    0x0); /* write src address */
			REG32_WRITE(reg_base + DMAC_CHX_DAR(1),
				    (u64)dst); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_DAR(1) + 0x4,
				    0x0); /* write dst address */
			REG32_WRITE(reg_base + DMAC_CHX_BLOCK_TS(1),
				    DATA_BURST_LEN -
					    1); /* write block size 16(byte) */

			reg_data = REG32_READ(
				reg_base +
				DMAC_CHX_CTL(1)); // config channel ctl
			reg_data = (reg_data & (~(0x1 << 4))) |
				   (0x0 << 4); /* src address increment */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x0 << 6); /* dst address increment */
			reg_data = (reg_data & (~(0x7 << 8))) |
				   (0x2 << 8); /* src width 32 bit */
			reg_data = (reg_data & (~(0x7 << 11))) |
				   (0x2 << 11); /* dst width 32 bit */
			reg_data = (reg_data & (~(0xf << 14))) |
				   (0x0 << 14); /* src msize 8 byte */
			reg_data = (reg_data & (~(0xf << 18))) |
				   (0x0 << 18); /* dst msize 8 byte */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(1), reg_data);
			reg_data = REG32_READ(reg_base + DMAC_CHX_CTL(1) +
					      0x4); /* config channel ctl */
			reg_data = (reg_data & (~(0x1 << 6))) |
				   (0x1 << 6); /* src burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 7))) | (0x1 << 7);
			reg_data = (reg_data & (~(0x1 << 15))) |
				   (0x1 << 15); /* dst burst len enable */
			/* 
			 * TODO src burst length AXI
			 */
			reg_data = (reg_data & (~(0xff << 16))) | (0x1 << 16);
			reg_data = (reg_data & (~(0x1 << 26))) | (0x1 << 24);
			reg_data = (reg_data & (~(0x1 << 30))) | (0x1 << 25);
			reg_data =
				(reg_data & (~(0x1 << 26))) |
				(0x1
				 << 26); /* enable shadow blk done interrupt */
			reg_data = (reg_data & (~(0x1 << 30))) |
				   (0x0 << 30); /* not last block */
			//if (i >= DATA_BURST_LEN*2/3)
			if (i >= 4000)
				reg_data = (reg_data & (~(0x1 << 30))) |
					   (0x1 << 30); /* last block */

			reg_data = (reg_data & (~(0x1 << 31))) |
				   (0x1 << 31); /* shadow register enable */
			REG32_WRITE(reg_base + DMAC_CHX_CTL(1) + 0x4, reg_data);
			if (flag_dma_lli_shadow_invalid != 0) {
				flag_dma_lli_shadow_invalid = 0;
				pr_debug(
					"\nhappen  shadow or lli invalid, set BLK_TFR_RESUMEREQ*****\n");
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							1),
					0x1);
				REG32_WRITE(
					reg_base +
						DMAC_CHX_BLK_TFR_RESUMEREQREG(
							1) +
						0x4,
					0x0);
				pr_debug("\nfinish resume channel...\n");
			}

			pr_debug("end config %d block\n", j++);
		}
		if (flag_dma_trans_done) {
			pr_debug("dma trans done\n");
			break;
		}
		pr_debug("->%d ", i);
		REG32_WRITE(SRAM_CPU_USE_BASE + i * 4, i);
		REG32_READ(src + 4 * i);
	}
	pr_debug("check data SRAM_CPU_USE_BASE:\n");
	for (j = 0; j < i; j++) {
		pr_debug("%08x ", REG32_READ(SRAM_CPU_USE_BASE + 4 * j));
	}
	pr_debug("check data dst:\n");
	for (j = 0; j < DATA_BURST_LEN / (1024); j++) {
		pr_debug("%08x ", REG32_READ(dst + 4 * j));
	}
	//dcache_enable();
}

static int gdma_burst_compete_sram_test(zebu_test_mod_t *mod, void *para)
{
	int i;
	int channel = 1;
	int reg_base = mod->reg_base;

	disable_channel_enable_dma(mod);

	pr_debug("\n test GDMA channel %d\n", channel);

	/* clear sram and ddr */
	pr_debug("\n gen data...\n");
	for (i = 0; i < DATA_BURST_LEN; i++) {
		REG32_WRITE(SRAM_A55_BASE + i * 4, i * 2);
		REG32_WRITE(SRAM_A55_BASE + DATA_BURST_LEN * 4 + i * 4, 0x0);
	}
	/* process buf to ddr0 */
	pr_debug(
		"\n****channel %d, start SRAM_A55_BASE to SRAM_A55_SENCOND ****\n",
		channel);
	shadow_burst_test(channel, reg_base, SRAM_A55_BASE,
			  SRAM_A55_BASE + DATA_BURST_LEN * 4);
	pr_debug("gdma burst compete sram test finish\n");
	return 0;
}

static zebu_test_mod_t zebu_test_dmac_mods[] = {
	{ "GDMAC", GDMA_BASE_ADDR },
};

// do not use char: c, a, r, d , s, q, ? ,  because these charactor has been used in test common
static zebu_test_entry_t zebu_test_dmac_entrys[] = {
	{ '1', 1, "case 1: Reg Dump", gdmac_test_registers_dump },
	{ '2', 1, "case 2 & 4: Reg W/R test and software reset",
	  gdmac_test_registers_wr },
	{ '3', 1, "case 6, 8, 29, 30: sram/DDR test", sdmac_single_test },
	{ '4', 1, "case W: multi-block contiguous address test",
	  sdmac_multi_blk_contiguous_address_test },
	{ '5', 1, "case X: multi-block auto reloading test",
	  sdmac_multi_blk_auto_reloading_test },
	{ '6', 1, "case Y: multi-block shandow register test",
	  sdmac_multi_blk_shadow_test },
	{ '7', 1, "case Z: multi-block linked list test",
	  sdmac_multi_blk_ll_test },
	{ '8', 1, "case S: dma channel lock test",
	  sdmac_dma_channel_lock_test },
	{ '9', 1, "case S: block channel lock test",
	  sdmac_block_channel_lock_test },
	{ 'z', 1, "case z: gdma burst compete sram test",
	  gdma_burst_compete_sram_test },
};

static zebu_test_ctrl_t zebu_mod_test_ctrl = { "DMAC",
					       SIZE_ARRAY(zebu_test_dmac_mods),
					       zebu_test_dmac_mods,
					       SIZE_ARRAY(
						       zebu_test_dmac_entrys),
					       zebu_test_dmac_entrys,
					       0 };

void sdmaMain(int para)
{
	int ret;
	u32 reg_data;
	zebu_mod_test_ctrl.current_mod = 0;
	pr_debug("\n 1605 \n");

	sdma_irq_init();
	reg_data = REG32_READ(TOP_CRM_BASE + GDMA_RESETREG);
	reg_data = (reg_data & (~(0x1 << 12))) | (0x1 << 12); // reset gdma
	REG32_WRITE(TOP_CRM_BASE + GDMA_RESETREG, reg_data);
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		pr_debug("auto test err %d!!!!\r\n", ret);
	} else {
		pr_debug("enter DMAC test!\r\n");
		TestLoop(&zebu_mod_test_ctrl);
	}
}

/**************************  IRQ TEST  *****************************/
void irq_dma(int para)
{
	u32 status = 0;
	pr_debug("enter dma interrupt \n");

	status = REG32_READ(GDMA_BASE_ADDR + DMAC_COMMONREG_INTSTATUSREG);
	pr_debug("\n int status: %08X \n", status);
	REG32_WRITE(GDMA_BASE_ADDR + DMAC_COMMONREG_INTCLEARREG,
		    status); // clear interrupt
	pr_debug("\n clear status: ok!...");
}

void irq_dma_chx(int para)
{
	u32 reg_data = 0;
	u32 status = 0;
	//sdma_irq_close();
	//REG32_WRITE( GDMA_BASE_ADDR + DMAC_CHX_INTSIGNAL_ENABLEREG(para) , 0x0); // mask interrupt
	status = REG32_READ(GDMA_BASE_ADDR + DMAC_CHX_INTSTATUS(para));
	reg_data =
		REG32_READ(GDMA_BASE_ADDR + DMAC_CHX_INTSTATUS_ENABLEREG(para));
	pr_debug("\nchannel %d, int status: %08X \n", para, status);
	if ((status & 0x1) == 0x1) // check  block transfer done
	{
		pr_debug("\n 0x1  irq detected \n");
		reg_data = reg_data | 0x1;
		flag_block_trans_done++;
	}
	if ((status & 0x2) == 0x2) // check dma transfer done  interrupt
	{
		pr_debug("\n 0x2  irq detected \n");
		reg_data = reg_data | 0x2;
		flag_dma_trans_done = 1;
	}
	if ((status & (0x1 << 3)) == (0x1 << 3)) // src trans done interrupt
	{
		pr_debug("\n src trans done  irq detected \n");
		reg_data = reg_data | (0x1 << 3);
		flag_dma_src_done_int++;
	}
	if ((status & (0x1 << 4)) == (0x1 << 4)) // dst trans done interrupt
	{
		pr_debug("\n dst trans done  irq detected \n");
		reg_data = reg_data | (0x1 << 4);
		flag_dma_dst_done_int++;
	}
	if ((status & (0x1 << 5)) == (0x1 << 5)) // src trans err interrupt
	{
		pr_debug("\n src_dec_err detected \n");
		reg_data = reg_data | (0x1 << 5);
		flag_dma_src_decode_err_int = 1;
	}
	if ((status & (0x1 << 6)) ==
	    (0x1 << 6)) // check destination decode error interrupt
	{
		pr_debug("\n 0x6  dst_dec_err detected \n");
		reg_data = reg_data | (0x1 << 6);
		flag_dma_dst_decode_err_int = 1;
	}
	if ((status & (0x1 << 13)) ==
	    (0x1 << 13)) // check lli or shadow invaild err
	{
		pr_debug("\n 0x13 lli or shadow invaild detected \n");
		reg_data = reg_data | (0x1 << 13);
		flag_dma_lli_shadow_invalid = 1;
	}

	REG32_WRITE(GDMA_BASE_ADDR + DMAC_CHX_INTCLEARREG(para),
		    reg_data); // clear interrupt
	pr_debug("\n ==== irq ok! %d \n", flag_dma_trans_done);
}

void sdma_irq_init(void)
{
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CMN_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH1_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH2_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH3_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH4_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH5_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH6_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH7_INTR);
	A55_IRQ_ENABLE(FIQ_IRQ_DMA_CH8_INTR);
}

void sdma_irq_close(void)
{
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CMN_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH1_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH2_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH3_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH4_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH5_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH6_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH7_INTR);
	A55_IRQ_DISABLE(FIQ_IRQ_DMA_CH8_INTR);
}

IRQ_DEFINE(FIQ_IRQ_DMA_CMN_INTR, irq_dma, GDMA_BASE_ADDR, "irq dma intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH1_INTR, irq_dma_chx, 1, "irq dma ch1 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH2_INTR, irq_dma_chx, 2, "irq dma ch2 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH3_INTR, irq_dma_chx, 3, "irq dma ch3 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH4_INTR, irq_dma_chx, 4, "irq dma ch4 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH5_INTR, irq_dma_chx, 5, "irq dma ch5 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH6_INTR, irq_dma_chx, 6, "irq dma ch6 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH7_INTR, irq_dma_chx, 7, "irq dma ch7 intr",
	   ISR_ATTR_A55);
IRQ_DEFINE(FIQ_IRQ_DMA_CH8_INTR, irq_dma_chx, 8, "irq dma ch8 intr",
	   ISR_ATTR_A55);

/*************************************************************************************************************************/

CMD_DEFINE(gdma, sdmaMain, 0, "gdma test", CMD_ATTR_AUTO_TEST);

struct dma_test_struct dma_test = {
	.contiguous = contiguous_test,
	.single = single_test,
	.auto_reload = auto_reload_test,
	.shadow = shadow_test,
	.llp = ll_test,
};
