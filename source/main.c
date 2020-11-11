#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"

int main(void) {
	i2c_init();
	uart_init();

	//eeprom_write_byte(PCA9685_MODE1, 0x00);
	
	eeprom_write_byte(PCA9685_MODE1, 0x01); // sleep mode enabled for changing prescaler
	eeprom_write_byte(PCA9685_PRESCALE, 0x79); // changing prescaler 121 = 0x79
	eeprom_write_byte(PCA9685_MODE1, 0x00); // disabled sleep mode for normal operation
	/*
		Prescale value = (osc_clock / (4096 * update_rate)) - 1
		121 = (25 Mhz / (4096 * 50)) - 1  
	*/
	
	eeprom_write_byte(PCA9685_LED0_ON_L, 0x00); // ON LOW
	printf_P(PSTR("------ON LOW------\n"));
	eeprom_write_byte(PCA9685_LED0_ON_H, 0x00); // ON HIGH
	printf_P(PSTR("------ON HIGH------\n"));

	eeprom_write_byte(PCA9685_LED0_OFF_L, 0xFF); // OFF LOW
	printf_P(PSTR("------OFF LOW------\n"));
	eeprom_write_byte(PCA9685_LED0_OFF_H, 0x05); // OFF HIGH
	printf_P(PSTR("------OFF HIGH------\n"));
	_delay_ms(1000);

	/*
	ON L = 0x00
	ON H = 0x00

	OFF L = 0xFF
	OFF H = 0x01-0x07 min-max ranges
	*/

	while (1) {

	}



	return 0;
}