
/*
 * main.c
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */

#ifndef __myMAIN
#define __myMAIN


#include "gecko_configuration.h"
#include "native_gecko.h"
#include "gpio.h"
#include "em_cmu.h"
#include "sleep.h"
#include "timers.h"
#include "oscillators.h"
#include "em_letimer.h"
#include "irq.h"

#define LFXO_FREQ 32768											// LFXO frequency for EM0, EM1, EM2 is 32.768 kHz
#define ULFRCO_FREQ 1000										// ULFRCO frequency for EM3 and EM4 is 1kHz
#define ENABLE_SLEEPING 1										// It controls whether any sleep modes are enabled or not
#define LOWEST_ENERGY_MODE	0									// Defines which Energy Mode is the Board working in

#if LOWEST_ENERGY_MODE==0
	#define LFACLK_FREQ_HZ			LFXO_FREQ					// Defining the frequency for EMO as LFXO
	#define PRESCALER 				(cmuClkDiv_4)				// Defining the prescaler for EMO as 4
	#define SLEEP_SleepBlockBegin(sleepEM1)						// Block Energy modes equal to and greater than EM1
#endif

#if LOWEST_ENERGY_MODE==1
	#define LFACLK_FREQ_HZ			LFXO_FREQ					// Defining the frequency for EM1 as LFXO
	#define PRESCALER 				(cmuClkDiv_4)				// Defining the prescaler for EM1 as 4
	#define SLEEP_SleepBlockBegin(sleepEM2)						// Block Energy modes equal to and greater than EM2
#endif

#if LOWEST_ENERGY_MODE==2
	#define LFACLK_FREQ_HZ			LFXO_FREQ					// Defining the frequency for EM2 as LFXO
	#define PRESCALER				(cmuClkDiv_4)				// Defining the prescaler for EM2 as 4
	#define SLEEP_SleepBlockBegin(sleepEM3)						// Block Energy modes equal to and greater than EM3
#endif

#if LOWEST_ENERGY_MODE==3
	#define LFACLK_FREQ_HZ			ULFRCO_FREQ					// Defining the frequency for EM3 as ULFRCO
	#define PRESCALER 				(cmuClkDiv_1)				// Defining the prescaler for EM3 as 1
	#define SLEEP_SleepBlockBegin(sleepEM4)						// Block Energy modes equal to and greater than EM4
#endif


/*
 * The main function that initializes all the necessary peripherals on the board
 * Return Type: Integer
 * Parameters: 	Object
 */
int appMain(gecko_configuration_t *config);

#endif

