/*
 * oscillators.c
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */


#include "oscillators.h"

void init_oscillators()
{
if (LOWEST_ENERGY_MODE < 3)								// Enable LFXO oscillator
{
	CMU_OscillatorEnable(cmuOsc_LFXO, true, true);		// Enable oscillator.
	CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_LFXO);	// Select reference clock/oscillator used for a clock branch.
}
else													// Enable ULFRCO oscillator
{
	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);	// Enable oscillator.
	CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_ULFRCO);	// Select reference clock/oscillator used for a clock branch.
}

}
