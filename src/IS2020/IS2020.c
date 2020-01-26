/*
 * IS2020.c
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#include "IS2020.h"
#include "AVRCP.h"
#include "SYSTEM_EVENTS.h"
#include "QueueLimits.h"

#include "../usb.h"

static struct TRACK_META_t IS2020_trackMeta;

static uint8_t IS2020_linkStatus[8];
static uint8_t IS2020_remoteDeviceNames[2][MAX_DEV_NAME_LENGTH] = { { 0 }, { 0 } };
static uint8_t IS2020_deviceSupportAvrcpV13[2] = { 0 };
static uint8_t IS2020_maxBatteryLevel[2] = { 0 };
static uint8_t IS2020_currentBatteryLevel[2] = { 0 };
static uint8_t IS2020_roamingStatus[2] = { 0 };
static uint8_t IS2020_maxPhoneSignalLevel[2] = { 0, 0 };
static uint8_t IS2020_currentSignalLevel[2] = { 0, 0 };
static long IS2020_playback_position = 0;

QueueHandle_t IS2020_qRx;
QueueHandle_t IS2020_qTx;
static uint8_t event[QUEUELIMIT_BLUETOOTH_MODULE_RX] = { 0 };

void *_f1;
void (*_f2)();

void IS2020_getRemoteDeviceName(uint8_t devId, char* name, uint8_t *len) {
	len = 0;
	for (int x = 0; x < MAX_DEV_NAME_LENGTH; x++) {
		if (IS2020_remoteDeviceNames[devId][x] != 0x00) {
			name[x] = IS2020_remoteDeviceNames[devId][x];
			len = x;
		}
	}
}

void IS2020_setQueues(QueueHandle_t Rx, QueueHandle_t Tx) {
	IS2020_qRx = Rx;
	IS2020_qTx = Tx;
}
/*
 void IS2020_setIO(void f1(uint8_t c), void f2()) {
 _f1 = f1;
 _f2 = f2;
 }
 */

void IS2020_WriteByteToBluetooth(uint8_t data) {
#define BLOCK_TIME_TX_OUT 50
	long lReturn;
	if ( xQueueSend( IS2020_qTx, &data, BLOCK_TIME_TX_OUT ) == pdPASS) {
		lReturn = pdPASS;
		//USART_ITConfig( xUARTS[ lPort ], USART_IT_TXE, ENABLE );
		/* Enable transmit interrupt so it sends back the data. */
	} else {
		lReturn = pdFAIL;
	}

	//return lReturn;
}

void IS2020_getnextEvent() {
	vTaskDelay(120);
	uint8_t awaiting_pkg_size = 0, tmp = 0;

	if ((uxQueueMessagesWaiting(IS2020_qRx) > 3)) {
		//check if first byte is STAR of msg
		uint8_t preamble = 0;
		if (xQueueReceive(IS2020_qRx, &preamble, 20) == pdPASS) {

			//check-preamble
			if (preamble == 0xAA) {
				//event[0] = preamble;
				xQueueReceive(IS2020_qRx, &tmp, 20);
				awaiting_pkg_size = tmp << 8;
				//event[1] = tmp;
				xQueueReceive(IS2020_qRx, &tmp, 20);
				awaiting_pkg_size += tmp & 0xFF;
				awaiting_pkg_size += 1; //add +1 for checksum
				//event[2] = tmp;

				if (awaiting_pkg_size >= QUEUELIMIT_BLUETOOTH_MODULE_RX) {

					while (1) {
						//empty buffer by reading #-chars
						//todo
					}
				}
			}

		}
	}
	// check if all data has arived
	if (awaiting_pkg_size > 0
			&& uxQueueMessagesWaiting(IS2020_qRx) >= awaiting_pkg_size) {

		//copy the data from QUEUE into the event point, for processing
		for (uint8_t x = 0; x < awaiting_pkg_size; x++) {
			xQueueReceive(IS2020_qRx, &tmp, 20);
			//overflow protection
			event[x] = tmp;
		}

		//shift event over to IS2020 driver
		//add +3 (0xaa + 2, +1 checksum)
		IS2020_decode(awaiting_pkg_size, &event);

		//reset pkg-size, a
		awaiting_pkg_size = 0;

		/*
		 if (event[3] == 0x00) {
		 //got ack
		 tmp = event[0];
		 }
		 */

	}

}

