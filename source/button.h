#ifndef BUTTON_H_
#define BUTTON_H_

#include <avr/io.h>

void button_init();

void button_set_buttonStateNow(volatile uint8_t* buttonStateNow, uint8_t port, uint8_t pin);

void button_set_flag(uint8_t* buttonStateNow, uint8_t* buttonStateLast, uint8_t* buttonFlag, uint8_t notPushedValue);

void button_set_buttonStateLast(uint8_t* buttonStateNow, uint8_t* buttonStateLast);
#endif