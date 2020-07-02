////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2018- by BST, Inc.
//
//  All rights reserved. No part of this file may be reproduced, stored
//  in a retrieval system, or transmitted, in any form, or by any means,
//  electronic, mechanical, photocopying, recording, or otherwise,
//  without the prior consent of BST, inc.
//
//  File Name: lpddr4_init_sequence.sv
//  Arthor Email: wei.cai@bst.ai
//  Time :
//  Description: 
////////////////////////////////////////////////////////////////////////
#include <common.h>

#include "a1000_evb.h"

//`ifndef LPDDR4_1066_TRAINING_INIT_SEQUENCE__SV
//`define LPDDR4_1066_TRAINING_INIT_SEQUENCE__SV
//`ifdef TRAINING_1D

/*
	class lpddr4_init_sequence extends uvm_sequence;
    ral_sys_top_regmodel top_regmodel;
    uvm_reg_map  access_map;
    uvm_status_e status;
    bit [31:0] bst_lpddr4_base_addr;
    string name ;
    string pmu_train_string[int];
    bst_lpddr4_subenv_cfg cfg;

    `uvm_object_utils(lpddr4_init_sequence)
    function new(input string name="lpddr4_init_sequence",uvm_component parent=null);
        super.new(name);
    endfunction:new
    `include "lpddr4_apb_config_1066.sv"

    virtual task body();
        `uvm_info("lpddr4_1066_training_init_sequence()",$psprintf("%0s frondoor init with training begin,please wait...",name),UVM_NONE);

        this.lpddr4_read_pmu_string_file();
        this.lpddr4_apb_config(name);
        this.lpddr4_ctrl_post_init();

        `uvm_info("lpddr4_1066_training_init_sequence()",$psprintf("%0s frontdoor init with training done",name),UVM_NONE);
    endtask; //body
*/
int lpddr4_init_sequence(void *ddr_base){
	//lpddr4_read_pmu_string_file()
	
	//lpddr4_apb_config
	if(ddr_base == (void *)A1000BASE_DDRC0)
		lpddr4_apb_config(DDRC0_NAME, ddr_base);
	else if(ddr_base == (void *)A1000BASE_DDRC1)
		lpddr4_apb_config(DDRC1_NAME, ddr_base);
	else {
		printf("Error ddrc address: 0x%08x\n", ddr_base);
		return -1;
	}
	
	pr_debug("---%s %d\n", __func__, __LINE__);
	lpddr4_ctrl_post_init(ddr_base);
	return 0;
}

