/*
 * PortableLight.c
 *
 * Created: 10/12/2025 12:52:15 AM
 * Author : marec
 */ 

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "MapMath/MapMath.h"


int main(void)
{
	//setup system clock (20MHz)
	ccp_write_io((void *) & (CLKCTRL.MCLKCTRLA), 0x0);
	ccp_write_io((void *) & (CLKCTRL.MCLKCTRLB), 0x0);
	ccp_write_io((void *) & (CLKCTRL.OSC20MCTRLA), 0b10);
	while(CLKCTRL.MCLKSTATUS & 1);
	
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm | TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.PER = 0x3FF;
	TCA0.SINGLE.CMP0 = 0x3FF;
	
    PORTA.DIRSET = 1 << 2;
	
	sei();
	
    while (1) 
    {
		_delay_ms(1000);
    }
}

ISR(TCA0_CMP0_vect)
{
	PORTA.OUTCLR = 1 << 2;
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
}

ISR(TCA0_OVF_vect)
{
	PORTA.OUTSET = 1 << 2;
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}
