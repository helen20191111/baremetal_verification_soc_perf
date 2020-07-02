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

#include <a1000/sysreg_base.h>
#include <general_inc.h>
#include <utility_lite.h>
#include <test_common.h>
#include <common.h>
#include <uart_sync.h>
#include <ut_test.h>
#include <int_num.h>
#include <module/uart.h>
#include <module/a55_timer.h>
#include <module/gpio.h>

#define DATA_LEN 256

struct _uart_irq {
	u32 modem_status;
	u32 thr_empty;
	u32 rx_available;
	u32 rx_line_status;
	u32 character_timeout;
	u32 busy_detect;
	u32 invalid;
} uart_irq;

u32 baud_rate[] = {
	2400, 4800, 9600, 19200, 38400, 43000, 56000, 57600, 115200,
};

extern unsigned char getc(void);
extern void putc(unsigned char);
extern test_static hvte_static;

//volatile u32 uart_buf[64] __attribute__((align(16)));
volatile u32 uart_buf[64];

u32 uart_irq_handler(u32 flags, s8 *name)
{
	u32 reg;

	reg = REG32_READ(UART_REG_IIR);
	while (NO_INTERRUPT_PENDING != reg) {
		switch (reg & 0xFF) {
		case 0x0:
			uart_irq.modem_status++;
			break;
		case 0x2:
			uart_irq.thr_empty++;
			break;
		case 0x4:
			uart_irq.rx_available++;
			break;
		case 0x6:
			uart_irq.rx_line_status++;
			break;
		case 0x7:
			uart_irq.busy_detect++;
			break;
		case 0xC:
			uart_irq.character_timeout++;
			break;
		default:
			uart_irq.invalid++;
			break;
		}

		reg = REG32_READ(UART_REG_IIR);
	}
	return 0;
}

u32 uart_irq_handler_with_htx(u32 flags, s8 *name)
{
	u32 reg;

	REG32_WRITE(UART_REG_HTX, 0x1); //halt tx
	reg = REG32_READ(UART_REG_IIR);
	while (NO_INTERRUPT_PENDING != reg) {
		switch (reg & 0xFF) {
		case 0x0:
			uart_irq.modem_status++;
			break;
		case 0x2:
			uart_irq.thr_empty++;
			break;
		case 0x4:
			uart_irq.rx_available++;
			break;
		case 0x6:
			uart_irq.rx_line_status++;
			break;
		case 0x7:
			uart_irq.busy_detect++;
			break;
		case 0xC:
			uart_irq.character_timeout++;
			break;
		default:
			uart_irq.invalid++;
			break;
		}

		reg = REG32_READ(UART_REG_IIR);
	}
	return 0;
}

void uart_irq_set(unsigned int reg_base, uart_irq_id irq, bool_enum enable)
{
	switch (irq) {
	case UART_IRQ_ERBFI:
		REG32_WRITE(reg_base + UART_REG_IER,
			    enable ? REG32_READ(reg_base + UART_REG_IER) |
					     (0x1 << UART_IRQ_ERBFI) :
				     REG32_READ(reg_base + UART_REG_IER) &
					     (~(0x1 << UART_IRQ_ERBFI)));
		break;
	case UART_IRQ_ETBEI:
		REG32_WRITE(reg_base + UART_REG_IER,
			    enable ? REG32_READ(reg_base + UART_REG_IER) |
					     (0x1 << UART_IRQ_ETBEI) :
				     REG32_READ(reg_base + UART_REG_IER) &
					     (~(0x1 << UART_IRQ_ETBEI)));
		break;
	case UART_IRQ_ELSI:
		REG32_WRITE(reg_base + UART_REG_IER,
			    enable ? REG32_READ(reg_base + UART_REG_IER) |
					     (0x1 << UART_IRQ_ELSI) :
				     REG32_READ(reg_base + UART_REG_IER) &
					     (~(0x1 << UART_IRQ_ELSI)));
		break;
	case UART_IRQ_EDSSI:
		REG32_WRITE(reg_base + UART_REG_IER,
			    enable ? REG32_READ(reg_base + UART_REG_IER) |
					     (0x1 << UART_IRQ_EDSSI) :
				     REG32_READ(reg_base + UART_REG_IER) &
					     (~(0x1 << UART_IRQ_EDSSI)));
		break;
	case UART_IRQ_ELCOLR:
		REG32_WRITE(reg_base + UART_REG_IER,
			    enable ? REG32_READ(reg_base + UART_REG_IER) |
					     (0x1 << UART_IRQ_ELCOLR) :
				     REG32_READ(reg_base + UART_REG_IER) &
					     (~(0x1 << UART_IRQ_ELCOLR)));
		break;
	case PTIME:
		REG32_WRITE(reg_base + UART_REG_IER,
			    enable ? REG32_READ(reg_base + UART_REG_IER) |
					     (0x1 << PTIME) :
				     REG32_READ(reg_base + UART_REG_IER) &
					     (~(0x1 << PTIME)));
		break;
	case UART_IRQ_ALL:
		REG32_WRITE(reg_base + UART_REG_IER,
			    enable ? REG32_READ(reg_base + UART_REG_IER) |
					     (0xFFFFFFFF) :
				     REG32_READ(reg_base + UART_REG_IER) &
					     (~0xFFFFFFFF));
		break;
	default:
		hprintf(ERROR, "invalid irq id: %s.\r\n", __FUNCTION__);
		break;
	}
}

u32 uart_test2(u32 flags, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	
	hprintf(DEBUG, "this is %s.\r\n", __FUNCTION__);

	return result;
}

void uart_pinmux_init(unsigned int reg_base)
{
#if 0  
	 /* 引脚复用 */
    if(reg_base == UART0_BASE_ADDR)
    {
        gpio_pinmux_init_one(Gpio_37, 0x0);//A_UART0_TXD
        gpio_pinmux_init_one(Gpio_38, 0x0);//A_UART0_RXD
        gpio_pinmux_init_one(Gpio_39, 0x0);//A_UART0_CTS
        gpio_pinmux_init_one(Gpio_40, 0x0);//A_UART0_RTS
    }else if(reg_base == UART1_BASE_ADDR)
    {
        gpio_pinmux_init_one(Gpio_41, 0x0);//A_UART1_TXD
        gpio_pinmux_init_one(Gpio_45, 0x0);//A_UART1_RXD
        gpio_pinmux_init_one(Gpio_46, 0x0);//A_UART1_CTS
        gpio_pinmux_init_one(Gpio_62, 0x0);//A_UART1_RTS
    }
    else if(reg_base == UART2_BASE_ADDR)
    {
        gpio_pinmux_init_one(Gpio_66, 0x0);//A_UART2_TXD
        gpio_pinmux_init_one(Gpio_70, 0x0);//A_UART2_RXD
        gpio_pinmux_init_one(Gpio_71, 0x0);//A_UART2_CTS
        gpio_pinmux_init_one(Gpio_47, 0x0);//A_UART2_RTS
    }
    else if(reg_base == UART3_BASE_ADDR)
    {
        gpio_pinmux_init_one(Gpio_48, 0x0);//A_UART3_TXD
        gpio_pinmux_init_one(Gpio_64, 0x0);//A_UART3_RXD
        gpio_pinmux_init_one(Gpio_68, 0x0);//A_UART3_CTS
        gpio_pinmux_init_one(Gpio_49, 0x0);//A_UART3_RTS
   }
#endif
    if(reg_base == UART0_BASE_ADDR)
    {
        unsigned int val  = REG32_READ(AON_PMM_REG_BASE_ADDR);
        val = val & (~(0xf << 13));
        REG32_WRITE(AON_PMM_REG_BASE_ADDR, val);
    }else if(reg_base == UART1_BASE_ADDR)
    {
        unsigned int val  = REG32_READ(AON_PMM_REG_BASE_ADDR);
        val = val & (~(0xf << 17));
        REG32_WRITE(AON_PMM_REG_BASE_ADDR, val);
    }
    else if(reg_base == UART2_BASE_ADDR)
    {
        unsigned int val  = REG32_READ(AON_PMM_REG_BASE_ADDR);
        val = val & (~(0xf << 21));
        REG32_WRITE(AON_PMM_REG_BASE_ADDR, val);
    }
    else if(reg_base == UART3_BASE_ADDR)
    {
        unsigned int val  = REG32_READ(AON_PMM_REG_BASE_ADDR);
        val = val & (~(0xf << 25));
        REG32_WRITE(AON_PMM_REG_BASE_ADDR, val);
    }
}