/*
    task lpddr4_ctrl_init();
        logic [31:0] r_data;
        logic [31:0] w_data;
        bit   [31:0] ddrc_base_addr=this.bst_lpddr4_base_addr+32'h*1000;
*/
void lpddr4_ctrl_init(void *ddr_base){
				void *ddrc_base_addr = ddr_base + 0x1000;
				
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000304); //DBG1
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000030); //PWRCTL
        writel(/*32'h*/0x80080020, ddrc_base_addr+/*32'h*/0x00000000); //MSTR
        writel(/*32'h*/0x00003010, ddrc_base_addr+/*32'h*/0x00000010); //MRCTRL0
        writel(/*32'h*/0x00000494, ddrc_base_addr+/*32'h*/0x00000014); //MRCTRL1
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000020); //DERATEEN
        writel(/*32'h*/0xb9e1a98c, ddrc_base_addr+/*32'h*/0x00000024); //DERATEINT
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x0000002c); //DERATECTL
        writel(/*32'h*/0x00000020, ddrc_base_addr+/*32'h*/0x00000030); //PWRCTL
        writel(/*32'h*/0x0040e804, ddrc_base_addr+/*32'h*/0x00000034); //PWRTMG
        writel(/*32'h*/0x00840000, ddrc_base_addr+/*32'h*/0x00000038); //HWLPCTL
        writel(/*32'h*/0x00210000, ddrc_base_addr+/*32'h*/0x00000050); //RFSHCTL0
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000060); //RFSHCTL3
        writel(/*32'h*/0x00208066, ddrc_base_addr+/*32'h*/0x00000064); //RFSHTMG
        writel(/*32'h*/0x00180000, ddrc_base_addr+/*32'h*/0x00000068); //RFSHTMG1
        writel(/*32'h*/0x063f7f10, ddrc_base_addr+/*32'h*/0x00000070); //ECCCFG0
        writel(/*32'h*/0x000007a0, ddrc_base_addr+/*32'h*/0x00000074); //ECCCFG1
        writel(/*32'h*/0x00000300, ddrc_base_addr+/*32'h*/0x0000007c); //ECCCTL
        writel(/*32'h*/0x000001c8, ddrc_base_addr+/*32'h*/0x000000b8); //ECCPOISONADDR0
        writel(/*32'h*/0x0401cfea, ddrc_base_addr+/*32'h*/0x000000bc); //ECCPOISONADDR1
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x000000c0); //CRCPARCTL0
        writel(/*32'h*/0xc0020002, ddrc_base_addr+/*32'h*/0x000000d0); //INIT0
        writel(/*32'h*/0x00010002, ddrc_base_addr+/*32'h*/0x000000d4); //INIT1
        writel(/*32'h*/0x00002705, ddrc_base_addr+/*32'h*/0x000000d8); //INIT2
        writel(/*32'h*/0x00140009, ddrc_base_addr+/*32'h*/0x000000dc); //INIT3
        writel(/*32'h*/0x00300008, ddrc_base_addr+/*32'h*/0x000000e0); //INIT4
        writel(/*32'h*/0x00020004, ddrc_base_addr+/*32'h*/0x000000e4); //INIT5
        writel(/*32'h*/0x0000004d, ddrc_base_addr+/*32'h*/0x000000e8); //INIT6
        writel(/*32'h*/0x0000004d, ddrc_base_addr+/*32'h*/0x000000ec); //INIT7
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x000000f0); //DIMMCTL
        writel(/*32'h*/0x0d0b110c, ddrc_base_addr+/*32'h*/0x00000100); //DRAMTMG0
        writel(/*32'h*/0x00030410, ddrc_base_addr+/*32'h*/0x00000104); //DRAMTMG1
        writel(/*32'h*/0x0305080c, ddrc_base_addr+/*32'h*/0x00000108); //DRAMTMG2
        writel(/*32'h*/0x00505000, ddrc_base_addr+/*32'h*/0x0000010c); //DRAMTMG3
        writel(/*32'h*/0x05040305, ddrc_base_addr+/*32'h*/0x00000110); //DRAMTMG4
        writel(/*32'h*/0x02030404, ddrc_base_addr+/*32'h*/0x00000114); //DRAMTMG5
        writel(/*32'h*/0x01010004, ddrc_base_addr+/*32'h*/0x00000118); //DRAMTMG6
        writel(/*32'h*/0x00000302, ddrc_base_addr+/*32'h*/0x0000011c); //DRAMTMG7
        writel(/*32'h*/0x00000101, ddrc_base_addr+/*32'h*/0x00000120); //DRAMTMG8
        writel(/*32'h*/0x00020000, ddrc_base_addr+/*32'h*/0x00000130); //DRAMTMG12
        writel(/*32'h*/0x0a100002, ddrc_base_addr+/*32'h*/0x00000134); //DRAMTMG13
        writel(/*32'h*/0x00000067, ddrc_base_addr+/*32'h*/0x00000138); //DRAMTMG14
        writel(/*32'h*/0xc10b0008, ddrc_base_addr+/*32'h*/0x00000180); //ZQCTL0
        writel(/*32'h*/0x00e3abc0, ddrc_base_addr+/*32'h*/0x00000184); //ZQCTL1
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000188); //ZQCTL2
        writel(/*32'h*/0x03858202, ddrc_base_addr+/*32'h*/0x00000190); //DFITMG0
        writel(/*32'h*/0x00090202, ddrc_base_addr+/*32'h*/0x00000194); //DFITMG1
        writel(/*32'h*/0x07705031, ddrc_base_addr+/*32'h*/0x00000198); //DFILPCFG0
        writel(/*32'h*/0x80400018, ddrc_base_addr+/*32'h*/0x000001a0); //DFIUPD0
        writel(/*32'h*/0x00450055, ddrc_base_addr+/*32'h*/0x000001a4); //DFIUPD1
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x000001a8); //DFIUPD2
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x000001b0); //DFIMISC
        writel(/*32'h*/0x00000502, ddrc_base_addr+/*32'h*/0x000001b4); //DFITMG2
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x000001c0); //DBICTL
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x000001c4); //DFIPHYMSTR
        writel(/*32'h*/0x00080808, ddrc_base_addr+/*32'h*/0x00000204); //ADDRMAP1
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000208); //ADDRMAP2
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x0000020c); //ADDRMAP3
        writel(/*32'h*/0x00001f1f, ddrc_base_addr+/*32'h*/0x00000210); //ADDRMAP4
        writel(/*32'h*/0x070f0707, ddrc_base_addr+/*32'h*/0x00000214); //ADDRMAP5
        writel(/*32'h*/0x07070707, ddrc_base_addr+/*32'h*/0x00000218); //ADDRMAP6
        writel(/*32'h*/0x00000f0f, ddrc_base_addr+/*32'h*/0x0000021c); //ADDRMAP7
        writel(/*32'h*/0x07070707, ddrc_base_addr+/*32'h*/0x00000224); //ADDRMAP9
        writel(/*32'h*/0x07070707, ddrc_base_addr+/*32'h*/0x00000228); //ADDRMAP10
        writel(/*32'h*/0x00000007, ddrc_base_addr+/*32'h*/0x0000022c); //ADDRMAP11
        writel(/*32'h*/0x06040610, ddrc_base_addr+/*32'h*/0x00000240); //ODTCFG
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000244); //ODTMAP
        writel(/*32'h*/0x00c21f01, ddrc_base_addr+/*32'h*/0x00000250); //SCHED
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000254); //SCHED1
        writel(/*32'h*/0x0f000001, ddrc_base_addr+/*32'h*/0x0000025c); //PERFHPR1
        writel(/*32'h*/0x0f00007f, ddrc_base_addr+/*32'h*/0x00000264); //PERFLPR1
        writel(/*32'h*/0x0f00007f, ddrc_base_addr+/*32'h*/0x0000026c); //PERFWR1
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000300); //DBG0
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000304); //DBG1
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x0000030c); //DBGCMD
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000320); //SWCTL
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000328); //SWCTLSTATIC
        writel(/*32'h*/0x00303031, ddrc_base_addr+/*32'h*/0x00000330); //OCPARCFG0
        writel(/*32'h*/0x00000834, ddrc_base_addr+/*32'h*/0x00000334); //OCPARCFG1
        writel(/*32'h*/0x00110011, ddrc_base_addr+/*32'h*/0x0000036c); //POISONCFG
        writel(/*32'h*/0x000000d2, ddrc_base_addr+/*32'h*/0x00000374); //ADVECCINDEX
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x0000037c); //ECCPOISONPAT0
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000384); //ECCPOISONPAT2
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x000003c0); //REGPARCFG
        writel(/*32'h*/0x00010001, ddrc_base_addr+/*32'h*/0x000003e0); //OCCAPCFG
        writel(/*32'h*/0x00010000, ddrc_base_addr+/*32'h*/0x000003e8); //OCCAPCFG1
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000490); //PCTRL_0
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000540); //PCTRL_1
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x000005f0); //PCTRL_2
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x000006a0); //PCTRL_3
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000400); //PCCFG
        writel(/*32'h*/0x00303030, ddrc_base_addr+/*32'h*/0x00000330); //OCPARCFG0
        writel(/*32'h*/0x0000100f, ddrc_base_addr+/*32'h*/0x00000404); //PCFGR_0
        writel(/*32'h*/0x0000100f, ddrc_base_addr+/*32'h*/0x000004b4); //PCFGR_1
        writel(/*32'h*/0x0000100f, ddrc_base_addr+/*32'h*/0x00000564); //PCFGR_2
        writel(/*32'h*/0x0000100f, ddrc_base_addr+/*32'h*/0x00000614); //PCFGR_3
        writel(/*32'h*/0x0000100f, ddrc_base_addr+/*32'h*/0x00000408); //PCFGW_0
        writel(/*32'h*/0x0000100f, ddrc_base_addr+/*32'h*/0x000004b8); //PCFGW_1
        writel(/*32'h*/0x0000100f, ddrc_base_addr+/*32'h*/0x00000568); //PCFGW_2
        writel(/*32'h*/0x0000100f, ddrc_base_addr+/*32'h*/0x00000618); //PCFGW_3
        writel(/*32'h*/0x000001a2, ddrc_base_addr+/*32'h*/0x00000030); //PWRCTL
}//    endtask:lpddr4_ctrl_init
    
