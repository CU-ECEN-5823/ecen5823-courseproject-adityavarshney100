/*
 * irq.h
 *
 *  Created on: 05-Feb-2021
 *      Author: aditya.vny95
 */

#ifndef SRC_IRQ_H_
#define SRC_IRQ_H_

#include"main.h"

/* This handler checks for pending LETIMER interrupt flags, clears them and turns on the LED as per the criteria specified
 * Return Type:		void
 * Parameter:		None
*/
void LETIMER0_IRQHandler();

#endif /* SRC_IRQ_H_ */
