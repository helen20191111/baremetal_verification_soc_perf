#include <common.h>
#include <command.h>

#include <module/soc_perf.h>
#include "module/dmac.h"
#include "autoconf.h"
#include <time.h>

#define COUNTER_BUFF_SIZE	(1024u*4u)

/* 测试总时间 */
u64 g_soc_perf_cycle_time = 10;
u64 g_soc_perf_ddr_grp_num = 0;
u8 g_soc_perf_test_port = 0;
/* 采样周期时间 */
u64 g_sample_cycle = 0u;

u64 g_start_time = 0u;
u64 g_sample_time = 0u;
u64 g_current_time = 0u;

u64 g_gdma_enable_flag = 0u;

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
u8	g_set_sample_cycle_flag = 0u;
u32 *pFlag = NULL;

u32 soc_perf_timestamp[COUNTER_BUFF_SIZE] = {0u,};

u32 soc_noc_gdma_buffer[COUNTER_BUFF_SIZE] = {0u,};
// u32 soc_noc_vsp_buffer[COUNTER_BUFF_SIZE] = {0u,};
u32 soc_noc_port0_ddr0_buffer[COUNTER_BUFF_SIZE] = {0u,};
u32 soc_noc_port0_ddr1_buffer[COUNTER_BUFF_SIZE] = {0u,};
u32 soc_noc_port1_ddr0_buffer[COUNTER_BUFF_SIZE] = {0u,};
u32 soc_noc_port1_ddr1_buffer[COUNTER_BUFF_SIZE] = {0u,};

/* ddr0 */
u64 soc_ddr0_perf_counter0_buffer[COUNTER_BUFF_SIZE] = {0u,};
u64 soc_ddr0_perf_counter1_buffer[COUNTER_BUFF_SIZE] = {0u,};
u64 soc_ddr0_perf_counter2_buffer[COUNTER_BUFF_SIZE] = {0u,};
u64 soc_ddr0_perf_counter3_buffer[COUNTER_BUFF_SIZE] = {0u,};
/* ddr1 */
u64 soc_ddr1_perf_counter0_buffer[COUNTER_BUFF_SIZE] = {0u,};
u64 soc_ddr1_perf_counter1_buffer[COUNTER_BUFF_SIZE] = {0u,};
u64 soc_ddr1_perf_counter2_buffer[COUNTER_BUFF_SIZE] = {0u,};
u64 soc_ddr1_perf_counter3_buffer[COUNTER_BUFF_SIZE] = {0u,};

static void cal_sample_time(void)
{
	if(g_set_sample_cycle_flag  == 0)
	{
		/* calculate ticks of g_sample_time */
		g_sample_cycle = g_soc_perf_cycle_time * 7380u / COUNTER_BUFF_SIZE / 3u * 2u ; 
	}
	else
	{
		;/* do nothing,use the set sample time */;
	}
	printf("cal stime %d\r\n",g_sample_cycle);
}

