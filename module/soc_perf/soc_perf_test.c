#include <common.h>
#include <module/a55_timer.h>
#include "soc_perf_test.h"

#define OBSERVE_WINDOWS_TIME_REG_VAL    (0x14u)    //2^20 cpu cycle time for a windows time
#define OBSERVE_CYCLE_TIMES            (0x100000) //2^20
#define OBSERVE_WINDOWS_TIME_NS         (OBSERVE_CYCLE_TIMES * 1000000 / A55_CLK_FREQ)
/**
 *  init observe probe 
 */
static void observe_probe_init(void *reg_base)
{
    void *probe_reg = reg_base;

    /* statEn = 1 */
    writel(0x08u, (probe_reg + MAIN_CTL_OFFSET)); 

    /* set portsel = 0 */
    writel(0,    (probe_reg + COUNTERS_0_PORT_SEL_OFFSET));
    
    /* set Counters_0_Src = 0x8 */
    writel(0x08, (probe_reg + COUNTERS_0_SRC_OFFSET)); 

    /*  Counters_1_Src =0x10, connect two counters to avoid overflow */
    writel(0x10, (probe_reg + COUNTERS_1_SRC_OFFSET)); 

    /* set 2^20 cpu cycle time for a windows time */ 
    writel(OBSERVE_WINDOWS_TIME_REG_VAL, (probe_reg + STAT_PORIOD_OFFSET)); 
}

/**
 * enable probe
 */
static void observe_probe_enable(void *reg_base)
{
    void *probe_reg = reg_base;

    writel(0x01u, (probe_reg + GLOBAL_ENABLE_OFFSET));
}

/**
 * observe packet num
 */
static unsigned int observe_get_counters_value(void * reg_base)
{
    unsigned int counter_0 = 0;
    unsigned int counter_1 = 0;
    unsigned int counter_val = 0;
    void *probe_reg = reg_base;

    counter_0 = readl((probe_reg + COUNTERS_0_VAL)); // read out counter 0
    counter_1 = readl((probe_reg + COUNTERS_1_VAL)); // read out counter 1
    
    counter_0 = counter_0 & (0x3FFFu); // counter_0 is valid for lsb 14 bit
    counter_1 = counter_1 & (0x3FFFu); //counter_1 is valid for lsb 14 bit

    counter_val = (counter_1 << 14) | counter_0;

    /* TODO connect counter 0 and counter 1 , they are valid for 14bit */
    return counter_val;
}

/**
 *  init vsp probe
 */
void vsp_probe_init(void)
{
    observe_probe_init((void *)VSP_PROBE_BASE_ADDR);
}

/**
 *  vsp probe enable
 */
void vsp_probe_enable(void)
{
    observe_probe_enable((void *)VSP_PROBE_BASE_ADDR);
}

/**
 *  gdma probe control init
 */
void gdma_probe_init(void)
{  
    observe_probe_init((void *)GDMA_PROBE_BASE_ADDR);
}

/**
 * gdma probe enable
 */
void gdma_probe_enable(void)
{
    observe_probe_enable((void *)GDMA_PROBE_BASE_ADDR);
}


/* P0 port ddr probe interface start */

/* ddr0 port0 probe init and enable */
void ddr0_port0_probe_init(void)
{
    observe_probe_init((void *)DDR0_P0_PROBE_BASE_ADDR);
}
void ddr0_port0_probe_enable(void)
{
    observe_probe_enable((void *)DDR0_P0_PROBE_BASE_ADDR);
}

/*  ddr1 port0 probe init and enable */
void ddr1_port0_probe_init(void)
{
    observe_probe_init((void *)DDR1_P0_PROBE_BASE_ADDR);
}
void ddr1_port0_probe_enable(void)
{
    observe_probe_enable((void *)DDR1_P0_PROBE_BASE_ADDR);
}
/* P0 port ddr probe interface end */


/* P1 port ddr probe interface start */

/* ddr0 port1 probe init and enable */
void ddr0_port1_probe_init(void)
{
    observe_probe_init((void *)DDR0_P1_PROBE_BASE_ADDR);
}
void ddr0_port1_probe_enable(void)
{
    observe_probe_enable((void *)DDR0_P1_PROBE_BASE_ADDR);
}

/* ddr1 port1 probe init and enable */
void ddr1_port1_probe_init(void)
{
    observe_probe_init((void *)DDR1_P1_PROBE_BASE_ADDR);
}
void ddr1_port1_probe_enable(void)
{
    observe_probe_enable((void *)DDR1_P1_PROBE_BASE_ADDR);
}
/* P1 port ddr probe interface end */


/**
 * get packet numbers tranfer by gdma
 */
unsigned int observe_get_gdma_packet_counts(void)
{
    unsigned int counter_val = 0;

    counter_val = observe_get_counters_value((void *)GDMA_PROBE_BASE_ADDR);

    return counter_val;
}
/**
 * clc baudrate of gdma
 */