/*
    task lpddr4_ctrl_post_cfg();
        logic [31:0] r_data;
        logic [31:0] w_data;
        bit   [31:0] ddrc_base_addr=this.bst_lpddr4_base_addr+32'h1000;
*/
void lpddr4_ctrl_post_cfg(void *ddr_base){
				unsigned int r_data, w_data;
				void *ddrc_base_addr = ddr_base + 0x1000;
    
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000304); 
        r_data=readl( ddrc_base_addr+/*32'h*/0x00000060); //RFSHCTL
        w_data=r_data|(/*32'h*/0x1<<0);
        writel(w_data, ddrc_base_addr+/*32'h*/0x00000060);       
        writel(/*32'h*/0x000001a0, ddrc_base_addr+/*32'h*/0x00000030); //PWRCTL
        writel(/*32'h*/0x00000202, ddrc_base_addr+/*32'h*/0x00000020); //DERATEEN
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000320);
        while(1){//begin
            r_data=readl(ddrc_base_addr+/*32'h*/0x00000324);
            if(r_data==/*32'h*/0x0){//begin
                break;
            }//end
        }//end
        writel(/*32'h*/0x00001000, ddrc_base_addr+/*32'h*/0x000001b0);
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000320);
        while(1){//begin
            r_data=readl(ddrc_base_addr+/*32'h*/0x00000324);
            if(r_data == /*32'h*/0x1){//begin
                break;
            }//end
        }//end
}//    endtask:lpddr4_ctrl_post_cfg
    