static void fill_gdma_chn_message(void)
{
#if 0
	/* fill 8 chn message start DDR0 to DDR1 & DDR1 to DDR0 */
	mem0.src = (dma_addr_t)0x80000000u;
    mem0.dst = (dma_addr_t)0x180000000u;
    mem0.length = 0x10000u;
    mem0.chan = 0u;

	mem1.src = (dma_addr_t)0x80010000;
    mem1.dst = (dma_addr_t)0x180010000;
    mem1.length = 0x10000u;;
    mem1.chan = 1u;

	mem2.src = (dma_addr_t)0x80020000;
    mem2.dst = (dma_addr_t)0x180020000;
    mem2.length = 0x10000u;;
    mem2.chan = 2u;

	mem3.src = (dma_addr_t)0x80030000;
    mem3.dst = (dma_addr_t)0x180030000;
    mem3.length = 0x10000u;;
    mem3.chan = 3u;

	mem4.src = (dma_addr_t)0x180040000;
    mem4.dst = (dma_addr_t)0x80040000;
    mem4.length = 0x10000u;;
    mem4.chan = 4u;

	mem5.src = (dma_addr_t)0x180050000;
    mem5.dst = (dma_addr_t)0x80050000;
    mem5.length = 0x10000u;;
    mem5.chan = 5u;

	mem6.src = (dma_addr_t)0x180060000;
    mem6.dst = (dma_addr_t)0x80060000;
    mem6.length = 0x10000u;;
    mem6.chan = 6u;

	mem7.src = (dma_addr_t)0x180070000;
    mem7.dst = (dma_addr_t)0x80070000u;
    mem7.length = 0x10000u;;
    mem7.chan = 7u;
	/* fill 8 chn message end */
#endif

#if 0
	/* fill 8 chn message start DDR1 to DDR1*/
	mem0.src = (dma_addr_t)0x180000000u;
    mem0.dst = (dma_addr_t)0x180010000u;
    mem0.length = 0x10000u;
    mem0.chan = 0u;

	mem1.src = (dma_addr_t)0x180020000u;
    mem1.dst = (dma_addr_t)0x180030000u;
    mem1.length = 0x10000u;
    mem1.chan = 1u;

	mem2.src = (dma_addr_t)0x180040000u;
    mem2.dst = (dma_addr_t)0x180050000u;
    mem2.length = 0x10000u;
    mem2.chan = 2u;

	mem3.src = (dma_addr_t)0x180060000u;
    mem3.dst = (dma_addr_t)0x180070000u;
    mem3.length = 0x100u;
    mem3.chan = 3u;

	mem4.src = (dma_addr_t)0x180080000u;
    mem4.dst = (dma_addr_t)0x180090000u;
    mem4.length = 0x100u;
    mem4.chan = 4u;

	mem5.src = (dma_addr_t)0x1800a0000u;
    mem5.dst = (dma_addr_t)0x1800b0000u;
    mem5.length = 0x100u;
    mem5.chan = 5u;

	mem6.src = (dma_addr_t)0x1800c0000u;
    mem6.dst = (dma_addr_t)0x1800d0000u;
    mem6.length = 0x100u;
    mem6.chan = 6u;

	mem7.src = (dma_addr_t)0x1800e0000u;
    mem7.dst = (dma_addr_t)0x1800f0000u;
    mem7.length = 0x100u;
    mem7.chan = 7u;
	/* fill 8 chn message end */
#endif

	/* fill 8 chn message start DDR0 to DDR0 */
	mem0.src = (dma_addr_t)0x80000000u;
    mem0.dst = (dma_addr_t)0x84000000u;
    mem0.length = 0x4000000u;
    mem0.chan = 0u;

	mem1.src = (dma_addr_t)0x88000000u;
    mem1.dst = (dma_addr_t)0x8C000000u;
    mem1.length = 0x4000000u;
    mem1.chan = 1u;

	mem2.src = (dma_addr_t)0x90000000u;
    mem2.dst = (dma_addr_t)0x94000000u;
    mem2.length = 0x4000000u;
    mem2.chan = 2u;

	mem3.src = (dma_addr_t)0x98000000u;
    mem3.dst = (dma_addr_t)0x9C000000u;
    mem3.length = 0x4000000u;
    mem3.chan = 3u;

	mem4.src = (dma_addr_t)0xA0000000u;
    mem4.dst = (dma_addr_t)0xA4000000u;
    mem4.length = 0x4000000u;
    mem4.chan = 4u;

	mem5.src = (dma_addr_t)0xA8000000u;
    mem5.dst = (dma_addr_t)0xAC000000u;
    mem5.length = 0x4000000u;
    mem5.chan = 5u;

	mem6.src = (dma_addr_t)0xB0000000u;
    mem6.dst = (dma_addr_t)0xB4000000u;
    mem6.length = 0x4000000u;
    mem6.chan = 6u;

	mem7.src = (dma_addr_t)0xB8000000u;
    mem7.dst = (dma_addr_t)0x8C000000u;
    mem7.length = 0x4000000u;
    mem7.chan = 7u;
	/* fill 8 chn message end */
}
static void update_ddr_monitor(void)
{
	ddr0_monitor_update_monitor();
	ddr1_monitor_update_monitor();
}
static void save_sample_data(void)
{
	static u32 uiBufCnt = 0;
	u32	ucPortEnable = 0u;

	/* check and reset the buffer counter */
	uiBufCnt = uiBufCnt % COUNTER_BUFF_SIZE;

	/* get timestamp */
	soc_perf_timestamp[uiBufCnt] = timer_get_us();
	/* get observe packet result */			
	soc_noc_gdma_buffer[uiBufCnt] = observe_get_gdma_baudrate();
	/* get ddr0 monitor result */
	ucPortEnable = g_soc_perf_test_port + 1u;
	
	if(ucPortEnable & 0x01u)
	{
		soc_noc_port0_ddr0_buffer[uiBufCnt] = observe_get_ddr0_p0_baudrate();
		soc_noc_port0_ddr1_buffer[uiBufCnt] = observe_get_ddr1_p0_baudrate();
	}
	if(ucPortEnable & 0x02u)
	{
		soc_noc_port1_ddr0_buffer[uiBufCnt] = observe_get_ddr0_p1_baudrate();
		soc_noc_port1_ddr1_buffer[uiBufCnt] = observe_get_ddr1_p1_baudrate();
	}
	soc_ddr0_perf_counter0_buffer[uiBufCnt] = ddr0_monitor_get_counters_value(0u);
	soc_ddr0_perf_counter1_buffer[uiBufCnt] = ddr0_monitor_get_counters_value(1u);
	soc_ddr0_perf_counter2_buffer[uiBufCnt] = ddr0_monitor_get_counters_value(2u);
	soc_ddr0_perf_counter3_buffer[uiBufCnt] = ddr0_monitor_get_counters_value(3u);

	/* get ddr1 monitor result */
	soc_ddr1_perf_counter0_buffer[uiBufCnt] = ddr1_monitor_get_counters_value(0u);
	soc_ddr1_perf_counter1_buffer[uiBufCnt] = ddr1_monitor_get_counters_value(1u);
	soc_ddr1_perf_counter2_buffer[uiBufCnt] = ddr1_monitor_get_counters_value(2u);
	soc_ddr1_perf_counter3_buffer[uiBufCnt] = ddr1_monitor_get_counters_value(3u);

	uiBufCnt++;
}
static void clear_ddr_monitor_counter(void)
{
	ddr0_monitor_clear_counter();
	ddr1_monitor_clear_counter();
}
static void select_ddr_monitor_group(void)
{
	ddr0_monitor_set_counter_group(g_soc_perf_ddr_grp_num);
	ddr1_monitor_set_counter_group(g_soc_perf_ddr_grp_num);
}
static void soc_perf_enable(void)
{
	u8 ucPortEnable =	0u;

	dma_init();

	// vsp_probe_enable();
	gdma_probe_enable();

	ucPortEnable = g_soc_perf_test_port + 1u;

	if(ucPortEnable & 0x01u)
	{
		ddr0_port0_probe_enable();
		ddr1_port0_probe_enable();
	}
	if(ucPortEnable & 0x02u)
	{
		ddr0_port1_probe_enable();
		ddr1_port1_probe_enable();
	}
	select_ddr_monitor_group();
}
static void soc_perf_transfer_ctrl(void)
{
	u32 uiChnCnt = 0;
	u32 readyChnCnt = 0;

	pFlag = dma_transfer_finished();

	for (uiChnCnt = 0u; uiChnCnt < CONFIG_DMAC_CHA_MAX; uiChnCnt++)
	{
		if (1 == pFlag[uiChnCnt])
		{
			readyChnCnt++;
		}
	}

	if (CONFIG_DMAC_CHA_MAX == readyChnCnt)
	{
		/* all channels completed data transfer */
		for (uiChnCnt = 0u; uiChnCnt < CONFIG_DMAC_CHA_MAX; uiChnCnt++)
		{
			dma_transfer_finished_flag_clear(uiChnCnt);
		}
		dma_run(mem_list);
	}
}
int check_all_config(void)
{
	if(g_soc_perf_cycle_time  == 0)
	{
		printf("Invalid cycle time,please call set_ctime cmd first\r\n");
		return CMD_RET_FAILURE;
	}	
	if(g_soc_perf_ddr_grp_num > 11)
	{
		printf("Invalid ddr group number,please call set_ddr_monitor_grp cmd first\r\n");
		return CMD_RET_FAILURE;
	}
	if(g_soc_perf_test_port > 2u)
	{
		printf("Invalid performance test port,please call set_test_port cmd first\r\n");
		return CMD_RET_FAILURE;
	}
	return CMD_RET_SUCCESS;
}
int set_soc_ddr_monitor_grp(int group_num)
{
	int iRet = 0;

	if(group_num > 11)
	{
		iRet = -1;
	}
	else
	{
		g_soc_perf_ddr_grp_num = group_num;
		iRet = 0;
	}
	return iRet;
}
static int soc_perf_set_ddr_monitor_group(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
    int i = 0, ret;

    if (argc != 2) {
        printf("Usage : set_ddr_monitor_grp group_num\r\n");
		return CMD_RET_USAGE;
    } else {
        i = simple_strtoul(argv[1], NULL, 10);
		ret = set_soc_ddr_monitor_grp(i);
		if(ret)
		{
			printf("Invalid ddr monitor number !\r\n");
			return CMD_RET_FAILURE;
		}
    }
    
    return CMD_RET_SUCCESS;
}
static int soc_perf_set_cycle_time(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
    int i = 0;

    if (argc != 2) {
        printf("Usage : set_cycle_time time_in_seconds\r\n");
		return CMD_RET_USAGE;
    } else {
        i = simple_strtoul(argv[1], NULL, 10);
		if(i == 0)
		{
			printf("Invalid perf cycle time !\r\n");
			g_soc_perf_cycle_time = 0u;
			return CMD_RET_FAILURE;
		}
		else
		{
			g_soc_perf_cycle_time = i;
			printf("Set cycle time %d\r\n",g_soc_perf_cycle_time);
		}
    }
    
    return CMD_RET_SUCCESS;
}