void IS2020_sendAck(uint8_t eventId) {
	IS2020_sendPacketInt(CMD_Event_ACK, eventId);
}

void IS2020_sendPacketInt(uint8_t cmd, uint8_t data) {
	IS2020_WriteByteToBluetooth(0xAA);
	IS2020_WriteByteToBluetooth((uint8_t) 0x00);
	IS2020_WriteByteToBluetooth(0x02);
	uint8_t chk = 0x02;
	IS2020_WriteByteToBluetooth(cmd);
	chk += cmd;
	IS2020_WriteByteToBluetooth(data);
	chk += data;
	IS2020_WriteByteToBluetooth(0x100 - chk);
}

void IS2020_sendArrayIntGeneric(uint8_t pkgSize, uint8_t cmd,
		uint8_t data[]) {
	uint8_t checksum = 0;
	//write startbyte
	IS2020_WriteByteToBluetooth(0xAA);
	//write MSB pklen
	IS2020_WriteByteToBluetooth(pkgSize >> 8);
	checksum = pkgSize >> 8;
	//write LSB pklen
	IS2020_WriteByteToBluetooth(pkgSize & 0xFF);
	checksum += pkgSize & 0xFF;
	//write cmd
	IS2020_WriteByteToBluetooth(cmd);
	checksum += cmd;
	////write devId
	//IS2020_WriteByteToBluetooth(devId);
	//checksum += devId;

	for (uint16_t x = 0; x < pkgSize - 2; x++) {
		IS2020_WriteByteToBluetooth(data[x]);
		checksum += data[x];
	}
	//write checksum out
	IS2020_WriteByteToBluetooth(0x100 - (checksum % 0x100));

}

void IS2020_sendArrayInt(uint8_t pkgSize, uint8_t cmd, uint8_t devId,
		uint8_t data[]) {
	uint8_t checksum = 0;
	//write startbyte
	IS2020_WriteByteToBluetooth(0xAA);
	//write MSB pklen
	IS2020_WriteByteToBluetooth(pkgSize >> 8);
	checksum = pkgSize >> 8;
	//write LSB pklen
	IS2020_WriteByteToBluetooth(pkgSize & 0xFF);
	checksum += pkgSize & 0xFF;
	//write cmd
	IS2020_WriteByteToBluetooth(cmd);
	checksum += cmd;
	////write devId
	IS2020_WriteByteToBluetooth(devId);
	checksum += devId;

	for (uint16_t x = 0; x < pkgSize - 2; x++) {
		IS2020_WriteByteToBluetooth(data[x]);
		checksum += data[x];
	}
	//write checksum out
	IS2020_WriteByteToBluetooth(0x100 - (checksum % 0x100));

}

