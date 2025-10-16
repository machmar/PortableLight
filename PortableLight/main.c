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
#include <stdint.h>
#include <stdbool.h>
#include "MapMath/MapMath.h"

#define LED_PWM(x) TCA0.SINGLE.CMP0 = x

uint16_t min(uint16_t a, uint16_t b);
void SetPWM(uint16_t val);
void ADCProcess();
void ADCStartConversion();

uint16_t pot_val_ = 0;

typedef enum {
	ADC_SOURCE_POT,
	ADC_SOURCE_SHUNT_BEFORE,
	ADC_SOURCE_SHUNT_AFTER,
	ADC_SOURCE_SUPPLY,
	ADC_SOURCE_COUNT
	} AdcSource_t;

AdcSource_t current_ADC_source_ = ADC_SOURCE_POT;
bool ADC_prepared_ = false;
uint16_t ADC_result_[ADC_SOURCE_COUNT] = {0};

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
	TCA0.SINGLE.DBGCTRL = TCA_SINGLE_DBGRUN_bm;
	TCA0.SINGLE.PER = 0x3FF;
	LED_PWM(2);
	
	TCB0.CTRLA = TCB_CLKSEL_CLKTCA_gc | TCB_ENABLE_bm;
	TCB0.CTRLB = TCB_CNTMODE_SINGLE_gc;
	TCB0.EVCTRL = TCB_CAPTEI_bm; // the timer is started by making a pulse at its event channel
	TCB0.CCMP = 5;
	
	EVSYS.SYNCCH0 = EVSYS_SYNCCH0_TCA0_OVF_LUNF_gc;
	EVSYS.ASYNCUSER0 = EVSYS_ASYNCUSER0_SYNCCH0_gc;
	EVSYS.SYNCCH1 = EVSYS_SYNCCH0_TCB0_gc;
	EVSYS.ASYNCUSER1 = EVSYS_ASYNCUSER1_SYNCCH1_gc;
	
	VREF.CTRLA = VREF_ADC0REFSEL_0V55_gc;
	VREF.CTRLB = VREF_ADC0REFSEL_0_bm;
	
	ADC0.CTRLA = ADC_RUNSTBY_bm | ADC_ENABLE_bm;
	ADC0.CTRLC = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc;
	ADC0.CTRLD = ADC_INITDLY_DLY16_gc;
	ADC0.EVCTRL = ADC_STARTEI_bm;
	ADC0.INTCTRL = ADC_RESRDY_bm;
	ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;
	
	// basically, TCA overflows, triggers TCB, TCB counts up to 5, triggers ADC, that triggers when it's ready, cycle repeats. I somewhat setup the peripherals for it but I haven't done the actual routing
	
    PORTA.DIRSET = 1 << 2;
	
	sei();
	
    while (1) 
    {		
		ADCProcess();
		SetPWM(mapClamp(ADC_result_[ADC_SOURCE_POT], 5, 1023, 0, 600));		
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

void ADCProcess()
{
	if (~ADC0.COMMAND & ADC_STCONV_bm && !ADC_prepared_) //the ADC isn't doing anything
	{
		//let's save the value and prep next measurement
		ADC_result_[(uint8_t)current_ADC_source_] = ADC0.RES;
		
		switch (current_ADC_source_)
		{
			case ADC_SOURCE_POT:
				current_ADC_source_ = ADC_SOURCE_SHUNT_BEFORE;
				break;
			
			case ADC_SOURCE_SHUNT_BEFORE:
				current_ADC_source_ = ADC_SOURCE_SHUNT_AFTER;
				break;
				
			case ADC_SOURCE_SHUNT_AFTER:
				current_ADC_source_ = ADC_SOURCE_POT;
				break;
				
			default:
				current_ADC_source_ = ADC_SOURCE_POT;
		}
		
		// now that it is changed to the next value, prepare the ADC for the measurement
		switch (current_ADC_source_)
		{
			case ADC_SOURCE_POT:
				ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;
				ADC0.CTRLC = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc;
				EVSYS.SYNCCH0 = EVSYS_SYNCCH0_TCA0_CMP0_gc;
				break;
			
			case ADC_SOURCE_SHUNT_BEFORE:
				ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;
				ADC0.CTRLC = ADC_PRESC_DIV32_gc | ADC_REFSEL_INTREF_gc;
				EVSYS.SYNCCH0 = EVSYS_SYNCCH0_TCA0_OVF_LUNF_gc;
				break;
				
			case ADC_SOURCE_SHUNT_AFTER:
				ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
				ADC0.CTRLC = ADC_PRESC_DIV32_gc | ADC_REFSEL_INTREF_gc;
				EVSYS.SYNCCH0 = EVSYS_SYNCCH0_TCA0_OVF_LUNF_gc;
				break;
				
			case ADC_SOURCE_SUPPLY:
				ADC0.MUXPOS = ADC_MUXPOS_INTREF_gc;
				ADC0.CTRLC = ADC_PRESC_DIV32_gc | ADC_REFSEL_VDDREF_gc;
				EVSYS.SYNCCH0 = EVSYS_SYNCCH0_TCA0_CMP0_gc;
				break;
				
			default:
				break; // make the compiler not cry
			
		}
		
		ADC_prepared_ = true;
	}
}

ISR(ADC0_RESRDY_vect)
{
	ADC_prepared_ = false;
	ADC0.INTFLAGS = ADC_RESRDY_bm;
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