/*
    task lpddr4_ctrl_post_init();
        logic [31:0] r_data;
        logic [31:0] w_data;
        bit   [31:0] ddrc_base_addr=this.bst_lpddr4_base_addr+32'h1000;
        logic [5:0]  rd2wr;
*/
void lpddr4_ctrl_post_init(void *ddr_base){
				unsigned int r_data, w_data;
				void *ddrc_base_addr = ddr_base + 0x1000;
				
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000304);
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000320); 
        while(1){//begin
            r_data=readl(ddrc_base_addr+/*32'h*/0x00000324);
            if(r_data==/*32'h*/0x00000000){//begin
                break;
            }//end
        }//end
        writel(/*32'h*/0x00001020, ddrc_base_addr+/*32'h*/0x000001b0); 
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000320); 
        while(1){//begin
            r_data=readl(ddrc_base_addr+/*32'h*/0x00000324);   
            if(r_data == /*32'h*/0x1){//begin
                break;
            }//end
        }//end
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000304);
        while(1){//begin
            r_data=readl(ddrc_base_addr+/*32'h*/0x000001bc);    
            if(r_data==/*32'h*/0x00000001){//begin
                break;
            }//end
        }//end
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000320); //SWCTL
        while(1){//begin
            r_data=readl(ddrc_base_addr+/*32'h*/0x00000324); //SWSTAT
            if(r_data==/*32'h*/0x00000000){//begin
                break;
            }//end
        }//end
        writel(/*32'h*/0x00001000, ddrc_base_addr+/*32'h*/0x000001b0); 
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000304); 
        r_data=readl( ddrc_base_addr+/*32'h*/0x00000108); //DRAMTMG2
        r_data += (0x1<<8);//rd2wr=r_data[13:8]+6'h1;r_data[13:8]=rd2wr;
        writel(r_data, ddrc_base_addr+/*32'h*/0x00000108); //DRAMTMG2
        r_data=readl( ddrc_base_addr+/*32'h*/0x00000108); //DRAMTMG2
        writel(/*32'h*/0x00000000, ddrc_base_addr+/*32'h*/0x00000304); 
        writel(/*32'h*/0x00001001, ddrc_base_addr+/*32'h*/0x000001b0); 
        writel(/*32'h*/0x00000180, ddrc_base_addr+/*32'h*/0x00000030); 
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000320); 
        while(1){//begin
            r_data=readl(ddrc_base_addr+/*32'h*/0x00000324);
            if(r_data==/*32'h*/0x1){//begin
                break;
            }//end
        }//end
        while(1){//begin
            r_data=readl(ddrc_base_addr+/*32'h*/0x00000004);
            if(r_data==/*32'h*/0x1){//begin
                break;
            }//end
        }//end

        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000490);
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x00000540);    
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x000005f0);    
        writel(/*32'h*/0x00000001, ddrc_base_addr+/*32'h*/0x000006a0); 

        r_data=readl(ddrc_base_addr+/*32'h*/0x00000060);
        w_data=r_data&(~(/*32'h*/0x1<<0));
        writel(w_data, ddrc_base_addr+/*32'h*/0x00000060);
}//    endtask:lpddr4_ctrl_post_init

/*
    task automatic lpddr4_read_pmu_string_file;
        integer data_file;
        integer code;
        string line;
        string index_str;
        string message;
        int index;
        string debug_message_file_path;
        
        begin
            debug_message_file_path = {`TB_HOME,"/seq/lpddr_seq/lpddr4_pmu_train.strings"};
            data_file = $fopen(debug_message_file_path, "r");
            if (!data_file ) begin
                $fclose(data_file);
                `uvm_fatal("pre_init()","data_file handle was NULL");
            end else begin
                `uvm_info("pre_init()","data_file is opened",UVM_LOW);
                while (!$feof(data_file)) begin
                    code = $fgets(line,data_file);
                    index_str = line.substr(2,9);
                    index = index_str.atohex();
                    message = line.substr(11,line.len()-1);
                    this.pmu_train_string[index] = message;
                end
            end
        end
    endtask

    task automatic lpddr4_dwc_ddrphy_phyinit_userCustom_overrideUserInput();
        `uvm_info("phy_init()","In lpddr4_dwc_ddrphy_phyinit_userCustom_overrideUserInput.",UVM_LOW);
    endtask

    task automatic lpddr4_dwc_ddrphy_phyinit_userCustom_A_bringupPower();
        `uvm_info("phy_init()","In lpddr4_dwc_ddrphy_phyinit_userCustom_A_bringupPower",UVM_LOW);
        if(name=="lpddr4_inst0")begin
            lpddr4_inst0_power_up();
        end
        else if(name=="lpddr4_inst1")begin
            lpddr4_inst1_power_up();
        end
    endtask
*/

/*
    task automatic lpddr4_dwc_ddrphy_phyinit_userCustom_B_startClockResetPhy();
        `uvm_info("phy_init()","In lpddr4_dwc_ddrphy_phyinit_userCustom_B_startClockResetPhy",UVM_LOW);
*/
void lpddr4_dwc_ddrphy_phyinit_userCustom_B_startClockResetPhy(char * name, void *bst_lpddr4_base_addr) {
        lpddr4_clk_rst_init(name, bst_lpddr4_base_addr);
        lpddr4_ctrl_post_cfg(bst_lpddr4_base_addr);
}//    endtask
    
/*
    task automatic lpddr4_dwc_ddrphy_phyinit_userCustom_customPreTrain();
        `uvm_info("phy_init()","In lpddr4_dwc_ddrphy_phyinit_userCustom_customPreTrain.",UVM_LOW);
    endtask
    
    task automatic lpddr4_dwc_ddrphy_phyinit_userCustom_E_setDfiClk(int PState);
        `uvm_info("phy_init()",$psprintf("In lpddr4_dwc_ddrphy_phyinit_userCustom_E_setDfiClk(%0d)", PState),UVM_LOW);
        //cfg.PState = PState;
        //repeat(30) @(posedge tb_top.A_XTAL_IN);
        bst_realtime.delay_ns(1200);
    endtask
    
    task automatic lpddr4_dwc_ddrphy_phyinit_userCustom_H_readMsgBlock(int Train2D=0);
        `uvm_info("phy_init()","In lpddr4_dwc_ddrphy_phyinit_userCustom_H_readMsgBlock.",UVM_LOW);
    endtask
      
    task automatic lpddr4_dwc_ddrphy_phyinit_userCustom_customPostTrain();
        `uvm_info("phy_init()","In lpddr4_dwc_ddrphy_phyinit_userCustom_customPostTrain.",UVM_LOW);
    endtask
*/
void lpddr4_dwc_ddrphy_phyinit_userCustom_H_readMsgBlock(void *ddr_phy_base_addr) {
	return;
}
void lpddr4_dwc_ddrphy_phyinit_userCustom_customPostTrain(void) {
	return;
}

