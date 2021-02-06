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

#define	LED0_port  gpioPortF
#define LED0_pin   4
#define LED1_port  gpioPortF
#define LED1_pin   5


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

#endif /* SRC_GPIO_H_ */