unsigned int observe_get_gdma_baudrate(void)
{
    u64 sample_time_us = 0;
    u64 packets_num = 0;
    unsigned int band_width = 0;

    sample_time_us = OBSERVE_CYCLE_TIMES / (A55_CLK_FREQ /1000000);
    packets_num = observe_get_gdma_packet_counts();    
    band_width = (packets_num * 1000) / sample_time_us; 

    return band_width;
}
/**
 * get packet numbers tranfer by ddr0 port0
 */
unsigned int observe_get_ddr0_p0_packet_counts(void)
{
    unsigned int counter_val = 0;

    counter_val = observe_get_counters_value((void *)DDR0_P0_PROBE_BASE_ADDR);

    return counter_val;    
}
/**
 * clc baudrate of ddr0 port0
 */

unsigned int observe_get_ddr0_p0_baudrate(void)
{
    unsigned int sample_time_us = 0;
    unsigned int packets_num = 0;
    unsigned int band_width = 0;

    sample_time_us = OBSERVE_CYCLE_TIMES / (A55_CLK_FREQ /1000000);
    packets_num = observe_get_ddr0_p0_packet_counts();
    band_width = (packets_num * 1000) / sample_time_us;

    return band_width;
}
/**
 * get packet number transfer by ddr1 port0
 */
unsigned int observe_get_ddr1_p0_packet_counts(void)
{
    unsigned int counter_val = 0;

    counter_val = observe_get_counters_value((void *)DDR1_P0_PROBE_BASE_ADDR);

    return counter_val;    
}

/**
 * clc baudrate of ddr1 port0
 */

unsigned int observe_get_ddr1_p0_baudrate(void)
{
    unsigned int sample_time_us = 0;
    unsigned int packets_num = 0;
    unsigned int band_width = 0;

    sample_time_us = OBSERVE_CYCLE_TIMES / (A55_CLK_FREQ /1000000);
    packets_num = observe_get_ddr1_p0_packet_counts();
    band_width = (packets_num * 1000) / sample_time_us;

    return band_width;
}

/**
 * get packet numbers tranfer by ddr0 port1
 */
unsigned int observe_get_ddr0_p1_packet_counts(void)
{
    unsigned int counter_val = 0;

    counter_val = observe_get_counters_value((void *)DDR0_P1_PROBE_BASE_ADDR);

    return counter_val;    
}

/**
 * clc baudrate of ddr0 port1
 */

unsigned int observe_get_ddr0_p1_baudrate(void)
{
    unsigned int sample_time_us = 0;
    unsigned int packets_num = 0;
    unsigned int band_width = 0;

    sample_time_us = OBSERVE_CYCLE_TIMES / (A55_CLK_FREQ /1000000);
    packets_num = observe_get_ddr0_p1_packet_counts();
    band_width = (packets_num * 1000) / sample_time_us;

    return band_width;
}

/**
 * get packet number transfer by ddr1 port1
 */
unsigned int observe_get_ddr1_p1_packet_counts(void)
{
    unsigned int counter_val = 0;

    counter_val = observe_get_counters_value((void *)DDR1_P1_PROBE_BASE_ADDR);

    return counter_val;    
}

/**
 * clc baudrate of ddr1 port1
 */

unsigned int observe_get_ddr1_p1_baudrate(void)
{
    unsigned int sample_time_us = 0;
    unsigned int packets_num = 0;
    unsigned int band_width = 0;

    sample_time_us = OBSERVE_CYCLE_TIMES / (A55_CLK_FREQ /1000000);
    packets_num = observe_get_ddr1_p1_packet_counts();
    band_width = (packets_num * 1000) / sample_time_us;

    return band_width;
}
/**
 * get packet numbers transfer by vsp
 */
unsigned int observe_get_vsp_packet_counts(void)
{
    unsigned int counter_val = 0;

    counter_val = observe_get_counters_value((void *)VSP_PROBE_BASE_ADDR);

    return counter_val;    
}

/**
 * clc baudrate of vsp
 */

unsigned int observe_get_vsp_baudrate(void)
{
    unsigned int sample_time_us = 0;
    unsigned int packets_num = 0;
    unsigned int band_width = 0;

    sample_time_us = OBSERVE_CYCLE_TIMES / (A55_CLK_FREQ /1000000);
    packets_num = observe_get_vsp_packet_counts();
    band_width = (packets_num * 1000) / sample_time_us;

    return band_width;
}

/**
 * monitor num
 */
static u64 monitor_get_counters_value(void * reg_base, u8 counter_num)
{
    u32     counter_l   =   0;
    u32     counter_h   =   0;
    u64     counter_val =   0;
    void    *probe_reg  =   reg_base;

    switch (counter_num)
    {
        case 0:
            counter_h = readl((probe_reg + LB_LPDDR4_APB_REG_R_PERF_MONITOR0_MSB_OFFSET)); 
            counter_l = readl((probe_reg + LB_LPDDR4_APB_REG_R_PERF_MONITOR0_LSB_OFFSET)); 

            break;
        case 1:
            counter_h = readl((probe_reg + LB_LPDDR4_APB_REG_R_PERF_MONITOR1_MSB_OFFSET)); 
            counter_l = readl((probe_reg + LB_LPDDR4_APB_REG_R_PERF_MONITOR1_LSB_OFFSET)); 
            break;
        case 2:
            counter_h = readl((probe_reg + LB_LPDDR4_APB_REG_R_PERF_MONITOR2_MSB_OFFSET)); 
            counter_l = readl((probe_reg + LB_LPDDR4_APB_REG_R_PERF_MONITOR2_LSB_OFFSET)); 
            break;
        case 3:
            counter_h = readl((probe_reg + LB_LPDDR4_APB_REG_R_PERF_MONITOR3_MSB_OFFSET)); 
            counter_l = readl((probe_reg + LB_LPDDR4_APB_REG_R_PERF_MONITOR3_LSB_OFFSET)); 
            break;                            
        default:
            break;
    }
    counter_val = (((u64)counter_h) << 32u) | counter_l;

    return counter_val;
}