/*
    task automatic lpddr4_dwc_ddrphy_phyinit_userCustom_G_waitFwDone;
        bit [31:0] ddr_phy_base_addr=this.bst_lpddr4_base_addr+32'h2000;
        reg [31:0] data;
        reg        train_done=0;
        reg [31:0] mail;
*/
void lpddr4_dwc_ddrphy_phyinit_userCustom_G_waitFwDone(void *ddr_base){
				unsigned int data, mail, train_done=0;
				void *ddr_phy_base_addr = ddr_base + 0x2000;

        {//begin
            //repeat(10) @(posedge tb_top.A_XTAL_IN);
            delay_1k(40);//bst_realtime.delay_ns(400);
            
            //$display ($time, " <%m> INFO: polling DWC_DDRPHYA_APBONLY0__UctShadowRegs, bit 0...");
            while(train_done == 0){//begin
                //repeat(500) @(posedge tb_top.A_XTAL_IN);
                delay_1k(2000);//bst_realtime.delay_ns(20000);
                data=readl(ddr_phy_base_addr+/*32'h*/0x000d0004*/*32'h*/0x4);
                if((data&0x1) == 0){//begin
                		mail=lpddr4_get_mail(ddr_base,0);
                    if(mail==/*16'h*/0xff || mail==/*16'h*/0x07){//begin
                        train_done = 1;
                    }//end
                    /*lpddr4_decode_major_message(mail);
                    if(mail == 16'h08) lpddr4_decode_streaming_message;
                    else               $display($time,"INFO: Mail Box = %x", mail);*/
                  pr_debug("INFO: Mail Box = 0x%x\n\r", mail);
                }//end
            }//end
            if(mail==/*16'h*/0xff) {//begin
                ;//`uvm_fatal(get_type_name(),$psprintf("Error: Firmware training failed."));
            }//end
        }//end
}//endtask:lpddr4_dwc_ddrphy_phyinit_userCustom_G_waitFwDone

/*
    task automatic lpddr4_get_mail;
        input         mode_32bits;
        output [31:0] mail;
        reg    [31:0] rd_data;
        int           wd_timer2;
        bit    [31:0] ddr_phy_base_addr=this.bst_lpddr4_base_addr+32'h2000;
*/
unsigned int lpddr4_get_mail(void *ddr_base, int mode_32bits){
        unsigned int mail, rd_data, high_byte_data;
        unsigned int wd_timer2 = 0;
        void *ddr_phy_base_addr = ddr_base + 0x2000;
	
        {//begin
            rd_data=readl(ddr_phy_base_addr+/*32'h*/0x000d0004*/*32'h*/0x4);
            while((rd_data&0x1) !=0) {//begin
                rd_data=readl(ddr_phy_base_addr+/*32'h*/0x000d0004*/*32'h*/0x4);
            }//end
            mail=readl(ddr_phy_base_addr+/*32'h*/0x000d0032*/*32'h*/0x4);
            if(mode_32bits){//begin
                high_byte_data=readl(ddr_phy_base_addr+/*32'h*/0x000d0034*/*32'h*/0x4);
                mail = (high_byte_data<<16) | mail;
            }//end
            writel(/*32'h*/0x0, ddr_phy_base_addr+/*32'h*/0x000d0031*/*32'h*/0x4);
            rd_data=readl(ddr_phy_base_addr+/*32'h*/0x000d0004*/*32'h*/0x4);
            while((rd_data&0x1) == 0){//begin
                rd_data=readl(ddr_phy_base_addr+/*32'h*/0x000d0004*/*32'h*/0x4);
                //$display ($time, " <%m> INFO: Msg read. Waiting acknowledgement from uCtl ...");
                // Watchdog timer to ensure no infiit looping during polling
                //$display($time, " INFO: Watchdog timer2   = %d", wd_timer2);
                wd_timer2++;
                if(wd_timer2 > 1000) {//begin
                    ;//`uvm_fatal(get_type_name(),$psprintf("TC ERROR: <%m> Watchdog timer2 overflow"));
                }//end 
            }//end
            wd_timer2 = 0;
            writel(/*32'h*/0x1, ddr_phy_base_addr+/*32'h*/0x000d0031*/*32'h*/0x4);
        }//end
        return mail;
}//endtask
      
