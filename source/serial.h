#ifndef SERIAL_H_
#define SERIAL_H_

#include <avr/pgmspace.h>
#include <stdio.h>

#define BAUDRATE 38400
#define UBRR (F_CPU/16/BAUDRATE-1)

void uart_init(void);

int uart_putchar(char chr, FILE* stream);

char uart_getchar(uint8_t *buttonFlag);

/*
void uart_get_string_from_user(unsigned char* stringInMain);
*/
#endif