void uart_init(unsigned int reg_base)
{
	uart_pinmux_init(reg_base);
	REG32_WRITE(reg_base + UART_REG_MCR, 0); //disable flow ctrl
	REG32_WRITE(reg_base + UART_REG_MCR,
		    (REG32_READ(reg_base + UART_REG_MCR) |
		     (0x1 << 1))); //clear rts

#if defined(CONFIG_ZEBU)
	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) |
		     (0x1 << 7))); //enable access DLL & DLH
	REG32_WRITE(reg_base + UART_REG_DLL, 1); //set baud rate

	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) &
		     (~(0x1 << 7)))); //clear DLAB bit
#else
	uart_baud_set(reg_base,38400);
#endif	 

	REG32_WRITE(reg_base+UART_REG_LCR, REG32_READ(reg_base+UART_REG_LCR)|(0x3)); //set data length to 8 bit, 1 stop bit,no parity
	udelay(10);
}

void uart_baud_set(unsigned int reg_base, u32 baud)
{
#if defined(CONFIG_ZEBU)
	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) |
		     (0x1 << 7))); //enable access DLL & DLH
	REG32_WRITE(reg_base + UART_REG_DLL, 1); //set baud rate

	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) &
		     (~(0x1 << 7)))); //clear DLAB bit
#else
	u32 int_val;
	u32 frac_val;
	u32 temp = 0;
    
	if((baud > MAX_BAUD) || (baud < MIN_BAUD)) {
		hprintf(ERROR, "baud rate parameter is not right %s.\r\n", __FUNCTION__);
		return;
	}

	temp = (UART_SRC_CLK<<(DLF_SIZE-4))/baud;
	int_val = temp>>DLF_SIZE;
	frac_val = temp&((1<<DLF_SIZE)-1);
	if(0 == int_val) { //DLL and DLH can not be all 0
		int_val = 0x1;
	}

	REG32_WRITE(reg_base+UART_REG_LCR, (REG32_READ(reg_base+UART_REG_LCR)|(0x1<<7))); //enable access DLL & DLH
	REG32_WRITE(reg_base+UART_REG_DLL, (int_val & 0xFF)); //integer low 8 bits
	REG32_WRITE(reg_base+UART_REG_DLH, ((int_val & 0xFF00)>>8)); //integer high 8 bits
	REG32_WRITE(reg_base+UART_REG_DLF, frac_val); //fractional part, DLF_SIZE = 6

	REG32_WRITE(reg_base+UART_REG_LCR, ( REG32_READ(reg_base+UART_REG_LCR)&(~(0x1<<7)) ) ); //clear DLAB bit

	//	for(i = 0; i < 1000; i++) {
	//		j = i;
	//	}
	udelay(10);
#endif
}

void uart_stop_bit_set(unsigned int reg_base, stop_bit stop_bit)
{
	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) & (~(0x1 << 2))) |
			    (stop_bit << 2));
}

void uart_data_bit_set(unsigned int reg_base, data_bit data_bit)
{
	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) & (~0x3)) | data_bit);
}

void uart_parity_enable(unsigned int reg_base, u32 enable)
{
	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) & (~(0x1 << 3))) |
			    (enable << 3));
}

void uart_fifo_enable(unsigned int reg_base, bool_enum enable)
{
	REG32_WRITE(reg_base + UART_REG_FCR, (enable << 0));
}

void uart_stick_parity_enable(unsigned int reg_base, bool_enum enable)
{
	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) & (~(0x1 << 5))) |
			    (enable << 5));
}

void uart_even_parity_set(unsigned int reg_base, bool_enum even_parity)
{
	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) & (~(0x1 << 4))) |
			    (even_parity << 4));
}

void uart_break_control_enable(unsigned int reg_base, bool_enum enable)
{
	REG32_WRITE(reg_base + UART_REG_LCR,
		    (REG32_READ(reg_base + UART_REG_LCR) & (~(0x1 << 6))) |
			    (enable << 6));
}

void rx_fifo_reset(unsigned int reg_base)
{
	REG32_WRITE(reg_base + UART_REG_FCR, (1 << 1));
}

void tx_fifo_reset(unsigned int reg_base)
{
	REG32_WRITE(reg_base + UART_REG_FCR, (1 << 2));
}

void uart_rts_set(unsigned int reg_base, bool_enum rts)
{
	REG32_WRITE(reg_base + UART_REG_MCR,
		    (REG32_READ(reg_base + UART_REG_MCR) & (~(0x1 << 1))) |
			    (rts << 1));
}

void uart_loopback_set(unsigned int reg_base, bool_enum loopback)
{
	REG32_WRITE(reg_base + UART_REG_MCR,
		    (REG32_READ(reg_base + UART_REG_MCR) & (~(0x1 << 4))) |
			    (loopback << 4));
}

void uart_afc_set(unsigned int reg_base, bool_enum afce)
{
	REG32_WRITE(reg_base + UART_REG_MCR,
		    (REG32_READ(reg_base + UART_REG_MCR) & (~(0x1 << 5))) |
			    (afce << 5));
}

void uart_reset(void)
{
//BST A1000 uart do not support SW reset
#if 0
	u32 i, j;

	REG32_WRITE(UART_REG_SRR, 0x1);

	for(i = 0; i < 1000; i++) {
		j = i;
	}

	REG32_WRITE(UART_REG_SRR, 0x0);
#endif
}

//test2