static int soc_perf_set_sample_cycle_time(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
    int i = 0;

    if (argc != 2) {
        printf("Usage : set_sample_cycle_time time_in_ticks\r\n");
		return CMD_RET_USAGE;
    } else {
        i = simple_strtoul(argv[1], NULL, 10);
		if(i == 0)
		{
			printf("Invalid sample cycle time !\r\n");
			return CMD_RET_FAILURE;
		}
		else
		{
			g_sample_cycle = i;
			g_set_sample_cycle_flag = 1u;
			printf("Set sample cycle time %d\r\n",g_sample_cycle);
		}
    }
    
    return CMD_RET_SUCCESS;
}

static int soc_perf_set_test_port(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
    int i = 0;

    if (argc != 2) {
        printf("Usage : set_test_port 0/1\r\n");
		return CMD_RET_USAGE;
    } else {
        i = simple_strtoul(argv[1], NULL, 10);
		if(i == 0 || i == 1 || i == 2)
		{
			g_soc_perf_test_port = i;
		}
		else
		{
			g_soc_perf_test_port = 0xffu;
			printf("Invalid test port number !\r\n");
			return CMD_RET_FAILURE;
		}
    }
    
    return CMD_RET_SUCCESS;
}

static int soc_perf_start_test(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{

	fill_gdma_chn_message();

	if (argc != 1) {
        printf("Usage : start_ddr_perf_test\r\n");
		return CMD_RET_USAGE;
    } else {
		if(check_all_config())
		{
			return CMD_RET_FAILURE;
		}

		cal_sample_time();

		soc_perf_enable();

		/* get start ticks */
		g_start_time = timer_get_us();
		g_sample_time = timer_get_us();
		printf("Start time is %d\r\n",g_start_time);
		printf("Please wait %d seconds for the test to complete...\r\n",g_soc_perf_cycle_time);

		dma_run(mem_list);

		while(1)
		{
			soc_perf_transfer_ctrl();
			
			/* get current ticks */
			g_current_time = timer_get_us();

			if (g_sample_cycle <= (g_current_time - g_sample_time))
			{
				update_ddr_monitor();

				save_sample_data();
				
				clear_ddr_monitor_counter();
				/* re-select group */
				select_ddr_monitor_group();
			
				g_current_time = timer_get_us();
				g_sample_time = g_current_time;
			}
			/* 检查是否到达监测统计时间？ */
			if((g_current_time - g_start_time) >= g_soc_perf_cycle_time * 7380u)
			{
				/* 清空相关寄存器和控制命令 */
				clear_ddr_monitor_counter();

				printf("Complete SOC performance test !\r\n");
				break;
			}
		}
    }
	return CMD_RET_SUCCESS;
}
static int soc_perf_get_gdma_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
	u32 uiCnt = 0;
	if (argc != 1) {
		printf("Usage : get_gdma_probe\r\n");
		return CMD_RET_USAGE;
	}else{
		for(uiCnt = 0;uiCnt < ARRAY_SIZE(soc_noc_gdma_buffer);uiCnt++)
		{
			printf("%d\t%d\tgdma baudrate is\t%d KBPS\r\n",uiCnt,soc_perf_timestamp[uiCnt], soc_noc_gdma_buffer[uiCnt]);		
		}
	}
	return CMD_RET_SUCCESS;
}
static int soc_perf_get_p0_ddr_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
	u32 uiCnt = 0;
	if (argc != 1) {
		printf("Usage : get_p0_ddr0_data\r\n");
		return CMD_RET_USAGE;
	}else{
		for(uiCnt = 0;uiCnt < ARRAY_SIZE(soc_noc_port0_ddr0_buffer);uiCnt++)
		{
			printf("%d\t%d\tddr0 port0 baudrate is\t%d KBPS\r\n",uiCnt,soc_perf_timestamp[uiCnt], soc_noc_port0_ddr0_buffer[uiCnt]);
		}
		for(uiCnt = 0;uiCnt < ARRAY_SIZE(soc_noc_port0_ddr1_buffer);uiCnt++)
		{
			printf("%d\t%d\tddr1 port0 baudrate is\t%d KBPS\r\n",uiCnt,soc_perf_timestamp[uiCnt], soc_noc_port0_ddr1_buffer[uiCnt]);
		}
	}
	return CMD_RET_SUCCESS;
}
static int soc_perf_get_p1_ddr_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
	u32 uiCnt = 0;
	if (argc != 1) {
		printf("Usage : get_p0_ddr0_data\r\n");
		return CMD_RET_USAGE;
	}else{
		for(uiCnt = 0;uiCnt < ARRAY_SIZE(soc_noc_port1_ddr0_buffer);uiCnt++)
		{
			printf("%d\t%d\tddr0 port1 baudrate is\t%d KBPS\r\n",uiCnt,soc_perf_timestamp[uiCnt], soc_noc_port1_ddr0_buffer[uiCnt]);
		}
		for(uiCnt = 0;uiCnt < ARRAY_SIZE(soc_noc_port1_ddr1_buffer);uiCnt++)
		{
			printf("%d\t%d\tddr1 port1 baudrate is\t%d KBPS\r\n",uiCnt,soc_perf_timestamp[uiCnt], soc_noc_port1_ddr1_buffer[uiCnt]);
		}
	}
	return CMD_RET_SUCCESS;
}

