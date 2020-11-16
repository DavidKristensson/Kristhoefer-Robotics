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

volatile unsigned int joystick_1_X_Value = 0;
volatile unsigned int joystick_1_Y_Value = 0;

volatile unsigned int joystick_2_X_Value = 0;
volatile unsigned int joystick_2_Y_Value = 0;

volatile unsigned int ADCincrementor = 0;
unsigned long mappedValue = 0;

/* Function to map the joystick value 0-1023 into robot servos values 400-2000 */
long mapValueFromJoystick(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main(void) {
	uint8_t prescalerValue = 0x64;
	timer0_init();
	uart_init();
	init_single_conversion_mode();
	i2c_init();


	//pca9685_set_prescaler(prescalerValue);

	//pca9685_set_pwm(bottomServo, 0, 0x41C);
	//pca9685_set_pwm(horizontalServo, 0, 0x3E8);
	//pca9685_set_pwm(verticalServo, 0, 0x640);
	//pca9685_set_pwm(clawServo, 0, 0x5DC);
	

	/*
		Bottom servo:
			Min val: 0x190 - Decimal: 400
			Max val: 0x7D0 - Decimal: 2000
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
			Min val: 0x5DC - Decimal: 1500 
			Max val: 0x76C - Decimal: 1900
			Mid val: 0x6A4 - Decimal: 1700
	*/

	char char_From_Esp32;
	while (1) {

		/* Testing to get char to our Atmega from ESP32 via UART. */
		char_From_Esp32 = uart_getchar();
		printf_P(PSTR("Message from arduino is: %c\n"), char_From_Esp32);
		//printf_P(PSTR("Testmessage uart"));
		_delay_ms(1000);
		
		/* All values mapped here for test. We use min 400 and max 2000. */

		/* Bottom servo mapped value */
		//mappedValue = mapValueFromJoystick(joystick_1_X_Value, 0, 1023, 400, 2000); // J1_X BOTTOM
		//pca9685_set_pwm(bottomServo, 0, mappedValue);
		//
		//mappedValue = mapValueFromJoystick(joystick_1_Y_Value, 0, 1023, 1100, 1900); // J1_X BOTTOM
		//if (mappedValue < 1500) {
		//	mappedValue = 1500 + (1500 - mappedValue);
		//}
		//pca9685_set_pwm(clawServo, 0, mappedValue);
		//printf_P(PSTR("claw value: %d\n"), mappedValue);


		///* Left side vertical servo mapped value */
		//mappedValue = mapValueFromJoystick(joystick_1_Y_Value, 0, 1023, 400, 2000); // J1_Y LEFT SIDE
		//printf_P(PSTR("mapped Left: %d\n"), mappedValue);

		///* Right side horizontal servo mapped value */
		//mappedValue = mapValueFromJoystick(joystick_2_X_Value, 0, 1023, 400, 2000); // J2_X = RIGHT SIDE
		//printf_P(PSTR("mapped Right: %d\n"), mappedValue);

		///* Claw servo mapped value */
		//mappedValue = mapValueFromJoystick(joystick_2_Y_Value, 0, 1023, 400, 2000); // J2_Y = CLAW
		//printf_P(PSTR("mapped Claw: %d\n"), mappedValue);



	}

	return 0;
}

/* timer0 interrupt to execute every 10 ms  */
ISR(TIMER0_COMPA_vect)
{
	/* Starts adc conversion and when 
	conversion is done the ADC_vect 
	interrupt function is started */
	ADCSRA |= (1 << ADSC);  
}

ISR(ADC_vect) {
	/* Since ADCL and ADCH alone can only hold 255 as highest value
	we add them together to get an higher value. In this case we strive for a value
	of 0 to 1023*/

	/* Save current values for ADCL and ADCH to low/high variables
	where we read FROM is */
	uint8_t low = ADCL; 
	uint8_t high = ADCH;

	/* Combine the two bytes */
	previousReadADCvalue = (high << 8) | low;

	//printf_P(PSTR("General HEX check for admux register before if statements: %x\n"), ADMUX);

	if (ADMUX == 0x40) {
		/* Setup ADMUX for the next ADC reading so that
		next time in this interrupt function we will read from
		ADC1 ie A1 analog input on arduino */
		ADMUX |=  (1 << MUX0);
		ADMUX &= ~(1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		joystick_1_Y_Value = previousReadADCvalue;
		//printf_P(PSTR("Joystick 1 Y-axis value: %d\n"), joystick_1_Y_Value);
	}

	else if (ADMUX == 0x41) {
		/* Setup ADMUX for the next ADC reading so that
		next time in this interrupt function we will read from
		ADC2 ie A2 analog input on arduino */
		ADMUX &= ~(1 << MUX0);
		ADMUX |=  (1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		joystick_1_X_Value = previousReadADCvalue;
		//printf_P(PSTR("Joystick 1 X-axis value: %d\n"), joystick_1_X_Value);
	}	

	else if (ADMUX == 0x42) {		
		/* Setup ADMUX for the next ADC reading so that
		next time in this interrupt function we will read from
		ADC3 ie A3 analog input on arduino */
		ADMUX |=  (1 << MUX0);
		ADMUX |=  (1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		joystick_2_Y_Value = previousReadADCvalue;
		//printf_P(PSTR("Joystick 2 Y-axis value: %d\n"), joystick_2_Y_Value);
	}

	else if (ADMUX == 0x43) {	
		/* Setup ADMUX for the next ADC reading so that 
		next time in this interrupt function we will read from 
		ADC0 ie A0 analog input on arduino */
		ADMUX &= ~(1 << MUX0);
		ADMUX &= ~(1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
		joystick_2_X_Value = previousReadADCvalue;
		//printf_P(PSTR("Joystick 2 X-axis value: %d\n"), joystick_2_X_Value);
	}
}




