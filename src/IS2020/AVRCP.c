/*
 * AVRCP.c
 *
 *  Created on: Nov 25, 2019
 *      Author: vincent
 */
#include "AVRCP.h"
#include "IS2020.h"
#include <stdlib.h>

void IS2020_SET_PIN() {
	uint8_t data[] = { 0, 'x', 'c', 'a', 'r' }; //2,3,4
	size_t s = sizeof(data) / sizeof(uint8_t);
	uint8_t pkgLen = s + 1;
	//IS2020_sendArrayInt(pkgLen, 0x06, 1, data);
	IS2020_sendArrayIntGeneric(pkgLen, 0x06, data);
}

void IS2020_AVRCP_REQUEST_META() {
	/*GET ELEMENT ATTRIBUTES */
	//AVR1.6
#define PDU 0x20
	/*
	 uint8_t data[] = { PDU,
	 0x00,
	 0x00, 0x11,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x02,
	 0x00,0x00,0x00,0x01,
	 0x00,0x00,0x00,0x02

	 };
	 */
	/*
	 uint8_t data[] = {
	 AVRCP_GET_ELEMENT_ATTRIBUTES,
	 0,
	 0,0x0d,
	 0,0,0,0,
	 2,
	 0,0,0,0,0,0,0,1,
	 0,0,0x07};
	 */

	uint8_t data[] = {
	AVRCP_GET_ELEMENT_ATTRIBUTES,
			0,
			0, 0x0d,
			0, 0, 0, 0,
			1,
			0, 0, 0, 0, 0, 0, 0, 1,
	//0,0,0x07};
			};

	usb_vcp_send_strn("meta\n", 5);
	/*
	 uint8_t data[45] = {
	 //AVRCP_GET_ELEMENT_ATTRIBUTES, //6
	 0x00, //7 Reserved
	 0x00, 0x29, // 8-9 D => 13 bytes
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //10,11,12,13,14,15,16,17 Identifier 8
	 0x02, //14 NumAttributes (N) 1
	 0x00, 0x00, 0x00, 0x01, //15,16,17,18,19,20,21,22
	 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
	 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
	 0x00, 0x07, 0x00, 0x00, 0x00, 0x08
	 }
	 ;
	 */
//23,24,25,26
	size_t s = sizeof(data) / sizeof(uint8_t);
	uint8_t pkgLen = s + 2;
	IS2020_sendArrayInt(pkgLen, CMD_AVRCP_Specific_Cmd, 0, data);
}

void IS2020_AVRCP_DISPLAYABLE_CHARS(uint8_t devId) {
	uint8_t data[] = { AVRCP_DISPLAYABLE_CHARSET, //6
			0x00, //7
			0x00, 0x02, //size
			0x00, 0x06

	};
	size_t s = sizeof(data) / sizeof(uint8_t);
	uint8_t pkgLen = s + 2;

	IS2020_sendArrayInt(pkgLen, CMD_AVRCP_Specific_Cmd, devId, data);

}
//mandatory in avrcp1.5
void IS2020_AVRCP_REGISTER_NOTIFICATION_TRACKCHANGED(uint8_t devId){
	IS2020_AVRCP_REGISTER_NOTIFICATION(devId, AVRCP_EVENT_TRACK_CHANGED,0,0,0,0);
}
//optional in avrcp1.5
void IS2020_AVRCP_REGISTER_NOTIFICATION_POSCHANGED(uint8_t devId){
	IS2020_AVRCP_REGISTER_NOTIFICATION(devId, AVRCP_EVENT_PLAYBACK_POS_CHANGED,0,0,0,30); //notify every 30 sec
}
//avrcp1.5 p141
void IS2020_AVRCP_REGISTER_NOTIFICATION_AVAILABLE_PLAYERS(uint8_t devId){
	IS2020_AVRCP_REGISTER_NOTIFICATION(devId, AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED, 0,0,0,0);
}


void IS2020_AVRCP_REGISTER_NOTIFICATION(uint8_t deviceId, uint8_t event, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4) {
	IS2020_getnextEvent();

	uint8_t data[9] = { AVRCP_REGISTER_NOTIFICATION, //PDU ID (0x31 – Register Notification)
			0x00, //reserved
			0x00, 0x05, // Parameter Length (0x5)
			event, //EventID
			param1, param2, param3, param4 };

	IS2020_sendArrayInt(11, CMD_AVRCP_Specific_Cmd, deviceId, data);

	//get ACK
	vTaskDelay(12);
	IS2020_getnextEvent();
	//max wait for register-notification-ack avrcp1.5 page138
	vTaskDelay(200);
	//this should be intrim message
	IS2020_getnextEvent();
	vTaskDelay(50);
		//for good measure
	IS2020_getnextEvent();

}

/*
 void IS2020_AVRCP_GETFOLDERITEMS(void out()) {
 uint8_t data[7] = { AVRCP_GET_PLAYER_VALUE_TEXT, //6
 0x00, //7 Reserved
 0x00, 0x01, //size
 attribute, 0x01, setting };

 IS2020
 ::sendPacketArrayInt(8, CMD_AVRCP_Specific_Cmd, deviceId, data);
 }
 */

void IS2020_AVRCP_GETPLAY_STATUS(uint8_t devId) {
	IS2020_sendPacketInt(0x30, devId);
}

void IS2020_AVRCP_REGISTER_ALL_EVENTS(uint8_t deviceId) {
//IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_STATUS_CHANGED, 0x00, 0x00, 0x00, 0x00, out);
	IS2020_AVRCP_REGISTER_NOTIFICATION_AVAILABLE_PLAYERS(0);
	IS2020_AVRCP_REGISTER_NOTIFICATION_POSCHANGED(0);
	IS2020_AVRCP_REGISTER_NOTIFICATION_TRACKCHANGED(0);

	/*
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_TRACK_CHANGED, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_TRACK_REACHED_END, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_TRACK_REACHED_START, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_PLAYBACK_POS_CHANGED, 0x00, 0x00, 0x13, 0x88);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_BATT_STATUS_CHANGED, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_SYSTEM_STATUS_CHANGED, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_UIDS_CHANGED, 0x00, 0x00, 0x00, 0x00);
	vTaskDelay(200);
	IS2020_AVRCP_REGISTER_NOTIFICATION(deviceId, AVRCP_EVENT_VOLUME_CHANGED, 0x00, 0x00, 0x00, 0x00);
	*/

}

void IS2020_avrcpListPlayerAttributes(uint8_t devId) {
	IS2020_getnextEvent();

	uint8_t data[5] = { AVRCP_LIST_PLAYER_ATTRIBUTES, //6
			0x00, //7 Reserved
			0x00, 0x01, 0x00 };
	IS2020_sendArrayInt(7, CMD_AVRCP_Specific_Cmd, devId, data);
	IS2020_getnextEvent();
}

void IS2020_AVRCP_GetFolderItems(uint8_t dev, uint8_t scope, uint8_t start[4], uint8_t end[4],uint8_t count, uint8_t* attribList){

}

