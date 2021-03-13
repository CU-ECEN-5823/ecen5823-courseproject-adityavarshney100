/*
 * ble.c
 *
 *  Created on: 26-Feb-2021
 *      Author: aditya.vny95
 */

#include "ble.h"

const uint8_t ThermoService[2] = {0x09, 0x18};			// Service UUID of health thermometer as defined by Bluetooth SIG
const uint8_t ThermoCharacteristic[2] = {0x1c, 0x2a};	// Characteristic UUID of temperature measurement as defined by Bluetooth SIG
uint8_t server_bt_addr[] = SERVER_BT_ADDRESS;
int8_t rssi;
extern float temperature;
bool BT_connection = false;
bool BT_indication = false;
ConnectionState State;
ConnectionProperties Properties;

uint8_t findServiceInAdvertisement(uint8_t *data, uint8_t len)
{
	uint8_t FieldLength;
	uint8_t FieldType;
	uint8_t i=0;
	while(i<len)
	{
		FieldLength = data[i];
		FieldType = data[i+1];
		if(FieldType == 0x02 || FieldType == 0x03)
		{
			if(memcmp(&data[i+2],ThermoService,2) == 0)
			{
				return 0;
			}
		}
		i=i+FieldLength+1;
	}
	return 1;
}

int32_t gattFloat32ToInt(const uint8_t *value_start_little_endian)
{
	uint8_t signByte = 0;
	int32_t mantissa;
	int8_t exponent = (int8_t)value_start_little_endian[4];
	if(value_start_little_endian[3] & 0x80)
	{
		signByte = 0xFF;
	}
	mantissa = (int32_t) (value_start_little_endian[1] << 0) | (value_start_little_endian[2] << 8) |
			(value_start_little_endian[3] << 16) | (signByte << 24);
	return (int32_t) (pow(10,exponent)*mantissa);		// value = 10^exponent*mantissa, pow returns a double type
}

