#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>


/* This is the timer set to execute an interrupt every 10 ms */
void timer0_init(){
 
  /*Set timer to CTC mode */
  TCCR0A |= (1 << WGM01);

  /* Generate an interrupt each time the counter (TCNT0) reaches 156 
  Output Compare Value calculation is: (16 000 000 / (1024 x 100)) – 1 = 155,25 Which is rounded to 155.*/
  //OCR0A = 155; // real prescaler we want // 10 MILLISECONDS DELAY
  OCR0A = 236; // test prescaler davve     // 15 MILLISECONDS DELAY

  /* ENABLE Interrupt for ISR. */
  TIMSK0 = (1 << OCIE0A);
  
  /* enable interrupts */
  sei();

  /* initialize the counter */
  TCNT0 = 0;

  /* initialize the prescaler 1024 and start the timer */  
  TCCR0B |= (1 << CS02) | (1 << CS00);

}