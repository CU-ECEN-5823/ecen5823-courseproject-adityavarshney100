/*
 * i2c.c
 *
 *  Created on: 12-Feb-2021
 *      Author: aditya.vny95
 */

#include "i2c.h"

uint8_t write_command[1]={0xF3};			// Temperature measurement command
uint8_t read_buffer[3]={0};					    // The value read from the sensor is stored here
uint16_t result;							// The value that is received from the sensor is 16 bit
float temperature;							// Measurement of the final temperature
I2C_TransferReturn_TypeDef transferstatus;
I2C_TransferSeq_TypeDef data;

//Air quality sensor values
uint8_t sgp_write[8]={0x26,0x0F,0x80,0x00,0xA2,0x66,0x66,0x93};		//command for raw value
uint8_t sgp_write2[2]={0x28,0x0E};									//command for self-test
#define device_addr (0x59)
extern uint16_t curr_voc_value;

extern bool BT_connection;
void InitI2C()
{
	I2CSPM_Init_TypeDef init;
	init.port = I2C0;						// Use I2C Instance 0
	init.sclPort=gpioPortC;					// SCL Port
	init.sclPin=10;							// SCL Pin
	init.sdaPort=gpioPortC;					// SDA Port
	init.sdaPin=11;							// SDA Pin
	init.portLocationScl=14;				// Location of SCL
	init.portLocationSda=16;				// Location of SDA
	init.i2cRefFreq=0;						// Using reference clock currently configured
	init.i2cMaxFreq=I2C_FREQ_STANDARD_MAX;	// Standard rate setting
	init.i2cClhr=i2cClockHLRStandard;		// Set to use 4:4 low/high duty cycle
	I2CSPM_Init(&init);
	NVIC_EnableIRQ(I2C0_IRQn);
}


void writeI2C()
{
	data.addr = device_addr<<1;				// Perform a write to device at I2C address 0x80
	data.flags = I2C_FLAG_WRITE;
	data.buf[0].data = &sgp_write[0];			// Write command for temperature measurement
	data.buf[0].len = 8;						// Length of write command
	transferstatus=I2C_TransferInit(I2C0, &data);

	LOG_INFO("Write command transfer_status=%d \t",transferstatus);

	//while loop to ensure, transfer is completed
	while (transferstatus == i2cTransferInProgress)
	{
		transferstatus = I2C_Transfer(I2C0);
	}
}

//Reference: Datasheet of SGP40
//Link: https://cdn.sparkfun.com/assets/e/6/2/6/d/Sensirion_Gas_Sensors_SGP40_Datasheet.pdf
//Calculates the CRC values based on the previously received bytes
uint8_t CalcCrc(uint8_t data[2])
{
	uint8_t crc = 0xFF;
	for(int i = 0; i < 2; i++) {
		crc ^= data[i];
		for(uint8_t bit = 8; bit > 0; --bit) {
			if(crc & 0x80) {
				crc = (crc << 1) ^ 0x31u;
			} else {
				crc = (crc << 1);
			}
		}
	}
	return crc;
}


void readI2C()
{
	data.addr = device_addr<<1;						// Perform a read from device at I2C address 0x80
	data.flags = I2C_FLAG_READ;
	data.buf[0].data = &read_buffer[0];				// Load MSB of read value into read_buffer[0]
	data.buf[0].len=3; 									// read 3 bytes, 2 bytes of data and 1 byte of CRC
	transferstatus=I2C_TransferInit(I2C0, &data);
	//LOG_INFO("Read command transfer_status=%d \t",transferstatus);

	//while loop to ensure, transfer is completed
	while (transferstatus == i2cTransferInProgress)
	{
		transferstatus = I2C_Transfer(I2C0);
	}

	uint8_t crc=CalcCrc(&read_buffer);
	curr_voc_value = read_buffer[0]+(read_buffer[1]<<8);
	curr_voc_value = curr_voc_value/MAPPING_VALUE; 		//Mapping 0-65535 to 0-500

	if(read_buffer[2]==crc) //Only display the value after CRC check
	{
		LOG_INFO("Voc value = %d", curr_voc_value);
		if(BT_connection==1)
			displayPrintf(DISPLAY_ROW_ACTION,"VOC = %d",curr_voc_value);
	}
	else
	{
		LOG_INFO("Invalid Data");
	}

	//Resetting the values
	read_buffer[0] = 0;
	read_buffer[1] = 0;
	read_buffer[2] = 0;
}

void DisableI2C()
{
	NVIC_DisableIRQ(I2C0_IRQn);								// Disable NVIC IRQ
	I2C_Enable(I2C0,false);
	GPIO_PinModeSet(gpioPortC, 10, gpioModeDisabled, 1);	// Disabling the SDA port
	GPIO_PinModeSet(gpioPortC, 11, gpioModeDisabled, 1);	// Disabling the SDL port
	CMU_ClockEnable(cmuClock_HFPER, false);
}
