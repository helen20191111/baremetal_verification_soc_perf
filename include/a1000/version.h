/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#ifndef __VERSION_H
#define __VERSION_H

#include <version.h>
#include <utsrelease.h>
#include <timestamp.h>

#define A1000_VERSION_STRING                                                   \
	"A1000 " UTS_RELEASE " (" A1000_DATE " - " A1000_TIME " " A1000_TZ ")"

#ifndef __ASSEMBLY__
extern const char version_string[];
#endif /* __ASSEMBLY__ */

#endif
