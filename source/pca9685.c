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

// Function to map the joystick value 0-1023 into robot servos values 400-2000 
long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
