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
bool BT_connection = false;
bool BT_indication = false;
uint16_t error = 0;

const uint8_t pushbuttonService[16] = {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x01, 0x00, 0x00, 0x00};
const uint8_t pushbuttonChar[16]	= {0x89, 0x62, 0x13, 0x2d, 0x2a, 0x65, 0xec, 0x87, 0x3e, 0x43, 0xc8, 0x38, 0x02, 0x00, 0x00, 0x00};
ConnectionState State;
ConnectionProperties Properties;
extern uint8_t button0_state;
extern uint8_t button1_state;


uint8_t connection_handle	= 0;
uint8_t connection1_handle 	= 0;
int8	bonding_handle 		= -10;	// this value can never be achieved
uint32	passkey				= 0;

bool bonding 		= false;		// to indicate if bonding complete          bonding_complete
bool wait_confirm 	= false;		// wait for confirmation
bool press_1 		= true;

uint8_t findServiceInAdvertisement(uint8_t *data, uint8_t len)
{
	uint8_t FieldLength;
	uint8_t FieldType;
	uint8_t i=0;
	while(i<len)													// Parse advertisement packet
	{
		FieldLength = data[i];
		FieldType = data[i+1];
		if(FieldType == 0x06 || FieldType == 0x07)					// Partial ($02) or complete ($03) list of 16-bit UUIDs
		{
			if(memcmp(&data[i+2],pushbuttonService,16) == 0)				// compare UUID to Health Thermometer service UUID
			{
				return 1;
			}
		}
		i=i+1+FieldLength;											// advance to the next AD struct
	}
	return 0;
}

int32_t gattFloat32ToInt(const uint8_t *value_start_little_endian)	// convert IEEE-11073 32-bit float to integer
{
	uint8_t signByte = 0;
	int32_t mantissa;
	int8_t exponent = (int8_t)value_start_little_endian[4];
	if(value_start_little_endian[3] & 0x80)							// sign extend the mantissa value if the mantissa is negative
	{
		signByte = 0xFF;
	}
	mantissa = (int32_t) (value_start_little_endian[1] << 0) | (value_start_little_endian[2] << 8) |
			(value_start_little_endian[3] << 16) | (signByte << 24);
	return (int32_t) (pow(10,exponent)*mantissa);					// value = 10^exponent*mantissa, pow returns a double type
}