int uart_register_default_val_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 final_result = HVTE_MODULE_TEST_PASS;
	unsigned int reg_base;
	reg_base = mod->reg_base;

	hprintf(DEBUG, "this is RBR.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_RBR)); //only read
	hprintf(DEBUG, "this is DLL.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_DLL));
	hprintf(DEBUG, "this is THR.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_THR));
	hprintf(DEBUG, "this is DLH.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_DLH));
	hprintf(DEBUG, "this is IER.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_IER));
	//	hprintf(DEBUG, "this is FCR.\r\n");//only write
	//	HCOMP(0x0,REG32_READ(reg_base+UART_REG_FCR));
	hprintf(DEBUG, "this is IIR.\r\n");
	HCOMP(0x1, REG32_READ(reg_base + UART_REG_IIR)); //only read
	hprintf(DEBUG, "this is LCR\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_LCR));
	hprintf(DEBUG, "this is MCR.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_MCR));
	hprintf(DEBUG, "this is LSR.\r\n");
	HCOMP(0x60, REG32_READ(reg_base + UART_REG_LSR)); //only read
	hprintf(DEBUG, "this is MSR.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_MSR)); //only read
	hprintf(DEBUG, "this is HTX.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_HTX));
	hprintf(DEBUG, "this is DMASA.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_DMASA));
	hprintf(DEBUG, "this is DLF.\r\n");
	HCOMP(0x0, REG32_READ(reg_base + UART_REG_DLF));
	hprintf(DEBUG, "this is PROT_LEVEL.\r\n");
	HCOMP(0x2, REG32_READ(reg_base + UART_REG_PROT_LEVEL));

	if (result == HVTE_MODULE_TEST_PASS) {
		hprintf(DEBUG, "OK\r\n");
	} else {
		final_result = HVTE_MODULE_TEST_FAIL;
	}
	return final_result;
}
//test3
int uart_register_rw_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 final_result = HVTE_MODULE_TEST_PASS;
	unsigned int reg_base;
	reg_base = mod->reg_base;

	hprintf(DEBUG, "RW resiter DLL \r\n");
	REG32_WRITE(UART_REG_LCR, (REG32_READ(UART_REG_LCR) |
				   (0x1 << 7))); //enable access DLAB
	REG_RW_TEST(reg_base + UART_REG_DLL, 0xFF);
	hprintf(DEBUG, "RW resiter DLH \r\n");
	REG_RW_TEST(reg_base + UART_REG_DLH, 0xFF);
	REG32_WRITE(UART_REG_LCR, (REG32_READ(UART_REG_LCR) &
				   (~(0x1 << 7)))); //clear DLAB bit
	hprintf(DEBUG, "RW resiter THR \r\n");
	REG_RW_TEST(reg_base + UART_REG_THR, 0xFF);
	hprintf(DEBUG, "RW resiter IER \r\n");
	REG_RW_TEST(reg_base + UART_REG_IER, 0xF);
	hprintf(DEBUG, "RW resiter LCR \r\n");
	REG_RW_TEST(reg_base + UART_REG_LCR, 0x40);
	hprintf(DEBUG, "RW resiter MCR \r\n");
	REG_RW_TEST(reg_base + UART_REG_MCR, 0x1F);
	if (result == HVTE_MODULE_TEST_PASS) {
		hprintf(DEBUG, "OK\r\n");
	} else {
		final_result = HVTE_MODULE_TEST_FAIL;
	}

	return final_result;
}
static void bits_menu(void)
{
	hprintf(DEBUG, "0.....5 bits data\r\n");
	hprintf(DEBUG, "1.....6 bits data\r\n");
	hprintf(DEBUG, "2.....7 bits data\r\n");
	hprintf(DEBUG, "3.....8 bits data\r\n");
}
//test6
int uart_fifo_dif_data_length_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	char c = 0;
	u32 data = 0;
	unsigned int reg_base;
	reg_base = mod->reg_base;
	bits_menu();

	uart_init(reg_base);
	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	uart_fifo_enable(reg_base, HVTE_TRUE);
	uart_parity_enable(reg_base, HVTE_TRUE);
	uart_even_parity_set(reg_base, UART_ODD_PARITY);
	uart_stick_parity_enable(reg_base, HVTE_FALSE);
	uart_stop_bit_set(reg_base, STOP_1_BIT);

	c = getc();
	putc(c);

	switch (c) {
	case '0':
		uart_data_bit_set(reg_base, DATA_5_BIT);
		data = REG32_READ(reg_base + UART_REG_LCR);
		hprintf(DEBUG, "this is  5 data bits 0x%x\r\n", data);
		REG32_WRITE(reg_base + UART_REG_THR, 0x51); //write to tx fifo
		break;
	case '1':
		uart_data_bit_set(reg_base, DATA_6_BIT);
		data = REG32_READ(reg_base + UART_REG_LCR);
		hprintf(DEBUG, "this is  6 data bits 0x%x\r\n", data);
		REG32_WRITE(reg_base + UART_REG_THR, 0x61); //write to tx fifo
		break;
	case '2':
		uart_data_bit_set(reg_base, DATA_7_BIT);
		data = REG32_READ(reg_base + UART_REG_LCR);
		hprintf(DEBUG, "this is  7 data bits 0x%x\r\n", data);
		REG32_WRITE(reg_base + UART_REG_THR, 0x71); //write to tx fifo
		break;
	case '3':
		uart_data_bit_set(reg_base, DATA_8_BIT);
		data = REG32_READ(reg_base + UART_REG_LCR);
		hprintf(DEBUG, "this is  8 data bits 0x%x\r\n", data);
		REG32_WRITE(reg_base + UART_REG_THR, 0x81); //write to tx fifo
		break;
	default:
		result = HVTE_MODULE_TEST_FAIL;
		break;
	}

	return result;
}

//test 8
int uart_fifo_dif_stop_length_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	unsigned int reg_base;
	char c = 0;
	reg_base = mod->reg_base;

	uart_init(reg_base);
	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	uart_fifo_enable(reg_base, HVTE_TRUE);
	uart_parity_enable(reg_base, HVTE_TRUE);
	uart_even_parity_set(reg_base, UART_ODD_PARITY);
	uart_stick_parity_enable(reg_base, HVTE_FALSE);

	hprintf(DEBUG, "0.....8  data word 1 stop\r\n");
	hprintf(DEBUG, "1.....8  data word 2 stop\r\n");
	hprintf(DEBUG, "2.....5  data word 1.5 stop\r\n");

	c = getc();
	putc(c);
	switch (c) {
	case '0':
		uart_data_bit_set(reg_base, DATA_8_BIT);
		uart_stop_bit_set(reg_base, STOP_1_BIT);
		hprintf(DEBUG, "this is  8 data bits 1 stop: 0x%x\r\n",
			REG32_READ(reg_base + UART_REG_LCR));
		REG32_WRITE(reg_base + UART_REG_THR, 0x81); //write to tx fifo
		break;
	case '1':
		uart_data_bit_set(reg_base, DATA_8_BIT);
		uart_stop_bit_set(reg_base, STOP_2_BIT);
		hprintf(DEBUG, "this is  8 data bits 2 stop  0x%x\r\n",
			REG32_READ(reg_base + UART_REG_LCR));
		REG32_WRITE(reg_base + UART_REG_THR, 0x82); //write to tx fifo
		break;
	case '2':
		uart_data_bit_set(reg_base, DATA_5_BIT);
		uart_stop_bit_set(reg_base, STOP_2_BIT);
		hprintf(DEBUG, "this is  5 data bits 1.5 stop: 0x%x\r\n",
			REG32_READ(reg_base + UART_REG_LCR));
		REG32_WRITE(reg_base + UART_REG_THR, 0x52); //write to tx fifo
		break;
	}

	return result;
}

//test 7
int uart_fifo_parity_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	unsigned int reg_base;
	char c = 0;
	reg_base = mod->reg_base;

	uart_init(reg_base);
	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	uart_fifo_enable(reg_base, HVTE_TRUE);
	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);

	uart_parity_enable(reg_base, HVTE_TRUE);
	uart_even_parity_set(reg_base, UART_ODD_PARITY);
	uart_stick_parity_enable(reg_base, HVTE_FALSE);

	hprintf(DEBUG, "0.....8  data word 1 stop odd parity\r\n");
	hprintf(DEBUG, "1.....8  data word 1 stop even parity\r\n");
	hprintf(DEBUG, "2.....8  data word 1 stop stick parity\r\n");
	hprintf(DEBUG, "3.....8  data word 1 stop no parity\r\n");

	c = getc();
	putc(c);
	switch (c) {
	case '0':
		hprintf(DEBUG, "this is odd parity\r\n");
		REG32_WRITE(reg_base + UART_REG_THR, 0x81); //write to tx fifo
		break;
	case '1':
		uart_even_parity_set(reg_base, UART_EVEN_PARITY);
		hprintf(DEBUG, "this is even parity\r\n");
		REG32_WRITE(reg_base + UART_REG_THR, 0xaa); //write to tx fifo
		break;
	case '2':
		uart_stick_parity_enable(reg_base, HVTE_TRUE);
		hprintf(DEBUG, "this is stick parity\r\n");
		REG32_WRITE(reg_base + UART_REG_THR, 0x55); //write to tx fifo
		break;
	case '3':
		uart_parity_enable(reg_base, HVTE_FALSE);
		hprintf(DEBUG, "this is no parity\r\n");
		REG32_WRITE(reg_base + UART_REG_THR, 0xee); //write to tx fifo
		break;
	}

	return result;
}

//test 13
int uart_afc_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	u32 i = 0;
	u8 fifo_data[DATA_LEN] = { 0 };
	u8 data[DATA_LEN];
	u32 final_result = HVTE_MODULE_TEST_PASS;
	unsigned int reg_base;
	reg_base = mod->reg_base;

	for (i = 0; i < DATA_LEN; i++) {
		data[i] = i;
	}

	uart_init(reg_base); // set clock divider radio
	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	uart_irq_set(reg_base, UART_IRQ_EDSSI, HVTE_TRUE); // enbale IER[3]
	uart_irq_set(reg_base, UART_IRQ_ERBFI,
		     HVTE_TRUE); // enbale received data availiable
	uart_fifo_enable(reg_base, HVTE_TRUE);


	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);
	uart_parity_enable(reg_base, HVTE_FALSE);
	//hvte_register_irq(UART_IDR_ID, uart_irq_handler_with_htx, NULL, "uart irq handler");
	memset(&uart_irq, 0x0, sizeof(uart_irq));
	/* rx fifo 1/4 full trigger */
	REG32_WRITE(reg_base + UART_REG_FCR, 0x69);

	uart_afc_set(reg_base, HVTE_TRUE);
	hprintf(DEBUG, "send 0xFF \r\n");
	REG32_WRITE(reg_base + UART_REG_THR, 0xFF); //write to tx fifo

#if 0
{	// debug for loopback
	u32 idx, tmp, jdx, flag = 0;
	REG32_WRITE(reg_base+UART_REG_MCR, REG32_READ(reg_base+UART_REG_MCR)|(0x1<<4));
	hprintf(DEBUG, "rx fifo[0] :%x\r\n", REG32_READ(reg_base+UART_REG_MCR));
	for(idx = 0; idx < 32; idx++) {
		REG32_WRITE(reg_base+UART_REG_THR, 0xa5);
		if((tmp=REG32_READ(reg_base+UART_REG_IIR) & 0xf) == 0x4) {
			hprintf(DEBUG, "IRQ happen @:%d\r\n", idx);
			flag = 1;
		} else {
			hprintf(DEBUG, "reg val :%x\r\n", tmp);
		}
		
		if(flag) {
			for(jdx = 0; jdx < 8; jdx++) {
				hprintf(DEBUG, "rx fifo[%d] :%x\r\n", jdx, REG32_READ(reg_base+UART_REG_RBR));
			}
			flag = 0;
		}
	}
}
#endif
	for (i = 0; i < DATA_LEN;) {
		if (i < 252) {
			while ((REG32_READ(reg_base + UART_REG_IIR) & 0xf) !=
			       0x4)
				;
		}

		while ((REG32_READ(reg_base + UART_REG_LSR) & RXDREADY)) {
			fifo_data[i] = REG32_READ(reg_base + UART_REG_RBR);
			HCOMP(data[i], fifo_data[i]);
			i++;
		}

		hprintf(DEBUG, "i:%d\r\n", i);
	}

	if (result == HVTE_MODULE_TEST_PASS) {
		hprintf(DEBUG, "OK\r\n");
	} else {
		final_result = HVTE_MODULE_TEST_FAIL;
	}
	return final_result;
}

int uart_tx_channel_select(zebu_test_mod_t *mod, int channel)
{
	unsigned int reg_base;
	int result = HVTE_MODULE_TEST_FAIL;
	u32 data, i;
	reg_base = mod->reg_base;

	uart_init(reg_base);
	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);
	uart_parity_enable(reg_base, HVTE_TRUE);
	uart_even_parity_set(reg_base, UART_ODD_PARITY);
	uart_stick_parity_enable(reg_base, HVTE_FALSE);

	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	uart_fifo_enable(reg_base, HVTE_TRUE);
	memset(&uart_irq, 0x0, sizeof(uart_irq));
	//loopback
	//REG32_WRITE(reg_base+UART_REG_MCR, REG32_READ(reg_base+UART_REG_MCR)|(0x1<<4));
	REG32_WRITE(reg_base + UART_REG_FCR, 0xb9);
	for (i = 0; i < 32; i++) {
		uart_buf[i] = i;
	}

#if 0
	for(i=0;i<32;i++){
		while(! (REG32_READ(reg_base+UART_REG_LSR) & (0x1<<6)));
		REG32_WRITE(reg_base+UART_REG_THR, i+1);
	}

#else
/* SDMA setting */
#if defined(CONFIG_A55)
	//reset gdma
	hprintf(DEBUG, "reset gdma\r\n");
	data = REG32_READ(TOP_CRM_BASE + GDMA_RESETREG);
	data = (data & (~(0x1 << 12))) | (0x1 << 12);
	REG32_WRITE(TOP_CRM_BASE + GDMA_RESETREG, data);
#endif

	/* DMAC_CFGREG */
	REG32_WRITE(DMAC_CFGREG, REG32_READ(DMAC_CFGREG) | 0x1); //enable dma
	/* DMA_ChEnReg:select available channel */
	data = REG32_READ(DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 1))) |
	       (0x0 << 1); //CH2_EN, this bit auto clear after transfer done
	if (channel == 1) {
		data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
		data = (data & (~(0x1 << 9))) | (0x0 << 9); //CH2_dEN_WE
	}
	if (channel == 2) {
		data = (data & (~(0x1 << 8))) | (0x0 << 8); //CH1_dEN_WE
		data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
	}
	REG32_WRITE(DMAC_CHENREG, data);
	/* CHX_CFG */
	data = REG32_READ(DMAC_CHX_CFG(channel));
	data = (data & (~(0x3 << 0))) |
	       (0x2
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x2
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0xF << 11))) |
	       (0xc
		<< 11); //DST_PER,assign a hardware handshaking interface to the destination of channelx
	REG32_WRITE(DMAC_CHX_CFG(channel), data);

	data = REG32_READ(DMAC_CHX_CFG(channel) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x1
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x1
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x0
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	data = (data & (~(0xF << 23))) |
	       (0x0 << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0x0 << 27); //DST_OSR_LMT,destination outstanding request limit
	REG32_WRITE(DMAC_CHX_CFG(channel) + 0x4, data);
	hprintf(DEBUG, "DMAC_CHX_CFG(%d)(0x%x):0x%x\r\n", channel,
		DMAC_CHX_CFG(channel) + 0x4,
		REG32_READ(DMAC_CHX_CFG(channel) + 0x4));
	/* CHX_SAR */
	REG32_WRITE(DMAC_CHX_SAR(channel), (size_t)uart_buf); //source addr
	REG32_WRITE(DMAC_CHX_SAR(channel) + 0x4, 0);

	/* CHX_DAR */
	REG32_WRITE(DMAC_CHX_DAR(channel),
		    reg_base + UART_REG_THR); //destination addr
	REG32_WRITE(DMAC_CHX_DAR(channel) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_DAR(1)(0x%x):0x%x\r\n", DMAC_CHX_DAR(1),
		REG32_READ(DMAC_CHX_DAR(1)));
	/* CHX_BLOCK_TS: block transfer size */
	REG32_WRITE(DMAC_CHX_BLOCK_TS(channel), 32 - 1);
	REG32_WRITE(DMAC_CHX_BLOCK_TS(channel) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_BLOCK_TS(1)(0x%x):0x%x\r\n",
		DMAC_CHX_BLOCK_TS(1), REG32_READ(DMAC_CHX_BLOCK_TS(1)));

	/* CHX_CTRL */
	data = REG32_READ(DMAC_CHX_CTL(channel));
	data = (data & (~(0x1 << 4))) |
	       (0x0
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x1
		<< 6); //destination address increment: 0(increment), 1(no change)
	//data = (data&(~(0x1<<6))) | (0x0<<6);
	data = (data & (~(0x7 << 8))) |
	       (0x2 << 8); //source transfer width: (32 bits)
	data = (data & (~(0x7 << 11))) |
	       (0x2 << 11); //destination transfer width: (32 bits)
	data = (data & (~(0xF << 14))) |
	       (0x3
		<< 14); //source burst transaction length: 0(burst length = 16)
	data = (data & (~(0xF << 18))) |
	       (0x3
		<< 18); //destination burst transaction length: 0(burst length = 16)
	REG32_WRITE(DMAC_CHX_CTL(channel), data);
	hprintf(DEBUG, "DMAC_CHX_CTL(1)(0x%x):0x%x\r\n", DMAC_CHX_CTL(1),
		REG32_READ(DMAC_CHX_CTL(1)));

	data = REG32_READ(DMAC_CHX_CTL(channel) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 6))) | (0x1 << 6); //source burst length enable
	data = (data & (~(0xFF << 7))) | (0x1 << 7); //source burst length
	data = (data & (~(0x1 << 15))) |
	       (0x1 << 15); //destination burst length enable
	data = (data & (~(0xFF << 16))) |
	       (0x0 << 16); //destination burst length
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	REG32_WRITE(DMAC_CHX_CTL(channel) + 0x4, data);
	hprintf(DEBUG, "DMAC_CHX_CTL(1)(0x%x):0x%x\r\n", DMAC_CHX_CTL(1) + 0x4,
		REG32_READ(DMAC_CHX_CTL(1)) + 0x4);

	/* enable DMA */
	data = REG32_READ(DMAC_CHENREG);
	if (channel == 1) {
		data = (data & (~(0x1 << 0))) |
		       (0x1
			<< 0); //CH1_EN, this bit auto clear after transfer done
		data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
		data = (data & (~(0x1 << 1))) |
		       (0x0
			<< 1); //CH2_EN, this bit auto clear after transfer done
		data = (data & (~(0x1 << 9))) | (0x0 << 9); //CH2_EN_WE
	}
	if (channel == 2) {
		data = (data & (~(0x1 << 0))) |
		       (0x0
			<< 0); //CH1_EN, this bit auto clear after transfer done
		data = (data & (~(0x1 << 8))) | (0x0 << 8); //CH1_EN_WE
		data = (data & (~(0x1 << 1))) |
		       (0x1
			<< 1); //CH2_EN, this bit auto clear after transfer done
		data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
	}
	REG32_WRITE(DMAC_CHENREG, data);

	if ((REG32_READ(DMAC_CHX_INTSTATUS(channel)) & 0x1) == 0x0) {
		hprintf(DEBUG, "DMA transfer is in progress.\r\n");
	}
	/* DMA CHX_INTSTATUS */
	while ((REG32_READ(DMAC_CHX_INTSTATUS(channel)) & 0x1) == 0x0)
		; //wait block transfer done interrupt
	result = HVTE_MODULE_TEST_PASS;
