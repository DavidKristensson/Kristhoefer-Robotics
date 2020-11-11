#include "i2c.h"
#include "adc.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "serial.h"
#include "timer.h"


volatile unsigned int previousReadADCvalue = 0;

int main (void) {

	timer0_init();
	uart_init();
	init_single_conversion_mode();
	
	
	//uart_putchar('x');

	while (1) {
		//printf_P(PSTR("Testar skriva till uart\n"));
	}

	return 0;
}

// timer0 interrupt to execute every 10 ms (WORKS NOW)
ISR(TIMER0_COMPA_vect)
{
	ADCSRA |= (1 << ADSC);  // Start the ADC conversion below here
	//printf_P(PSTR("Testar skriva till uart\n"));
}

ISR(ADC_vect) {
	/* Since ADCL and ADCH alone can only hold 255 as highest value
	we add them together to get an higher value. In this case we strive for a value
	of 0 to 1023*/

	uint8_t low = ADCL; 
	uint8_t high = ADCH;

	// combine the two bytes
	previousReadADCvalue = (high << 8) | low;
	printf_P(PSTR("Previous read adc value: %d\n"), previousReadADCvalue);
}

