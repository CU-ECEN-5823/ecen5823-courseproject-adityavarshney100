/*
 * ble.h
 *
 *  Created on: 26-Feb-2021
 *      Author: aditya.vny95
 */

#ifndef SRC_BLE_H_
#define SRC_BLE_H_

#include "main.h"
#include "native_gecko.h"
#include "scheduler.h"
#include "bg_types.h"
#include "gatt_db.h"
#include "display.h"
#include "i2c.h"
#include "gecko_ble_errors.h"
#include <math.h>
#include "bg_gattdb_def.h"

#define ADVERTISING_MINIMUM 			400			// Minimum advertising interval. Value in units of 0.625 ms, for 250 ms, 250/0.625 = 400
#define ADVERTISING_MAXIMUM 			400			// Minimum advertising interval. Value in units of 0.625 ms, for 250 ms, 250/0.625 = 400

#define CONNECTION_INTERVAL_MINIMUM 	80			// Time = Value x 1.25 ms, for 100 ms, value = 80
#define CONNECTION_INTERVAL_MAXIMUM 	80			// Time = Value x 1.25 ms, for 100 ms, value = 80
#define SLAVE_LATENCY_MS 				300
#define SLAVE_LATENCY 					0			// 300 / 75 = 4 - 1 = 3, no latency
#define SUPERVISION_TIMEOUT 			50000		// (1 + 300) *(60 * 2) = 45150
#define CONNECTION_TIMEOUT				100			//
#define SCAN_INTERVAL 					80			// 50 ms
#define SCAN_WINDOW						40 			// 25 ms
#define SCAN_PASSIVE					0

#define TEMP_INVALID					(uint32_t)0xFFFFFFFFu
#define RSSI_INVALID					(int8_t)127
#define CONNECTION_HANDLE_INVALID		(uint8_t)0xFFu
#define SERVICE_HANDLE_INVALID			(uint32_t)0xFFFFFFFFu
#define CHARACTERISTIC_HANDLE_INVALID	(uint16_t)0xFFFFu
#define TABLE_INDEX_INVALID				(uint8_t)0xFFu

#define EXT_SIGNAL_PRINT_RESULTS		0x01

//#define gattdb_button_state 			11

typedef enum {
	scanning,
	opening,
	discoverServices,
	discoverCharacteristics,
	enableIndication,
	running
} ConnectionState;

typedef struct{
	uint8_t 	connectionHandle;
	int8_t 		rssi;
	uint16_t	serverAddress;
	uint32_t	thermometerServiceHandle;
	uint16_t	thermometerCharacteristicHandle;
	uint32_t	temperature;
} ConnectionProperties;

/*
 * The ble function that handles the gecko BLE events
 * Return Type: NONE
 * Parameters: 	BLE connection
 */
void handler_ble_event(struct gecko_cmd_packet *event);


/*
 * Parse advertisements looking for advertised Health Thermometer service
 * Return Type: 8bit unsigned int
 * Parameters: data and length of data
 */
uint8_t findServiceInAdvertisement(uint8_t *data, uint8_t len);

/*
 * this function changes float value of the temperature to 32bitint
 * Return Type: int
 * Parameters: 	float temperature value
 */
int32_t gattFloat32ToInt(const uint8_t *value_start_little_endian);

#endif /* SRC_BLE_H_ */