#endif
	return result;
}

//test 10 transmit singal select
int uart_tx_dma_channel_test(zebu_test_mod_t *mod, void *para)
{
	u32 final_result = HVTE_MODULE_TEST_PASS;
	char c = 0;
	hprintf(DEBUG, "1.....select channel 1\r\n");
	hprintf(DEBUG, "2.....select channel 2\r\n");
	c = getc();
	putc(c);
	switch (c) {
	case '1':
		final_result = uart_tx_channel_select(mod, 1);
		break;
	case '2':
		final_result = uart_tx_channel_select(mod, 2);
		break;
	}
	return final_result;
}
//test 10 transmit
int uart_tx_dma_test(zebu_test_mod_t *mod, void *para)
{
	unsigned int reg_base;
	u32 data, i;
	//  int result = HVTE_MODULE_TEST_PASS;
	reg_base = mod->reg_base;

	uart_init(reg_base);
	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);
	uart_parity_enable(reg_base, HVTE_TRUE);
	uart_even_parity_set(reg_base, UART_ODD_PARITY);
	uart_stick_parity_enable(reg_base, HVTE_FALSE);

	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	uart_fifo_enable(reg_base, HVTE_TRUE);
	memset(&uart_irq, 0x0, sizeof(uart_irq));
	//loopback
	//	REG32_WRITE(reg_base+UART_REG_MCR, REG32_READ(reg_base+UART_REG_MCR)|(0x1<<4));
	REG32_WRITE(reg_base + UART_REG_FCR, 0xb9);
	for (i = 0; i < 32; i++) {
		uart_buf[i] = i;
	}

