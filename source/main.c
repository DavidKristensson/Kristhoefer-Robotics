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

//Position_Max, Position_Min, Position, dsIncreasing, dsDeacreasing, ds, analog, analogLast, analogMid, address
SERVO servoBottom =		{ 0x7D0, 0x190,	0x41C, 4, -4, 4, 0, 0, 1211, PCA9685_LED0_ON_L };
SERVO servoClaw =		{ 0x76C, 0x5DC, 0x5DC, 4, -4, 4, 0, 0, 1698, PCA9685_LED3_ON_L };
SERVO servoVertical	=	{ 0x7CF, 0x514, 0x514, 4, -4, 4, 0, 0, 1655, PCA9685_LED2_ON_L };
SERVO servoHorizontal = { 0x7D0, 0x2BC, 0x4B0, 4, -4, 4, 0, 0, 1365, PCA9685_LED1_ON_L };

int main(void) {
	uint8_t prescalerValue = 0x64;
	timer0_init();
	uart_init();
	init_single_conversion_mode();
	i2c_init();


	pca9685_set_prescaler(prescalerValue);

	//Start positon of arm on reboot
	pca9685_set_pwm(servoBottom.address, 0, servoBottom.position);
	pca9685_set_pwm(servoClaw.address, 0, servoClaw.position);
	pca9685_set_pwm(servoVertical.address, 0, servoVertical.position);
	pca9685_set_pwm(servoHorizontal.address, 0, servoVertical.position);

	while (1) {
		/*
		-----------------------------------------------------------------------------------------
											MAPPING VALUES and SETTING Velocity
		-----------------------------------------------------------------------------------------
		*/
		//Bottom servo
		servoBottom.analog_Map = map(joystick_1_X_Value, 0, 1023, servoBottom.position_Min, servoBottom.position_Max);
		if (servoBottom.analog_Map > servoBottom.analog_Map_Mid &&
			servoBottom.analog_Map > servoBottom.analog_Map_Last) {
			servoBottom.velocity = servoBottom.velocity_Increasing;
		}
		else if (servoBottom.analog_Map < servoBottom.analog_Map_Mid &&
			servoBottom.analog_Map < servoBottom.analog_Map_Last) {
			servoBottom.velocity = servoBottom.velocity_Decreasing;
		}

		//Claw servo
		servoClaw.analog_Map = map(joystick_1_Y_Value, 0, 1023, servoClaw.position_Min, servoClaw.position_Max);
		if (servoClaw.analog_Map > servoClaw.analog_Map_Mid &&
			servoClaw.analog_Map > servoClaw.analog_Map_Last) {
			servoClaw.velocity = servoClaw.velocity_Increasing;
		}
		else if (servoClaw.analog_Map < servoClaw.analog_Map_Mid &&
			servoClaw.analog_Map < servoClaw.analog_Map_Last) {
			servoClaw.velocity = servoClaw.velocity_Decreasing;
		}

		//Vertical servo
		servoVertical.analog_Map = map(joystick_2_X_Value, 0, 1023, servoVertical.position_Min, servoVertical.position_Max);
		if (servoVertical.analog_Map > servoVertical.analog_Map_Mid&&
			servoVertical.analog_Map > servoVertical.analog_Map_Last) {
			servoVertical.velocity = servoVertical.velocity_Increasing;
		}
		else if (servoVertical.analog_Map < servoVertical.analog_Map_Mid &&
			servoVertical.analog_Map < servoVertical.analog_Map_Last) {
			servoVertical.velocity = servoVertical.velocity_Decreasing;
		}

		//Horizontal servo
		servoHorizontal.analog_Map = map(joystick_2_Y_Value, 0, 1023, servoHorizontal.position_Min, servoHorizontal.position_Max);
		if (servoHorizontal.analog_Map > servoHorizontal.analog_Map_Mid&&
			servoHorizontal.analog_Map > servoHorizontal.analog_Map_Last) {
			servoHorizontal.velocity = servoHorizontal.velocity_Increasing;
		}
		else if (servoHorizontal.analog_Map < servoHorizontal.analog_Map_Mid &&
			servoHorizontal.analog_Map < servoHorizontal.analog_Map_Last) {
			servoHorizontal.velocity = servoHorizontal.velocity_Decreasing;
		}
		/*
		-----------------------------------------------------------------------------------------
											SET PWN
		-----------------------------------------------------------------------------------------
		*/
		//Bottom servo
		if ((servoBottom.analog_Map > (servoBottom.analog_Map_Mid + 5) ||
		servoBottom.analog_Map < (servoBottom.analog_Map_Mid - 5)) &&
		servoBottom.position <= servoBottom.position_Max &&
		servoBottom.velocity == servoBottom.velocity_Increasing) {
			pca9685_set_pwm(servoBottom.address, 0, (servoBottom.position + servoBottom.velocity));
			servoBottom.position += servoBottom.velocity;
		}
		else if ((servoBottom.analog_Map > (servoBottom.analog_Map_Mid + 5) ||
		servoBottom.analog_Map < (servoBottom.analog_Map_Mid - 5)) &&
		servoBottom.position >= servoBottom.position_Min &&
		servoBottom.velocity == servoBottom.velocity_Decreasing) {
			pca9685_set_pwm(servoBottom.address, 0, (servoBottom.position + servoBottom.velocity));
			servoBottom.position += servoBottom.velocity;
		}


		//Claw
		if ((servoClaw.analog_Map > (servoClaw.analog_Map_Mid + 5) ||
		servoClaw.analog_Map < (servoClaw.analog_Map_Mid - 5)) &&
		servoClaw.position <= servoClaw.position_Max &&
		servoClaw.velocity == servoClaw.velocity_Increasing) {
			pca9685_set_pwm(servoClaw.address, 0, (servoClaw.position + servoClaw.velocity));
			servoClaw.position += servoClaw.velocity;
		}
		else if ((servoClaw.analog_Map > (servoClaw.analog_Map_Mid + 5) ||
		servoClaw.analog_Map < (servoClaw.analog_Map_Mid - 5)) &&
		servoClaw.position >= servoClaw.position_Min &&
		servoClaw.velocity == servoClaw.velocity_Decreasing) {
			pca9685_set_pwm(servoClaw.address, 0, (servoClaw.position + servoClaw.velocity));
			servoClaw.position += servoClaw.velocity;
		}

		//Vertical
		if ((servoVertical.analog_Map > (servoVertical.analog_Map_Mid + 5) ||
		servoVertical.analog_Map < (servoVertical.analog_Map_Mid - 5)) &&
		servoVertical.position <= servoVertical.position_Max &&
		servoVertical.velocity == servoVertical.velocity_Increasing) {
			pca9685_set_pwm(servoVertical.address, 0, (servoVertical.position + servoVertical.velocity));
			servoVertical.position += servoVertical.velocity;
		}
		else if ((servoVertical.analog_Map > (servoVertical.analog_Map_Mid + 5) ||
		servoVertical.analog_Map < (servoVertical.analog_Map_Mid - 5)) &&
		servoVertical.position >= servoVertical.position_Min &&
		servoVertical.velocity == servoVertical.velocity_Decreasing) {
			pca9685_set_pwm(servoVertical.address, 0, (servoVertical.position + servoVertical.velocity));
			servoVertical.position += servoVertical.velocity;
		}

		//Horizontal
		if ((servoHorizontal.analog_Map > (servoHorizontal.analog_Map_Mid + 5) ||
			servoHorizontal.analog_Map < (servoHorizontal.analog_Map_Mid - 5)) &&
			servoHorizontal.position <= servoHorizontal.position_Max &&
			servoHorizontal.velocity == servoHorizontal.velocity_Increasing) {
			pca9685_set_pwm(servoHorizontal.address, 0, (servoHorizontal.position + servoHorizontal.velocity));
			servoHorizontal.position += servoHorizontal.velocity;
		}
		else if ((servoHorizontal.analog_Map > (servoHorizontal.analog_Map_Mid + 5) ||
			servoHorizontal.analog_Map < (servoHorizontal.analog_Map_Mid - 5)) &&
			servoHorizontal.position >= servoHorizontal.position_Min &&
			servoHorizontal.velocity == servoHorizontal.velocity_Decreasing) {
			pca9685_set_pwm(servoHorizontal.address, 0, (servoHorizontal.position + servoHorizontal.velocity));
			servoHorizontal.position += servoHorizontal.velocity;
		}

		/*
		-----------------------------------------------------------------------------------------
									Assign last
		-----------------------------------------------------------------------------------------
		*/

		servoBottom.analog_Map_Last = servoBottom.analog_Map;
		servoClaw.analog_Map_Last = servoClaw.analog_Map;
		servoVertical.analog_Map_Last = servoVertical.analog_Map;
		servoHorizontal.analog_Map_Last = servoHorizontal.analog_Map;
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