/*
    task automatic lpddr4_decode_major_message;
        input [15:0] mail;

        begin
            case(mail)
                16'h00: $display ($time, " <%m> PMU Major Msg: End of initialization                                         ");
                16'h01: $display ($time, " <%m> PMU Major Msg: End of fine write leveling                                    ");
                16'h02: $display ($time, " <%m> PMU Major Msg: End of read enable training                                   ");
                16'h03: $display ($time, " <%m> PMU Major Msg: End of read delay center optimization                         ");
                16'h04: $display ($time, " <%m> PMU Major Msg: End of write delay center optimization                        ");
                16'h05: $display ($time, " <%m> PMU Major Msg: End of 2D read delay/voltage center optimization              ");
                16'h06: $display ($time, " <%m> PMU Major Msg: End of 2D write delay /voltage center optimization            ");
                16'h07: $display ($time, " <%m> PMU Major Msg: Firmware run has completed                                    ");
                16'h08: $display ($time, " <%m> PMU Major Msg: Enter streaming message mode                                  ");
                16'h0a: $display ($time, " <%m> PMU Major Msg: End of read dq deskew training                                ");
                16'h0b: $display ($time, " <%m> PMU Major Msg: End of LCDL offset calibration                                ");
                16'h0c: $display ($time, " <%m> PMU Major Msg: End of LRDIMM Specific training (DWL, MREP, MRD and MWD)      ");
                16'h0d: $display ($time, " <%m> PMU Major Msg: End of CA training                                            ");
                16'hfd: $display ($time, " <%m> PMU Major Msg: End of MPR read delay center optimization                     ");
                16'hfe: $display ($time, " <%m> PMU Major Msg: End of Write leveling coarse delay                            ");
                16'hff: $display ($time, " <%m> PMU Major Msg: FATAL ERROR.                                                  ");
                default: $display ($time, " <%m> PMU Major Msg: Un-recognized message... !");
            endcase
        end
    endtask

    task automatic lpddr4_decode_streaming_message;
      reg [31:0] codede_message_hex;
      reg [15:0] num_args;
      reg [15:0] args_list[20];
      string debug_string;
      begin
        lpddr4_get_mail(1,codede_message_hex);
        if(pmu_train_string.exists(codede_message_hex))begin
          // Get the number of argument need to be read from mailbox
          num_args = 16'hFFFF & codede_message_hex;
          for(integer i=0;i<num_args;i++) begin
             lpddr4_get_mail(1,args_list[i]);
          end
          $display ($time, " <%m> PMU Streaming Msg decoded ...  ");
          case(num_args)
            0:  $display (pmu_train_string[codede_message_hex]);
            1:  $display (pmu_train_string[codede_message_hex],args_list[0]);
            2:  $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1]);
            3:  $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2]);
            4:  $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3]);
            5:  $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4]);
            6:  $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5]);
            7:  $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6]);
            8:  $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7]);
            9:  $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8]);
            10: $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8],args_list[9]);
            11: $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8],args_list[9],args_list[10]);
            12: $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8],args_list[9],args_list[10],args_list[11]);
            13: $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8],args_list[9],args_list[10],args_list[11],args_list[12]);
            14: $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8],args_list[9],args_list[10],args_list[11],args_list[12],args_list[13]);
            15: $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8],args_list[9],args_list[10],args_list[11],args_list[12],args_list[13],args_list[14]);
            16: $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8],args_list[9],args_list[10],args_list[11],args_list[12],args_list[13],args_list[14],args_list[15]);
            17: $display (pmu_train_string[codede_message_hex],args_list[0],args_list[1],args_list[2],args_list[3],args_list[4],args_list[5],args_list[6],args_list[7],args_list[8],args_list[9],args_list[10],args_list[11],args_list[12],args_list[13],args_list[14],args_list[15],args_list[16]);
            default:$display (pmu_train_string[codede_message_hex]);
          endcase
        end else begin
          $display ($time, " <%m> PMU Streaming Msg: Debug message not recognized !!  code: %h", codede_message_hex);
          //error;
        end
      end
    endtask
*/

