#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"
#include "pca9685.h"

#include <stdint.h>



int main(void) {
	uint8_t prescalerValue = 0x64;
	i2c_init();
	uart_init();

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
	}
	return 0;
}