/* SDMA setting */
#if defined(CONFIG_A55)
	//reset gdma
	hprintf(DEBUG, "reset gdma\r\n");
	data = REG32_READ(TOP_CRM_BASE + GDMA_RESETREG);
	data = (data & (~(0x1 << 12))) | (0x1 << 12);
	REG32_WRITE(TOP_CRM_BASE + GDMA_RESETREG, data);
#endif

	/* DMAC_CFGREG */
	REG32_WRITE(DMAC_CFGREG, REG32_READ(DMAC_CFGREG) | 0x1); //enable dma
	/* DMA_ChEnReg:select available channel */
	data = REG32_READ(DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 1))) |
	       (0x0 << 1); //CH2_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
	REG32_WRITE(DMAC_CHENREG, data);
	/* CHX_CFG */
	data = REG32_READ(DMAC_CHX_CFG(1));
	data = (data & (~(0x3 << 0))) |
	       (0x2
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x2
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0xF << 11))) |
	       (0xc
		<< 11); //DST_PER,assign a hardware handshaking interface to the destination of channelx
	REG32_WRITE(DMAC_CHX_CFG(1), data);

	data = REG32_READ(DMAC_CHX_CFG(1) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x1
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x1
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x0
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	data = (data & (~(0xF << 23))) |
	       (0x0 << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0x0 << 27); //DST_OSR_LMT,destination outstanding request limit
	REG32_WRITE(DMAC_CHX_CFG(1) + 0x4, data);
	hprintf(DEBUG, "DMAC_CHX_CFG(1)(0x%x):0x%x\r\n", DMAC_CHX_CFG(1) + 0x4,
		REG32_READ(DMAC_CHX_CFG(1) + 0x4));
	/* CHX_SAR */
	REG32_WRITE(DMAC_CHX_SAR(1), (size_t)uart_buf); //source addr
	REG32_WRITE(DMAC_CHX_SAR(1) + 0x4, 0);

	/* CHX_DAR */
	REG32_WRITE(DMAC_CHX_DAR(1),
		    reg_base + UART_REG_THR); //destination addr
	REG32_WRITE(DMAC_CHX_DAR(1) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_DAR(1)(0x%x):0x%x\r\n", DMAC_CHX_DAR(1),
		REG32_READ(DMAC_CHX_DAR(1)));
	/* CHX_BLOCK_TS: block transfer size */
	REG32_WRITE(DMAC_CHX_BLOCK_TS(1), 32 - 1);
	REG32_WRITE(DMAC_CHX_BLOCK_TS(1) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_BLOCK_TS(1)(0x%x):0x%x\r\n",
		DMAC_CHX_BLOCK_TS(1), REG32_READ(DMAC_CHX_BLOCK_TS(1)));

	/* CHX_CTRL */
	data = REG32_READ(DMAC_CHX_CTL(1));
	data = (data & (~(0x1 << 4))) |
	       (0x0
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x1
		<< 6); //destination address increment: 0(increment), 1(no change)
	//data = (data&(~(0x1<<6))) | (0x0<<6);
	data = (data & (~(0x7 << 8))) |
	       (0x2 << 8); //source transfer width: (32 bits)
	data = (data & (~(0x7 << 11))) |
	       (0x2 << 11); //destination transfer width: (32 bits)
	data = (data & (~(0xF << 14))) |
	       (0x3
		<< 14); //source burst transaction length: 0(burst length = 16)
	data = (data & (~(0xF << 18))) |
	       (0x3
		<< 18); //destination burst transaction length: 0(burst length = 16)
	REG32_WRITE(DMAC_CHX_CTL(1), data);
	hprintf(DEBUG, "DMAC_CHX_CTL(1)(0x%x):0x%x\r\n", DMAC_CHX_CTL(1),
		REG32_READ(DMAC_CHX_CTL(1)));

	data = REG32_READ(DMAC_CHX_CTL(1) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 6))) | (0x1 << 6); //source burst length enable
	data = (data & (~(0xFF << 7))) | (0x1 << 7); //source burst length
	data = (data & (~(0x1 << 15))) |
	       (0x1 << 15); //destination burst length enable
	data = (data & (~(0xFF << 16))) |
	       (0x0 << 16); //destination burst length
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	REG32_WRITE(DMAC_CHX_CTL(1) + 0x4, data);
	hprintf(DEBUG, "DMAC_CHX_CTL(1)(0x%x):0x%x\r\n", DMAC_CHX_CTL(1) + 0x4,
		REG32_READ(DMAC_CHX_CTL(1)) + 0x4);

	/* enable DMA */
	data = REG32_READ(DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x1 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 1))) |
	       (0x1 << 1); //CH2_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
	REG32_WRITE(DMAC_CHENREG, data);

	if ((REG32_READ(DMAC_CHX_INTSTATUS(1)) & 0x1) == 0x0) {
		hprintf(DEBUG, "DMA transfer is in progress.\r\n");
	}
	/* DMA CHX_INTSTATUS */
	while ((REG32_READ(DMAC_CHX_INTSTATUS(1)) & 0x1) == 0x0)
		; //wait block transfer done interrupt

	/*
	while(!(REG32_READ(reg_base+UART_REG_LSR) & RXDREADY));
	while((REG32_READ(reg_base+UART_REG_LSR) & RXDREADY)){
		hprintf(DEBUG, "rev data:%x\r\n",REG32_READ(reg_base+UART_REG_RBR));
	}
    */
	return 0;
}

//test 10 received
int uart_rx_dma_double_channel_test(zebu_test_mod_t *mod, void *para)
{
	unsigned int reg_base;
	u32 result = HVTE_MODULE_TEST_PASS;
	int final_result = HVTE_MODULE_TEST_PASS;
	u32 data, i;
	u8 fifo_data[32] = { 0 };

	reg_base = mod->reg_base;

	uart_init(reg_base);
	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);
	uart_parity_enable(reg_base, HVTE_TRUE);
	uart_even_parity_set(reg_base, UART_ODD_PARITY);
	uart_stick_parity_enable(reg_base, HVTE_FALSE);

	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	REG32_WRITE(reg_base + UART_REG_FCR, 0x1); //FCR[7:6] = 0

	memset(&uart_irq, 0x0, sizeof(uart_irq));
	hprintf(DEBUG, "LCR :%x\r\n", REG32_READ(reg_base + UART_REG_LCR));

#if 0

	hprintf(DEBUG, "send 0xFF \r\n");
	REG32_WRITE(reg_base+UART_REG_THR, 0xFF); //write to tx fifo

    while((REG32_READ(reg_base+UART_REG_IIR) & 0xf) != 0x4);
    i = 0;
    while((REG32_READ(reg_base+UART_REG_LSR) & RXDREADY)) {
		fifo_data[i] = REG32_READ(reg_base+UART_REG_RBR);
        hprintf(DEBUG, "data[%d] :%x\r\n", i, fifo_data[i]);
        i++;
	}

#else
	for (i = 0; i < 32; i++) {
		fifo_data[i] = i;
	}
	hprintf(DEBUG, "send 0xFF \r\n");
	REG32_WRITE(reg_base + UART_REG_THR, 0xFF); //write to tx fifo

/* SDMA setting */
#if defined(CONFIG_A55)
	//reset gdma
	hprintf(DEBUG, "reset gdma\r\n");
	data = REG32_READ(TOP_CRM_BASE + GDMA_RESETREG);
	data = (data & (~(0x1 << 12))) | (0x1 << 12);
	REG32_WRITE(TOP_CRM_BASE + GDMA_RESETREG, data);
