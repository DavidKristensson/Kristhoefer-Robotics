#include "stateHandler.h"

ROBOT_CONTROL_STATES state_changer(ROBOT_CONTROL_STATES currentControlState, uint8_t *buttonFlag) {
    /*
        If buttonflag has been raised, change state and reset flag
    */
    if (*buttonFlag == 1) {
        if (currentControlState == WEBSERVER_CONTROL) {
            currentControlState = MANUAL_CONTROL;
            *buttonFlag = 0;
        }
        else if (currentControlState == MANUAL_CONTROL) {
            currentControlState = WEBSERVER_CONTROL;
            *buttonFlag = 0;
        }       
    }

    return currentControlState;
}