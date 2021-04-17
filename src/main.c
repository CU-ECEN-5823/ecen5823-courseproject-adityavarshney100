/*
 * main.c
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */


#include "main.h"
#include "init_app.h"
#include "log.h"
#include "em_core.h"
#include "udelay.h"
SLEEP_EnergyMode_t energy_Mode_Entered;

int appMain(gecko_configuration_t *config)
{
	gecko_init(config);							// Initialize stack
	gpioInit();									// Initialize the GPIO
	init_oscillators();							// Initialize oscillators
	initLETIMER0();								// Initialize LETIMER0
	initApp();
	SLEEP_Init_t sleepInitExData;
	memset (&sleepInitExData, 0, sizeof(sleepInitExData));
	SLEEP_InitEx (&sleepInitExData);
	logInit();									// Initialize the log to see Prints on terminal
	enable_button_interrupt();
	SLEEP_SleepBlockBegin(sleepEM3);
	displayInit();

	InitI2C();
	int i=0;
	//Created a while just for the testing of i2c, will remove this in the final project
	while(1)
	{
		writeI2C();
		for(i=0;i<1000;i++)		//Delay of 250ms, suggested in the datasheet, without this delay I2C does not work
			UDELAY_Delay(250);

		readI2C();
		for(i=0;i<1000;i++)
			UDELAY_Delay(250);
	}


	//Commenting this part to avoid unnecessary confusion and focusing just on I2C
/*
	while (1)
	{
		struct gecko_cmd_packet* evt;
		if (!gecko_event_pending())
		{ 										// no more events
			logFlush(); 							// flush the LOG before we sleep
		}
		evt = gecko_wait_event(); 					// get 1 event
		handler_ble_event(evt);					// Handle Bluetooth stack events
		if(BGLIB_MSG_ID(evt->header) == gecko_evt_system_external_signal_id)
		{
			process_event(evt); 						// handle events
		}
	}
*/
}
