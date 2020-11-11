#ifndef _ADC_H_
#define _ADC_H_

#include <avr/pgmspace.h>
#include <avr/io.h>

void steerJoystick(int mappedJoystickValue, int previousServoValue);

#endif