#ifndef _pca9685_H_
#define _pca9685_H_

#include "i2c.h"

typedef struct {
	uint16_t position_Max;
	uint16_t position_Min;
	uint16_t position;
	
	int8_t velocity_Increasing;
	int8_t velocity_Decreasing;
	int8_t velocity;

	uint16_t analog_Map; 
	uint16_t analog_Map_Last; 
	uint16_t analog_Map_Mid; 

	uint8_t address;
} SERVO;

void pca9685_set_prescaler(uint8_t prescaler);
void pca9685_set_pwm(uint8_t servoAddress, uint16_t on, uint16_t off);
void pca9685_servo_start_position(SERVO servo1, SERVO servo2,SERVO servo3, SERVO servo4);
void pca9685_set_velocity(SERVO* servo);
void pca9685_step_servo(SERVO* servo);
void pca9685_step_servo_uart(char charFromUart, SERVO* servo1, SERVO* servo2, SERVO* servo3, SERVO* servo4);
long map(long x, long in_min, long in_max, long out_min, long out_max);
#endif