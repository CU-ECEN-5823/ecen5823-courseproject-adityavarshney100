/*
 * timers.h
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */

#ifndef SRC_TIMERS_H_
#define SRC_TIMERS_H_

#include "main.h"
#include <em_letimer.h>

#define LETIMER_PERIOD_MS 3000									// This is the time period of the signal in milliseconds
#define CLK_FREQ (LFACLK_FREQ_HZ/PRESCALER)						// This is the Clock frequency which is calculated by dividing the Low Freq Clock (32,768 OR 1000) by the prescaler value (4 or 1)
#define COMP0 ((LETIMER_PERIOD_MS * CLK_FREQ)/1000)				// The first Interrupt which turns on the LED

/*
 * This function is responsible to initiate the LETIMER. It also starts the LETIMEr clock, sets clock divisor/prescaler, also
 * enables the interrupts COMP0
 * Return type: VOID
 * Parameter:	None
*/
void initLETIMER0();

/*
 * Function to generate delay in micro seconds.
 * Return type: VOID
 * Parameter:	delay in microseconds
*/
void TimerWaitUs(uint32_t us_wait);

#endif /* SRC_TIMERS_H_ */
