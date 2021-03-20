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

}
