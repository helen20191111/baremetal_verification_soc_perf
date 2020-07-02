#include <stdexcept>
#include <exception>
#include <queue>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <curses.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>

#include "/home/shares/zebu_ip_2017.12/XTOR/ZLPDDR4_XTOR.N-2017.12-SP1-1/include/ZLPDDR4Xtor.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/CAN_FD.N-2017.12-SP1-1/include/CAN_FD.hh"

#define THREADING svt_pthread_threading
#include "/home/shares/zebu_ip_2017.12/include/svt_pthread_threading.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_c_runtime_cfg.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_zebu_platform.hh"
#include "ZEMI3Xtor.hh"
#include "ZEMI3Manager.hh"

using namespace ZEBU;
using namespace ZEBU_IP;
using namespace ZEBU_MP;
using namespace ZEBU_CAN_FD;

using namespace DRAM_SW;
using namespace std;
CAN_FD *glcan_fd_i_1 = NULL;
extern svt_report      *glreporter;

void read_rxd_frame_cb(void* xtor_a, uint32_t* frame_ptr)
{
  static int cell_count = 0;
  char* local_str = new char[10000];
  std::stringstream frame_string;
  CAN_FD* xtor = (CAN_FD*)xtor_a;
  
  glreporter->info( "TEST","inside CALLBACK function read_rxd_frame_cb ", svt_report::MEDIUM);
  uint32_t frame_length = 2*frame_ptr[0]+2;
  glreporter->info("TEST","CB########RECIEVED_FRAME########", svt_report::HIGH);

  frame_string << "received_frame\t";
  for(uint32_t i=0; i<frame_length; i++)
  {
      sprintf(local_str,"Index: %32d, DATA 0x%32X",i,frame_ptr[i]);
      glreporter->info("TEST", local_str, svt_report::HIGH);
      frame_string << "0x" <<std::hex << frame_ptr[i] << "\t" << std::dec;
  }

  glreporter->info("TEST",frame_string.str().c_str(), svt_report::NONE);
  glreporter->info("TEST","########################", svt_report::HIGH);

  cell_count++;
  sprintf(local_str, "cell received by now %0d", cell_count);
  glreporter->info("TEST",local_str, svt_report::NONE);

  delete[] local_str;

}

int CanFDInit(Board* board)
{
	zCAN_FDCommonConfig *m_cfg_1;
	glcan_fd_i_1 = CAN_FD::get("A1000_tb.glcan_fd_i_1", runtime);
	if (glcan_fd_i_1 == NULL) {
		glreporter->fatal("SVT/ENV", "Invalid path to glcan_fd_i_1 transactor specified");
		return -1;
	}
	glcan_fd_i_1->set_debug_file("glcan_fd_i_1.log", 0);

	//can config
	m_cfg_1 = new zCAN_FDCommonConfig();
	m_cfg_1->retransmit_count = 5;

	m_cfg_1->time_quanta_n = 10;
	m_cfg_1->prop_seg_n   = 6;
	m_cfg_1->phase_seg1_n = 7;
	m_cfg_1->phase_seg2_n = 7;
	m_cfg_1->resync_jump_width_n = 3;
	m_cfg_1->time_quanta_d = 10;
	m_cfg_1->prop_seg_d   = 2;
	m_cfg_1->phase_seg1_d = 4;
	m_cfg_1->phase_seg2_d = 4;
	m_cfg_1->resync_jump_width_d = 2;
   
	m_cfg_1->early_sof_enable = 0;

	m_cfg_1->identifier = 0x123456;
	m_cfg_1->can_proto_type = ZEBU_CAN_FD::CAN_FD_0;  

	if(m_cfg_1 != NULL)
	{
	  if(!(glcan_fd_i_1->config(m_cfg_1)))
		  glreporter->fatal("SVT/DUT_CONFIG", "Invalid configuration of can_fd_i_1 transactor");
	}

	printf("DDEBUG_SW waiting for reset ...\n");
	while(!glcan_fd_i_1->run_until_reset());
	printf("DDEBUG_SW hw came out of reset ...\n");

	
	printf("DDEBUG_SW waiting for boot	...\n");
	while(!glcan_fd_i_1->check_boot_complete()); //waiting untill the Booting is complete
	printf("DDEBUG_SW hw boot completed  ...\n");
	glcan_fd_i_1->register_callback(&read_rxd_frame_cb, glcan_fd_i_1);

}

int CanFDRun()
{
	int num_frames = 3;
	ZEBU::Signal &can_clk_stop_ack_handle = *board->getSignal("can_fd_i_1.can_clk_stop_ack_1"); 
    svBitVecVal* local_frame_ptr;
	int local_frame_type_1 = 0;
	int num_frames_transmitted = 0;
	bool send_frame_status = true;
//yanfeng
	while (can_clk_stop_ack_handle == NULL)
	{
		printf("failed to get the ack signal\n")
	}
	/********** RUN CAN-FD testbench ********************/		
	for(int i=0; i < num_frames; i++)
	{
		local_frame_type_1 = rand()%4+4;
		local_frame_ptr = can_fd_i_1->generate_frame((can_frame_type_enum)(local_frame_type_1));
		send_frame_status = can_fd_i_1->send_frame(local_frame_ptr);
		if(send_frame_status == true){
			printf("\n####	send_frame called ####\n");
			num_frames_transmitted++;
			can_fd_i_1->dump_msg(local_frame_ptr);
			save_frame(local_frame_ptr, 1);
		}
		else
		{
			printf("\n#### send_frame cannot be called ####\n");
		}
	}

		while((can_fd_i_1->is_frame_tx_done() != 1))
		{
	//			printf("\n####	 waiting for can_fd_i_1 ####\n");
		}

	can_fd_i_1->run_clk(1000);


}
