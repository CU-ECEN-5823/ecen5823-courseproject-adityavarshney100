/*
 * display.h
 *
 *  Created on: Jan 1, 2019
 *      Author: Dan Walkes
 *      Edited: Dave Sluiter, Dec 1 2020
 *
 * Use these steps to integrate the display module with your source code:
 *
 * 3 edits are required to display.c, see the header comment at the top of display.c
 *
 * 1) Add a BT Stack timer which can provide a 1Hz update for the display EXTCOMIN pin
 *  	through a call to displayUpdate().  Include needed header files in the top of
 *  	display.c.  #define these values in appropriate header files:
 *  	#define SCHEDULER_SUPPORTS_DISPLAY_UPDATE_EVENT 1
 *  	#define TIMER_SUPPORTS_1HZ_TIMER_EVENT	1
 *
 * 2) Create functions gpioI2CSensorEnSetOn() and gpioSetDisplayExtcomin(bool value) in
 *      your gpio.c and gpio.h files, and include.
 *
 * 		#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	1
 * 		and
 *		#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		1
 *		definitions in your gpio.h file
 *
 *		** Note that the Blue Gecko development board uses the same pin for both the sensor and display enable
 *		pins.  This means you cannot disable the temperature sensor for load power management if enabling the display.  
 *		Your GPIO routines need to account for this **
 *
 * 3) Call displayInit() before attempting to write the display. A good place to call this
 *      would be your initialization code prior to your main while (1) loop,
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#define ECEN5823_INCLUDE_DISPLAY_SUPPORT 1

#include "glib.h"
#include "main.h"
#include "native_gecko.h"

#include "graphics.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "glib.h"
#include "gpio.h"
#include "log.h"
#include "hardware/kit/common/drivers/display.h"


// and for gpio
#include "gpio.h"

#include "ble_device_type.h"





/**
 * Display row definitions, used for writing specific content based on assignment requirements.
 * See assignment text for details.
 */
enum display_row {
	DISPLAY_ROW_NAME,
	DISPLAY_ROW_BTADDR,
	DISPLAY_ROW_BTADDR2,
	DISPLAY_ROW_CLIENTADDR,
	DISPLAY_ROW_CONNECTION,
	DISPLAY_ROW_PASSKEY,
	DISPLAY_ROW_ACTION,
	DISPLAY_ROW_TEMPVALUE,
	DISPLAY_ROW_MAX,
};


// function prototypes
#if ECEN5823_INCLUDE_DISPLAY_SUPPORT
void displayInit();
void displayUpdate();
void displayPrintf(enum display_row row, const char *format, ... );
#else
static inline void displayInit() { }
static inline void displayUpdate() { return true; }
static inline void displayPrintf(enum display_row row, const char *format, ... ) { row=row; format=format;}
#endif



#endif /* SRC_DISPLAY_H_ */
