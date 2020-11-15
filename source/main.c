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

#define maxBottomServo 0x7D0
#define minBottomServo 0x190

#define maxClawServo 0x76C
#define minClawServo 0x5DC

#define maxVerticalServo 0x7CF
#define minVerticalServo 0x514

#define maxHorizontalServo 0x7D0
#define minHorizontalServo 0x2BC

#define direction_increasing 8 //Value of 4 when not having prints in loop
#define direction_decreasing -8 //Value of -4 when not having prints in loop

/* Value we get from reading of ADC */
volatile unsigned int previousReadADCvalue = 0;

volatile unsigned int joystick_1_X_Value = 0;
volatile unsigned int joystick_1_Y_Value = 0;

volatile unsigned int joystick_2_X_Value = 0;
volatile unsigned int joystick_2_Y_Value = 0;

volatile unsigned int ADCincrementor = 0;
unsigned int mappedValue_J1_X = 0;
unsigned int mappedValue_J1_Y = 0;
unsigned int mappedValue_J2_X = 0;
unsigned int mappedValue_J2_Y = 0;

unsigned int mappedValue_J1_X_mid = 1211; //Assigned values from idle joysticks
unsigned int mappedValue_J1_Y_mid = 1497;
unsigned int mappedValue_J2_X_mid = 1655;
unsigned int mappedValue_J2_Y_mid = 1365;

unsigned int mappedValue_J1_X_last = 0;
unsigned int mappedValue_J1_Y_last = 0;
unsigned int mappedValue_J2_X_last = 0;
unsigned int mappedValue_J2_Y_last = 0;

unsigned int servoBottom_position = 0x4B0;
unsigned int servoClaw_position = 0x5DC;
unsigned int servoVertical_position = 0x4B0;
unsigned int servoHorizontal_position = 0x514;

