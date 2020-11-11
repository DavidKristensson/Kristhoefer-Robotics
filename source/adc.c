#include <avr/interrupt.h>
#include "adc.h"

void init_single_conversion_mode()
{
    /* To use ADC0 Set all the MUX to 0 
    (0000) for using ADC0
    ADC0 is analog input 0 on arduino pin*/
    //ADMUX |= (0 << MUX3 | 0 << MUX2 | 0 << MUX1 | 0 << MUX0);

    /* To use ADC1 instead Set all the MUX to 0 except for
    MUX0 that should be 1. (0001) for using ADC1
    ADC1 is analog input 1 on arduino pin*/

    /* this is for analog pin 1, start with analog pin 0 only
    i think we should just enable both 1 and 0 once and then we can
    read from both of them =) */

    ADMUX |= (0 << MUX3 | 0 << MUX2 | 0 << MUX1 | 1 << MUX0); 

 

    ADMUX |= (1 << REFS0);      // use AVcc as the reference
                                
    //ADMUX |= (1 << ADLAR);      // Left adjusted bit data // WITH THIS COMMENTED OUT IT WORKS (0-1023 VALUE FROM ADC)
 

    ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // Set prescaler value to 8
    ADCSRA |= (1 << ADEN);      // Enable the ADC
    ADCSRA |= (1 << ADIE);      // Enable Interrupts 

    sei();                      // Enable GLOBAL interrupts
}