/*
    task lpddr4_clk_rst_init();
        logic [31:0] r_data;
        logic [31:0] w_data;
        bit   [31:0] ddr_local_base_addr;
        bit   [31:0] ddrc_base_addr;
        bit   [31:0] ddr_phy_base_addr;
    
        ddr_local_base_addr = this.bst_lpddr4_base_addr+32'h0;
        ddrc_base_addr      = this.bst_lpddr4_base_addr+32'h1000;
        ddr_phy_base_addr   = this.bst_lpddr4_base_addr+32'h2000;
*/
void lpddr4_clk_rst_init(char * name, void *ddr_base){
        unsigned int r_data, w_data;
        void *ddr_local_base_addr, *ddrc_base_addr, *ddr_phy_base_addr;
        
        ddr_local_base_addr = ddr_base+/*32'h*/0x0;
        ddrc_base_addr      = ddr_base+/*32'h*/0x1000;
        ddr_phy_base_addr   = ddr_base+/*32'h*/0x2000;
        
	    pr_debug("---%s %d\n", __func__, __LINE__);
    
        r_data=readl(TOP_CRM_BASE_ADDR+0x04);
		printf("*******r_data: %08x\n", r_data);
        /*if(strcmp(name, "lpddr4_inst0")==0)*/ r_data |= 1<<5;
       /* if(strcmp(name, "lpddr4_inst1")==0)*/ r_data |= 1<<4;
        writel(r_data, TOP_CRM_BASE_ADDR+0x04);
        r_data=readl(TOP_CRM_BASE_ADDR+0x04);
		pr_debug("*******r_data: %08x\n", r_data);
	    pr_debug("---%s %d\n", __func__, __LINE__);
    
        r_data=readl(ddr_local_base_addr+/*32'h*/0x0);
        r_data |= 1<<15;//r_data[15]=1'b1; 
        r_data |= 1<<14;//r_data[14]=1'b1;
        r_data &= ~(0xf<<10);//r_data[13:10]=4'h0;
        writel(r_data, ddr_local_base_addr+/*32'h*/0x0);
		printf("---%s %d\n", __func__, __LINE__);
    
        lpddr4_ddr_pll_init(ddr_base);
    
		pr_debug("---%s %d\n", __func__, __LINE__);
        delay_1k(100);//bst_realtime.delay_ns(1000);
        r_data=readl(ddr_local_base_addr+/*32'h*/0x0);
        r_data &= ~(1<<15);//r_data[15] = 1'b0;
        writel(r_data, ddr_local_base_addr+/*32'h*/0x0);
    
	    pr_debug("---%s %d\n", __func__, __LINE__);
        delay_1k(132);//bst_realtime.delay_ns(1319);
        r_data=readl(ddr_local_base_addr+/*32'h*/0x0);       
        w_data    = r_data;
        w_data |= (1<<9);//w_data[9] = 1'b1;
        writel(w_data, ddr_local_base_addr+/*32'h*/0x0);     
    
	    pr_debug("---%s %d\n", __func__, __LINE__);
        lpddr4_ctrl_init(ddr_base);
    
	    pr_debug("---%s %d\n", __func__, __LINE__);
        delay_1k(105);//bst_realtime.delay_ns(1042);
        r_data=readl(ddr_local_base_addr+/*32'h*/0x0);      
        w_data    = r_data;
        w_data |= (1<<8);//w_data[8] = 1'b1; 
        writel(w_data, ddr_local_base_addr+/*32'h*/0x0);     
    
	    pr_debug("---%s %d\n", __func__, __LINE__);
        delay_1k(53);//bst_realtime.delay_ns(430);
        r_data=readl(ddr_local_base_addr+/*32'h*/0x0);
        r_data |= 1<<15;//r_data[15] = 1'b1; 
        writel(r_data, ddr_local_base_addr+/*32'h*/0x0);
    
        r_data=readl(ddr_local_base_addr+/*32'h*/0x0);
        r_data |= 0xf<<10;//r_data[13:10]=4'hf; 
        writel(r_data, ddr_local_base_addr+/*32'h*/0x0);
	    pr_debug("---%s %d\n", __func__, __LINE__);
}//    endtask:lpddr4_clk_rst_init
 
/*   
    task lpddr4_ddr_pll_init();
        bit   [31:0] ddr_local_base_addr;
        bit[31:0] rdata;
        bit[31:0] wdata;
    
        ddr_local_base_addr = this.bst_lpddr4_base_addr+32'h0;
*/
void lpddr4_ddr_pll_init(void *ddr_base){
        unsigned int rdata;
        void *ddr_local_base_addr = ddr_base+/*32'h*/0x0;
        
        rdata=readl(ddr_local_base_addr+/*32'h*/0x08);
        rdata |= 1<<13;//rdata[13]=1'b1;      //DSMEN
        rdata |= 1<<5;//rdata[5]=1'b1;       //EN
        writel(rdata, ddr_local_base_addr+/*32'h*/0x08);
    
        rdata=readl(ddr_local_base_addr+/*32'h*/0x08);
        rdata = (rdata&(~(0xfff<<16)))|(0x2a<<16);//rdata[27:16]=12'h2A; //FBDIV
        rdata = (rdata&(~(0x7<<9)))|(0x4<<9);//rdata[11:9] =3'h4;   //POSTDIV1
        rdata = (rdata&(~(0x7<<6)))|(0x1<<6);//rdata[8:6]  =3'h1;   //POSTDIV2
        writel(rdata, ddr_local_base_addr+/*32'h*/0x08);
    
        rdata=readl(ddr_local_base_addr+/*32'h*/0x0c);
      //rdata[23:0]=24'hA3_E70A; //FRAC
        rdata = (rdata&(~(0xffffff)))|(0xaa5c29);;//rdata[23:0]=24'hAA_5C29; //FRAC
        writel(rdata, ddr_local_base_addr+/*32'h*/0x0c);
    
        while(1){//begin
            rdata=readl(ddr_local_base_addr+/*32'h*/0x10);
            if((rdata&0x1)==0x1){//begin
                break;
            }//end
        }//end
    
        rdata=readl(ddr_local_base_addr+/*32'h*/0x04);
        rdata |= 1<<4;//rdata[4]=1'b1;
        writel(rdata, ddr_local_base_addr+/*32'h*/0x04);
}//    endtask:lpddr4_ddr_pll_init

