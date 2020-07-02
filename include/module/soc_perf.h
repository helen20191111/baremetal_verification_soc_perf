#ifndef _SOC_PERF_TEST_H_
#define _SOC_PERF_TEST_H_

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