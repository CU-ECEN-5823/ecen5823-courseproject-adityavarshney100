/*
 * ble.c
 *
 *  Created on: 26-Feb-2021
 *      Author: aditya.vny95
 */

#include "ble.h"

int8_t rssi;
extern float temperature;
bool BT_connection = false;
bool BT_indication = false;

void handler_ble_event(struct gecko_cmd_packet *evt)
{
	switch(BGLIB_MSG_ID(evt->header))
		{
			case gecko_evt_system_boot_id:							// Initializing the gecko ble

				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0,ADVERTISING_MINIMUM,ADVERTISING_MAXIMUM,0,0));
				displayPrintf(DISPLAY_ROW_NAME,"%s", "Server");
				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Advertising");
				displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x", gecko_cmd_system_get_bt_address()->address.addr[5],
						gecko_cmd_system_get_bt_address()->address.addr[4], gecko_cmd_system_get_bt_address()->address.addr[3],
						gecko_cmd_system_get_bt_address()->address.addr[2], gecko_cmd_system_get_bt_address()->address.addr[1],
						gecko_cmd_system_get_bt_address()->address.addr[0]);
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));

			break;

			case gecko_evt_le_connection_opened_id:					// When the connection is opened change the parameters of interval and latency

				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(evt->data.evt_le_connection_opened.connection,CONNECTION_INTERVAL_MINIMUM,CONNECTION_INTERVAL_MAXIMUM,SLAVE_LATENCY,SUPERVISION_TIMEOUT));
				BT_connection = true;
				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Connected");

			break;

			case gecko_evt_gatt_server_characteristic_status_id:		// Getting the gatt server status

				if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_measurement
					&& evt->data.evt_gatt_server_characteristic_status.status_flags == gatt_server_client_config)
				{
						if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_indication)
						{
							BT_indication = true;
						}
						else if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_disable)
						{
							BT_indication = false;
						}
				}
				gecko_cmd_le_connection_get_rssi(evt->data.evt_gatt_server_characteristic_status.connection);
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

				BT_connection = false;
				gecko_cmd_system_set_tx_power(0);
				gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable);
				displayPrintf(DISPLAY_ROW_TEMPVALUE, "%s", " ");
				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Advertising");

				break;

			case gecko_evt_hardware_soft_timer_id:

				displayUpdate();

			default:
				break;
		}
}
