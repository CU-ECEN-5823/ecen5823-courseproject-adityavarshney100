/*
 * i2c.c
 *
 *  Created on: 12-Feb-2021
 *      Author: aditya.vny95
 */

#include "i2c.h"

uint8_t write_command[1]={0xF3};			// Temperature measurement command
//DOS should be a 8-bit data type array, uint16_t read_buffer[2];					// The value read from the sensor is stored here
uint8_t read_buffer[2];					    // The value read from the sensor is stored here
uint16_t result;							// The value that is received from the sensor is 16 bit
float temperature;							// Measurement of the final temperature
I2C_TransferReturn_TypeDef transferstatus;
I2C_TransferSeq_TypeDef data;


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
	InitI2C();									// Initialize the I2C to start I2C communication

	data.addr = SI7021_ADDRESS;				// Perform a write to device at I2C address 0x80
	data.flags = I2C_FLAG_WRITE;
	data.buf[0].data = &write_command[0];	// Write command for temperature measurement
	data.buf[0].len = 1;					// Length of write command
	transferstatus=I2C_TransferInit(I2C0, &data);
}

void readI2C()
{
	InitI2C();									// Initialize the I2C to start I2C communication

	data.addr = SI7021_ADDRESS;						// Perform a read from device at I2C address 0x80
	data.flags = I2C_FLAG_READ;
	data.buf[0].data = &read_buffer[0];				// Load MSB of read value into read_buffer[0]
// DOS: bug:	data.buf[0].len=1;
	data.buf[0].len=2; // read 2 bytes
	// data.buf[1] is only used for FLAG_WRITE_READ and FLAG_WRITE_WRITE transfers. I talked about
    // in lecture.
//	data.buf[1].data = read_buffer;					// Load LSB of read value into read_buffer[1]
//	data.buf[1].len=1;								// Length of read value in bytes
	transferstatus=I2C_TransferInit(I2C0, &data);

}

void Temperature()
{
	result = 0; // DOS: you were not initializing this variable, never rely on the compiler
	            //      to do this for you!
	result |= (uint16_t)(read_buffer[0]<<8);		// Load MSB of read value into variable result
	result |= read_buffer[1];						// Load LSB of read read value into variable result

	temperature=((175.72*result)/65536.0)-46.85;							// Temperature Calculation

	LOG_INFO("Measured Temperature in Degrees C: %f\n", temperature);		// Log measured temperature
}
