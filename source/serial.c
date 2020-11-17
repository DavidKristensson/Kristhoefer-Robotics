#include <avr/io.h>

#include <stdio.h>

#include "serial.h"

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void uart_init(void) {
	UBRR0H = (unsigned char)(UBRR >> 8);
	UBRR0L = (unsigned char)(UBRR);
	UCSR0A = 0;
	UCSR0B = (1 << TXEN0 | 1 << RXEN0);
	UCSR0C = (1 << UCSZ01 | 1 << UCSZ00);

	stdout = &uart_stdout;
}
/*
void uart_get_string_from_user(unsigned char* stringInMain, uint8_t buttonFlag) { // Gets input from user with uart_getchar() 
															  // until read character is \n, then null terminates
															  // the string and saves it to stringInMain.
	char character = 'a';

	int i = 0;
	while (character != '\n') {
		character = uart_getchar(buttonFlag);
		stringInMain[i] = character;
		i++;
	}
	stringInMain[i] = '\0'; // Null terminate string
}
*/

int uart_putchar(char chr, FILE *stream) {
	if (chr == '\n') {
		uart_putchar('\r', NULL);
	}
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = chr;
	return 0;
}

char uart_getchar(uint8_t *buttonFlag) {
	/* Bit 7 – RXC0:?USART Receive Complete
	This flag bit is set when there are unread data in the receive buffer and cleared when the receive buffer is
	empty (i.e., does not contain any unread data). If the Receiver is disabled, the receive buffer will be
	flushed and consequently the RXC0 bit will become zero. The RXC0 Flag can be used to generate a
	Receive Complete interrupt (see description of the RXCIE0 bit).
	*/
	
	while (!(UCSR0A & (1 << RXC0))) {
		if (*buttonFlag == 1) {
			break;
		}
	}
	return UDR0;
}

