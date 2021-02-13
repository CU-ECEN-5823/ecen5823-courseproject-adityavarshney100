/*
 * irq.c
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */

#include "irq.h"
#include <stdio.h>
#include <time.h>

void LETIMER0_IRQHandler()
{
	uint32_t value = LETIMER_IntGet(LETIMER0);								// Get pending LETIMER interrupt flags.
	LETIMER_IntClear(LETIMER0,value);										// Clear one or more pending LETIMER interrupts.
	if(value == (LETIMER_IF_UF | LETIMER_IF_COMP0 | LETIMER_IF_COMP1))		// Condition to check and let the routine through
	{
		CORE_DECLARE_IRQ_STATE;
		CORE_ENTER_CRITICAL();												// Enter the critical mode
		schedulerSetEventTemp();											// Set the identifier for the event
		CORE_EXIT_CRITICAL();												// Exit the critical mode
	}

}