#endif
	/* DMAC_CFGREG */
	REG32_WRITE(DMAC_CFGREG, REG32_READ(DMAC_CFGREG) | 0x1); //enable dma
	/* DMA_ChEnReg:select available channel */
	data = REG32_READ(DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 1))) |
	       (0x0 << 1); //CH2_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
	REG32_WRITE(DMAC_CHENREG, data);
	/* CHX_CFG */
	data = REG32_READ(DMAC_CHX_CFG(1));
	data = (data & (~(0x3 << 0))) |
	       (0x2
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x2
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0xF << 4))) | (0xd << 4); //SRC_PER
	REG32_WRITE(DMAC_CHX_CFG(1), data);

	data = REG32_READ(DMAC_CHX_CFG(1) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x2
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x0
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	data = (data & (~(0xF << 23))) |
	       (0x0 << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0x0 << 27); //DST_OSR_LMT,destination outstanding request limit
	REG32_WRITE(DMAC_CHX_CFG(1) + 0x4, data);

	/* CHX_SAR */
	REG32_WRITE(DMAC_CHX_SAR(1), reg_base + UART_REG_RBR); //source addr
	REG32_WRITE(DMAC_CHX_SAR(1) + 0x4, 0);

	/* CHX_DAR */
	REG32_WRITE(DMAC_CHX_DAR(1), (size_t)uart_buf); //destination addr
	REG32_WRITE(DMAC_CHX_DAR(1) + 0x4, 0);

	/* CHX_BLOCK_TS: block transfer size */
	REG32_WRITE(DMAC_CHX_BLOCK_TS(1), 32 - 1);
	REG32_WRITE(DMAC_CHX_BLOCK_TS(1) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_BLOCK_TS(1)(0x%x):0x%x\r\n",
		DMAC_CHX_BLOCK_TS(1), REG32_READ(DMAC_CHX_BLOCK_TS(1)));

	/* CHX_CTRL */
	data = REG32_READ(DMAC_CHX_CTL(1));
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x0
		<< 6); //destination address increment: 0(increment), 1(no change)
	//data = (data&(~(0x1<<6))) | (0x0<<6);
	data = (data & (~(0x7 << 8))) |
	       (0x2 << 8); //source transfer width: 0(8 bits)
	data = (data & (~(0x7 << 11))) |
	       (0x2 << 11); //destination transfer width: 2(32 bits)
	data = (data & (~(0xF << 14))) |
	       (0x0
		<< 14); //source burst transaction length: 0(burst length = 1)
	data = (data & (~(0xF << 18))) |
	       (0x0
		<< 18); //destination burst transaction length: 0(burst length = 1)
	REG32_WRITE(DMAC_CHX_CTL(1), data);
	hprintf(DEBUG, "DMAC_CHX_CTL(1)(0x%x):0x%x\r\n", DMAC_CHX_CTL(1),
		REG32_READ(DMAC_CHX_CTL(1)));

	data = REG32_READ(DMAC_CHX_CTL(1) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 6))) | (0x1 << 6); //source burst length enable
	data = (data & (~(0xFF << 7))) | (0x1 << 7); //source burst length
	data = (data & (~(0x1 << 15))) |
	       (0x1 << 15); //destination burst length enable
	data = (data & (~(0xFF << 16))) |
	       (0x0 << 16); //destination burst length
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	REG32_WRITE(DMAC_CHX_CTL(1) + 0x4, data);
	hprintf(DEBUG, "DMAC_CHX_CTL(1)(0x%x):0x%x\r\n", DMAC_CHX_CTL(1) + 0x4,
		REG32_READ(DMAC_CHX_CTL(1)) + 0x4);

	/* enable DMA */
	data = REG32_READ(DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x1 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 1))) |
	       (0x1 << 1); //CH2_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
	data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
	REG32_WRITE(DMAC_CHENREG, data);

	if ((REG32_READ(DMAC_CHX_INTSTATUS(1)) & 0x1) == 0x0) {
		hprintf(DEBUG, "DMA transfer is in progress.\r\n");
	}

	/* DMA CHX_INTSTATUS */
	while ((REG32_READ(DMAC_CHX_INTSTATUS(1)) & 0x1) == 0x0)
		; //wait block transfer done interrupt

	for (i = 0; i < 32; i++) {
		hprintf(DEBUG, "uart_buf[%d]:%d\r\n", i, uart_buf[i]);
		HCOMP(fifo_data[i], uart_buf[i]);
	}
	if (result == HVTE_MODULE_TEST_PASS) {
		hprintf(DEBUG, "OK\r\n");
	} else {
		final_result = HVTE_MODULE_TEST_FAIL;
	}
#endif

	return final_result;
}

// channel 2
int uart_rx_channel_select(zebu_test_mod_t *mod, int channel)
{
	unsigned int reg_base;
	int result = HVTE_MODULE_TEST_PASS;
	int final_result = HVTE_MODULE_TEST_PASS;
	u32 data, i;
	u8 fifo_data[32] = { 0 };

	reg_base = mod->reg_base;

	uart_init(reg_base);
	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);
	uart_parity_enable(reg_base, HVTE_TRUE);
	uart_even_parity_set(reg_base, UART_ODD_PARITY);
	uart_stick_parity_enable(reg_base, HVTE_FALSE);

	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	REG32_WRITE(reg_base + UART_REG_FCR, 0x1);

	memset(&uart_irq, 0x0, sizeof(uart_irq));
	hprintf(DEBUG, "LCR :%x\r\n", REG32_READ(reg_base + UART_REG_LCR));
#if 0
	//loopback
	REG32_WRITE(reg_base+UART_REG_MCR, REG32_READ(reg_base+UART_REG_MCR)|(0x1<<4));
 	REG32_WRITE(reg_base+UART_REG_FCR, 0xb9);


	for(i = 0; i < 32; i++) {
		while(! (REG32_READ(reg_base+UART_REG_LSR) & (0x1<<6)));
		REG32_WRITE(reg_base+UART_REG_THR, i+1);
	}
#endif
	for (i = 0; i < 32; i++) {
		fifo_data[i] = i;
	}
	hprintf(DEBUG, "send 0xFF \r\n");
	REG32_WRITE(reg_base + UART_REG_THR, 0xFF); //write to tx fifo

/* SDMA setting */
#if defined(CONFIG_A55)
	//reset gdma
	hprintf(DEBUG, "reset gdma\r\n");
	data = REG32_READ(TOP_CRM_BASE + GDMA_RESETREG);
	data = (data & (~(0x1 << 12))) | (0x1 << 12);
	REG32_WRITE(TOP_CRM_BASE + GDMA_RESETREG, data);
