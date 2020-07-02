/*
 *(C) Copyright Black Sesame Technologies (Shanghai)Ltd. Co., 2020. All rights reserved.
 *
 * This file contains proprietary information that is the sole intellectual property of Black
 * Sesame Technologies (Shanghai)Ltd. Co. No part of this material or its documentation 
 * may be reproduced, distributed, transmitted, displayed or published in any manner 
 * without the written permission of Black Sesame Technologies (Shanghai)Ltd. Co.
 */

#include <common.h>
#include <command.h>
#include <utility_lite.h>

static int do_old_cli(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
    cmd_main();
	puts("\n\r");
	return 0;
}

BST_CMD(old_cli, 1, 0, do_old_cli,
	   "Use the old command line interface", "");
