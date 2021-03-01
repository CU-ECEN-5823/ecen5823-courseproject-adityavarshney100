/*
 * i2c.h
 *
 *  Created on: 12-Feb-2021
 *      Author: aditya.vny95
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#include <i2cspm.h>
#include "timers.h"
#include "gpio.h"
#include "log.h"
#include <em_i2c.h>
#include "main.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "infrastructure.h"

#define INCLUDE_LOG_DEBUG 1
#define SI7021_ADDRESS 0x80							// Address of SI7021 Temperature sensor

/*
 * Initializes I2C peripherals
 * Return type: VOID
 * Parameter:	None
*/
void InitI2C();


/*
 * Perform I2C write operation
 * Return type: VOID
 * Parameter:	None
*/
void writeI2C();


/*
 * Performs I2C read operation
 * Return type: VOID
 * Parameter:	None
*/
void readI2C();

/*
 * Calculation for temperature
 * Return type: VOID
 * Parameter:	None
*/
void Temperature();

/*
 * Disables I2C
 * Return type: VOID
 * Parameter:	None
 *
 */

void DisableI2C();

#endif /* SRC_I2C_H_ */
