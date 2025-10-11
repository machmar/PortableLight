/*
 * PortableLight.c
 *
 * Created: 10/12/2025 12:52:15 AM
 * Author : marec
 */ 

#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    PORTA.DIRSET = 1 << 2;
	
    while (1) 
    {
		PORTA.OUTTGL = 1 << 2;
		_delay_ms(1000);
    }
}

