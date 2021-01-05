/*

Add your header comment here, name, date, etc. See the Coding style guideline document

*/


#include "main.h"


static void delayApproxOneSecond(void)
{
	/**
	 * Wait loops are a bad idea in general!  Don't copy this code in future assignments!
	 * We'll discuss how to do this a better way in the next assignment.
	 */
	volatile int i;
	for (i = 0; i < 3500000; ) {
		  i=i+1;
	}

} // delayApproxOneSecond()


int appMain(gecko_configuration_t *config)
{


  // Initialize stack
  gecko_init(config);

  // Students:
  // add a function call to gpioInit() here.




  /* Infinite loop */
  while (1) {

    // For assignment 1 we want a 50-50 duty cycle:
    // LED(s) on for ~1 sec, LED(s) off ~1 sec.
	delayApproxOneSecond();
	gpioLed0SetOn();
	//gpioLed1SetOn();


	delayApproxOneSecond();
	gpioLed0SetOff();
	//gpioLed1SetOff();


  } // while(1)

} // appMain()
