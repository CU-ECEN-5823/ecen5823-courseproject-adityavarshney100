/*
 * scheduler.h
 *
 *  Created on: 12-Feb-2021
 *  Author: Aditya Varshney and Pankaj Solanki
 */

#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <em_core.h>
#include "main.h"
#include "i2c.h"
#include "gpio.h"


typedef enum State
{
	STATE0_TIMER_WAIT,
	STATE1_WARMUP,
	STATE2_TEMP_WAIT,
	STATE3_READ_WAIT,
	STATE4_REPORT,
	//MY_NUM_STATES
}State_t;

typedef enum uint32_t
{
	POWER_OFF			= 0X00,
	WAIT_FOR_POWER_UP	= 0X01,
	LETIMER0_COMP1		= 0X02,
	I2C_TRANSFER_DONE	= 0X04,
	EVT_BUTTON_PRESS	= 0x10
} Temp_Event_t;

#define ON 1							// Mode to turn on the identifier for the event
#define OFF 0							// Mode to turn off the identifier for the event
#define POWERUP_TIME_US 1000			// Time taken by the air quality sensor to power up, as per the datasheet it is 0.6ms but keeping it 1ms to be safe
#define CONVERSION_TIME_US 250000		// Time taken by by the sensor to convert data
#define GOOD_AIR_QUALITY_THRES 200
#define MAPPING_VALUE	131				//Mapping 0-65535 to 0-500

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
void process_event(struct gecko_cmd_packet *event);

/*
 * Set event for power up
 * Returns: none
 * Parameter: none
*/
void SetEventTempWaitPowerUP();

/*
 * Set event for COMP1
 * Returns: none
 * Parameter: none
*/
void SetEventComp1();

/*
 * Set event for I2C transfer
 * Returns: none
 * Parameter: none
*/
void SetEventI2CTransferDone();

/*
 * Set event for button press
 * Returns: none
 * Parameter: none
*/
void schedulerSetEventButtonPress(void);


/*
 * Based on the VOC value, decided the state of the relay
 * and sends both the values (VOC and Relay_state)
 * Returns: none
 * Parameter: none
 */
void send_values();

#endif /* SRC_SCHEDULER_H_ */
