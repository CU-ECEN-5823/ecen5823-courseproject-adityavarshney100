/*
 * scheduler.c
 *
 *  Created on: 12-Feb-2021
 *  Author: Aditya Varshney and Pankaj Solanki
 */
#include "scheduler.h"
volatile int temp_event;
State_t current_state;
State_t nextState = STATE0_TIMER_WAIT;

uint32_t measure_temp = OFF;				// Identifier to set the event
uint16_t curr_voc_value=0;

void send_values()
{
	uint8_t buffer[4];
	uint8_t *voc_ptr = buffer;
	uint8_t relay_value;

	UINT32_TO_BITSTREAM(voc_ptr,curr_voc_value);
	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF,gattdb_voc_value, 4, buffer));

	if(curr_voc_value>GOOD_AIR_QUALITY_THRES)
		relay_value=1;
	else
		relay_value=0;

	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_write_attribute_value(gattdb_relay_state,0,1,&relay_value));
	BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF,gattdb_relay_state,1,&relay_value));
	displayPrintf(DISPLAY_ROW_TEMPVALUE,"Relay = %d",relay_value);
}
void process_event(struct gecko_cmd_packet *event)
{
	current_state = nextState;
	switch(current_state)
	{
	case STATE0_TIMER_WAIT:
		nextState = STATE0_TIMER_WAIT;
		LOG_INFO("STATE0_TIMER_WAIT");
		if ((event->data.evt_system_external_signal.extsignals & WAIT_FOR_POWER_UP) != 0)
		{
			TimerWaitUs(POWERUP_TIME_US);			// Wait for the power up time
			nextState=STATE1_WARMUP;
		}
		break;

	case STATE1_WARMUP:
		LOG_INFO("STATE1_WARMUP");
		nextState = STATE2_TEMP_WAIT;
		if ((event->data.evt_system_external_signal.extsignals & LETIMER0_COMP1) != 0)
		{
			SLEEP_SleepBlockBegin(sleepEM2);
			CMU_ClockEnable (cmuClock_I2C0, true);  // enable=true
			writeI2C();								// Perform I2C write
			nextState=STATE2_TEMP_WAIT;
		}
		break;

	case STATE2_TEMP_WAIT:
		nextState = STATE3_READ_WAIT;
		LOG_INFO("STATE2_TEMP_WAIT");
		if ((event->data.evt_system_external_signal.extsignals & I2C_TRANSFER_DONE) !=0)
		{
			SLEEP_SleepBlockEnd(sleepEM2);
			TimerWaitUs(CONVERSION_TIME_US);		// Wait for the conversion time
			nextState=STATE3_READ_WAIT;
		}
		break;

	case STATE3_READ_WAIT:
		LOG_INFO("STATE3_READ_WAIT");
		nextState = STATE3_READ_WAIT;
		if ((event->data.evt_system_external_signal.extsignals & LETIMER0_COMP1) != 0)
		{
			SLEEP_SleepBlockBegin(sleepEM2);
			if(DEVICE_IS_BLE_SERVER == 1)
				readI2C();								// Perform I2C read
			nextState=STATE4_REPORT;
		}
		break;

	case STATE4_REPORT:
		nextState = STATE4_REPORT;
		LOG_INFO("STATE4_REPORT\n\n");
		if ((event->data.evt_system_external_signal.extsignals & I2C_TRANSFER_DONE) !=0)
		{
			SLEEP_SleepBlockEnd(sleepEM2);
			CMU_ClockEnable (cmuClock_I2C0, false);  // enable=false
			send_values();
			// disable the GPIOs for SCL/SDA - these were enabled by I2CSPM_Init()
			//gpioI2cSDADisable();
			//gpioI2cSCLDisable();
			//DisableI2C();
			nextState=STATE0_TIMER_WAIT;
		}
		break;
	}
}

void SetEventTempWaitPowerUP()
{
	gecko_external_signal(WAIT_FOR_POWER_UP);
}

void SetEventComp1()
{
	gecko_external_signal(LETIMER0_COMP1);
}

void SetEventI2CTransferDone()
{
	gecko_external_signal(I2C_TRANSFER_DONE);
}

void schedulerSetEventButtonPress(void)
{
	gecko_external_signal(EVT_BUTTON_PRESS);
}

uint32_t get_event()
{
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();					// Enter critical section
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
