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

#define LED_PWM(x) TCA0.SINGLE.CMP0 = x

uint16_t min(uint16_t a, uint16_t b);
void SetPWM(uint16_t val);

uint16_t pot_val = 0;

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
	LED_PWM(2);
	
	VREF.CTRLA = VREF_ADC0REFSEL_0V55_gc;
	VREF.CTRLB = VREF_ADC0REFSEL_0_bm;
	
	ADC0.CTRLA = ADC_RUNSTBY_bm | ADC_ENABLE_bm;
	ADC0.CTRLC = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc;
	ADC0.CTRLD = ADC_INITDLY_DLY16_gc;
	ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;
	
    PORTA.DIRSET = 1 << 2;
	
	sei();
	
    while (1) 
    {
		if (~ADC0.COMMAND & ADC_STCONV_bm)
		{
			pot_val = ADC0.RES;
			ADC0.COMMAND = ADC_STCONV_bm;
		}
		
		SetPWM(mapClamp(pot_val, 5, 1023, 0, 600));		
    }
}

uint16_t min(uint16_t a, uint16_t b)
{
	if (a > b) return b;
	return a;
}

void SetPWM(uint16_t val)
{
	if (val > 0)
	{
		TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm | TCA_SINGLE_OVF_bm;
		LED_PWM(val);
	}
	else
	{
		TCA0.SINGLE.INTCTRL = 0;
		PORTA.OUTCLR = 1 << 2;
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
