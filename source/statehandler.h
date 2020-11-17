#ifndef STATEHANDLER_H_
#define STATEHANDLER_H_
#include <avr/io.h>

typedef enum {           // enum for webserver and manual controlled
    WEBSERVER_CONTROL = 1,
    MANUAL_CONTROL = 2
}ROBOT_CONTROL_STATES;

ROBOT_CONTROL_STATES state_changer(ROBOT_CONTROL_STATES currentControlState, uint8_t *buttonFlag);

#endif