static int soc_perf_get_ddr_monitor_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
	char	*output_msg[4] = {0,};
	u16		usbufCnt = 0u;
	u8      ucCnt	= 0u;
	int		i =	0u;

	if (argc != 2) {
		printf("Usage : get_p0_ddr0_data 0/1\r\n");
		return CMD_RET_USAGE;
	}else{
		switch (g_soc_perf_ddr_grp_num)
		{
			case 0u:
				output_msg[0] = "%d\t%d\tddr%d perf_hif_wr is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_hif_rd is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_hif_rmw is\t%d\r\n";
				output_msg[3] = "%d\t%d\tddr%d perf_hif_hi_pri_rd is\t%d\r\n";
				break;
			case 1u:
				output_msg[0] = "%d\t%d\tddr%d perf_dfi_wr_data_cycles is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_dfi_rd_data_cycles is\t%d\r\n";
				output_msg[2] = "";
				output_msg[3] = "";
				break;	
			case 2u:
				output_msg[0] = "%d\t%d\tddr%d perf_hpr_xact_when_critical is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_lpr_xact_when_critical is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_wr_xact_when_critical is\t%d\r\n";
				output_msg[3] = "";
				break;
			case 3u:
				output_msg[0] = "%d\t%d\tddr%d perf_op_is_activate is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_op_is_rd_activate\tis %d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_rdwr_ransitions is\t%d\r\n";
				output_msg[3] = "";
				break;
			case 4u:
				output_msg[0] = "%d\t%d\tddr%d perf_op_is_wr is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_op_is_rd is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_op_is_mwr is\t%d\r\n";
				output_msg[3] = "";
				break;
			case 5u:
				output_msg[0] = "%d\t%d\tddr%d perf_op_is_precharge is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_precharge_for_other is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_precharge_for_rdwr is\t%d\r\n";
				output_msg[3] = "";
				break;
			case 6u:
				output_msg[0] = "%d\t%d\tddr%d perf_write_combine is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_write_combine_noecc is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_write_combine_wrecc is\t%d\r\n";
				output_msg[3] = "";
				break;
			case 7u:
				output_msg[0] = "%d\t%d\tddr%d perf_war_hazard is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_raw_hazard is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_waw_hazard is\t%d\r\n";
				output_msg[3] = "%d\t%d\tddr%d perf_ie_blk_hazard is\t%d\r\n";
				break;
			case 8u:
				output_msg[0] = "%d\t%d\tddr%d perf_op_is_enter_selfref is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_op_is_enter_powerdown is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_op_is_enter_deeppowerdown is\t%d\r\n";
				output_msg[3] = "%d\t%d\tddr%d perf_selfref_mode is\t%d\r\n";
				break;
			case 9u:
				output_msg[0] = "%d\t%d\tddr%d perf_op_is_refresh is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_op_is_crit_ref is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_op_is_spec_ref is\t%d\r\n";
				output_msg[3] = "";
				break;
			case 10u:
				output_msg[0] = "%d\t%d\tddr%d perf_op_is_load_mode is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_op_is_zqcl is\t%d\r\n";
				output_msg[2] = "%d\t%d\tddr%d perf_op_is_zqcs is\t%d\r\n";
				output_msg[3] = "";
				break;
			case 11u:
				output_msg[0] = "%d\t%d\tddr%d perf_hpr_req_with_nocredit is\t%d\r\n";
				output_msg[1] = "%d\t%d\tddr%d perf_lpr_req_with_nocredit is\t%d\r\n";
				output_msg[2] = "";
				output_msg[3] = "";
				break;
			default:
				break;
		}
		i = simple_strtoul(argv[1], NULL, 10);
		if(i == 0)
		{
			for(usbufCnt = 0;usbufCnt < ARRAY_SIZE(soc_ddr0_perf_counter0_buffer);usbufCnt++)
			{
				printf(output_msg[0],usbufCnt,soc_perf_timestamp[usbufCnt],ucCnt,soc_ddr0_perf_counter0_buffer[usbufCnt]);
			}

			for(usbufCnt = 0;usbufCnt < ARRAY_SIZE(soc_ddr0_perf_counter1_buffer);usbufCnt++)
			{
				printf(output_msg[1],usbufCnt,soc_perf_timestamp[usbufCnt],ucCnt,soc_ddr0_perf_counter1_buffer[usbufCnt]);
			}

			if(strcmp(output_msg[2],""))
			{
				for(usbufCnt = 0;usbufCnt < ARRAY_SIZE(soc_ddr0_perf_counter2_buffer);usbufCnt++)
				{
					printf(output_msg[2],usbufCnt,soc_perf_timestamp[usbufCnt],ucCnt,soc_ddr0_perf_counter2_buffer[usbufCnt]);
				}
				if(strcmp(output_msg[3],""))
				{
					for(usbufCnt = 0;usbufCnt < ARRAY_SIZE(soc_ddr0_perf_counter3_buffer);usbufCnt++)
					{
						printf(output_msg[3],usbufCnt,soc_perf_timestamp[usbufCnt],ucCnt,soc_ddr0_perf_counter3_buffer[usbufCnt]);
					}
				}
			}
		}
		else if(i == 1)
		{
			/* ddr1 monitor */
			ucCnt = 1u;
			for(usbufCnt = 0;usbufCnt < ARRAY_SIZE(soc_ddr1_perf_counter0_buffer);usbufCnt++)
			{
				printf(output_msg[0],usbufCnt,soc_perf_timestamp[usbufCnt],ucCnt,soc_ddr1_perf_counter0_buffer[usbufCnt]);
			}

			for(usbufCnt = 0;usbufCnt < ARRAY_SIZE(soc_ddr1_perf_counter1_buffer);usbufCnt++)
			{
				printf(output_msg[1],usbufCnt,soc_perf_timestamp[usbufCnt],ucCnt,soc_ddr1_perf_counter1_buffer[usbufCnt]);
			}

			if(strcmp(output_msg[2],""))
			{
				for(usbufCnt = 0;usbufCnt < ARRAY_SIZE(soc_ddr1_perf_counter2_buffer);usbufCnt++)
				{
					printf(output_msg[2],usbufCnt,soc_perf_timestamp[usbufCnt],ucCnt,soc_ddr1_perf_counter2_buffer[usbufCnt]);
				}
				if(strcmp(output_msg[3],""))
				{
					for(usbufCnt = 0;usbufCnt < ARRAY_SIZE(soc_ddr1_perf_counter3_buffer);usbufCnt++)
					{
						printf(output_msg[3],usbufCnt,soc_perf_timestamp[usbufCnt],ucCnt,soc_ddr1_perf_counter3_buffer[usbufCnt]);
					}
				}
			}
		}
		else
		{
			printf("Usage : get_p0_ddr0_data 0/1\r\n");
			return CMD_RET_USAGE;
		}
	}
	
	return CMD_RET_SUCCESS;
}
static int soc_perf_clear_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char * const argv[])
{
	if (argc != 1) {
		printf("Usage : clear_data\r\n");
		return CMD_RET_USAGE;
	}else{
		/* 清空所有数据buffer */
		memset(soc_perf_timestamp,0u,sizeof(soc_perf_timestamp));
		memset(soc_noc_gdma_buffer,0u,sizeof(soc_noc_gdma_buffer));
		memset(soc_noc_port0_ddr0_buffer,0u,sizeof(soc_noc_port0_ddr0_buffer));
		memset(soc_noc_port0_ddr1_buffer,0u,sizeof(soc_noc_port0_ddr1_buffer));					
		memset(soc_noc_port1_ddr0_buffer,0u,sizeof(soc_noc_port1_ddr0_buffer));
		memset(soc_noc_port1_ddr1_buffer,0u,sizeof(soc_noc_port1_ddr1_buffer));

		memset(soc_ddr0_perf_counter0_buffer,0u,sizeof(soc_ddr0_perf_counter0_buffer));
		memset(soc_ddr0_perf_counter1_buffer,0u,sizeof(soc_ddr0_perf_counter1_buffer));					
		memset(soc_ddr0_perf_counter2_buffer,0u,sizeof(soc_ddr0_perf_counter2_buffer));
		memset(soc_ddr0_perf_counter3_buffer,0u,sizeof(soc_ddr0_perf_counter3_buffer));

		memset(soc_ddr1_perf_counter0_buffer,0u,sizeof(soc_ddr1_perf_counter0_buffer));
		memset(soc_ddr1_perf_counter1_buffer,0u,sizeof(soc_ddr1_perf_counter1_buffer));					
		memset(soc_ddr1_perf_counter2_buffer,0u,sizeof(soc_ddr1_perf_counter2_buffer));
		memset(soc_ddr1_perf_counter3_buffer,0u,sizeof(soc_ddr1_perf_counter3_buffer));
	}
	return CMD_RET_SUCCESS;
}
static cmd_tbl_t cmd_soc_perf_sub[] = {
    BST_CMD_MKENT(set_ctime, 2, 1, soc_perf_set_cycle_time, "", ""),
	BST_CMD_MKENT(set_stime, 2, 1, soc_perf_set_sample_cycle_time, "", ""),
    BST_CMD_MKENT(set_ddr_grp, 2, 1, soc_perf_set_ddr_monitor_group, "", ""),
	BST_CMD_MKENT(set_port_num, 2, 1, soc_perf_set_test_port, "", ""),
	BST_CMD_MKENT(start_perf_test, 1, 1, soc_perf_start_test, "", ""),
	BST_CMD_MKENT(get_gdma_data, 1, 1, soc_perf_get_gdma_data, "", ""),
	BST_CMD_MKENT(get_p0_ddr_data, 1, 1, soc_perf_get_p0_ddr_data, "", ""),
	BST_CMD_MKENT(get_p1_ddr_data, 1, 1, soc_perf_get_p1_ddr_data, "", ""),
	BST_CMD_MKENT(get_ddr_mdata, 2, 1, soc_perf_get_ddr_monitor_data, "", ""),
	BST_CMD_MKENT(clear_data, 1, 1, soc_perf_clear_data, "", ""),
};

