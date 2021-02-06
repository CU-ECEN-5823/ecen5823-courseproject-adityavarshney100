/*
 * main.c
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */


#include "main.h"

SLEEP_EnergyMode_t energy_Mode_Entered;

int appMain(gecko_configuration_t *config)
{

  gecko_init(config);							// Initialize stack
  gpioInit();									// Initialize the GPIO
  init_oscillators();							// Initialize oscillators
  initLETIMER0();								// Initialize LETIMER0

  while (1)
  {
	  if(ENABLE_SLEEPING ==1)					// It controls whether any sleep modes are enabled or not
		  energy_Mode_Entered=SLEEP_Sleep();	// Sets the system into lowest possible energy mode
  }

}
