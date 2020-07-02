#ifndef BST_GLOBAL_H
#define BST_GLOBAL_H

#include <stdexcept>
#include <exception>
#include <queue>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
//#include <curses.h>
#include "libZebu.hh"

#include "board.h"

#ifdef test_zcei_share_EXPORTS

#include "/home/shares/zebu_ip_2017.12/XTOR/UART.N-2017.12-SP1-1/include/Uart.hh"
#include "/home/shares/zebu_ip/XTOR/I2C.M-2017.03-2/include/HS_I2c.hh"
#include "/home/shares/zebu_ip/XTOR/ZLPDDR4_XTOR.M-2017.03-2/include/ZLPDDR4Xtor.hh"
//#include "/home/shares/zebu_ip/XTOR/I2S.M-2017.03-2/include/Audio.hh" 
#include "/home/shares/zebu_ip_2017.12/XTOR/I2S.N-2017.12-SP1-1/include/I2s.hh"
//#include "/home/shares/zebu_ip/XTOR/I2S.M-2017.03-2/include/I2s.hh"
#include "/home/shares/zebu_ip/XTOR/USB.M-2017.03-2/include/UsbStruct.hh"
#include "/home/shares/zebu_ip/XTOR/USB.M-2017.03-2/include/HostChannel.hh"
#include "/home/shares/zebu_ip/XTOR/USB.M-2017.03-2/include/DevEndpoint.hh"
#include "/home/shares/zebu_ip/XTOR/USB.M-2017.03-2/include/Usb.hh"
#endif

#ifdef test_zemi3_EXPORTS
#include <zlib.h>
#include "libZebuZEMI3.hh"
//#include "ZFSDB.hh"
#include "/home/shares/zebu_ip_2017.12/include/ZFSDB.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/UART.N-2017.12-SP1-1/include/Uart.hh"
#include "/home/shares/zebu_ip/XTOR/I2C.M-2017.03-2/include/HS_I2c.hh"
#include "/home/shares/zebu_ip/XTOR/ZLPDDR4_XTOR.M-2017.03-2/include/ZLPDDR4Xtor.hh"
//#include "/home/shares/zebu_ip/XTOR/I2S.M-2017.03-2/include/Audio.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/I2S.N-2017.12-SP1-1/include/I2s.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/UsbStruct.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/HostChannel.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/DevEndpoint.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/Usb.hh"
#include "/home/shares/zebu_ip_2017.12/XTOR/USB.N-2017.12-SP1-1/include/ZFSDB.hh"
#include "libZebuZEMI3.hh"
#include "ZEMI3Manager.hh"
#define THREADING svt_pthread_threading
#include "ZEMI3Xtor.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_zebu_platform.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_c_runtime_cfg.hh"
#include "/home/shares/zebu_ip_2017.12/include/svt_pthread_threading.hh"
#endif


using namespace ZEBU;
using namespace ZEBU_IP;
using namespace std;

#ifdef test_zemi3_EXPORTS
using namespace DRAM_SW;
using namespace ZEBU_MP;
#endif

extern Board * g_board;

Board* BoardOpen();
int BoardInit();

int I2S0Init(Board * board);
int I2S0Run();

int I2S1Init(Board * board);
int I2S1Run();


int USB2DevInit(Board* board);
int USB2DevRun();

int USB3DevInit(Board* board);
int USB3DevRun();

int USB3HostInit(Board* board);
int USB3HostRun();

int CanFDInit(Board * board);
int CanFDRun();

#endif

