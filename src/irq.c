/*
 * irq.c
 *
 *  Created on: 05-Feb-2021
 *  Author: Aditya Varshney and Pankaj Solanki
 */

#include "irq.h"
#include <time.h>
extern I2C_TransferReturn_TypeDef transferstatus;
uint32_t timestamp = 0;
int      rollover  = 0;
uint16_t button0_state;
uint16_t button1_state;
uint8_t flag=0;
uint8_t flag1;


void I2C0_IRQHandler()
{
	transferstatus=I2C_Transfer(I2C0);						// TO check if the transfer has successfully happened

	if(transferstatus == i2cTransferDone)
	{
		flag=1;
		CORE_DECLARE_IRQ_STATE;
		CORE_ENTER_CRITICAL();								// Enter the critical mode
		SetEventI2CTransferDone();							// Set the identifier for the event
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
		rollover++; // increment rollover counter
		CORE_ENTER_CRITICAL();												// Enter the critical mode
		SetEventTempWaitPowerUP();											// Set the identifier for the event
		CORE_EXIT_CRITICAL();												// Exit the critical mode
		timestamp = timestamp + COMP0; //
	}
	if(value & LETIMER_IF_COMP1)
	{
		CORE_ENTER_CRITICAL();
		SetEventComp1();
		CORE_EXIT_CRITICAL();
		LETIMER_IntDisable(LETIMER0,LETIMER_IF_COMP1);
	}
}

void GPIO_EVEN_IRQHandler()
{
	GPIO_IntClear(1 << PB0_pin);
	if(GPIO_PinInGet(PB0_port, PB0_pin)==1)
	{
		button0_state = 0x00;
	}
	else
	{
		button0_state = Button0_Pressed;
	}
	schedulerSetEventButtonPress();
}

void GPIO_ODD_IRQHandler()
{
	GPIO_IntClear(1 << PB1_pin);
		if(GPIO_PinInGet(PB1_port, PB1_pin)==1)
		{
			button1_state = 0x00;
		}
		else
		{
			button1_state = Button1_Pressed;
		}
		schedulerSetEventButtonPress();
}