/*
    task writel(input bit[31:0] waddr, input bit[31:0] wdata);
        svt_axi_master_transaction axi_trans;
        `uvm_do_with(axi_trans,{
            xact_type == svt_axi_transaction::WRITE; 
            atomic_type == svt_axi_transaction::NORMAL;
            burst_type ==  svt_axi_transaction::INCR;
            burst_size ==  svt_axi_transaction::BURST_SIZE_32BIT;
            burst_length == 1;
            addr == waddr;
            data.size() == 1;
            wstrb.size() == 1;
            wvalid_delay.size() == 1;
            data[0][31:0] == wdata;
            wstrb[0][3:0] == 4'hf;})
        get_response(rsp);
    endtask

    task readl(input bit [31:0] raddr, output bit [31:0] rdata);
        svt_axi_master_transaction axi_trans;
        `uvm_do_with(axi_trans,{
            xact_type == svt_axi_transaction::READ; 
            atomic_type == svt_axi_transaction::NORMAL;
            burst_type ==  svt_axi_transaction::INCR;
            burst_size ==  svt_axi_transaction::BURST_SIZE_32BIT;
            burst_length == 1;
            addr == raddr;})
        get_response(rsp);
        $cast(axi_trans, rsp);
        rdata = axi_trans.data[0][31:0];
    endtask

    task lpddr4_inst0_power_up();
        `ifdef POST_SIM
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_0.VDD        =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_0.VDDQ       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_0.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_0.VSS        =  1'b0;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_1.VDD        =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_1.VDDQ       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_1.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_1.VSS        =  1'b0;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_2.VDD        =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_2.VDDQ       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_2.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_2.VSS        =  1'b0;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_3.VDD        =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_3.VDDQ       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_3.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_3.VSS        =  1'b0;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_4.VDD        =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_4.VDDQ       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_4.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_4.VSS        =  1'b0;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_5.VDD        =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_5.VDDQ       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_5.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_5.VSS        =  1'b0;

            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_0.VDD       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_0.VDDQ      =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_0.VDDQLP    =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_0.VSS       =  1'b0;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_1.VDD       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_1.VDDQ      =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_1.VDDQLP    =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_1.VSS       =  1'b0;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_2.VDD       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_2.VDDQ      =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_2.VDDQLP    =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_2.VSS       =  1'b0;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_3.VDD       =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_3.VDDQ      =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_3.VDDQLP    =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_3.VSS       =  1'b0;

            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VAA    =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VDD    =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VDDQ   =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VDDQLP =  1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VSS    =  1'b0;
        `else
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_MAS  = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_0 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_1 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_2 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_3 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_4 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_5 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_DB_0 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_DB_1 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_DB_2 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_DB_3 = 1'b1;

            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_MAS  = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_0 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_1 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_2 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_3 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_4 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_5 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_DB_0 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_DB_1 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_DB_2 = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_DB_3 = 1'b1;

            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDD       = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VAA       = 1'b1;
            force `HIER_LPDDR4_0.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VSS       = 1'b0;
        `endif
    endtask : lpddr4_inst0_power_up

    task lpddr4_inst1_power_up();
        `ifdef POST_SIM
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_0.VDD        =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_0.VDDQ       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_0.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_0.VSS        =  1'b0;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_1.VDD        =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_1.VDDQ       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_1.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_1.VSS        =  1'b0;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_2.VDD        =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_2.VDDQ       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_2.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_2.VSS        =  1'b0;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_3.VDD        =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_3.VDDQ       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_3.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_3.VSS        =  1'b0;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_4.VDD        =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_4.VDDQ       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_4.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_4.VSS        =  1'b0;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_5.VDD        =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_5.VDDQ       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_5.VDDQLP     =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYACX4_5.VSS        =  1'b0;

            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_0.VDD       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_0.VDDQ      =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_0.VDDQLP    =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_0.VSS       =  1'b0;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_1.VDD       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_1.VDDQ      =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_1.VDDQLP    =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_1.VSS       =  1'b0;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_2.VDD       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_2.VDDQ      =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_2.VDDQLP    =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_2.VSS       =  1'b0;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_3.VDD       =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_3.VDDQ      =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_3.VDDQLP    =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYDBYTE_3.VSS       =  1'b0;

            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VAA    =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VDD    =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VDDQ   =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VDDQLP =  1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.u_DWC_DDRPHYMASTER_top.VSS    =  1'b0;
        `else
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_MAS  = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_0 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_1 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_2 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_3 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_4 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_AC_5 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_DB_0 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_DB_1 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_DB_2 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQ_DB_3 = 1'b1;
            
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_MAS  = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_0 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_1 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_2 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_3 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_4 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_AC_5 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_DB_0 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_DB_1 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_DB_2 = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDDQLP_DB_3 = 1'b1;
            
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VDD       = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VAA       = 1'b1;
            force `HIER_LPDDR4_1.lb_lpddr4_top_pre.u_DWC_DDRPHY0_top.VSS       = 1'b0;
        `endif
    endtask : lpddr4_inst1_power_up
*/

//-wyc-//endclass:lpddr4_init_sequence 

//-wyc-//`endif //TRAINING_1D
//-wyc-//`endif //LPDDR4_1066_TRAINING_INIT_SEQUENCE__SV
