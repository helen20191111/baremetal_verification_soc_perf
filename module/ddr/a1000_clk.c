// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2016 Broadcom Ltd.
 */
#include <linux/kernel.h>
#include "a1000_evb.h"
/*#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/armv8/mmu.h>
*/

//inner use
u32 pins_tbl[] = {	0,0x1,0x3,0x7,0xf,
													0x1f,0x3f,0x7f,0xff,
													0x1ff,0x3ff,0x7ff,0xfff,
													0x1fff,0x3fff,0x7fff,0xffff,
													0x1ffff,0x3ffff,0x7ffff,0xfffff,
													0x1fffff,0x3fffff,0x7fffff,0xffffff,
													0x1ffffff,0x3ffffff,0x7ffffff,0xfffffff,
													0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};
void iomux_set(void *base, int pinstart, int pincnt, int pval)
{
	u32 regval, tmp;
	//u32 * ptrreg = (u32 *)A1000BASE_PINMUX;
	
	regval = readl(base);
	tmp = (pins_tbl[pincnt])<<pinstart;
	regval = (pval==0)?(regval&(~tmp)):(regval|tmp);
	writel(regval, base);
}

void iomux_set_byidx(int idx, int pinstart, int pincnt, int pval)
{
	if((idx >= 0) && (idx <= 21))
		return iomux_set((void *)A1000BASE_PINMUX+(idx<<2), pinstart, pincnt, pval);
}

//------------------------------------------------------------------------------
void delay_1k(unsigned int uicnt)
{
	int i, j;

	for (i = 0; i < uicnt; i++)
		for (j = 0; j < 1000; j++);
}

void switch_en(void *base, int shift, int val)
{
	val = (val)?1:0;
	switch_clk(base, shift, 1, val);
}

void reset_en(void *base, int shift, int val)
{
	val = (val)?1:0;
	switch_clk(base, shift, 1, val);
}

int set_pllrate(unsigned long refspeed, unsigned long outspeed, void *base, int dochk)
{
	u32 pllreg0;
	
	
	//disable PLL
	pllreg0 = readl(base);
/*	pllreg0 &= ~(1<<PLL_PLLEN);//PLLEN = disable
	REG32_WRITE(base, pllreg0);
	printf("---%s %d\n", __func__, __LINE__);
	
	pll_val = calc_pll_val((u64)outspeed, (u64)refspeed);
	fbdiv = (u32) (pll_val >> PLL_FRAC_SHIFT);
	frac = (u32) (pll_val & ((1<<PLL_FRAC_SHIFT) - 1));
	
	pllreg0 &= ~(PLL_FBDIV_MASK<<PLL_FBDIV); //clear fbdiv
	pllreg0 |= (fbdiv & PLL_FBDIV_MASK)<<PLL_FBDIV; //re-write fbdiv
	if(frac) {
		pllreg0 |= (1<<PLL_DSMEN); //enable DSMEN
	}else {
		pllreg0 &= ~(1<<PLL_DSMEN); //disable DSMEN
	}
	REG32_WRITE(base, pllreg0);
	
	if(frac) {
		pllreg1 = REG32_READ(base+4);
		//set PLL freqency parameter 
		pllreg1 &= ~(PLL_FRAC_MASK<<PLL_FRAC); //clear frac
		pllreg1 |= (frac & PLL_FRAC_MASK)<<PLL_FRAC; //re-write frac
		REG32_WRITE(base+4, pllreg1);
	}
*/
	printf("---%s %d\n", __func__, __LINE__);
	//enable PLL
	pllreg0 |= (1<<PLL_PLLEN);
	writel(pllreg0, base);
	
	/* wait pll lock */
	if(dochk) {
		do {
			if(readl(base+0x8) & (1<<PLL_LOCK))
				break;
			delay_1k(1);
		}while(1);
	}
	else {
		delay_1k(10);
	}
	printf("---%s %d\n", __func__, __LINE__);
	
	return 0;
}

void init_sys_pll(void)
{	
	//sysbus PLL
	set_pllrate(FREQ_CRYSTAL, FOUT_PLL_SYSBUS, (void *)A1000BASE_PLL_SYSBUS, 1);
//	switch_en(TOPCRM_REG_R_PLL_CLKMUX_SEL, 25, 0x1); //CLK2400_SYSBUS
	switch_en((void *)TOPCRM_REG_R_PLL_CLKMUX_SEL, 26, 0x1); //CLK800_SYSBUS
}