#endif

	/* DMAC_CFGREG */
	REG32_WRITE(DMAC_CFGREG, REG32_READ(DMAC_CFGREG) | 0x1); //enable dma
	/* DMA_ChEnReg:select available channel */
	data = REG32_READ(DMAC_CHENREG);
	data = (data & (~(0x1 << 0))) |
	       (0x0 << 0); //CH1_EN, this bit auto clear after transfer done
	data = (data & (~(0x1 << 1))) |
	       (0x0 << 1); //CH2_EN, this bit auto clear after transfer done
	if (channel == 1) {
		data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
		data = (data & (~(0x1 << 9))) | (0x0 << 9); //CH2_dEN_WE
	}
	if (channel == 2) {
		data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
		data = (data & (~(0x1 << 8))) | (0x0 << 8); //CH1_dEN_WE
	}
	REG32_WRITE(DMAC_CHENREG, data);
	/* CHX_CFG */
	data = REG32_READ(DMAC_CHX_CFG(channel));
	data = (data & (~(0x3 << 0))) |
	       (0x2
		<< 0); //SRC_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0x3 << 2))) |
	       (0x2
		<< 2); //DST_MULTBLK_TYPE,00:continus;01:reload;10:shadow register;11:link list
	data = (data & (~(0xF << 4))) | (0xd << 4); //SRC_PER
	REG32_WRITE(DMAC_CHX_CFG(channel), data);

	data = REG32_READ(DMAC_CHX_CFG(channel) + 0x4); //high 32 bit
	data = (data & (~(0x7 << 0))) |
	       (0x2
		<< 0); //TT_FC,00:memory to memory and flow control is DMAC;01:memory to peripher and flow control is DMAC
	data = (data & (~(0x1 << 3))) |
	       (0x0
		<< 3); //HS_SEL_SRC,0:hardware handshaking,1:software handshaking
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //HS_SEL_DST,0:hardware handshaking,1:software handshaking
	data = (data & (~(0xF << 23))) |
	       (0x0 << 23); //SRC_OSR_LMT,source outstanding request limit
	data = (data & (~(0xF << 27))) |
	       (0x0 << 27); //DST_OSR_LMT,destination outstanding request limit
	REG32_WRITE(DMAC_CHX_CFG(channel) + 0x4, data);

	/* CHX_SAR */
	REG32_WRITE(DMAC_CHX_SAR(channel),
		    reg_base + UART_REG_RBR); //source addr
	REG32_WRITE(DMAC_CHX_SAR(channel) + 0x4, 0);

	/* CHX_DAR */
	REG32_WRITE(DMAC_CHX_DAR(channel), (size_t)uart_buf); //destination addr
	REG32_WRITE(DMAC_CHX_DAR(channel) + 0x4, 0);

	/* CHX_BLOCK_TS: block transfer size */
	REG32_WRITE(DMAC_CHX_BLOCK_TS(channel), 32 - 1);
	REG32_WRITE(DMAC_CHX_BLOCK_TS(channel) + 0x4, 0);
	hprintf(DEBUG, "DMAC_CHX_BLOCK_TS(channel)(0x%x):0x%x\r\n",
		DMAC_CHX_BLOCK_TS(channel),
		REG32_READ(DMAC_CHX_BLOCK_TS(channel)));

	/* CHX_CTRL */
	data = REG32_READ(DMAC_CHX_CTL(channel));
	data = (data & (~(0x1 << 4))) |
	       (0x1
		<< 4); //source address increment: 0(increment), 1(no change)
	data = (data & (~(0x1 << 6))) |
	       (0x0
		<< 6); //destination address increment: 0(increment), 1(no change)
	//data = (data&(~(0x1<<6))) | (0x0<<6);
	data = (data & (~(0x7 << 8))) |
	       (0x2 << 8); //source transfer width: 0(8 bits)
	data = (data & (~(0x7 << 11))) |
	       (0x2 << 11); //destination transfer width: 2(32 bits)
	data = (data & (~(0xF << 14))) |
	       (0x0
		<< 14); //source burst transaction length: 0(burst length = 1)
	data = (data & (~(0xF << 18))) |
	       (0x0
		<< 18); //destination burst transaction length: 0(burst length = 1)
	REG32_WRITE(DMAC_CHX_CTL(channel), data);
	hprintf(DEBUG, "DMAC_CHX_CTL(%d)(0x%x):0x%x\r\n", channel,
		DMAC_CHX_CTL(channel), REG32_READ(DMAC_CHX_CTL(channel)));

	data = REG32_READ(DMAC_CHX_CTL(channel) + 0x4); //high 32 bit
	data = (data & (~(0x1 << 6))) | (0x1 << 6); //source burst length enable
	data = (data & (~(0xFF << 7))) | (0x1 << 7); //source burst length
	data = (data & (~(0x1 << 15))) |
	       (0x1 << 15); //destination burst length enable
	data = (data & (~(0xFF << 16))) |
	       (0x0 << 16); //destination burst length
	data = (data & (~(0x1 << 26))) |
	       (0x1 << 26); //Interrput on completion of block transfer
	data = (data & (~(0x1 << 30))) |
	       (0x1 << 30); //Last shadow register/linked list item
	data = (data & (~(0x1 << 31))) |
	       (0x1 << 31); //shadow register comntent/link list item valid
	REG32_WRITE(DMAC_CHX_CTL(channel) + 0x4, data);
	hprintf(DEBUG, "DMAC_CHX_CTL(%d)(0x%x):0x%x\r\n", channel,
		DMAC_CHX_CTL(channel) + 0x4,
		REG32_READ(DMAC_CHX_CTL(channel)) + 0x4);

	/* enable DMA */
	data = REG32_READ(DMAC_CHENREG);
	if (channel == 1) {
		data = (data & (~(0x1 << 0))) |
		       (0x1
			<< 0); //CH1_EN, this bit auto clear after transfer done
		data = (data & (~(0x1 << 8))) | (0x1 << 8); //CH1_EN_WE
		data = (data & (~(0x1 << 1))) |
		       (0x0
			<< 1); //CH2_dEN, this bit auto clear after transfer done
		data = (data & (~(0x1 << 9))) | (0x0 << 9); //CH2_dEN_WE
	}
	if (channel == 2) {
		data = (data & (~(0x1 << 0))) |
		       (0x0
			<< 0); //CH1_dEN, this bit auto clear after transfer done
		data = (data & (~(0x1 << 8))) | (0x0 << 8); //CH1_dEN_WE
		data = (data & (~(0x1 << 1))) |
		       (0x1
			<< 1); //CH2_EN, this bit auto clear after transfer done
		data = (data & (~(0x1 << 9))) | (0x1 << 9); //CH2_EN_WE
	}
	REG32_WRITE(DMAC_CHENREG, data);

	if ((REG32_READ(DMAC_CHX_INTSTATUS(channel)) & 0x1) == 0x0) {
		hprintf(DEBUG, "DMA transfer is in progress.\r\n");
	}

	/* DMA CHX_INTSTATUS */
	while ((REG32_READ(DMAC_CHX_INTSTATUS(channel)) & 0x1) == 0x0)
		; //wait block transfer done interrupt

	for (i = 0; i < 32; i++) {
		hprintf(DEBUG, "uart_buf[%d]:%d\r\n", i, uart_buf[i]);
		HCOMP(fifo_data[i], uart_buf[i]);
	}
	if (result == HVTE_MODULE_TEST_PASS) {
		hprintf(DEBUG, "OK\r\n");
	} else {
		final_result = HVTE_MODULE_TEST_FAIL;
	}
	return final_result;
}
// test10 select 1 or 2 channel
int uart_rx_dma_test(zebu_test_mod_t *mod, void *para)
{
	int final_result = HVTE_MODULE_TEST_PASS;
	char c = 0;
	hprintf(DEBUG, "1.....select channel 1\r\n");
	hprintf(DEBUG, "2.....select channel 2\r\n");
	c = getc();
	putc(c);
	switch (c) {
	case '1':
		final_result = uart_rx_channel_select(mod, 1);
		break;
	case '2':
		final_result = uart_rx_channel_select(mod, 2);
		break;
	}
	return final_result;
}

volatile u32 irq_flag = 0xFF;

void uart_interrupt_handler(int reg_base)
{
	irq_flag = 2;

#if defined(CONFIG_R5_SAFETY)
	if (reg_base == UART0_BASE_ADDR) {
		SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_UART0);
	} else if (reg_base == UART1_BASE_ADDR) {
		SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP0_UART1);
	} else if (reg_base == UART2_BASE_ADDR) {
		SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP1_UART0);
	} else if (reg_base == UART3_BASE_ADDR) {
		SAFETY_IRQ_DISABLE(FIQ_IRQ_LSP1_UART1);
	}
#elif defined(CONFIG_A55)
	if (reg_base == UART0_BASE_ADDR) {
		A55_IRQ_DISABLE(FIQ_IRQ_LSP0_UART0);
	} else if (reg_base == UART1_BASE_ADDR) {
		A55_IRQ_DISABLE(FIQ_IRQ_LSP0_UART1);
	} else if (reg_base == UART2_BASE_ADDR) {
		A55_IRQ_DISABLE(FIQ_IRQ_LSP1_UART0);
	} else if (reg_base == UART3_BASE_ADDR) {
		A55_IRQ_DISABLE(FIQ_IRQ_LSP1_UART1);
	}

#endif
	return;
}

int uart2_irq_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	unsigned int reg_base;
	u8 fifo_data[32] = { 0 };
	u8 data[32];
	u32 i = 0;
	u32 arr_len = sizeof(fifo_data) / sizeof(fifo_data[0]);
	reg_base = mod->reg_base;
	irq_flag = 0xFF;

	for (i = 0; i < arr_len; i++) {
		data[i] = i;
	}

	if (reg_base == UART2_BASE_ADDR) {
		SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP1_UART0);
		A55_IRQ_ENABLE(FIQ_IRQ_LSP1_UART0);
	}

	uart_init(reg_base); // set clock divider radio
	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	uart_irq_set(reg_base, UART_IRQ_EDSSI, HVTE_TRUE); // enbale IER[3]
	uart_irq_set(reg_base, UART_IRQ_ERBFI,
		     HVTE_TRUE); // enbale received data availiable
	uart_fifo_enable(reg_base, HVTE_TRUE);
	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);
	uart_parity_enable(reg_base, HVTE_FALSE);
	memset(&uart_irq, 0x0, sizeof(uart_irq));
	/* rx fifo 1/4 full trigger */
	REG32_WRITE(reg_base + UART_REG_FCR, 0x69);

	uart_afc_set(reg_base, HVTE_TRUE);
	REG32_WRITE(reg_base + UART_REG_THR, 0xFF); //write to tx fifo

	while (1) {
		if (irq_flag == 2)
			break;
	}

	i = 0;
	for (i = 0; i < arr_len;) {
		while ((REG32_READ(reg_base + UART_REG_LSR) & RXDREADY)) {
			fifo_data[i] = REG32_READ(reg_base + UART_REG_RBR);
			hprintf(DEBUG, "rev data : 0x%x\r\n", fifo_data[i]);
			HCOMP(data[i], fifo_data[i]);
			i++;
		}
	}
	if (result == HVTE_MODULE_TEST_PASS) {
		hprintf(DEBUG, "UART2 interrupt is ok\r\n");
	} else {
		result = HVTE_MODULE_TEST_FAIL;
	}

	uart_irq_set(reg_base, UART_IRQ_EDSSI, HVTE_FALSE); // enbale IER[3]
	uart_irq_set(reg_base, UART_IRQ_ERBFI,
		     HVTE_FALSE); // enbale received data availiable

	return result;
}

