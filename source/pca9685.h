#ifndef _pca9685_H_
#define _pca9685_H_

#include "i2c.h"

#define bottomServo PCA9685_LED0_ON_L
#define horizontalServo PCA9685_LED1_ON_L
#define verticalServo PCA9685_LED2_ON_L
#define clawServo PCA9685_LED3_ON_L

void pca9685_init(void);
void pca9685_set_prescaler(uint8_t prescaler);
void pca9685_set_pwm(uint8_t servoAddress, uint16_t on, uint16_t off);
#endif