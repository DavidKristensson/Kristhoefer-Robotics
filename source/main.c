#include "i2c.h"
#include "adc.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdint.h>

#include "i2c.h"
#include "pca9685.h"

#include "serial.h"
#include "timer.h"

	/* Value we get from reading of ADC */
volatile unsigned int previousReadADCvalue = 0;
volatile unsigned int Y_Value = 0;
volatile unsigned int X_Value = 0;

int main (void) {
  uint8_t prescalerValue = 0x64;
	timer0_init();
	uart_init();
	init_single_conversion_mode();
	i2c_init();

	pca9685_set_prescaler(prescalerValue);
	
	pca9685_set_pwm(bottomServo, 0, 0x7D0);
	pca9685_set_pwm(horizontalServo, 0, 0x3E8);
	pca9685_set_pwm(verticalServo, 0, 0x640);
	pca9685_set_pwm(clawServo, 0, 0x5DC);
	/*
		Bottom servo:
			Min val: 0x190
			Max val: 0x7D0
			Middle val: 0x41C
		Horizontal servo:
			Min val: 0x2BC Watch out for vertical servo values
			Max val: 0x7D0
			Middle val: 0x3E8
		Vertical servo:
			Min val: 0x514
			Max val: 0x7D0
			Middle val: 0x640
		Claw servo:
			Min val: 0x5DC
			Max val: 0x76C
	*/

	while (1) {
		printf_P(PSTR("X-axis value: %d\n"), X_Value);
		printf_P(PSTR("Y-axis value: %d\n"), Y_Value);
	}

	return 0;
}

	/* timer0 interrupt to execute every 10 ms  */
ISR(TIMER0_COMPA_vect)
{
	/* Start the ADC conversion below here */
	ADCSRA |= (1 << ADSC);  // 
}

ISR(ADC_vect) {
	/* Since ADCL and ADCH alone can only hold 255 as highest value
	we add them together to get an higher value. In this case we strive for a value
	of 0 to 1023*/

	uint8_t low = ADCL; 
	uint8_t high = ADCH;

	/* Combine the two bytes */
	previousReadADCvalue = (high << 8) | low;

	/* Toggle analog pin to read so next time 
	in the interrupt we read from pin 1 
	instead of 0 or the other way around*/
	ADMUX ^= (1 << MUX0);


	/* Print out value of X-axis or Y-axis 
	If index MUX0 is 1 in ADMUX registry do this*/
	if (ADMUX & (1 << MUX0)) {
		//printf_P(PSTR("Y-axis value: %d\n"), previousReadADCvalue);
		
		Y_Value = previousReadADCvalue; 
		
	}
	else {
		//printf_P(PSTR("X-axis value: %d\n"), previousReadADCvalue);
		X_Value = previousReadADCvalue;
	}


}
