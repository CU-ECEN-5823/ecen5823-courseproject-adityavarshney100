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

	GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInput, true);
	GPIO_PinModeSet(gpioPortC,10,gpioModeWiredAndPullUpFilter,1);
	GPIO_PinModeSet(gpioPortC,11,gpioModeWiredAndPullUpFilter,1);
	GPIO_PinModeSet(Relay_port,Relay_pin,gpioModePushPull,false);
}

void gpioRelayOn()
{
	GPIO_PinOutSet(Relay_port,Relay_pin);
}

void gpioRelayOff()
{
	GPIO_PinOutClear(Relay_port,Relay_pin);
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
	GPIO_ExtIntConfig(PB1_port, PB1_pin, PB1_pin, true, true, true);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

void disable_button_interrupt()
{
	NVIC_DisableIRQ(GPIO_EVEN_IRQn);
	NVIC_DisableIRQ(GPIO_ODD_IRQn);
}
