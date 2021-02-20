/*
 * timers.c
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */


#include "timers.h"

void initLETIMER0()
{

	CMU_ClockEnable(cmuClock_LFA,1);						// Enable LFA
	CMU_ClockEnable(cmuClock_LETIMER0,1);					// Enable LEtimer 0 clock
	if(LOWEST_ENERGY_MODE <3)
	{
		CMU_ClockDivSet(cmuClock_LETIMER0,cmuClkDiv_4);		// Set clock divisor/prescaler.
	}
	else
	{
		CMU_ClockDivSet(cmuClock_LETIMER0,cmuClkDiv_1);		// Set clock divisor/prescaler.
	}

	LETIMER_Init_TypeDef init=								// LETIMER initialization structure.
	{
			.enable = false,								// Start counting when init completed.
			.debugRun = false,								// Counter shall keep running during debug halt.
			.comp0Top = true,								// Load Comp0 register into CNT when counter underflows
			.bufTop = false,								// Load COMP1 into COMP0 when REP0 reaches 0.
			.out0Pol = 0,									// Idle value for output 0.
			.out1Pol = 0,									// Idle value for output 1.
			.ufoa0 = letimerUFOANone,						// Underflow output 0 action.
			.ufoa1 = letimerUFOANone,						// Underflow output 1 action.
			.repMode = letimerRepeatFree					// Repeat mode.
	};

	LETIMER_Init(LETIMER0,&init);							// Initializing LEtimer
	LETIMER_CompareSet(LETIMER0,0,COMP0);					// Set value of COMP0
	LETIMER_CompareSet(LETIMER0,1,1500);					// Set value of COMP1
	LETIMER_IntEnable(LETIMER0,LETIMER_IF_COMP0);			// Enable interrupt of COMP0
	NVIC_EnableIRQ(LETIMER0_IRQn);							// Configure NVIC for LETIMER
	LETIMER_Enable(LETIMER0,1);								// Enable LETIMER

}

void TimerWaitUs(uint32_t delay_us)
{
	uint32_t current_ticks,ticker_max;
	uint32_t ticker_us;

	if(delay_us > (LETIMER_PERIOD_MS*1000))					// Condition to check if the delay time can be handled
	{
		LOG_ERROR("The delay is greater than the vale which can be handled by LETIMER");
		delay_us=3000000;									// Maximum possible delay
	}

	ticker_us=CLK_FREQ * delay_us;
	ticker_us=ticker_us/1000000;							//Calculate the ticks required in us

	current_ticks=LETIMER_CounterGet(LETIMER0); 			//Get the present value of timer count

	ticker_max=current_ticks-ticker_us;
	if(ticker_max>COMP0)									// Case when timer overflows
	{
		ticker_us=65535-ticker_max;
		ticker_max=COMP0-ticker_us;
	}

	LETIMER_CompareSet(LETIMER0,1,ticker_max);
	LETIMER_IntEnable(LETIMER0,LETIMER_IF_COMP1);			// Enable interrupt for COMP1

}

uint32_t get_timestamp()
{
	return timestamp;
}