void handler_ble_event(struct gecko_cmd_packet *evt)
{
	uint8_t* value;
	switch(BGLIB_MSG_ID(evt->header))
		{
			case gecko_evt_system_boot_id:							  // This boot event is generated when the system boots up after reset
				if(DEVICE_IS_BLE_SERVER == 1)
				{
				BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());
				BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_configure(0x0F, sm_io_capability_displayyesno));
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
					BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());
					BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_configure(0x0F, sm_io_capability_displayyesno));
					displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Discovering");
					displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x", gecko_cmd_system_get_bt_address()->address.addr[5],
											gecko_cmd_system_get_bt_address()->address.addr[4], gecko_cmd_system_get_bt_address()->address.addr[3],
											gecko_cmd_system_get_bt_address()->address.addr[2], gecko_cmd_system_get_bt_address()->address.addr[1],
											gecko_cmd_system_get_bt_address()->address.addr[0]);
					displayPrintf(DISPLAY_ROW_NAME,"%s", "Client");
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_discovery_type(le_gap_phy_1m, SCAN_PASSIVE));					// Set 1Mb PHY passive scanning
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_discovery_timing(le_gap_phy_1m, SCAN_INTERVAL, SCAN_WINDOW));	// Setting Scan interval and scan window
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_conn_parameters(CONNECTION_INTERVAL_MINIMUM, CONNECTION_INTERVAL_MAXIMUM, SLAVE_LATENCY, CONNECTION_TIMEOUT));		// Setting default connection parameters for subsequent connections
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_discovery(le_gap_phy_1m, le_gap_discover_generic));			// Start scanning - looking for thermometer devices
					State = scanning;
				}
			break;

			case gecko_evt_le_gap_scan_response_id:					// This event is generated when an advertisement packet or a scan response is received from a slave
				if(evt->data.evt_le_gap_scan_response.packet_type == 0)		// Parse advertisement packets
				{
					if(findServiceInAdvertisement(&(evt->data.evt_le_gap_scan_response.data.data[0]), evt->data.evt_le_gap_scan_response.data.len) != 0)		// If a thermometer advertisement is found...
					{
						BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_end_procedure());				// then stop scanning for a while
						BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_connect(evt->data.evt_le_gap_scan_response.address,evt->data.evt_le_gap_scan_response.address_type, le_gap_phy_1m));			// and connect to that device
						State = opening;
					}
				}
				break;

			case gecko_evt_le_connection_opened_id:					  // This event is generated when a new connection is established

				connection_handle = evt->data.evt_le_connection_opened.connection;
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
					BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_primary_services_by_uuid(evt->data.evt_le_connection_opened.connection, 16, (const uint8_t*) pushbuttonService));					// Discover Health Thermometer service on the slave device
					State = discoverServices;
				}

			break;

			case gecko_evt_sm_bonded_id:
				LOG_INFO("Bonding Complete");
				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Bonded");
				bonding = true;
			break;

			case gecko_evt_sm_confirm_passkey_id:
				LOG_INFO("Confirm Passkey Event");
				displayPrintf(DISPLAY_ROW_PASSKEY, "Passkey: %d", evt->data.evt_sm_confirm_passkey.passkey);
				displayPrintf(DISPLAY_ROW_ACTION, "%s", "Confirm with PB0");
				wait_confirm = true;
				passkey = evt->data.evt_sm_confirm_passkey.passkey;				// Saving the passkey
				break;

			case gecko_evt_gatt_service_id:							// This event is generated when a new service is discovered

				Properties.pushbuttonServiceHandle = evt->data.evt_gatt_service.service;		// Save service handle for future reference

			break;

			case gecko_evt_sm_confirm_bonding_id:
				LOG_INFO("Bonding Confirmed\n");
				struct gecko_msg_sm_bonding_confirm_rsp_t	*bonding_confirm_ptr;
				bonding_confirm_ptr = gecko_cmd_sm_bonding_confirm(evt->data.evt_sm_confirm_bonding.connection, 1);
				if(bonding_confirm_ptr->result != 0)
				{
					LOG_ERROR("non zero error code returned = %x", bonding_confirm_ptr->result);
				}
				else
				{
					bonding_handle = evt->data.evt_sm_confirm_bonding.bonding_handle;
				}
				break;

			case gecko_evt_system_external_signal_id:
				if(DEVICE_IS_BLE_SERVER == 1)
				{
					if(wait_confirm && (evt->data.evt_system_external_signal.extsignals & EVT_BUTTON_PRESS))
					{
						struct gecko_msg_sm_passkey_confirm_rsp_t *passkey_confirm_ptr;
						passkey_confirm_ptr = gecko_cmd_sm_passkey_confirm(connection_handle,1);
						LOG_INFO("Called: gecko_cmd_sm_passkey_confirm()");

						if(passkey_confirm_ptr->result)
						{
							LOG_ERROR("no xer error code by passkey confirm%x", passkey_confirm_ptr->result);
						}
						displayPrintf(DISPLAY_ROW_ACTION, "%s", " ");
						displayPrintf(DISPLAY_ROW_PASSKEY, "%s", " ");
						wait_confirm = false;
					}
					else if(evt->data.evt_system_external_signal.extsignals & EVT_BUTTON_PRESS)
					{
						LOG_INFO("Button_State");
						gecko_cmd_gatt_server_write_attribute_value(gattdb_button_state,0,sizeof(button0_state),&button0_state);
					}
				}
				else
				{
					if(evt->data.evt_system_external_signal.extsignals & EVT_BUTTON_PRESS)
					{
						if((wait_confirm) && (button0_state == Button0_Pressed))
						{
							BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_passkey_confirm(connection_handle,1));
							LOG_INFO("Passkey Confirmed");
							displayPrintf(DISPLAY_ROW_ACTION, "%s", " ");
							displayPrintf(DISPLAY_ROW_PASSKEY, "%s", " ");
						}
						else if (button1_state == Button1_Pressed)
						{
							BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_read_characteristic_value_by_uuid(connection_handle,Properties.pushbuttonServiceHandle, 16, (const uint8_t*) pushbuttonChar));
						}
					}
				}
				break;

			case gecko_evt_gatt_characteristic_id:					// This event is generated when a new characteristic is discovered

				Properties.pushbuttonCharacteristicHandle = evt->data.evt_gatt_characteristic.characteristic;			  // Save characteristic handle for future reference

			break;

			case gecko_evt_gatt_procedure_completed_id:				// This event is generated for various procedure completions, e.g. when a write procedure is completed, or service discovery is completed
				if(State == discoverServices && Properties.pushbuttonServiceHandle != SERVICE_HANDLE_INVALID)
				{
					BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_characteristics_by_uuid(connection_handle, Properties.pushbuttonServiceHandle, 16, (const uint8_t*)pushbuttonChar));
					State = discoverCharacteristics;
					break;
				}
				if(State == discoverCharacteristics && Properties.pushbuttonCharacteristicHandle != CHARACTERISTIC_HANDLE_INVALID)
				{
					/*BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_end_procedure());			// discovering stop
					BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_set_characteristic_notification(evt->data.evt_gatt_procedure_completed.connection, Properties.thermometerCharacteristicHandle, gatt_indication));
					displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Handling Indications");
					State = enableIndication;*/
					if(evt->data.evt_gatt_procedure_completed.result == 0x040F)
					{
						LOG_INFO("Implemented Security ");
						gecko_cmd_sm_increase_security(connection_handle);
						press_1 = false;
					}
					break;
				}
				/*if(State == enableIndication)
				{
					State = running;
				}
				break;*/

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

			case gecko_evt_gatt_characteristic_value_id:			  // This event is generated when a characteristic value was received e.g. an indication

				value = &(evt->data.evt_gatt_characteristic_value.value.data[0]);
				if(value[0] == 0x00)
				{
					displayPrintf(DISPLAY_ROW_TEMPVALUE,"%s", "Button Released");
				}
				else
				{
					displayPrintf(DISPLAY_ROW_TEMPVALUE,"%s", "Button Pressed");
				}
				/*
				Properties.temperature = (value[1] << 0) + (value[2] << 8) + (value[3] << 16);
				BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection));			// Send confirmation for the indication
				uint32_t temp;
				temp = gattFloat32ToInt(evt->data.evt_gatt_characteristic_value.value.data);														  // Trigger RSSI measurement on the connection
				displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp = %d", temp);*/
				break;

			case gecko_evt_le_connection_parameters_id:								// To check the parameter value when a phone is connected
				LOG_INFO("Connection Parameters: interval: %u Latency: %u Timeout: %u",
						evt->data.evt_le_connection_parameters.interval,
						evt->data.evt_le_connection_parameters.latency,
						evt->data.evt_le_connection_parameters.timeout);

				break;

			case gecko_evt_le_connection_closed_id:									// steps to perform when the connection is closed

				connection_handle 	= 0;
				bonding_handle		= -10;		// cannot achieve this value
				passkey 			= 0;
				if(DEVICE_IS_BLE_SERVER == 1)
				{
				BT_connection = false;
				BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());
				gecko_cmd_system_set_tx_power(0);					// setting tx power to 0 here
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));// start advertising again
				enable_button_interrupt();
				displayPrintf(DISPLAY_ROW_TEMPVALUE, "%s", " ");
				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Advertising");
				displayPrintf(DISPLAY_ROW_ACTION, "%s", " ");
				displayPrintf(DISPLAY_ROW_PASSKEY, "%s", " ");
				}
				else						// Starting to find new devices
				{
					BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_discovery(le_gap_phy_1m, le_gap_discover_generic));
					BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());
					displayPrintf(DISPLAY_ROW_TEMPVALUE, "%s", " ");
					displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Discovery");
					displayPrintf(DISPLAY_ROW_ACTION, "%s", " ");
					displayPrintf(DISPLAY_ROW_PASSKEY, "%s", " ");
					State = scanning;

				}
				break;

			case gecko_evt_hardware_soft_timer_id:

				displayUpdate();
				break;

			case gecko_evt_sm_bonding_failed_id:

				displayPrintf(DISPLAY_ROW_CONNECTION, "%s", "Bonding Failed");
				displayPrintf(DISPLAY_ROW_PASSKEY, "%d", " ");
				wait_confirm 	= false;
				bonding 		= false;
				bonding_handle 	= -10;	// cannot reach this value
				passkey 		= 0;
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_close(evt->data.evt_sm_bonding_failed.connection));
				break;

			default:
				break;
		}
}