/* ddr0 monitor control interface start */
void ddr0_monitor_init(void)
{
    u32 regVal = 0;

    /* ddr0 block soft reset */
    regVal = readl(A55_CRM_ADDR+TOP_CRM_REG_R_TOP_CRM_BLOCK_SW_RST0_OFFSET);
    regVal = (regVal | (1u << 5u));
    writel(regVal,A55_CRM_ADDR+TOP_CRM_REG_R_TOP_CRM_BLOCK_SW_RST0_OFFSET);

    /* controller and phy soft reset */
    regVal = readl(DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_LOCAL_CTRL_OFFSET);
    regVal = (regVal | (1u << 8u));
    writel(regVal,DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_LOCAL_CTRL_OFFSET);
}

void ddr0_monitor_set_counter_group(u8 group_num)
{
    u32 regVal = 0;

    regVal = readl(DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_COUNTER_SEL_OFFSET);
    regVal = (regVal & ~(0x0fu << 2u));
    regVal = (regVal | (group_num << 2u));
    writel(regVal,DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_COUNTER_SEL_OFFSET);  

    /* reset value of update monitor reg and performance counter clear reg */
    regVal = readl(DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
    regVal = (regVal & ~((1u << 4u) | (1u << 6u)));
    writel(regVal,DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL); 
}
void ddr0_monitor_update_monitor(void)
{
    u32 regVal = 0;

    regVal = readl(DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
    regVal = (regVal | (1u << 6u));
    writel(regVal,DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
}
void ddr0_monitor_clear_counter(void)
{
    u32 regVal = 0;

    regVal = readl(DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
    regVal = (regVal | (1u << 4u));
    writel(regVal,DDRC0_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
}
u64 ddr0_monitor_get_counters_value(u8 counter_num)
{
    u64 counter_val = 0;

    counter_val = monitor_get_counters_value((void *)DDRC0_CTRL_PHY_ADDR,counter_num);

    return counter_val;
}
/* ddr0 monitor control interface end */

/* ddr1 monitor control interface start */
void ddr1_monitor_init(void)
{
    u32 regVal = 0;

    /* ddr1 block soft reset */
    regVal = readl(A55_CRM_ADDR+TOP_CRM_REG_R_TOP_CRM_BLOCK_SW_RST0_OFFSET);
    regVal = (regVal | (1u << 4u));
    writel(regVal,A55_CRM_ADDR+TOP_CRM_REG_R_TOP_CRM_BLOCK_SW_RST0_OFFSET);

    /* controller and phy soft reset */
    regVal = readl(DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_LOCAL_CTRL_OFFSET);
    regVal = (regVal | (1u << 8u));
    writel(regVal,DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_LOCAL_CTRL_OFFSET);
}

void ddr1_monitor_set_counter_group(u8 group_num)
{
    u32 regVal = 0;

    regVal = readl(DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_COUNTER_SEL_OFFSET);
    regVal = (regVal & ~(0x0fu << 2u));
    regVal = (regVal | (group_num << 2u));
    writel(regVal,DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_COUNTER_SEL_OFFSET);  

    /* reset value of update monitor reg and performance counter clear reg */
    regVal = readl(DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
    regVal = (regVal & ~((1u << 4u) | (1u << 6u)));
    writel(regVal,DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
}
void ddr1_monitor_update_monitor(void)
{
    u32 regVal = 0;

    regVal = readl(DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
    regVal = (regVal | (1u << 6u));
    writel(regVal,DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
}
void ddr1_monitor_clear_counter(void)
{
    u32 regVal = 0;

    regVal = readl(DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
    regVal = (regVal | (1u << 4u));
    writel(regVal,DDRC1_CTRL_PHY_ADDR+LB_LPDDR4_APB_REG_R_MONITOR_CTRL);
}
u64 ddr1_monitor_get_counters_value(u8 counter_num)
{
    u64 counter_val = 0;

    counter_val = monitor_get_counters_value((void *)DDRC1_CTRL_PHY_ADDR,counter_num);

    return counter_val;
}
/* ddr1 monitor control interface end */

void soc_perf_init(void)
{
    vsp_probe_init();
    gdma_probe_init();
    ddr0_port0_probe_init();
    ddr0_port1_probe_init();
    ddr1_port0_probe_init();
    ddr1_port1_probe_init();
    ddr0_monitor_init();
    ddr1_monitor_init();
}