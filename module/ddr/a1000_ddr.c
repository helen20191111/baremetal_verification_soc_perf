// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2016 Broadcom Ltd.
 */
#include "a1000_evb.h"
#include <common.h>

extern u32 pins_tbl[];

void switch_clk(void *base, int shift, int maskbits, int val)
{
	int regval;
	
	if((maskbits < 0) || (maskbits > 32))
		return;
		
	regval = readl(base);
	regval &= ~(pins_tbl[maskbits]<<shift);
	regval |= (val & pins_tbl[maskbits]) << shift;
	writel(regval, base);
}

//------------------------------------------------------------------------------
void init_ddrc_clkreset(void)
{
	printf("---%s %d\n", __func__, __LINE__);
	init_sys_pll();
	
	printf("---%s %d\n", __func__, __LINE__);
	//set input axi & apb clock
	switch_clk((void *)TOPCRM_REG_R_CLKMUX_SEL0, 14, 2, SW_SYSBUS_AXI_800M);//CLK800_SYSBUS_AXI;
	switch_clk((void *)TOPCRM_REG_R_CLKMUX_SEL1, 26, 2, SW_SYSBUS1_APB_100M);//CLK100_SYSBUS_APB;
	printf("---%s %d\n", __func__, __LINE__);
	
	//switch on DDRC clock enable, axi & apb
	switch_clk((void *)TOPCRM_REG_R_CLKGATE_EN0, 21, 8, 0xff);
	switch_clk((void *)TOPCRM_REG_R_CLKGATE_EN0, 19, 2, 0x3);
}

int ddrc_init(void *ddr_base)
{
	lpddr4_init_sequence(ddr_base);
	pr_debug("Completed lpddr4 initial.\n\r");
//------------------------------------------------------------------------------

	//`uvm_info("lpddr4_inst0_backdoor_init",$psprintf("%s backdoor init done",name),UVM_NONE);
	pr_debug( "backdoor init done\n\r");
	return 0;
}

void closeInterleave(void){
	int value;

    value = readl(0x33000058);
    value |= BIT(0);
    writel(value, 0x33000058);
}

/************************************************/
int bst_a1000_setup_ddr(void)
{	
	pr_debug("---%s %d\n", __func__, __LINE__);
	closeInterleave();
	pr_debug("---%s %d\n", __func__, __LINE__);
	init_ddrc_clkreset();
	
	pr_debug("---%s %d\n", __func__, __LINE__);
	ddrc_init((void *)A1000BASE_DDRC0);
	ddrc_init((void *)A1000BASE_DDRC1);
	
	return 0;
}

void ddr_rw_test0(void)
{
 int i, j, pos;
 u32 ui;
 void *ptrui;
	pr_debug("---%s %d\n", __func__, __LINE__);
 //test 1G of interval with 12MByte
 for(i=0; i<8; i++){
  ptrui = ((void *)0x80000000 + (i*(128<<20)));
  for(j=0; j<4096; j++) {
   pos = 0;
   writel(0x55555555, ptrui);
   ui = readl(ptrui);
   if(ui != 0x55555555){
    goto test_err;
   }
   pos += 1;
   writel(0xaaaaaaaa, ptrui);
   ui = readl(ptrui);
   if(ui != 0xaaaaaaaa){
    goto test_err;
   }
   pos += 1;
   writel(0xa5a5a5a5, ptrui);
   ui = readl(ptrui);
   if(ui != 0xa5a5a5a5){
    goto test_err;
   }
   pos += 1;
   writel(0x5a5a5a5a, ptrui);
   ui = readl(ptrui);
   if(ui != 0x5a5a5a5a){
    goto test_err;
   }
   
   pos += 1;
   writel(0xffffffff, ptrui);
   ui = readl(ptrui);
   if(ui != 0xffffffff){
    goto test_err;
   }
   pos += 1;
   writel(0x00000000, ptrui);
   ui = readl(ptrui);
   if(ui != 0x00000000){
    goto test_err;
   }
  }//end for(j)
  printf("Test Block-%d completed.\n\r", i);
 }//end for(i)
 printf("Test DDR access OK.\n\r");
 return;
 
test_err:
 printf("Test Block-%d(seq:%d, pos:%d) error.\n\r", i, j, pos);
 printf("Test DDR access failed!\n\r");
 pr_debug("DDR access check error, need do check and debug.\n\r");;
}


void ddr_rw_test1(void)
{
 int i, j, pos;
 u64 ui;
 void *ptrui;
	pr_debug("---%s %d\n", __func__, __LINE__);

 //test 1G of interval with 12MByte
 for(i=0; i<8; i++){
  ptrui = ((void *)0x180000000 + (i*(128<<20)));
  for(j=0; j<4096; j++) {
   pos = 0;
   writeq(0x55555555, ptrui);
   ui = readq(ptrui);
   if(ui != 0x55555555){
    goto test_err;
   }
   pos += 1;
   writeq(0xaaaaaaaa, ptrui);
   ui = readq(ptrui);
   if(ui != 0xaaaaaaaa){
    goto test_err;
   }
   pos += 1;
   writeq(0xa5a5a5a5, ptrui);
   ui = readq(ptrui);
   if(ui != 0xa5a5a5a5){
    goto test_err;
   }
   pos += 1;
   writeq(0x5a5a5a5a, ptrui);
   ui = readq(ptrui);
   if(ui != 0x5a5a5a5a){
    goto test_err;
   }
   
   pos += 1;
   writeq(0xffffffff, ptrui);
   ui = readq(ptrui);
   if(ui != 0xffffffff){
    goto test_err;
   }
   pos += 1;
   writeq(0x00000000, ptrui);
   ui = readq(ptrui);
   if(ui != 0x00000000){
    goto test_err;
   }
  }//end for(j)
  printf("Test Block-%d completed.\n\r", i);
 }//end for(i)
 printf("Test DDR access OK.\n\r");
 return;
 
test_err:
 printf("Test Block-%d(seq:%d, pos:%d) error.\n\r", i, j, pos);
 printf("Test DDR access failed!\n\r");
 pr_debug("DDR access check error, need do check and debug.\n\r");;
}
