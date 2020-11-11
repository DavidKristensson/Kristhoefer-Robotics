#ifndef _I2C_H_
#define _I2C_H_

#include "serial.h"

#define I2C_R	1
#define I2C_W	0

// REGISTER ADDRESSES
#define PCA9685_MODE1 0x00      /**< Mode Register 1 */
#define PCA9685_MODE2 0x01      /**< Mode Register 2 */
#define PCA9685_SUBADR1 0x02    /**< I2C-bus subaddress 1 */
#define PCA9685_SUBADR2 0x03    /**< I2C-bus subaddress 2 */
#define PCA9685_SUBADR3 0x04    /**< I2C-bus subaddress 3 */
#define PCA9685_ALLCALLADR 0x05 /**< LED All Call I2C-bus address */
//-------------------------------------------------------
#define PCA9685_LED0_ON_L 0x06  /**< LED0 on tick, low byte*/
#define PCA9685_LED0_ON_H 0x07  /**< LED0 on tick, high byte*/
#define PCA9685_LED0_OFF_L 0x08 /**< LED0 off tick, low byte */
#define PCA9685_LED0_OFF_H 0x09 /**< LED0 off tick, high byte */

#define PCA9685_LED1_ON_L 0xA0  /**< LED1 on tick, low byte*/
#define PCA9685_LED1_ON_H 0xA1  /**< LED1 on tick, high byte*/
#define PCA9685_LED1_OFF_L 0xA2 /**< LED1 off tick, low byte */
#define PCA9685_LED1_OFF_H 0xA3 /**< LED1 off tick, high byte */

#define PCA9685_LED2_ON_L 0xA4  /**< LED2 on tick, low byte*/
#define PCA9685_LED2_ON_H 0xA5  /**< LED2 on tick, high byte*/
#define PCA9685_LED2_OFF_L 0xA6 /**< LED2 off tick, low byte */
#define PCA9685_LED2_OFF_H 0xA7 /**< LED2 off tick, high byte */

#define PCA9685_LED3_ON_L 0xA8  /**< LED3 on tick, low byte*/
#define PCA9685_LED3_ON_H 0xA9  /**< LED3 on tick, high byte*/
#define PCA9685_LED3_OFF_L 0xB0 /**< LED3 off tick, low byte */
#define PCA9685_LED3_OFF_H 0xB1 /**< LED3 off tick, high byte */
// etc all 16:  LED15_OFF_H 0x45
//-----------------------------------------------------------
#define PCA9685_ALL_LED_ON_L 0xFA  /**< load all the LEDn_ON registers, low */
#define PCA9685_ALL_LED_ON_H 0xFB  /**< load all the LEDn_ON registers, high */
#define PCA9685_ALL_LED_OFF_L 0xFC /**< load all the LEDn_OFF registers, low */
#define PCA9685_ALL_LED_OFF_H 0xFD /**< load all the LEDn_OFF registers,high */
#define PCA9685_PRESCALE 0xFE     /**< Prescaler for PWM output frequency */
#define PCA9685_TESTMODE 0xFF     /**< defines the test mode to be entered */

// MODE1 bits
#define MODE1_ALLCAL 0x01  /**< respond to LED All Call I2C-bus address */
#define MODE1_SUB3 0x02    /**< respond to I2C-bus subaddress 3 */
#define MODE1_SUB2 0x04    /**< respond to I2C-bus subaddress 2 */
#define MODE1_SUB1 0x08    /**< respond to I2C-bus subaddress 1 */
#define MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20      /**< Auto-Increment enabled */
#define MODE1_EXTCLK 0x40  /**< Use EXTCLK pin clock */
#define MODE1_RESTART 0x80 /**< Restart enabled */
// MODE2 bits
#define MODE2_OUTNE_0 0x01 /**< Active LOW output enable input */
#define MODE2_OUTNE_1                                                          \
  0x02 /**< Active LOW output enable input - high impedience */
#define MODE2_OUTDRV 0x04 /**< totem pole structure vs open-drain */
#define MODE2_OCH 0x08    /**< Outputs change on ACK vs STOP */
#define MODE2_INVRT 0x10  /**< Output logic state inverted */

#define PCA9685_I2C_ADDRESS 0x40      /**< Default PCA9685 I2C Slave Address */
#define FREQUENCY_OSCILLATOR 25000000 /**< Int. osc. frequency in datasheet */

#define PCA9685_PRESCALE_MIN 3   /**< minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /**< maximum prescale value */

void i2c_init(void);

void i2c_start(void);
void i2c_stop(void);

uint8_t i2c_get_status(void);
void i2c_meaningful_status(uint8_t status);


void i2c_xmit_byte(uint8_t byte);
void eeprom_write_byte(uint8_t eeprom_addr_write, uint8_t byte);

void eeprom_wait_until_write_complete(void);

void i2c_xmit_addr(uint8_t eeprom_addr, uint8_t i2c_rw);
#endif