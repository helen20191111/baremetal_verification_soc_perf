// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#include <common.h>
#include <cli.h>
#include <command.h>
#include <errno.h>
#include <module/dmac.h>
#include <linux/compiler.h>

struct dma_memcpy_struct mem;

static int do_dmac_init(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    dma_init();
    return 0;
}

static int do_dmac_xfer(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    if (argc != 6) {
        return CMD_RET_USAGE;
    }

    printf("start dmac xfer...\n");
    memset(&mem, 0, sizeof mem);
    
    mem.src = simple_strtoul(argv[1], NULL, 16);
    mem.dst = simple_strtoul(argv[2], NULL, 16);
    mem.length = simple_strtoul(argv[3], NULL, 16);
    mem.chan = simple_strtoul(argv[4], NULL, 16);
    mem.tran_type =simple_strtoul(argv[5], NULL, 16);
    return dma_start(&mem);
}

static struct dma_memcpy_struct mem0;
static struct dma_memcpy_struct mem1;
static struct dma_memcpy_struct mem2;
static struct dma_memcpy_struct mem3;
static struct dma_memcpy_struct mem4;
static struct dma_memcpy_struct mem5;
static struct dma_memcpy_struct mem6;
static struct dma_memcpy_struct mem7;

static struct dma_memcpy_struct *mem_list[] = {
    &mem0,
    &mem1,
    &mem2,
    &mem3,
    &mem4,
    &mem5,
    &mem6,
    &mem7,
};

static void mem_list_dump(struct dma_memcpy_struct *mem)
{
    printf("mem descripts %d\n", mem->chan);
    printf("\tdescript src:    %p\n", mem->src);
    printf("\tdescript dst:    %p\n", mem->dst);
    printf("\tdescript length: %08x\n", mem->length);
    printf("\tdescript chan:   %d\n\n", mem->chan);
}

static int do_dmac_setup(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    u8 chan, i;
    if (argc == 1) {
        for (i = 0; i < CONFIG_DMAC_CHA_MAX; i++) {
            if (mem_list[i]->length == 0)
                continue;
            mem_list_dump(mem_list[i]);
        }
    } else {
        if (argc != 5)
            return CMD_RET_USAGE;

        chan = simple_strtoul(argv[4], NULL, 16);
        if (chan >= 8)
            return CMD_RET_USAGE;

        memset(mem_list[chan], 0, sizeof (struct dma_memcpy_struct));
        mem_list[chan]->src = simple_strtoul(argv[1], NULL, 16);
        mem_list[chan]->dst = simple_strtoul(argv[2], NULL, 16);
        mem_list[chan]->length = simple_strtoul(argv[3], NULL, 16);
        mem_list[chan]->chan = chan;
        mem_list_dump(mem_list[chan]);
    }
    return 0;
}

static int do_dmac_run(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
    if (argc != 1) {
        return CMD_RET_USAGE;
    }

    return dma_run(mem_list);
}

#define SEQ_START		0xa596
#define SEQ_MIN			0x2000
void gen_seq(struct dma_memcpy_struct * pmem)
{
	unsigned int ui, uicnt;
	unsigned short usval;
	unsigned short usstep;
	
	unsigned long * ptrul;

	ptrul = (unsigned long *)mem.src;
	uicnt = mem.length >> 3;
	usval = SEQ_START;
	usstep = 1;
	for(ui=0; ui<uicnt; ui++) {
		ptrul[ui] = ((unsigned long)usval<<48)|((unsigned long)usval<<(32-1))|((unsigned long)usval<<(16-2))|((unsigned long)usval>>3);
		
		if(usval >= SEQ_MIN) {
			usval -= usstep;
		}
		else{
			usval = SEQ_START;
			usstep = (usstep>=0x20)?1:(usstep+1);
		}
	}

	asm volatile ("dsb sy" : : : "memory");
}

