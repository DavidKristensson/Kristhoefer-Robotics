#include "pca9685.h"

void pca9685_set_prescaler(uint8_t prescaler) {
	/*
	Prescale value = (osc_clock / (4096 * update_rate)) - 1
	0x79 = 121 = (25 Mhz / (4096 * 50)) - 1  // 50 hz
	0x64 = 100 = (25 Mhz / (4096 * 60)) -1 // 60 hz

	Sleep mode enabled and after changing prescaler disabled for continous normal operation
*/
	write_byte(PCA9685_MODE1, 0x01);
	write_byte(PCA9685_PRESCALE, prescaler);
	write_byte(PCA9685_MODE1, 0x00);
}

void pca9685_set_pwm(uint8_t servoAddress, uint16_t on, uint16_t off){
	/*
		Splitting 16bit uints into on and off registers
		Incrementing on each write
	*/
	write_byte(servoAddress, on);
	write_byte(servoAddress + 1, (on >> 8));

	write_byte(servoAddress + 2, off);
	write_byte(servoAddress + 3, (off >> 8));
}

void pca9685_servo_start_position(SERVO servo1, SERVO servo2, SERVO servo3, SERVO servo4) {
	/*
		Send position to each servo
	*/
	pca9685_set_pwm(servo1.address, 0, servo1.position);
	pca9685_set_pwm(servo2.address, 0, servo2.position);
	pca9685_set_pwm(servo3.address, 0, servo3.position);
	pca9685_set_pwm(servo4.address, 0, servo4.position);
}


long map(long x, long in_min, long in_max, long out_min, long out_max) {
	/*
		Maps a value to another
	*/
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void pca9685_set_velocity(SERVO* servo) {
	/*
		Sets the velocity to increasing or decreasing depending on if the 
		mapped analog value is below the middle point and decreasing or above it and increasing.
		This prevents a change in velocity when a joystick for example returns to the middle after release.
	*/
	if (servo->analog_Map > servo->analog_Map_Mid && servo->analog_Map > servo->analog_Map_Last) {
		servo->velocity = servo->velocity_Increasing;
	}
	else if (servo->analog_Map < servo->analog_Map_Mid && servo->analog_Map < servo->analog_Map_Last) {
		servo->velocity = servo->velocity_Decreasing;
	}
}

void pca9685_step_servo(SERVO* servo) {
	/*
		Sends the position and the velocity to travel to the servo and updates the position.
		A span of the middle value is checked for to avoid servo writing when for example a joystick is in the center.
		Also accounts for end ranges
	*/
	if ((servo->analog_Map > (servo->analog_Map_Mid + 5) || servo->analog_Map < (servo->analog_Map_Mid - 5)) &&
		servo->position <= servo->position_Max && servo->velocity == servo->velocity_Increasing) {
		pca9685_set_pwm(servo->address, 0, (servo->position + servo->velocity));
		servo->position += servo->velocity;
	}
	else if ((servo->analog_Map > (servo->analog_Map_Mid + 5) || servo->analog_Map < (servo->analog_Map_Mid - 5)) &&
		servo->position >= servo->position_Min && servo->velocity == servo->velocity_Decreasing) {
		pca9685_set_pwm(servo->address, 0, (servo->position + servo->velocity));
		servo->position += servo->velocity;
	}
}

void pca9685_step_servo_uart(char charFromEsp32, SERVO* servo1, SERVO* servo2, SERVO* servo3, SERVO* servo4){
	//Bottom
	if (charFromEsp32 == 'r' && servo1->position < servo1->position_Max) {
		servo1->velocity = servo1->velocity_Increasing;
		pca9685_set_pwm(servo1->address, 0, (servo1->position + servo1->velocity));
		servo1->position += servo1->velocity;
	}
	else if (charFromEsp32 == 'l' && servo1->position > servo1->position_Min) {
		servo1->velocity = servo1->velocity_Decreasing;
		pca9685_set_pwm(servo1->address, 0, (servo1->position + servo1->velocity));
		servo1->position += servo1->velocity;
	}
	//Claw
	else if (charFromEsp32 == 'o' && servo2->position < servo2->position_Max) {
		servo2->velocity = servo2->velocity_Increasing;
		pca9685_set_pwm(servo2->address, 0, (servo2->position + servo2->velocity));
		servo2->position += servo2->velocity;
	}
	else if (charFromEsp32 == 'c' && servo2->position > servo2->position_Min) {
		servo2->velocity = servo2->velocity_Decreasing;
		pca9685_set_pwm(servo2->address, 0, (servo2->position + servo2->velocity));
		servo2->position += servo2->velocity;
	}
	//Vertical
	else if (charFromEsp32 == 'u' && servo3->position < servo3->position_Max) {
		servo3->velocity = servo3->velocity_Increasing;
		pca9685_set_pwm(servo3->address, 0, (servo3->position + servo3->velocity));
		servo3->position += servo3->velocity;
	}
	else if (charFromEsp32 == 'd' && servo3->position > servo3->position_Min) {
		servo3->velocity = servo3->velocity_Decreasing;
		pca9685_set_pwm(servo3->address, 0, (servo3->position + servo3->velocity));
		servo3->position += servo3->velocity;
	}
	//Horizontal
	else if (charFromEsp32 == 'f' && servo4->position < servo4->position_Max) {
		servo4->velocity = servo4->velocity_Increasing;
		pca9685_set_pwm(servo4->address, 0, (servo4->position + servo4->velocity));
		servo4->position += servo4->velocity;
	}
	else if (charFromEsp32 == 'b' && servo4->position > servo4->position_Min) {
		servo4->velocity = servo4->velocity_Decreasing;
		pca9685_set_pwm(servo4->address, 0, (servo4->position + servo4->velocity));
		servo4->position += servo4->velocity;
	}
}
