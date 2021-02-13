/*
 * scheduler.c
 *
 *  Created on: 12-Feb-2021
 *      Author: aditya.vny95
 */
#include "scheduler.h"


uint32_t measure_temp = OFF;				// Identifier to set the event

void schedulerSetEventTemp()				// Schedule routine to sea a scheduler event, must be called from a critical section
{
	measure_temp=ON;						// setting the identifier as per the event
}

uint32_t get_event()
{
	uint32_t get_temp;
	CORE_DECLARE_IRQ_STATE;
	get_temp=measure_temp;
	CORE_ENTER_CRITICAL();					// Enter critical section
	measure_temp=OFF;						// Clear/Reset the event
	CORE_EXIT_CRITICAL();					// Exit critical section
	return get_temp;
}

bool events_present()
{
	if(get_event() == 0)
		return false;
	else
		return true;

}

void process_event(uint32_t event)
{
	switch(event)
	{
	case ON:
	TempSensorSetOn();						// Turn on the temperature sensor to collect the readings
	TimerWaitUs(POWERUP_TIME_US);			// Wait for the power up time
	writeI2C();								// Perform I2C write
	TimerWaitUs(CONVERSION_TIME_US);		// Wait for the conversion time
	readI2C();								// Perform I2C read
	TempSensorSetOff();						// Turn the sensor off
	break;

	default:
		printf("Incorrect event");
	}
}

