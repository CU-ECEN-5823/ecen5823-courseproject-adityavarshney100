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
  // DOS: you shouldn't need this, initApp();

  // DOS: trying my code out...
  //SLEEP_Init_t a={0};
  //SLEEP_InitEx(&a);
  SLEEP_Init_t         sleepInitExData;
  memset (&sleepInitExData, 0, sizeof(sleepInitExData));
  SLEEP_InitEx (&sleepInitExData);

  logInit();									// Initialize the log to see Prints on terminal

  // DOS: moved to i2c write and read functions in i2c.c
  //InitI2C();									// Initialize the I2C to start I2C communication

  // DOS: don't call this if you want to sleep to EM3, SLEEP_SleepBlockBegin(sleepEM4);
  uint32_t evt=0;

  while (1)
  {
	  if (!events_present())
	  { 										// no more events
		  logFlush(); 							// flush the LOG before we sleep
		  SLEEP_Sleep(); 						// go to lowest energy level
		  //__WFI();
	  }
	  evt = get_event(); 						// get 1 event
	  process_event(evt); 						// handle events
  }

}
