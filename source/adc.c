#include <avr/interrupt.h>
#include "adc.h"

void init_single_conversion_mode(){
    ADMUX |= (1 << MUX0);   // Init for activating analog pin 1, we start with analog pin 1 
                            //and then read out analog pin 0 in the interrupt.
    
    ADMUX |= (1 << REFS0);      // use AVcc as the reference
                                
    //ADMUX |= (1 << ADLAR);      // Left adjusted bit data // WITH THIS COMMENTED OUT IT WORKS (0-1023 VALUE FROM ADC)
 
    ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // Set prescaler value to 8
    ADCSRA |= (1 << ADEN);      // Enable the ADC
    ADCSRA |= (1 << ADIE);      // Enable Interrupts 

    sei();                      // Enable GLOBAL interrupts
}
