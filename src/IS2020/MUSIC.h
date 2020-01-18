/*
 * MUSIC.h
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#ifndef IS2020_MUSIC_H_
#define IS2020_MUSIC_H_


enum ENUM_MUSIC_CMD {
	eMUSIC_NEXT =0,
	eMUSIC_PREVIOUS,
	eMUSIC_PAUSE,
	eMUSIC_PLAY
};

#define CRLDI_query_device_name 0x00
#define CRLDI_query_in_band_ringtone_status 0x01
#define CRLDI_query_if_remote_device_is_iAP_device 0x02
#define CRLDI_query_if_remote_device_support_AVRCP_v13 0x03
#define CRLDI_query_HF_A2DP_gain 0x04
#define CRLDI_query_Line_In_gain 0x05

/*commands on music-CMD*/

#define Music_Control_STOP_FFW_RWD 0x00 // Stop fast forward or rewind
#define Music_Control_FFW  0x01  //fast forward
#define Music_Control_REPFFW 0x02  //fast forward with repeat send fast forward command every 800ms
#define Music_Control_RWD 0x03  //rewind
#define Music_Control_REPRWD 0x04  //rewind with repeat send rewind command every 800ms
#define Music_Control_PLAY 0x05  //PLAY command
#define Music_Control_PAUSE 0x06  //PAUSE command
#define Music_Control_TOGLE_PLAY_PAUSE 0x07  //PLAY PAUSE toggle
#define Music_Control_STOP 0x08  //STOP command




#endif /* IS2020_MUSIC_H_ */
