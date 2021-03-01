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

#define ADVERTISING_MINIMUM 400 				// Minimum advertising interval. Value in units of 0.625 ms, for 250 ms, 250/0.625 = 400
#define ADVERTISING_MAXIMUM 400					// Minimum advertising interval. Value in units of 0.625 ms, for 250 ms, 250/0.625 = 400

#define CONNECTION_INTERVAL_MINIMUM 60			// Time = Value x 1.25 ms, for 75 ms, value = 60
#define CONNECTION_INTERVAL_MAXIMUM 60			// Time = Value x 1.25 ms, for 75 ms, value = 60
#define SLAVE_LATENCY_MS 300
#define SLAVE_LATENCY 3							// 300 / 75 = 4 - 1 = 3

#define SUPERVISION_TIMEOUT 50000				// (1 + 300) *(60 * 2) = 45150


/*
 * The ble function that handles the gecko BLE events
 * Return Type: NONE
 * Parameters: 	BLE connection
 */
void handler_ble_event(struct gecko_cmd_packet *event);

#endif /* SRC_BLE_H_ */
