
/*
 * main.c
 *
 *  Created on: 05-Feb-2021
 *  Author: Aditya Varshney and Pankaj Solanki
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
#include "scheduler.h"
#include "i2c.h"
#include "ble.h"
#include "display.h"

#define LFXO_FREQ 32768											// LFXO frequency for EM0, EM1, EM2 is 32.768 kHz
#define ULFRCO_FREQ 1000										// ULFRCO frequency for EM3 and EM4 is 1kHz
#define ENABLE_SLEEPING 1										// It controls whether any sleep modes are enabled or not
#define LOWEST_ENERGY_MODE	3									// Defines which Energy Mode is the Board working in

#if LOWEST_ENERGY_MODE==0
	#define LFACLK_FREQ_HZ			LFXO_FREQ					// Defining the frequency for EMO as LFXO
	#define PRESCALER 				(cmuClkDiv_4)				// Defining the prescaler for EMO as 4
#endif

#if LOWEST_ENERGY_MODE==1
	#define LFACLK_FREQ_HZ			LFXO_FREQ					// Defining the frequency for EM1 as LFXO
	#define PRESCALER 				(cmuClkDiv_4)				// Defining the prescaler for EM1 as 4
#endif

#if LOWEST_ENERGY_MODE==2
	#define LFACLK_FREQ_HZ			LFXO_FREQ					// Defining the frequency for EM2 as LFXO
	#define PRESCALER				(cmuClkDiv_4)				// Defining the prescaler for EM2 as 4
#endif

#if LOWEST_ENERGY_MODE==3
	#define LFACLK_FREQ_HZ			ULFRCO_FREQ					// Defining the frequency for EM3 as ULFRCO
	#define PRESCALER 				(cmuClkDiv_1)				// Defining the prescaler for EM3 as 1
#endif


/*
 * The main function that initializes all the necessary peripherals on the board
 * Return Type: Integer
 * Parameters: 	Object
 */
int appMain(gecko_configuration_t *config);

#endif

