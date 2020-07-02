///////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                           //
//  This file contains proprietary information that is the sole intellectual property        //
//  of Black Sesame Technologies, Inc. and its affiliates.  No portions of this material     //
//  may be reproduced in any form without the written permission of:                         //
//                                                                                           //
//  Black Sesame Technologies, Inc. and its affiliates                                       //
//  2255 Martin Ave. Suite D                                                                 //
//  Santa Clara, CA  95050                                                                   //
//  Copyright \@2018: all right reserved.                                                    //
//                                                                                           //
//  Notice:                                                                                  //
//                                                                                           //
//  You are running an EVALUATION distribution of the neural network tools provided by       //
//  Black Sesame Technologies, Inc. under NDA.                                               //
//  This copy may NOT be used in production or distributed to any third party.               //
//  For distribution or production, further Software License Agreement is required.          //
//                                                                                           //
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PWM_H
#define _PWM_H

#define AON_REG_0 0x0
#define AON_REG_1 0x04

#define PINMUX_PWM0 (1 << 24)
#define PINMUX_PWM1 (1 << 25)
#define PINMUX_PWM2 (1 << 26)
#define PINMUX_PWM3 (1 << 27)

#define TIMERNLOADCOUNT(_n) (0X00 + (_n - 1) * 0X14)
#define TIMERNCURRENTVALUE(_n) (0X04 + (_n - 1) * 0X14)
#define TIMERNCTROLREG(_n) (0X08 + (_n - 1) * 0X14)
#define TIMERNEOI(_n) (0X0C + (_n - 1) * 0X14)
#define TIMERNINTSTATUS(_n) (0X10 + (_n - 1) * 0X14)
#define TIMERSINTSTATUS (0XA0)
#define TIMERSEOI (0XA4)
#define TIMERSRAWINTSTATUS (0XA8)
#define TIMERS_COMP_VERSI (0XAC)
#define TIMERNLOADCOUNT2(_n) (0XB0 + (_n - 1) * 0X04)
#define TIMER_N_PROT_LEVEL(_n) (0XD0 + (_n - 1) * 0X04)

#define CFG_TIMER_A55_CNT (8) //A55 : 1*8
#define CFG_TIMER_LSP_CNT (2) //LSP : 2*2
#define CFG_TIMER_CNT_MAX CFG_TIMER_A55_CNT

#define TIMER_LOAD_VALUE (0x20ffff)

#define LSP0_GLB_CTRL_REG (0x4)
#define LSP1_GLB_CTRL_REG (0x4)

#define LSP0_TIMER_CH1_PAUSE_EN	(1<<21)
#define LSP0_TIMER_CH2_PAUSE_EN	(1<<22)

#define LSP1_TIMER_CH1_PAUSE_EN	(1<<20)
#define LSP1_TIMER_CH2_PAUSE_EN	(1<<21)

#endif
