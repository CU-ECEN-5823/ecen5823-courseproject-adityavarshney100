/*
   gpio.h
  
    Created on: Dec 12, 2018
        Author: Dan Walkes
    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.
    Updated by Aditya Varshney Feb 5, 2021. Minor edits with function descriptions
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>
#include "em_gpio.h"
#include <string.h>

#define	LED0_port  			gpioPortF
#define LED0_pin   			4
#define LED1_port  			gpioPortF
#define LED1_pin  			5
#define Temperature_port 	gpioPortD
#define Temperature_pin   	9
#define Display_port 		gpioPortD
#define Display_pin 		13
#define PB0_port			gpioPortF
#define PB0_pin				6

#define SensorI2cSCL_port gpioPortC
#define SensorI2cSCL_pin  10
/**
 * See alternate functionality table entry for PC10 in part
 * datasheet at https://www.silabs.com/documents/login/data-sheets/efr32bg13-datasheet.pdf
 */
#define SensorI2cSCL_location 14

/**
 * See alternate functionality table entry for PC11 in part
 * datasheet at https://www.silabs.com/documents/login/data-sheets/efr32bg13-datasheet.pdf
 */
#define SensorI2cSDA_port gpioPortC
#define SensorI2cSDA_pin  11
#define SensorI2cSDA_location 16



/*
 * Used to Initiate the GPIO Pins of the board
 * Parameter: 	None
 * Return type: VOID
 */
void gpioInit();


/*
 * Turns on LED0
 * Parameter: 	None
 * Return type: VOID
 */
void gpioLed0SetOn();


/*
 * Turns off LED0
 * Parameter: 	None
 * Return type: VOID
 */
void gpioLed0SetOff();

/*
 * Turns on LED1
 * Parameter: 	None
 * Return type: VOID
 */
void gpioLed1SetOn();

/*
 * Turns off the LED1
 * Parameter: 	None
 * Return type: VOID
 */
void gpioLed1SetOff();

/*
 * Turns on the Temperature sensor
 * Parameter: 	None
 * Return type: VOID
 */
void TempSensorSetOn();

/*
 * Turns off the Temperature sensor
 * Parameter: 	None
 * Return type: VOID
 */
void TempSensorSetOff();

/*
 * Disable SDA GPIO
 * Parameter: 	None
 * Return type: VOID
 */
void gpioI2cSDADisable();

/*
 * Disable SCL GPIO
 * Parameter: 	None
 * Return type: VOID
 */
void gpioI2cSCLDisable();

/*
 * Enable Gecko Display
 * Parameter: 	None
 * Return type: VOID
 */
void gpioDisplayEnable();

/*
 * Disable Gecko Display
 * Parameter: 	flag to clear or set pinout
 * Return type: VOID
 */
void gpioSetDisplay(bool flag);

/*
 * Enable Button interrupt
 * Parameter: 	flag to clear or set pinout
 * Return type: VOID
 */
void enable_button_interrupt();

/*
 * Disable Button interrupt
 * Parameter: 	flag to clear or set pinout
 * Return type: VOID
 */
void disable_button_interrupt();


#endif /* SRC_GPIO_H_ */
