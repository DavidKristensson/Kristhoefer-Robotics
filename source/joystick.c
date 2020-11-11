#include "joystick.h"

void analogSpakTest(int mappedJoystickValue, int previousServoValue, int servoToSteer) {

    if (mappedAnalogStickValue > previousServoValue) {
        for (int i = previousServoValue; i < mappedAnalogStickValue; i++) {
            servoToSteer = i;  // enter any servo you like and give it the value of i
            //delay(5);        // lets try without delay since interrupt is 10 ms for ADC convert
        }
    }

    else if (mappedAnalogStickValue < previousServoValue) {
        for (int i = previousServoValue; i > mappedAnalogStickValue; i--) {
            servoToSteer = i;   // enter any servo you like and give it the value of i
            //delay(5);         // lets try without delay since interrupt is 10 ms for ADC convert
        }
    }
}