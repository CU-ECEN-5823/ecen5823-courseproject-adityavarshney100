/*
 * ble.c
 *
 *  Created on: 26-Feb-2021
 *      Author: aditya.vny95
 */

#include "ble.h"

int8_t rssi;

void handler_ble_event(struct gecko_cmd_packet *evt)
{
	switch(BGLIB_MSG_ID(evt->header))
		{
			case gecko_evt_system_boot_id:							// Initializing the gecko ble

				if(gecko_cmd_le_gap_set_advertise_timing(0,ADVERTISING_MINIMUM,ADVERTISING_MAXIMUM,0,0) == -1)
						{
							LOG_ERROR("ERROR with advertising timing");
						}

				if(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable) == -1 )
				{
					LOG_ERROR("ERROR with advertising");
				}

			break;

			case gecko_evt_le_connection_opened_id:					// When the connection is opened change the parameters of interval and latency

				if(gecko_cmd_le_connection_set_parameters(evt->data.evt_le_connection_opened.connection,CONNECTION_INTERVAL_MINIMUM,CONNECTION_INTERVAL_MAXIMUM,SLAVE_LATENCY,SUPERVISION_TIMEOUT) == -1)

					{
						LOG_ERROR("ERROR with connection");
					}

			break;

			case gecko_evt_gatt_server_characteristic_status_id:		// Getting the gatt server status

				if(gecko_cmd_le_connection_get_rssi(evt->data.evt_gatt_server_characteristic_status.connection) == -1)
				{
										LOG_ERROR("ERROR with Status id");
				}
			break;

			case gecko_evt_le_connection_rssi_id:						// To change the power levels of the Bluetooth according to the RSSI value of the signal between phone and gecko

				rssi=evt->data.evt_le_connection_rssi.rssi;

				if(rssi>-35)
				{
					gecko_cmd_system_set_tx_power(-300);
				}

				else if((rssi>-45) && (rssi<=-35))
				{
					gecko_cmd_system_set_tx_power(-200);
				}
				else if((rssi>-55) && (rssi<=-45))
				{
					gecko_cmd_system_set_tx_power(-150);
				}
				else if((rssi>-65) && (rssi<=-55))
				{
					gecko_cmd_system_set_tx_power(-50);
				}
				else if((rssi>-75) && (rssi<=-65))
				{
					gecko_cmd_system_set_tx_power(0);
				}
				else if((rssi>-85) && (rssi<=-75))
				{
					gecko_cmd_system_set_tx_power(50);
				}
				else if(rssi<=-85)
				{
					gecko_cmd_system_set_tx_power(100);
				}

				break;

			case gecko_evt_le_connection_parameters_id:								// To check the parameter value when a phone is connected
				LOG_INFO("Connection Parameters: interval: %u Latency: %u Timeout: %u",
						evt->data.evt_le_connection_parameters.interval,
						evt->data.evt_le_connection_parameters.latency,
						evt->data.evt_le_connection_parameters.timeout);

				break;
			case gecko_evt_le_connection_closed_id:									// steps to perform when the connection is closed

				gecko_cmd_system_halt(1);
				gecko_cmd_system_set_tx_power(0);
				gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);

				break;

			default:
				break;
		}
}
