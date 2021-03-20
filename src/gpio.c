/*
  gpio.c
 
   Created on: Dec 12, 2018
       Author: Dan Walkes
   Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.
 */



// Students see file: gpio.h for instructions
#include "gpio.h"






void gpioInit()
{
	GPIO_DriveStrengthSet(Temperature_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(Temperature_port, Temperature_pin, gpioModePushPull, false);
	GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInput, true);
}

void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}

void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}

void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}

void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

void TempSensorSetOn()
{
	GPIO_PinOutSet(Temperature_port,Temperature_pin);
}

void TempSensorSetOff()
{
	GPIO_PinOutClear(Temperature_port,Temperature_pin);
}

void gpioI2cSDADisable()
{
	GPIO_PinModeSet(SensorI2cSDA_port, SensorI2cSDA_pin, gpioModeDisabled, 0);
}

void gpioI2cSCLDisable()
{
	GPIO_PinModeSet(SensorI2cSCL_port, SensorI2cSCL_pin, gpioModeDisabled, 0);
}

void gpioDisplayEnable()
{
	GPIO_DriveStrengthSet(Display_port, Display_pin);
}

void gpioSetDisplay(bool flag)
{
	if(flag)
	{
		GPIO_PinOutSet(Display_port, Display_pin);
	}
	else if(!flag)
	{
		GPIO_PinOutClear(Display_port, Display_pin);
	}
}

void enable_button_interrupt()
{
	GPIO_ExtIntConfig(PB0_port, PB0_pin, PB0_pin, true, true, true);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

void disable_button_interrupt()
{
	NVIC_DisableIRQ(GPIO_EVEN_IRQn);
}
