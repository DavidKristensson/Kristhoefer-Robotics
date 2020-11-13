#include <avr/interrupt.h>
#include "adc.h"

void init_single_conversion_mode(){
    /* Setup ADMUX for the next ADC reading so that
    next time in this interrupt function we will read from
    ADC0 ie A0 analog input on arduino */
    ADMUX = 0x00;
    
    ADMUX |= (1 << REFS0);      // use AVcc as the reference
                                
    //ADMUX |= (1 << ADLAR);      // Left adjusted bit data // WITH THIS COMMENTED OUT IT WORKS (0-1023 VALUE FROM ADC)
 
    ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // Set prescaler value to 8
    ADCSRA |= (1 << ADEN);      // Enable the ADC
    ADCSRA |= (1 << ADIE);      // Enable Interrupts 

    sei();                      // Enable GLOBAL interrupts
}
