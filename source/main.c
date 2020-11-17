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
#include "button.h"

#include "statehandler.h"

/* Value we get from reading of ADC */
volatile unsigned int previousReadADCvalue = 0;

volatile unsigned int joystick_1_X_Value = 0;
volatile unsigned int joystick_1_Y_Value = 0;

volatile unsigned int joystick_2_X_Value = 0;
volatile unsigned int joystick_2_Y_Value = 0;

volatile unsigned int ADCincrementor = 0;

uint8_t button1_State_Now = 0; //This one is high when not pushed in for some reason
uint8_t button2_State_Now = 0; //This one is low
uint8_t button1_State_Last = 0;
uint8_t button2_State_Last = 0;
uint8_t button1_Flag = 0;
uint8_t button2_Flag = 0;

/* Initiate statehandler and 
assign the starting state as manual controlled */
ROBOT_CONTROL_STATES currentControlState;


//Position_Max, Position_Min, Position, dsIncreasing, dsDeacreasing, ds, analog, analogLast, analogMid, address
SERVO servoBottom =		{ 0x7D0, 0x190,	0x41C, 4, -4, 4, 0, 0, 1212, PCA9685_LED0_ON_L };
SERVO servoClaw =		{ 0x76C, 0x5DC, 0x5DC, 4, -4, 4, 0, 0, 1698, PCA9685_LED3_ON_L };
SERVO servoVertical	=	{ 0x7CF, 0x514, 0x514, 4, -4, 4, 0, 0, 1645, PCA9685_LED2_ON_L };
SERVO servoHorizontal = { 0x7D0, 0x2BC, 0x4B0, 4, -4, 4, 0, 0, 1365, PCA9685_LED1_ON_L };

int main(void) {
	uint8_t prescalerValue = 0x64;
	timer0_init();
	uart_init();
	adc_init();
	i2c_init();
	button_init();

	currentControlState = MANUAL_CONTROL;


	pca9685_set_prescaler(prescalerValue);
	pca9685_servo_start_position(servoBottom, servoClaw, servoVertical, servoHorizontal);
	



	while (1) {

		currentControlState = state_changer(currentControlState, &button1_Flag);

		if (currentControlState == MANUAL_CONTROL) {
			servoBottom.analog_Map = map(joystick_1_X_Value, 0, 1023, servoBottom.position_Max, servoBottom.position_Min);
			servoClaw.analog_Map = map(joystick_1_Y_Value, 0, 1023, servoClaw.position_Min, servoClaw.position_Max);
			servoVertical.analog_Map = map(joystick_2_X_Value, 0, 1023, servoVertical.position_Max, servoVertical.position_Min);
			servoHorizontal.analog_Map = map(joystick_2_Y_Value, 0, 1023, servoHorizontal.position_Min, servoHorizontal.position_Max);
			//printf_P(PSTR("vert analog map val: %d\n"), servoVertical.analog_Map);
			pca9685_set_velocity(&servoBottom);
			pca9685_set_velocity(&servoClaw);
			pca9685_set_velocity(&servoVertical);
			pca9685_set_velocity(&servoHorizontal);

			pca9685_step_servo(&servoBottom);
			pca9685_step_servo(&servoClaw);
			pca9685_step_servo(&servoVertical);
			pca9685_step_servo(&servoHorizontal);

			servoBottom.analog_Map_Last = servoBottom.analog_Map;
			servoClaw.analog_Map_Last = servoClaw.analog_Map;
			servoVertical.analog_Map_Last = servoVertical.analog_Map;
			servoHorizontal.analog_Map_Last = servoHorizontal.analog_Map;
		}
		else if (currentControlState == WEBSERVER_CONTROL) {
				char char_From_Esp32 = uart_getchar(&button1_Flag);
				//printf_P(PSTR("Message from arduino is: %c\n"), char_From_Esp32);
				/* act on message received from esp32 */
				pca9685_step_servo_uart(char_From_Esp32, &servoBottom, &servoClaw, &servoVertical, &servoHorizontal);
		}
	}

	return 0;
}

/* timer0 interrupt to execute every 10 ms  */
ISR(TIMER0_COMPA_vect){




	button_set_buttonStateNow(&button1_State_Now, PIND, PD4);
	button_set_buttonStateNow(&button2_State_Now, PIND, PD2);
	button_set_flag(&button1_State_Now, &button1_State_Last, &button1_Flag, 1);
	button_set_flag(&button2_State_Now, &button2_State_Last, &button2_Flag, 0);
	button_set_buttonStateLast(&button1_State_Now, &button1_State_Last);
	button_set_buttonStateLast(&button2_State_Now, &button2_State_Last);
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




