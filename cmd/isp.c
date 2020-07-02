// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>
#include <command.h>
#include <module/isp.h>

int do_isp_dev(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret;
    ret = isp_dev();
    if (!ret)
        return CMD_RET_FAILURE;
    
    return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_isp_sub[] = {
	BST_CMD_MKENT(dev, 1, 1, do_isp_dev, "", ""),
};

static int do_isp(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 1)
		return CMD_RET_USAGE;

	/* Strip off leading 'i2c' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_isp_sub[0], ARRAY_SIZE(cmd_isp_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char isp_help_text[] =
    "dev isp - show or set current vsp\n\r"
    "isp test\n\r";
#endif

BST_CMD(isp, 7, 0, do_isp, "ISP sub-system", isp_help_text);