u32 uart_irq_tx_test(unsigned int reg_base, void *para)
{
	u32 result = HVTE_MODULE_TEST_FAIL;
	irq_flag = 0xFF;

#if defined(CONFIG_R5_SAFETY)
	if (reg_base == UART0_BASE_ADDR) {
		SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_UART0);
	} else if (reg_base == UART1_BASE_ADDR) {
		SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP0_UART1);
	} else if (reg_base == UART2_BASE_ADDR) {
		SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP1_UART0);
	} else if (reg_base == UART3_BASE_ADDR) {
		SAFETY_IRQ_ENABLE(FIQ_IRQ_LSP1_UART1);
	}
#elif defined(CONFIG_CONFIG_A55)
	if (reg_base == UART0_BASE_ADDR) {
		A55_IRQ_ENABLE(FIQ_IRQ_LSP0_UART0);
	} else if (reg_base == UART1_BASE_ADDR) {
		A55_IRQ_ENABLE(FIQ_IRQ_LSP0_UART1);
	} else if (reg_base == UART2_BASE_ADDR) {
		A55_IRQ_ENABLE(FIQ_IRQ_LSP1_UART0);
	} else if (reg_base == UART3_BASE_ADDR) {
		A55_IRQ_ENABLE(FIQ_IRQ_LSP1_UART1);
	}

#endif
	uart_init(reg_base); // set clock divider radio
	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);
	uart_irq_set(reg_base, UART_IRQ_EDSSI, HVTE_TRUE); // enbale IER[3]
	uart_irq_set(
		reg_base, UART_IRQ_ETBEI,
		HVTE_TRUE); // enable the generation of Transmitter Holding Register Empty Interrupt.
	uart_fifo_enable(reg_base, HVTE_TRUE);


	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);
	uart_parity_enable(reg_base, HVTE_FALSE);

	//	REG32_WRITE(reg_base+UART_REG_THR, 0x41); //write to tx fifo 'A'

	while (1) {
		if (irq_flag == 2) {
			result = HVTE_MODULE_TEST_PASS;
			break;
		}
	}

	uart_irq_set(reg_base, UART_IRQ_EDSSI, HVTE_FALSE); // enbale IER[3]
	uart_irq_set(
		reg_base, UART_IRQ_ETBEI,
		HVTE_FALSE); // disable the generation of Transmitter Holding Register Empty Interrupt.

	return result;
}

int uart013_irq_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;

	result = uart_irq_tx_test(UART0_BASE_ADDR, 0);
	HCOMP(HVTE_MODULE_TEST_PASS, result);
	if (result == HVTE_MODULE_TEST_PASS)
		hprintf(DEBUG, "UART0 interrupt is ok\r\n");
	else
		hprintf(DEBUG, "UART0 interrupt is fail\r\n");

	result = uart_irq_tx_test(UART1_BASE_ADDR, 0);
	HCOMP(HVTE_MODULE_TEST_PASS, result);
	if (result == HVTE_MODULE_TEST_PASS)
		hprintf(DEBUG, "UART1 interrupt is ok\r\n");
	else
		hprintf(DEBUG, "UART1 interrupt is fail\r\n");

	result = uart_irq_tx_test(UART3_BASE_ADDR, 0);
	HCOMP(HVTE_MODULE_TEST_PASS, result);
	if (result == HVTE_MODULE_TEST_PASS)
		hprintf(DEBUG, "UART3 interrupt is ok\r\n");
	else
		hprintf(DEBUG, "UART3 interrupt is fail\r\n");

	return result;
}

int uart_break_test(zebu_test_mod_t *mod, void *para)
{
	u32 result = HVTE_MODULE_TEST_PASS;
	unsigned int reg_base = mod->reg_base;

	uart_init(reg_base);
	uart_data_bit_set(reg_base, DATA_8_BIT);
	uart_stop_bit_set(reg_base, STOP_1_BIT);
	uart_parity_enable(reg_base, HVTE_TRUE);
	uart_even_parity_set(reg_base, UART_ODD_PARITY);
	uart_stick_parity_enable(reg_base, HVTE_FALSE);
	uart_fifo_enable(reg_base, HVTE_TRUE);

	uart_irq_set(reg_base, UART_IRQ_ALL, HVTE_FALSE);

	REG32_WRITE(reg_base + UART_REG_THR, 0x55); //write to tx fifo

	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));

	//enable break control
	uart_break_control_enable(reg_base, HVTE_TRUE);

	//delay
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));
	hprintf(DEBUG, "UART_REG_LSR : 0x%08x\r\n",
		REG32_READ(reg_base + UART_REG_LSR));

	//disable break control
	uart_break_control_enable(reg_base, HVTE_FALSE);

	REG32_WRITE(reg_base + UART_REG_THR, 0x37); //write to tx fifo

	return result;
}

/*==================================UART ZEBU TEST=====================================================*/

zebu_test_mod_t zebu_test_uart_mods[] = {
	//	{"UART0" , UART0_BASE_ADDR},
		{"UART1" , UART1_BASE_ADDR},
	//	{"UART2", UART2_BASE_ADDR },
	//	{"UART3" , UART3_BASE_ADDR},
};

zebu_test_entry_t zebu_test_uart_entrys[] = {
	//must
	{ '0', 0, "fifo mode different data lenght test",
	  uart_fifo_dif_data_length_test }, //test6
	{ '1', 0, "fifo mode different parity bit test",
	  uart_fifo_parity_test }, //tset7
	{ '2', 0, "fifo mode different stop bit test",
	  uart_fifo_dif_stop_length_test }, //test8
	{ '3', 0, "AFC mode fifi 1/4 full ", uart_afc_test }, //test13
	{ '4', 0, "transmit by DMA enable 2 channel",
	  uart_tx_dma_test }, //test10 transmit
	{ '5', 0, "transmit by DMA enable singal",
	  uart_tx_dma_channel_test }, //test10 transmit select channel
	{ '6', 0, "receiced by DMA enable singal",
	  uart_rx_dma_test }, //test10 received select channel
	{ '7', 0, "receiced by DMA enable 2 channel",
	  uart_rx_dma_double_channel_test }, //test10 received 2 channel
	{ '8', 0, "uart2 interrupt connect line ",
	  uart2_irq_test }, //test interrupt connect line is right
	{ '9', 0, "uart013 interrupt connect line ",
	  uart013_irq_test }, //test interrupt
	{ 'b', 0, "uart break down test ", uart_break_test },
	// not must
	//{'9' , 1, "read default register", uart_register_default_val_test},//test2
	//{'b' , 1, "Reg W/R test" , uart_register_rw_test},//test3
};

static zebu_test_ctrl_t zebu_mod_test_ctrl = { "uart",
					       SIZE_ARRAY(zebu_test_uart_mods),
					       zebu_test_uart_mods,
					       SIZE_ARRAY(
						       zebu_test_uart_entrys),
					       zebu_test_uart_entrys,
					       0 };

/*
 * uart test entry function, main() will call uartMain base user input CMD
 * para:
 *    0->enter uart test while loop, rx test cmd and run related test case then go back to rx test cmd
 *    1->run all the uart test case and return
 */
void uartMain(int para)
{
	int ret;
	zebu_mod_test_ctrl.current_mod = 0;
	if (para) {
		ret = zebu_mod_test_all_mod(&zebu_mod_test_ctrl);
		hprintf(TSENV, "！！auto test err %d!!!!\r\n", ret);
	} else {
		hprintf(TSENV, "！enter uart test! uart addr : 0x%08x\r\n",
			UART2_BASE_ADDR);
		//TestLoop();
		TestLoop(&zebu_mod_test_ctrl);
	}
}

CMD_DEFINE(uart, uartMain, 0, "uart test", 0);

#if defined(CONFIG_R5_SAFETY) || defined(CONFIG_A55)
IRQ_DEFINE(FIQ_IRQ_LSP0_UART0, uart_interrupt_handler, UART0_BASE_ADDR,
	   "irq timer isr uart0",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL); // uart0
IRQ_DEFINE(FIQ_IRQ_LSP0_UART1, uart_interrupt_handler, UART1_BASE_ADDR,
	   "irq timer isr uart1",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL); // uart1
IRQ_DEFINE(FIQ_IRQ_LSP1_UART0, uart_interrupt_handler, UART2_BASE_ADDR,
	   "irq timer isr uart2",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL); // uart2
IRQ_DEFINE(FIQ_IRQ_LSP1_UART1, uart_interrupt_handler, UART3_BASE_ADDR,
	   "irq timer isr uart3",
	   ISR_ATTR_SAFETY | ISR_ATTR_A55 | ISR_ATTR_LEVEL); // uart3
#endif
