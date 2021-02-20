/*
 * irq.c
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */

#include "irq.h"
#include <time.h>
extern I2C_TransferReturn_TypeDef transferstatus;
uint32_t timestamp;
int rollover;




void I2C0_IRQHandler()
{
	LOG_INFO("in IRQ\n");
	transferstatus=I2C_Transfer(I2C0);						// TO check if the transfer has successfully happened
	if(transferstatus == i2cTransferDone)
	{
		CORE_DECLARE_IRQ_STATE;
		CORE_ENTER_CRITICAL();								// Enter the critical mode
		SetEventI2CTransferDone();					// Set the identifier for the event
		CORE_EXIT_CRITICAL();								// Exit the critical mode
	}
}



void LETIMER0_IRQHandler()
{
	CORE_DECLARE_IRQ_STATE;
	uint32_t value = LETIMER_IntGet(LETIMER0);								// Get pending LETIMER interrupt flags.
	LETIMER_IntClear(LETIMER0,value);										// Clear one or more pending LETIMER interrupts.
	if(value & LETIMER_IF_COMP0)											// Condition to check and let the routine through
	{
		rollover++;
		CORE_ENTER_CRITICAL();												// Enter the critical mode
		SetEventTempWaitPowerUP();											// Set the identifier for the event
		CORE_EXIT_CRITICAL();												// Exit the critical mode
		uint32_t n = (rollover*COMP0)-LETIMER_CounterGet(LETIMER0);			// Get timer count
		timestamp=(n*1000)/CLK_FREQ;										// Return Timestamp in ms
	}
	if(value & LETIMER_IF_COMP1)
	{
		CORE_ENTER_CRITICAL();
		SetEventComp1();
		CORE_EXIT_CRITICAL();
		LETIMER_IntDisable(LETIMER0,LETIMER_IF_COMP1);
	}
}

