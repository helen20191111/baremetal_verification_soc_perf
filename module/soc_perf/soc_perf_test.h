#ifndef _SOC_PERF_TEST_H_
#define _SOC_PERF_TEST_H_

#include <a1000/sysreg_base.h>

/* define CPU NOC probe base address*/
#define GDMA_PROBE_OFFSET               (0x3800u)
#define GDMA_PROBE_BASE_ADDR            (CPU_NOC_BASE_ADDR + GDMA_PROBE_OFFSET)
#define DDR0_P0_PROBE_OFFSET            (0x2800u)
#define DDR0_P0_PROBE_BASE_ADDR         (CPU_NOC_BASE_ADDR + DDR0_P0_PROBE_OFFSET)
#define DDR1_P0_PROBE_OFFSET            (0x3000u)
#define DDR1_P0_PROBE_BASE_ADDR         (CPU_NOC_BASE_ADDR + DDR1_P0_PROBE_OFFSET)
#define DDR0_P1_PROBE_OFFSET            (0x2c00u)
#define DDR0_P1_PROBE_BASE_ADDR         (CPU_NOC_BASE_ADDR + DDR0_P1_PROBE_OFFSET)
#define DDR1_P1_PROBE_OFFSET            (0x3400u)
#define DDR1_P1_PROBE_BASE_ADDR         (CPU_NOC_BASE_ADDR + DDR1_P1_PROBE_OFFSET)
/* define core noc probe base address */
#define VSP_PROBE_OFFSET                (0x3800u)
#define VSP_PROBE_BASE_ADDR             (CORE_NOC_BASE_ADDR + VSP_PROBE_OFFSET)
 /* define probe register address offset */
    #define MAIN_CTL_OFFSET             (0x08u)
    #define COUNTERS_0_PORT_SEL_OFFSET  (0x134u)
    #define COUNTERS_0_SRC_OFFSET       (0x138u)
    #define COUNTERS_1_SRC_OFFSET       (0x14Cu)
    #define STAT_PORIOD_OFFSET          (0x24u)
    #define GLOBAL_ENABLE_OFFSET        (0x0Cu)
    #define COUNTERS_0_VAL              (0x140u)
    #define COUNTERS_1_VAL              (0x154u)

/* DDR monitor control */
#define TOP_CRM_REG_R_TOP_CRM_BLOCK_SW_RST0_OFFSET      (0x04u)     //#define A55_CRM_ADDR (0x33002000)
#define LB_LPDDR4_APB_REG_R_LOCAL_CTRL_OFFSET           (0u)        //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_COUNTER_SEL_OFFSET          (0x30)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_MONITOR_CTRL                (0x2c)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)

#define LB_LPDDR4_APB_REG_R_PERF_MONITOR0_MSB_OFFSET    (0x8c)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_PERF_MONITOR0_LSB_OFFSET    (0x90)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_PERF_MONITOR1_MSB_OFFSET    (0x94)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_PERF_MONITOR1_LSB_OFFSET    (0x98)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_PERF_MONITOR2_MSB_OFFSET    (0x9c)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_PERF_MONITOR2_LSB_OFFSET    (0xa0)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_PERF_MONITOR3_MSB_OFFSET    (0xa4)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)
#define LB_LPDDR4_APB_REG_R_PERF_MONITOR3_LSB_OFFSET    (0xa8)      //#define DDRC0_CTRL_PHY_ADDR (0x38000000)

/* vsp probe */
extern void vsp_probe_init(void);
extern void vsp_probe_enable(void);
extern unsigned int observe_get_vsp_packet_counts(void);
extern unsigned int observe_get_vsp_baudrate(void);
/* GDMA probe */
extern void gdma_probe_init(void);
extern void gdma_probe_enable(void);
extern unsigned int observe_get_gdma_packet_counts(void);
extern unsigned int observe_get_gdma_baudrate(void);
/* DDR0 probe */
extern void ddr0_port0_probe_init(void);
extern void ddr0_port0_probe_enable(void);
extern void ddr0_port1_probe_init(void);
extern void ddr0_port1_probe_enable(void);
extern unsigned int observe_get_ddr0_p0_packet_counts(void);
extern unsigned int observe_get_ddr0_p1_packet_counts(void);
extern unsigned int observe_get_ddr0_p0_baudrate(void);
extern unsigned int observe_get_ddr0_p1_baudrate(void);
/* DDR1 probe */
extern void ddr1_port0_probe_init(void);
extern void ddr1_port0_probe_enable(void);
extern void ddr1_port1_probe_init(void);
extern void ddr1_port1_probe_enable(void);
extern unsigned int observe_get_ddr1_p0_packet_counts(void);
extern unsigned int observe_get_ddr1_p1_packet_counts(void);
extern unsigned int observe_get_ddr1_p0_baudrate(void);
extern unsigned int observe_get_ddr1_p1_baudrate(void);
/* DDR0 monitor */
extern void ddr0_monitor_init(void);
extern void ddr0_monitor_set_counter_group(u8 group_num);
extern void ddr0_monitor_update_monitor(void);
extern void ddr0_monitor_clear_counter(void);
extern u64 ddr0_monitor_get_counters_value(u8 counter_num);
/* DDR1 monitor */
extern void ddr1_monitor_init(void);
extern void ddr1_monitor_set_counter_group(u8 group_num);
extern void ddr1_monitor_update_monitor(void);
extern void ddr1_monitor_clear_counter(void);
extern u64 ddr1_monitor_get_counters_value(u8 counter_num);

extern void soc_perf_init(void);
#endif