void handler_ble_event(struct gecko_cmd_packet *evt)
{
	uint8_t* value;
	switch(BGLIB_MSG_ID(evt->header))
		{
			case gecko_evt_system_boot_id:							// Initializing the gecko ble
				if(DEVICE_IS_BLE_SERVER == 1)
				{
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0,ADVERTISING_MINIMUM,ADVERTISING_MAXIMUM,0,0));
				displayPrintf(DISPLAY_ROW_NAME,"%s", "Server");
				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Advertising");
				displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x", gecko_cmd_system_get_bt_address()->address.addr[5],
						gecko_cmd_system_get_bt_address()->address.addr[4], gecko_cmd_system_get_bt_address()->address.addr[3],
						gecko_cmd_system_get_bt_address()->address.addr[2], gecko_cmd_system_get_bt_address()->address.addr[1],
						gecko_cmd_system_get_bt_address()->address.addr[0]);
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
				}
				else
				{
					displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Discovering");
					displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x", gecko_cmd_system_get_bt_address()->address.addr[5],
											gecko_cmd_system_get_bt_address()->address.addr[4], gecko_cmd_system_get_bt_address()->address.addr[3],
											gecko_cmd_system_get_bt_address()->address.addr[2], gecko_cmd_system_get_bt_address()->address.addr[1],
											gecko_cmd_system_get_bt_address()->address.addr[0]);
					displayPrintf(DISPLAY_ROW_NAME,"%s", "Client");
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_discovery_type(le_gap_phy_1m, SCAN_PASSIVE));					// Set 1Mb PHY passive scanning
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_discovery_timing(le_gap_phy_1m, SCAN_INTERVAL, SCAN_WINDOW));	// Setting Scan interval and scan window
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_conn_parameters(CONNECTION_INTERVAL_MINIMUM, CONNECTION_INTERVAL_MAXIMUM, SLAVE_LATENCY, CONNECTION_TIMEOUT));		// Setting default connection parameters for subsequent connections
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_discovery(le_gap_phy_1m, le_gap_discover_generic));
					State = scanning;
				}
			break;

			case gecko_evt_le_gap_scan_response_id:
				if(evt->data.evt_le_gap_scan_response.packet_type == 0)
				{
					displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Getting scan response");
					if(findServiceInAdvertisement(&(evt->data.evt_le_gap_scan_response.data.data[0]), evt->data.evt_le_gap_scan_response.data.len) != 0)
					{
						BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_end_procedure());
						BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_connect(evt->data.evt_le_gap_scan_response.address,evt->data.evt_le_gap_scan_response.address_type, le_gap_phy_1m));
						State = opening;
					}
				}
				break;

			case gecko_evt_le_connection_opened_id:					// When the connection is opened change the parameters of interval and latency

				if(DEVICE_IS_BLE_SERVER == 1)
				{
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(evt->data.evt_le_connection_opened.connection,CONNECTION_INTERVAL_MINIMUM,CONNECTION_INTERVAL_MAXIMUM,SLAVE_LATENCY,SUPERVISION_TIMEOUT));
				BT_connection = true;
				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Connected");
				}
				else
				{
					displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Connected");
					displayPrintf(DISPLAY_ROW_BTADDR2, "%x:%x:%x:%x:%x:%x", server_bt_addr[5], server_bt_addr[4], server_bt_addr[3], server_bt_addr[2], server_bt_addr[1], server_bt_addr[0]);
					BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_primary_services_by_uuid(evt->data.evt_le_connection_opened.connection, 2, (const uint8_t*) ThermoService));
					State = discoverServices;
				}

			break;

			case gecko_evt_gatt_service_id:

				Properties.thermometerServiceHandle = evt->data.evt_gatt_service.service;

			break;

			case gecko_evt_gatt_characteristic_id:

				Properties.thermometerCharacteristicHandle = evt->data.evt_gatt_characteristic.characteristic;

			break;

			case gecko_evt_gatt_procedure_completed_id:
				if(State == discoverServices && Properties.thermometerServiceHandle != SERVICE_HANDLE_INVALID)
				{
					BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_characteristics_by_uuid(evt->data.evt_gatt_procedure_completed.connection, Properties.thermometerServiceHandle, 2, (const uint8_t*)ThermoCharacteristic));
					State = discoverCharacteristics;
					break;
				}
				if(State == discoverCharacteristics && Properties.thermometerCharacteristicHandle != CHARACTERISTIC_HANDLE_INVALID)
				{
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_end_procedure());			// discovering stop
					BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_set_characteristic_notification(evt->data.evt_gatt_procedure_completed.connection, Properties.thermometerCharacteristicHandle, gatt_indication));
					displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Handling Indications");
					State = enableIndication;
					break;
				}
				if(State == enableIndication)
				{
					State = running;
				}
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

			case gecko_evt_gatt_characteristic_value_id:

				value = &(evt->data.evt_gatt_characteristic_value.value.data[0]);
				Properties.temperature = (value[1] << 0) + (value[2] << 8) + (value[3] << 16);
				BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection));
				uint32_t temp;
				temp = gattFloat32ToInt(evt->data.evt_gatt_characteristic_value.value.data);
				displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp = %d", temp);
				break;

			case gecko_evt_le_connection_parameters_id:								// To check the parameter value when a phone is connected
				LOG_INFO("Connection Parameters: interval: %u Latency: %u Timeout: %u",
						evt->data.evt_le_connection_parameters.interval,
						evt->data.evt_le_connection_parameters.latency,
						evt->data.evt_le_connection_parameters.timeout);

				break;

			case gecko_evt_le_connection_closed_id:									// steps to perform when the connection is closed

				if(DEVICE_IS_BLE_SERVER == 1)
				{
				BT_connection = false;
				gecko_cmd_system_set_tx_power(0);					// setting tx power to 0 here
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));		// start advertising again
				displayPrintf(DISPLAY_ROW_TEMPVALUE, "%s", " ");
				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Advertising");
				}
				else						// Starting to find new devices
				{
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_discovery(le_gap_phy_1m, le_gap_discover_generic));
					displayPrintf(DISPLAY_ROW_TEMPVALUE, "%s", " ");
					displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Discovery");
				}
				break;

			case gecko_evt_hardware_soft_timer_id:

				displayUpdate();
				break;

			default:
				break;
		}
}
