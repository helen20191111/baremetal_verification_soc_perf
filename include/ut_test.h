/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef _UT_TEST_H
#define _UT_TEST_H

#include <linux/types.h>

#define HVTE_TEST_PASS (0)
#define HVTE_TEST_FAIL (1)
#define HVTE_MODULE_TEST_PASS (0)
#define HVTE_MODULE_TEST_FAIL (1)

#define _po_test __attribute__((unused, section(".poweron.test")))
#define _module_main __attribute__((unused, section(".module.main")))

#define hvte_potest_test_struct test_struct _po_test
#define hvte_module_test_struct test_struct _module_main

#define HCOMP(exp_val, real_val)                                                 \
	do {                                                                     \
		if ((u32)(exp_val) != (u32)(real_val)) {                         \
			result = HVTE_MODULE_TEST_FAIL;                          \
			hprintf(ERROR,                                           \
				"Error: %s:%s:line %d: expect(%d) real(%d)\r\n", \
				__FILE__, __FUNCTION__, __LINE__,                \
				(u32)(exp_val), (u32)(real_val));                \
		}                                                                \
	} while (0)

#define UT_CASE(_fp, _flags, _para, _name, _description)                       \
	do {                                                                   \
		hvte_static.total_test_case++;                                 \
		if (HVTE_TEST_PASS != _fp(_flags, _para)) {                    \
			hprintf(TSENV, "@@ %s(%s) test FAIL. @@\r\n", _name,   \
				_description);                                 \
			hvte_static.total_fail_case++;                         \
			hvte_static.ip_fail_flag = HVTE_MODULE_TEST_FAIL;      \
		} else {                                                       \
			hprintf(TSENV, "@@ %s(%s) test PASS. @@\r\n", _name,   \
				_description);                                 \
			hvte_static.total_pass_case++;                         \
		}                                                              \
	} while (0)

#endif
