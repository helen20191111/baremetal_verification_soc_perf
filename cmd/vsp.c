// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>
#include <command.h>
#include <module/vsp.h>

static int do_vsp_dev(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int ret;
    ret = vsp_dev();
    if (!ret)
        return CMD_RET_FAILURE;
    
    return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_vsp_sub[] = {
	BST_CMD_MKENT(dev, 1, 1, do_vsp_dev, "", ""),
};

static int do_vsp(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 1)
		return CMD_RET_USAGE;

	/* Strip off leading 'i2c' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_vsp_sub[0], ARRAY_SIZE(cmd_vsp_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char vsp_help_text[] =
    "dev vsp - show or set current vsp\n\r"
    "isp test\n\r";
#endif

BST_CMD(vsp, 7, 0, do_vsp, "VSP sub-system", vsp_help_text);
