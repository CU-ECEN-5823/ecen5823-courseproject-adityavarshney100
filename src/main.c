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
  SLEEP_Init_t a={0};
  SLEEP_InitEx(&a);
  logInit();									// Initialize the log to see Prints on terminal
  InitI2C();									// Initialize the I2C to start I2C communication
  SLEEP_SleepBlockBegin(sleepEM4);
  uint32_t evt=0;

  while (1)
  {
	  if (!events_present())
	  { 										// no more events
		  logFlush(); 							// flush the LOG before we sleep
		  SLEEP_Sleep(); 						// go to lowest energy level
	  }
	  evt = get_event(); 						// get 1 event
	  process_event(evt); 						// handle events
  }

}
