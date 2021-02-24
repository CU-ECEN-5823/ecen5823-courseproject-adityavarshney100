/*
 * scheduler.c
 *
 *  Created on: 12-Feb-2021
 *      Author: aditya.vny95
 */
#include "scheduler.h"
volatile int temp_event;
State_t current_state;
State_t nextState = STATE0_TIMER_WAIT;

uint32_t measure_temp = OFF;				// Identifier to set the event


void process_event(uint32_t event)
{
	current_state = nextState;
	switch(current_state)
	{
	case STATE0_TIMER_WAIT:
		//LOG_INFO("0\n");
		nextState = STATE0_TIMER_WAIT;
		if (event == WAIT_FOR_POWER_UP)
		{
			#ifdef Debug
			LOG_INFO("0");
			#endif
			TempSensorSetOn();						// Turn on the temperature sensor to collect the readings
			TimerWaitUs(POWERUP_TIME_US);			// Wait for the power up time
			nextState=STATE1_WARMUP;
		}
	break;

	case STATE1_WARMUP:
		//LOG_INFO("1");
		nextState = STATE1_WARMUP;
		if (event == LETIMER0_COMP1)
		{
			#ifdef Debug
			LOG_INFO("1");
			#endif
			//DOS don't need this, SLEEP_SleepBlockEnd(sleepEM4);
			SLEEP_SleepBlockBegin(sleepEM2);

			// DOS: Turn on I2C clock in CMU
			CMU_ClockEnable (cmuClock_I2C0, true);  // enable=true
			writeI2C();								// Perform I2C write

			//DOS while(1);
			nextState=STATE2_TEMP_WAIT;
		}
	break;

	case STATE2_TEMP_WAIT:
		//LOG_INFO("2\n");
		nextState = STATE2_TEMP_WAIT;
		if (event == I2C_TRANSFER_DONE)
		{
			#ifdef Debug
			LOG_INFO("2");
			#endif
			SLEEP_SleepBlockEnd(sleepEM2);
			TimerWaitUs(CONVERSION_TIME_US);		// Wait for the conversion time
			nextState=STATE3_READ_WAIT;
		    //DOS while(1);
		}
	break;

	case STATE3_READ_WAIT:
		nextState = STATE3_READ_WAIT;
		//LOG_INFO("3");
		if (event == LETIMER0_COMP1)
		{
			#ifdef Debug
			LOG_INFO("3");
			#endif
			SLEEP_SleepBlockBegin(sleepEM2);
			readI2C();								// Perform I2C read
			nextState=STATE4_REPORT;
		}
	break;

	case STATE4_REPORT:
		//LOG_INFO("4\n");
		nextState = STATE4_REPORT;
		if (event == I2C_TRANSFER_DONE)
		{
			SLEEP_SleepBlockEnd(sleepEM2);
			//DOS don't need this, SLEEP_SleepBlockBegin(sleepEM4);
			#ifdef Debug
			LOG_INFO("4");
			#endif
			Temperature();                          // print temp in C
			TempSensorSetOff();						// Turn the sensor off

			// DOS: save more energy
			CMU_ClockEnable (cmuClock_I2C0, false);  // enable=false
			// disable the GPIOs for SCL/SDA - these were enabled by I2CSPM_Init()
			gpioI2cSDADisable();
			gpioI2cSCLDisable();

			nextState=STATE0_TIMER_WAIT;
		}
	break;
	}
}

void SetEventTempWaitPowerUP()
{
	temp_event |= WAIT_FOR_POWER_UP;
}

void SetEventComp1()
{
	temp_event |= LETIMER0_COMP1;
}

void SetEventI2CTransferDone()
{
	temp_event |= I2C_TRANSFER_DONE;
}

uint32_t get_event()
{
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();					// Enter critical section
//	if(temp_event & POWER_OFF)
//	{
//		temp_event ^= POWER_OFF;
//		CORE_EXIT_CRITICAL();
//		return(POWER_OFF);
//	}
//	else if(temp_event & WAIT_FOR_POWER_UP)
	if(temp_event & WAIT_FOR_POWER_UP)
	{
		temp_event^=WAIT_FOR_POWER_UP;
		CORE_EXIT_CRITICAL();
		return(WAIT_FOR_POWER_UP);
	}
	else if(temp_event & LETIMER0_COMP1)
	{
		temp_event^=LETIMER0_COMP1;
		CORE_EXIT_CRITICAL();
		return(LETIMER0_COMP1);
	}
	else if(temp_event & I2C_TRANSFER_DONE)
	{
		temp_event^=I2C_TRANSFER_DONE;
		CORE_EXIT_CRITICAL();
		return(I2C_TRANSFER_DONE);
	}
	CORE_EXIT_CRITICAL();					// Exit critical section
	return 0;
}

bool events_present()
{
	if(temp_event == 0)
		return false;
	else
		return true;

}
