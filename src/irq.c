/*
 * irq.c
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */

#include "irq.h"

void LETIMER0_IRQHandler()
{
	uint32_t value = LETIMER_IntGet(LETIMER0);		// Get pending LETIMER interrupt flags.
	LETIMER_IntClear(LETIMER0,value);				// Clear one or more pending LETIMER interrupts.

	if(value == (LETIMER_IF_UF | LETIMER_IF_COMP0))
	{
		gpioLed0SetOn();							// When the timer is at Comp0 value, turn on the LED
	}
	else if (value == LETIMER_IF_COMP1)
	{
		gpioLed0SetOff();							// When timer is at Comp1 value, turn off the LED
	}
}
