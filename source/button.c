#include "button.h"

void button_init() {
	/*
		Initializing Arduino pin D4 by setting DDD4 bit to 0(INPUT) in DDRD register
		Initializing Arduino pin D2 by setting DDD2 bit to 0(INPUT) in DDRD register
	*/
	DDRD &= ~(1 << DDD4);
	DDRD &= ~(1 << DDD2);
}


void button_set_buttonStateNow(volatile uint8_t* buttonStateNow, uint8_t port, uint8_t pin) {
	/*
		If button is pressed PIND2 bit in PIND register is high
		If it is high buttonStateNow is changed it accordingly
	*/
	if (port & (1 << pin)) {
		*buttonStateNow = 1;
	}
	else {
		*buttonStateNow = 0;
	}
}

void button_set_flag(uint8_t* buttonStateNow, uint8_t* buttonStateLast, uint8_t* buttonFlag, uint8_t notPushedValue) {
	/*
		notPushedValue compensates for buttons with different values when button is not pushed
		When a new button push comes in the buttonFlag is raised
	*/
	if (notPushedValue == 0) {
		if (*buttonStateNow == 1 && *buttonStateLast == 0) {
			*buttonFlag = 1;
		}
	}
	else if (notPushedValue == 1) {
		if (*buttonStateNow == 0 && *buttonStateLast == 1) {
			*buttonFlag = 1;
		}
	}
}

void button_set_buttonStateLast(uint8_t* buttonStateNow, uint8_t* buttonStateLast) {
	/*
		The last button state is updated
	*/
	*buttonStateLast = *buttonStateNow;
}