void IS2020_decode(uint8_t length, uint8_t* event) {

	uint8_t cmd = event[IS2020_EVENT_IDX_CMD];
	uint8_t t1 = 0;

	if (length > 40) {
		usb_vcp_send_strn("big\n", 4);
	}
	/*
	 * Send Ack if response not ack from IS2020
	 */
	if (cmd != EVT_Command_ACK) {
		vTaskDelay(12);
		IS2020_sendAck(cmd);
	}

	/*
	 * handle actual payload
	 */
	switch (cmd) {
		//0x00
		case EVT_Command_ACK:
			//cmd
			t1 = event[2];
			switch (t1) {
				case 0x00:
					//completed cmd
					usb_vcp_send_strn("ack ", 4);
					break;
				case 0x01:
					//cmd disallowed
					usb_vcp_send_strn("disallowed ", 11);
					break;
				case 0x02:
					//unkown cmd
					usb_vcp_send_strn("unknown ", 8);
					break;
				case 0x03:
					usb_vcp_send_strn("paraminc ", 9);
					//parameter incorrect
					break;
				case 0x04:
					//btm busy
					usb_vcp_send_strn("bussy ", 6);
					break;
				case 0x05:
					//btm memory full
					usb_vcp_send_strn("memfull ", 8);
					break;

				default:
					{ //fall-trough-all-errors
					  //catch-all
					  //uint8_t b[5] = { 'A', 'C', 'K,', 'X', '0' };
					  //b[4] = t1 + 0x30;			//convert to ascii
					  //usb_vcp_send_strn(b, 5);

				}
					return;
			}
			//write byte to term
			usb_vcp_send_strn("evt: ", 5);
			usb_vcp_send_byte((event[1] >> 4) + 0x30);
			usb_vcp_send_byte((event[1] & 0x0F) + 0x30);
			usb_vcp_send_strn("\r\n", 2);

			break;

		case EVT_BTM_Status:

			//IS2020_read_link_status();

			break;

		case EVT_Call_Status:

			break;

		case EVT_Caller_ID:

			break;

		case EVT_Phone_Max_Battery_Level:
			IS2020_maxBatteryLevel[event[1]] = event[2];
			postBMcommand(EVT_BATTERY_STATUS_CHANGED);
			break;

		case EVT_Phone_Current_Battery_Level:
			IS2020_currentBatteryLevel[event[1]] = event[2];
			postBMcommand(EVT_BATTERY_STATUS_CHANGED);
			break;

		case EVT_Roaming_Status:
			IS2020_roamingStatus[event[1]] = event[2];
			postBMcommand(EVT_ROAMING_CHANGED);
			break;

		case EVT_Phone_Max_Signal_Strength_Level:
			IS2020_maxPhoneSignalLevel[event[1]] = event[2];
			postBMcommand(EVT_SIGNAL_STRENGHT_CHANGED);
			break;

		case EVT_Phone_Current_Signal_Strength_Level:
			postBMcommand(EVT_SIGNAL_STRENGHT_CHANGED);
			IS2020_currentSignalLevel[event[1]] = event[2];
			break;

		case EVT_AVRCP_Specific_Rsp: { //response from IS2020mcu
			usb_vcp_send_strn("avrcpresp\r\n", 11);

			uint8_t devId = event[IS2020_EVENT_IDX_DEVID];
			uint8_t avrcpResponse = event[2]; //byte 0 of AVRCP pgk
			uint8_t subUnitId = event[3];
			uint8_t opcode = event[4];
			uint8_t compId[3] = { event[5], event[6], event[7] };
			uint8_t pdu = event[8];
			uint8_t res = event[9];
			uint8_t paramLen = (event[10] << 8) + event[11];
			uint8_t a = 1;

			if (paramLen == 1) {
				switch (event[12]) {
					case AVRCP_STATUS_INVALID_COMMAND:
						//break;
					case AVRCP_STATUS_INVALID_PARAM:
						//break;
					case AVRCP_STATUS_PARAM_NOT_FOUND:
						//break;
					case AVRCP_STATUS_INTERNAL_ERROR:
						//break;
					case AVRCP_STATUS_SUCCESS:
						//break;

					default:
						{ //fall-trough-all-errors
						  //catch-all
						uint8_t b[6] = { 'a', 'v', 'r', 'x', ':', '0' };
						b[5] = event[12] + 0x30;						//convert to ascii
						usb_vcp_send_strn(b, 6);
					}
				}
				//invalid command issued
				return;
			}		//paramlen

			if (pdu == AVRCP_REGISTER_NOTIFICATION) {

				switch (event[12]) {
					case AVRCP_EVENT_STATUS_CHANGED: {

						switch (event[13]) {
							case AVRCP_PLAY_STATUS_STOPPED:
								postApplicationEvent(EVT_MUSIC_STOP);
								break;

							case AVRCP_PLAY_STATUS_PLAYING:
								postApplicationEvent(EVT_MUSIC_PLAY);
								break;

							case AVRCP_PLAY_STATUS_PAUSED:
								postApplicationEvent(EVT_MUSIC_PAUSE);
								break;

							case AVRCP_PLAY_STATUS_FWD_SEEK:
								postApplicationEvent(EVT_MUSIC_FF);
								break;

							case AVRCP_PLAY_STATUS_REV_SEEK:
								postApplicationEvent(EVT_MUSIC_FR);

								break;

							case AVRCP_PLAY_STATUS_ERROR:
								break;
						}
					}
						break;

					case AVRCP_EVENT_TRACK_CHANGED:

						for (uint16_t parameter_byte = 14; parameter_byte < length;
								parameter_byte++) {
							//DBG(String(event[parameter_byte], HEX));
						}
						usb_vcp_send_strn("TRKch\r\n", 7);
						//IS2020_AVRCP_REGISTER_NOTIFICATION_TRACKCHANGED(0);
						switch (avrcpResponse) {
							case AVRCP_EVENT_EVENTMSGTYPE_NOTIFY:
								break;
							case AVRCP_EVENT_EVENTMSGTYPE_INTERIM:
								break;
							case AVRCP_EVENT_EVENTMSGTYPE_CHANGED:
								//re-active event
								IS2020_AVRCP_REGISTER_NOTIFICATION_TRACKCHANGED(0);
								postApplicationEvent(EVT_BM_MUSIC_TRACK_CHANGED);
						}

						break;

					case AVRCP_EVENT_PLAYBACK_POS_CHANGED:
						IS2020_playback_position = event[13] << 24 | event[14] << 16
								| event[15] << 8 | event[16];
						usb_vcp_send_strn("POSch\r\n", 7);
						//req-requist next update
						switch (avrcpResponse) {
							case AVRCP_EVENT_EVENTMSGTYPE_NOTIFY:
								break;
							case AVRCP_EVENT_EVENTMSGTYPE_INTERIM:
								break;
							case AVRCP_EVENT_EVENTMSGTYPE_CHANGED:
								//re-active event
								IS2020_AVRCP_REGISTER_NOTIFICATION_POSCHANGED(0);
								postApplicationEvent(EVT_BM_MUSIC_TRACK_POS_CHANGED);
						}

						break;

				}		//switch
			}		//avr-register-notify

			if (pdu == AVRCP_LIST_PLAYER_ATTRIBUTES)
			{/*
			 for (uint16_t parameter_byte = 13;
			 parameter_byte < (13 + event[12]); parameter_byte++) {
			 //IS2020_vrcpGetPlayerAttributeText(event[1], event[parameter_byte]);
			 //                      IS2020::avrcpListPlayerValues(event[1], event[parameter_byte]); //this cose module to reset
			 }
			 */
				uint8_t numPlayerAttribs = event[12];

				usb_vcp_send_strn("plyattr\r\n", 8);
			}

			//break;

			if (pdu == AVRCP_GET_ELEMENT_ATTRIBUTES) {
				//meta response?
				uint8_t numberOfAttribs = event[12];
				uint8_t attribOffset = 16;

				for (int x = 0; x < numberOfAttribs; x++) {

					uint8_t attribTypeOfField = /*events 13 ->16*/event[attribOffset + 0];
					uint16_t codingOfString = event[attribOffset + 1] << 8 | event[attribOffset + 2]; //will default to only 18... 0x6a == UTF8
					uint16_t atribValLength = event[attribOffset + 3] << 8 | event[attribOffset + 4];
					uint8_t cappedLen = atribValLength; //used to copy into buff
					switch (attribTypeOfField) {
						case AVRCP_MEDIA_ATTRIBUTE_TITLE:

							if (cappedLen > MAX_META_SIZE) {
								cappedLen = MAX_META_SIZE - 1;
							}
							memcpy(&IS2020_trackMeta.title[0], &event[attribOffset + 5], cappedLen);
							IS2020_trackMeta.title[cappedLen + 1] = 0;
							IS2020_trackMeta.title_len = cappedLen;

							//IS2020_trackMeta[MAX_META_SIZE - 1] = 0;
							usb_vcp_send_strn("\r\ntitle:", 8);
							usb_vcp_send_strn(&event[attribOffset + 5], atribValLength);
							break;
						case AVRCP_MEDIA_ATTRIBUTE_ARTIST:
							usb_vcp_send_strn("\r\nartist: ", 9);
							usb_vcp_send_strn(&event[attribOffset + 5], atribValLength);
							break;
						case AVRCP_MEDIA_ATTRIBUTE_ALBUM:
							usb_vcp_send_strn("\r\nalbum:", 8);
							usb_vcp_send_strn(&event[attribOffset + 5], atribValLength);
							break;
						case AVRCP_MEDIA_ATTRIBUTE_TRACK:
							usb_vcp_send_strn("\r\ntrack:", 8);
							usb_vcp_send_strn(&event[attribOffset + 5], atribValLength);
							break;

							//case AVRCP_MEDIA_ATTRIBUTE_LAST: //implicitly due to the defines
						case AVRCP_MEDIA_ATTRIBUTE_DURATION:
							usb_vcp_send_strn("\r\nduration:", 11);
							usb_vcp_send_strn(&event[attribOffset + 5], atribValLength);
							break;

						default:
							usb_vcp_send_strn("\r\not-impl:", 10);
							//usb_vcp_send_strn(&event[attribOffset + 5], atribValLength);
					}
					//update offset to point to next attribute (if any)
#define THREE_NULL_CHARS_TRAILING 3
					attribOffset += 5 + atribValLength + THREE_NULL_CHARS_TRAILING;
				}

				postApplicationEvent(EVT_MUSIC_UPDATED_META);
				usb_vcp_send_strn("meta_resp\r\n", 11);
			}
			/*
			 default:
			 {
			 uint8_t b[] = { 'a', 'v', 'r', 'r', 'e', 's', 'p', ':', 0, 0, '\n' };
			 b[8] = 0x30;
			 b[9] = 0x30;

			 usb_vcp_send_strn(b, 11);

			 }
			 */

		}		//AVRCP
			break;

		case EVT_Read_Linked_Device_Information_Reply:
			switch (event[2]) {

				case 0x00: //reporting of remote device name
				{
					//clear old name
					memset(IS2020_remoteDeviceNames[event[1]], 0x00, MAX_DEV_NAME_LENGTH);
					//for (uint8_t i = 0; i < MAX_DEV_NAME_LENGTH; i++) {
					//	IS2020_remoteDeviceNames[event[1]][i] = 0x00;
					//}

					//set new name
					for (uint8_t i = 3; i < length - 1; i++) { //package-lengt -1
						if (event[i] == 0x00)
							break;
						//break if device-name overflows storage
						if (i == MAX_DEV_NAME_LENGTH - 1)
							break;

						IS2020_remoteDeviceNames[event[1]][i - 3] = event[i];

					}
					//send event to notify a new name
					switch (event[1]) {
						case 0:
							postApplicationEvent(EVT_PHONE_0_REMOTE_NAME_UPDATED);
							break;
						case 1:
							postApplicationEvent(EVT_PHONE_1_REMOTE_NAME_UPDATED);
							break;

					}
				}
					break;

				case 0x01: //reply in-band ringtone status
					;
					break;

				case 0x02: //reply if remote device is a iAP device
					;
					break;

				case 0x03: //reply if remote device support AVRCP v1.3
					IS2020_deviceSupportAvrcpV13[event[1]] = event[3];
					break;

				case 0x04: //reply HF&A2DP gain
					;
					break;

				case 0x05: //reply Line_In gain
					;
					break;

				default:
					;
					break;

			}
			break;

		case EVT_Read_Link_Status_Reply: {
			for (uint8_t i = 0; i < 8; i++) {
				IS2020_linkStatus[i] = event[i + 1]; //link status is array of 7 bytes, like response is 7bytes.
			}

			if (IS2020_linkStatus[1] > 0) {
				//query dev name
				//set displaychar
				//getAVRC_caps
				//getlistplayerattribs
				//postBMcommand(EVT_PHONE_0_CONNECTED);
				IS2020_queryRemoteDeviceName(0);
				//IS2020_avrcpListPlayerAttributes(0);

			} else {
				postBMcommand(EVT_PHONE_0_DISCONNECTED);
				IS2020_queryRemoteDeviceName(1);
			}

			if (IS2020_linkStatus[2] > 0) {
				postBMcommand(EVT_PHONE_1_CONNECTED);
				IS2020_queryRemoteDeviceName(1);
				IS2020_avrcpListPlayerAttributes(1);
			} else {
				postBMcommand(EVT_PHONE_1_DISCONNECTED);
			}
		}
			break;

		default:
			//
			return;

	} //avrcp_specific case

} //switch cmd

void IS2020_queryIfRemoteDeviceSupportAvrcpV13(uint8_t devId) {
	IS2020_readLinkedDeviceInformation(devId,
	CRLDI_query_if_remote_device_support_AVRCP_v13);

}

void IS2020_queryRemoteDeviceName(uint8_t devId) {
	IS2020_readLinkedDeviceInformation(devId, CRLDI_query_device_name);
}

//used for link-info

void IS2020_readLinkedDeviceInformation(uint8_t devId, uint8_t property) {
	IS2020_getnextEvent();
	uint8_t data[1] = { property };
	IS2020_sendArrayInt(0x03, CMD_Read_Linked_Device_Information, devId, data);
	IS2020_getnextEvent();

}

//used to get BTM-status
void IS2020_read_link_status(uint8_t devId) {
	IS2020_sendPacketInt(CMD_Read_Link_Status, DUMMYBYTE);
}
