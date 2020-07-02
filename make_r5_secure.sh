#
# (C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
#
# This file contains proprietary information that is the sole intellectual property of Black
# Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
# may be reproduced, distributed, transmitted, displayed or published in any manner 
# without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.

#!/bin/sh

make ARCH=r5_secure CROSS_COMPILE=arm-none-eabi- $1 $2 $3 $4 $5 $6 $7 $8