//The higher the value the larger each step will be, directly affecting the speed of the servo
signed char servoBottom_direction = 8;
signed char servoClaw_direction = 8;
signed char servoVertical_direction = 32;
signed char servoHorizontal_direction = 32;

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


	pca9685_set_prescaler(prescalerValue);

	pca9685_set_pwm(bottomServo, 0, 0x41C);
	pca9685_set_pwm(clawServo, 0, 0x5DC);
	pca9685_set_pwm(horizontalServo, 0, 0x4B0);
	pca9685_set_pwm(verticalServo, 0, 0x514);

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
			Middle val: 0x514
		Claw servo:
			Min val: 0x5DC - Decimal: 1500 
			Max val: 0x76C - Decimal: 1900
			Mid val: 0x6A4 - Decimal: 1700
	*/
	while (1) {
		//Claw servo reacts to vertical servo joystick
		/*
		-----------------------------------------------------------------------------------------
											MAPPING VALUES and SETTING DIRECTION
		-----------------------------------------------------------------------------------------
		*/
		/* Bottom servo mapped value */
		mappedValue_J1_X = mapValueFromJoystick(joystick_1_X_Value, 0, 1023, 400, 2000); // J1_X BOTTOM
		if (mappedValue_J1_X > mappedValue_J1_X_mid && mappedValue_J1_X > mappedValue_J1_X_last) {
			servoBottom_direction = direction_increasing;
		}
		else if (mappedValue_J1_X < mappedValue_J1_X_mid && mappedValue_J1_X < mappedValue_J1_X_last) {
			servoBottom_direction = direction_decreasing;
		}
		


		//Claw servo
		mappedValue_J1_Y = mapValueFromJoystick(joystick_1_Y_Value, 0, 1023, 1100, 1900); // J1_Y BOTTOM
		if (mappedValue_J1_Y > mappedValue_J1_Y_mid && mappedValue_J1_Y > mappedValue_J1_Y_last) {
			servoClaw_direction = direction_increasing;
		}
		else if (mappedValue_J1_Y < mappedValue_J1_Y_mid && mappedValue_J1_Y < mappedValue_J1_Y_last) {
			servoClaw_direction = direction_decreasing;
		}

		//Vertical servo
		mappedValue_J2_X = mapValueFromJoystick(joystick_2_X_Value, 0, 1023, 0x514, 0x7D0);
		if (mappedValue_J2_X > mappedValue_J2_X_mid && mappedValue_J2_X > mappedValue_J2_X_last) {
			servoVertical_direction = direction_increasing;
		}
		else if (mappedValue_J2_X < mappedValue_J2_X_mid && mappedValue_J2_X < mappedValue_J2_X_last) {
			servoVertical_direction = direction_decreasing;
		}

		//Horizontal servo
		mappedValue_J2_Y = mapValueFromJoystick(joystick_2_Y_Value, 0, 1023, 0x2BC, 0x7D0);
		if (mappedValue_J2_Y > mappedValue_J2_Y_mid && mappedValue_J2_Y > mappedValue_J2_Y_last) {
			servoHorizontal_direction = direction_increasing;
		}
		else if (mappedValue_J2_Y < mappedValue_J2_Y_mid && mappedValue_J2_Y < mappedValue_J2_Y_last) {
			servoHorizontal_direction = direction_decreasing;
		}
		/*
		*/
		/*

		-----------------------------------------------------------------------------------------
											SET PWN
		-----------------------------------------------------------------------------------------
		*/

		//Bottom servo send position + direction to servo
		if ((mappedValue_J1_X > (mappedValue_J1_X_mid + 5) || mappedValue_J1_X < (mappedValue_J1_X_mid - 5)) && 
			servoBottom_position <= maxBottomServo && servoBottom_direction == direction_increasing) {
			pca9685_set_pwm(bottomServo, 0, (servoBottom_position + servoBottom_direction));
			servoBottom_position += servoBottom_direction;
		}
		else if ((mappedValue_J1_X > (mappedValue_J1_X_mid + 5) || mappedValue_J1_X < (mappedValue_J1_X_mid - 5)) && 
			servoBottom_position >= minBottomServo && servoBottom_direction == direction_decreasing) {
			pca9685_set_pwm(bottomServo, 0, (servoBottom_position + servoBottom_direction));
			servoBottom_position += servoBottom_direction;
		}
		//Claw servo send position + direction to servo
		if ((mappedValue_J1_Y > (mappedValue_J1_Y_mid + 5) || mappedValue_J1_Y < (mappedValue_J1_Y_mid - 5)) && 
			servoClaw_position <= maxClawServo && servoClaw_direction == direction_increasing) {
			pca9685_set_pwm(clawServo, 0, (servoClaw_position + servoClaw_direction));
			servoClaw_position += servoClaw_direction; //TEST
		}
		else if ((mappedValue_J1_Y > (mappedValue_J1_Y_mid + 5) || mappedValue_J1_Y < (mappedValue_J1_Y_mid - 5)) && 
			servoClaw_position >= minClawServo && servoClaw_direction == direction_decreasing) {
			pca9685_set_pwm(clawServo, 0, (servoClaw_position + servoClaw_direction));
			servoClaw_position += servoClaw_direction; //TEST
		}

		//Vertical servo send position + direction to servo
		if ((mappedValue_J2_X > (mappedValue_J2_X_mid + 5) || mappedValue_J2_X < (mappedValue_J2_X_mid - 5)) && 
			servoVertical_position <= maxVerticalServo && servoVertical_direction == direction_increasing) {
			pca9685_set_pwm(verticalServo, 0, (servoVertical_position + servoVertical_direction));
			servoVertical_position += servoVertical_direction;
		}
		else if ((mappedValue_J2_X > (mappedValue_J2_X_mid + 5) || mappedValue_J2_X < (mappedValue_J2_X_mid - 5)) && 
			servoVertical_position >= minVerticalServo && servoVertical_direction == direction_decreasing) {
			pca9685_set_pwm(verticalServo, 0, (servoVertical_position + servoVertical_direction));
			servoVertical_position += servoVertical_direction;
		}

		//Horizontal servo send position + direction to servo
		if ((mappedValue_J2_Y > (mappedValue_J2_Y_mid + 5) || mappedValue_J2_Y < (mappedValue_J2_Y_mid - 5)) && 
			servoHorizontal_position <= maxHorizontalServo && servoHorizontal_direction == direction_increasing) {
			pca9685_set_pwm(horizontalServo, 0, (servoHorizontal_position + servoHorizontal_direction));
			servoHorizontal_position += servoHorizontal_direction;
		}
		else if ((mappedValue_J2_Y > (mappedValue_J2_Y_mid + 5) || mappedValue_J2_Y < (mappedValue_J2_Y_mid - 5)) && 
			servoHorizontal_position >= minHorizontalServo && servoHorizontal_direction == direction_decreasing) {
			pca9685_set_pwm(horizontalServo, 0, (servoHorizontal_position + servoHorizontal_direction));
			servoHorizontal_position += servoHorizontal_direction;
		}
		/*
		*/

		/*
		-----------------------------------------------------------------------------------------
											ADD DIRECTION TO POSITON
		-----------------------------------------------------------------------------------------
		*/

		//THESE IF STATEMENTS ARE NOT WORKING, THE VALUES GO PAST ITS MAX
		//Middle value is not specific, needs to be a span of values
		//If value has passed max value reset value to be below max

		/*
		//  Bottom servo add to positon
		if (mappedValue_J1_X != mappedValue_J1_X_mid && 
			servoBottom_direction == direction_increasing && servoBottom_position <= maxBottomServo) {
			servoBottom_position += servoBottom_direction;
		}
		else if (mappedValue_J1_X != mappedValue_J1_X_mid && 
			servoBottom_direction == direction_decreasing && servoBottom_position >= minBottomServo) {
			servoBottom_position += servoBottom_direction;
		}
		//  Claw servo add to positon
		if (mappedValue_J1_Y > (mappedValue_J1_Y_mid + 5) || mappedValue_J1_Y < (mappedValue_J1_Y_mid - 5) &&
			servoClaw_direction == direction_increasing && servoClaw_position <= maxClawServo) {
			servoClaw_position += servoClaw_direction;
		}
		else if (mappedValue_J1_Y > (mappedValue_J1_Y_mid + 5) || mappedValue_J1_Y < (mappedValue_J1_Y_mid - 5) &&
			servoClaw_direction == direction_decreasing && servoClaw_position >= minClawServo) {
			servoClaw_position += servoClaw_direction;
		}
		*/
		/*
		//Vertical servo add to position
		if (mappedValue_J2_X != mappedValue_J2_X_mid && servoVertical_direction == direction_increasing && servoVertical_position <= maxVerticalServo) {
			servoVertical_position += servoVertical_direction;
		}
		else if (mappedValue_J2_X != mappedValue_J2_X_mid && servoVertical_direction == direction_decreasing && servoVertical_position >= minBottomServo) {
			servoVertical_position += servoVertical_direction;
		}

		//  Horizontal servo add to positon
		if (mappedValue_J2_Y != mappedValue_J2_Y_mid && servoHorizontal_direction == direction_increasing && servoHorizontal_position <= maxHorizontalServo) {
			servoHorizontal_position += servoHorizontal_direction;
		}
		else if (mappedValue_J2_Y != mappedValue_J2_Y_mid && servoHorizontal_direction == direction_decreasing && servoHorizontal_position >= minHorizontalServo) {
			servoHorizontal_position += servoHorizontal_direction;
		}
		*/

		/*
		if (mappedValue_J1_Y != 1211 && servoClaw_direction == direction_increasing && servoBottom_position <= maxBottomServo) {
			servoBottom_position += servoBottom_direction;
		}
		else if (mappedValue_J1_X != 1211 && servoBottom_direction == direction_decreasing && servoBottom_position >= minBottomServo) {
			servoBottom_position += servoBottom_direction;
		}
		*/
		// BUG Vertical servo goes up to top angle sometimes
		// WARNING
		// A timer is needed for all the code in the while loop. Removing these prints below make the loop too fast
		// PS just chanign the direction speed might help
		// WARNING
		/*
		printf_P(PSTR("servoBottom_position: %d\n"), servoVertical_position);
		printf_P(PSTR("mappedValue_J1_X: %d\n"), mappedValue_J2_X);
		printf_P(PSTR("mappedValue_J1_X_last: %d\n"), mappedValue_J2_X_last);
		printf_P(PSTR("servoBottom_direction: %d\n"), servoVertical_direction);
		printf_P(PSTR("------------------------\n"));
		*/
		/*
		printf_P(PSTR("servoBottom_position: %d\n"), servoHorizontal_position);
		printf_P(PSTR("mappedValue_J1_X: %d\n"), mappedValue_J2_Y);
		printf_P(PSTR("mappedValue_J1_X_last: %d\n"), mappedValue_J2_Y_last);
		printf_P(PSTR("servoBottom_direction: %d\n"), servoHorizontal_direction);
		printf_P(PSTR("------------------------\n"));
		*/
		if (!(mappedValue_J1_Y > (mappedValue_J1_Y_mid + 5) || mappedValue_J1_Y < (mappedValue_J1_Y_mid - 5))) {
			printf_P(PSTR("Claw joystickValue: %d\n"), mappedValue_J1_Y);
		}
		else if (mappedValue_J1_X != mappedValue_J1_X_mid) {
			printf_P(PSTR("Bottom joystickValue: %d\n"), mappedValue_J1_X);
		}
		/*
		1497 .... 1211
		printf_P(PSTR("Bottom mappedValue: %d\n"), mappedValue_J1_X);
		printf_P(PSTR("Bottom joystickValue: %d\n"), joystick_1_X_Value);
		printf_P(PSTR("Claw mappedValue: %d\n"), mappedValue_J1_Y);
		printf_P(PSTR("Claw joystickValue: %d\n"), joystick_1_Y_Value);
		printf_P(PSTR("------------------------\n"));
		*/

		mappedValue_J1_X_last = mappedValue_J1_X;
		mappedValue_J1_Y_last = mappedValue_J1_Y;
		mappedValue_J2_X_last = mappedValue_J2_X;
		mappedValue_J2_Y_last = mappedValue_J2_Y;
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
		//joystick_1_Y_Value = previousReadADCvalue;
		joystick_1_X_Value = previousReadADCvalue;
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
		//joystick_1_X_Value = previousReadADCvalue;
		joystick_1_Y_Value = previousReadADCvalue;
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




