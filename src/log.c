/*
 * log.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Dan Walkes
 *
 *      Jan 5, 2021, updates by Dave Sluiter to loggerGetTimestamp()
 */

#include "retargetserial.h"
#include "log.h"
#include <stdbool.h>
#include "timers.h"

#if INCLUDE_LOGGING

extern uint32_t timestamp;
/**
 * @return a timestamp value for the logger, typically based on a free running timer.
 * This will be printed at the beginning of each log message.
 */
uint32_t loggerGetTimestamp(void)
{
    #ifdef MY_USE_SYSTICKS
    
       // Students: Look in the CMSIS library for systick routines. For debugging
       //           purposes this can provide greater resolution than a timestamp based on
       //           LETIMER0. Do not turn in any code that executes systick routines
       //           as this may effect your energy measurements and subsequently your grade.
       
       // Develop this function if you so desire for debugging purposes only
	   return getSysTicks();
	   
    #else
    
       return(get_timestamp());
	   
    #endif
}

/**
 * Initialize logging for Blue Gecko.
 * See https://www.silabs.com/community/wireless/bluetooth/forum.topic.html/how_to_do_uart_loggi-ByI
 */
void logInit(void)
{
	RETARGET_SerialInit();
	/**
	 * See https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__RetargetIo.html#ga9e36c68713259dd181ef349430ba0096
	 * RETARGET_SerialCrLf() ensures each linefeed also includes carriage return.  Without it, the first character is shifted in TeraTerm
	 */
	RETARGET_SerialCrLf(true);
	LOG_INFO("Initialized Logging");
}

/**
 * Block for chars to be flushed out of the serial port.  Important to do this before entering SLEEP() or you may see garbage chars output.
 */
void logFlush(void)
{
	RETARGET_SerialFlush();
}
#endif