unsigned int chk_seq(struct dma_memcpy_struct * pmem)
{
	unsigned int ui, uicnt, uiret;
	unsigned short usval;
	unsigned short usstep;
	unsigned long ulval;
	
	unsigned long * ptrul;

	ptrul = (unsigned long *)mem.src;
	uicnt = mem.length >> 3;
	uiret = mem.length & 0xfffffff8;
	usval = SEQ_START;
	usstep = 1;
	
	for(ui=0; ui<uicnt; ui++) {
		ulval = ((unsigned long)usval<<48)|((unsigned long)usval<<(32-1))|((unsigned long)usval<<(16-2))|((unsigned long)usval>>3);
		if(ptrul[ui] != ulval){
			uiret = ui << 3;
			printf("Do check seqence error, at 0x%08x, seq:0x%llx, fact:0x%llx.\n\r", uiret, ulval, ptrul[ui]);
			break;
		}
		
		if(usval >= SEQ_MIN) {
			usval -= usstep;
		}
		else{
			usval = SEQ_START;
			usstep = (usstep>=0x20)?1:(usstep+1);
		}
	}

	return uiret;
}

extern unsigned long long get_ticks(void);
static int do_gen_seq(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	unsigned long long sticks;
	
    if (argc != 3) {
        return CMD_RET_USAGE;
    }

    printf("start dmac xfer...\n");
    memset(&mem, 0, sizeof mem);
    
    mem.src = simple_strtoul(argv[1], NULL, 16);
    mem.length = simple_strtoul(argv[2], NULL, 16);

	sticks = get_ticks();
	gen_seq(&mem);
	printf("gen_seq source-addr:0x%llx, dest-addr:0x%llx, length:0x%08x, start:0x%llx, end:0x%llx\n", 
				mem.src, mem.dst, mem.length, sticks, get_ticks());
	return ret;
}

static int do_chk_seq(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	unsigned int uiret;
	unsigned long long sticks;
	
	if (argc != 3) {
	    return CMD_RET_USAGE;
	}
	
	printf("start do_chk_seq ... ");
	memset(&mem, 0, sizeof mem);
	
	mem.src = simple_strtoul(argv[1], NULL, 16);
	mem.length = simple_strtoul(argv[2], NULL, 16);
	
	sticks = get_ticks();
	uiret = chk_seq(&mem);
	printf("done\n\r");
	printf("chk_seq source-addr:0x%llx, dest-addr:0x%llx, length:0x%08x, start:0x%llx, end:0x%llx, over at:0x%08x\n", 
				mem.src, mem.dst, mem.length, sticks, get_ticks(), uiret);
	return ret;
}

static int do_interleave(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	if (argc == 1) {
	    if(readl(0x33000058) == 0)
	    	printf("Current ddr interleave is open.\n\r");
	    else
	    	printf("Current ddr interleave is close.\n\r");
	}
	if (argc != 2) {
	    return CMD_RET_USAGE;
	}
	
	printf("start dmac xfer...\n");
	memset(&mem, 0, sizeof mem);
	
	if(strcmp(argv[1], "open") == 0){
		writel(0x0, 0x33000058);
		printf("Do open interleave\n\r");
	}
	else if(strcmp(argv[1], "close") == 0){
		writel(0x1, 0x33000058);
		printf("Do close interleave\n\r");
	}
	else {
		printf("invalid operate : %s\n\r", argv[1]);
		return -1;
	}
	
	return 0;
}

static cmd_tbl_t cmd_dmac_sub[] = {
    BST_CMD_MKENT(init, 1, 0, do_dmac_init, "", ""),
    BST_CMD_MKENT(xfer, 6, 0, do_dmac_xfer, "", ""),
    BST_CMD_MKENT(genseq, 4, 0, do_gen_seq, "", ""),
    BST_CMD_MKENT(chkseq, 4, 0, do_chk_seq, "", ""),
    BST_CMD_MKENT(interleave, 2, 0, do_interleave, "", ""),
    BST_CMD_MKENT(setup, 5, 0, do_dmac_setup, "", ""),
    BST_CMD_MKENT(run, 1, 0, do_dmac_run, "", ""),
};

static int do_dmac(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'i2c' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_dmac_sub[0], ARRAY_SIZE(cmd_dmac_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}


BST_CMD(
    dmac, 7, 0, do_dmac,
    "gdma test",
    "dmac init\n\r"
    "dmac xfer src dst length chan type\n\r"
    "\n\r"
    "dmac genseq addr length\n\r"
    "dmac chkseq addr length\n\r"
    "dmac interleave open|close\n\r"
    "\n\r"
    "dmac setup [src dsr length chan] - show or set descriptor\n\r"
    "dmac run - base on setup \n\r"
);