static int do_soc_test(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'soc' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_soc_perf_sub[0], ARRAY_SIZE(cmd_soc_perf_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char soc_test_help_text[] =
    "set_ctime cycle_time_in_seconds       - set current test cycle time，defualt 10 seconds\n\r"
	"soc_perf_test set_stime sample_time_in_ticks        - set current test cycle time\n\r"
    "soc_perf_test set_ddr_grp group_num                 - set ddr monitor group number,default 0\n\r"
    "soc_perf_test set_port_num 0/1/2                    - set the test port,0 : Port0, 1 : Port1, 2 : all Ports,default 0\n\r"
    "soc_perf_test start_perf_test                       - start the soc perf test\n\r"
	"soc_perf_test get_gdma_data                         - get gdma probe data\n\r"
	"soc_perf_test get_p0_ddr_data                       - get p0 ddr probe data\n\r"
	"soc_perf_test get_p1_ddr_data                       - get p1 ddr probe data\n\r"
	"soc_perf_test get_ddr_mdata ddr_num(0 or 1)         - get ddr monitor data\n\r"
	"soc_perf_test clear_data                            - clear all soc perf test data\n\r";
#endif

BST_CMD(
    soc_perf_test, 7, 0, do_soc_test,
    "SOC performance test",
    soc_test_help_text
);