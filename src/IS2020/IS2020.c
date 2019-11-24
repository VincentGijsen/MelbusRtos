/*
 * IS2020.c
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#include "IS2020.h"
#include "AVRCP.h"
#include "SYSTEM_EVENTS.h"

void IS2020_sendArrayInt(uint8_t pkgSize, uint8_t cmd, uint8_t devId, uint8_t data[], void funct()){
	uint8_t checksum =0;
	funct(0xAA);
	funct(pkgSize >>8);
	checksum = pkgSize>>8;
	funct(pkgSize & 0xFF);
	checksum += pkgSize & 0xFF;
	funct(cmd);
	checksum += cmd;
	funct(devId);
	checksum += devId;

	for (uint16_t x=0; x< pkgSize -2; x++){
		funct(data[x]);
		checksum += data[x];
	}
	funct(0x100 - (checksum % 0x100));

}

void IS2020_decode(uint8_t length, uint8_t* event) {

	uint8_t OPS = event[IS2020_EVENT_IDX_CMD];
	switch (OPS) {
	case EVT_Command_ACK:
		//cmd
	{
		uint8_t ack = 1;
	}
		break;
	case EVT_BTM_Status:

		break;

	case EVT_Call_Status:

		break;

	case EVT_Caller_ID:

		break;

	case EVT_AVRCP_Specific_Rsp:
	{
		uint8_t devId = event[IS2020_EVENT_IDX_DEVID];
		uint8_t paramLen = (event[10] << 8) + event[11];
		uint8_t a = 1;

		if (event[8] == AVRCP_REGISTER_NOTIFICATION) {
			if (event[12] == AVRCP_EVENT_STATUS_CHANGED) {


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
		}
	}
		break;

	case EVT_Phone_Current_Battery_Level:

		break;

	case EVT_Phone_Current_Signal_Strength_Level:
		//
		break;

	default:
		//
		return;
	}

}
