/*
 * scheduler.h
 *
 *  Created on: 12-Feb-2021
 *      Author: aditya.vny95
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <em_core.h>
#include "main.h"

#define ON 1							// Mode to turn on the identifier for the event
#define OFF 0							// Mode to turn off the identifier for the event
#define POWERUP_TIME_US 80000			// Time taken by the Temperature sensor to power up
#define CONVERSION_TIME_US 10800		// Time taken by by the sensor to convert data


/*
 * Schedule routine to sea a scheduler event, must be called from a critical section
 * Return type: VOID
 * Parameter:	None
*/
void schedulerSetEventTemp();

/*
 * Scheduler routine to return 1 event to main()
 * Returns: the event
 * Parameter:	None
*/
uint32_t get_event();


/*
 * Returns true if event present, false if not.
 * Returns: TRUE or False
 * Parameter: None
*/
bool events_present();


/*
 * Process event occurred during interrupt service routine
 * Returns: the event
 * Parameter: Event to be processed
*/
void process_event(uint32_t event);



#endif /* SRC_SCHEDULER_H_ */
