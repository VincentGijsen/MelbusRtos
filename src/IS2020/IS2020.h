/*
 * IS2020.h
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#ifndef IS2020_IS2020_H_
#define IS2020_IS2020_H_
#include <stdint.h>

/*
 * prototypes
 */
void IS2020_sendArrayInt(uint8_t pkgSize, uint8_t cmd, uint8_t devId, uint8_t data[], void funct());

void IS2020_decode(uint8_t length, uint8_t* event);

#define IS2020_EVENT_IDX_CMD 0
#define IS2020_EVENT_IDX_DEVID (IS2020_EVENT_IDX_CMD+1)



#define EVT_Command_ACK 0x00
#define EVT_BTM_Status  0x01
#define EVT_Call_Status 0x02
#define EVT_Caller_ID 0x03
#define EVT_SMS_Received_Indication 0x04
#define EVT_Missed_Call_Indication  0x05
#define EVT_Phone_Max_Battery_Level 0x06
#define EVT_Phone_Current_Battery_Level 0x07
#define EVT_Roaming_Status  0x08
#define EVT_Phone_Max_Signal_Strength_Level 0x09
#define EVT_Phone_Current_Signal_Strength_Level 0x0A
#define EVT_Phone_Service_Status  0x0B
#define EVT_BTM_Battery_Status  0x0C
#define EVT_BTM_Charging_Status 0x0D
#define EVT_Reset_To_Default  0x0E
#define EVT_Report_HF_Gain_Level  0x0F
#define EVT_EQ_Mode_Indication  0x10
#define EVT_PBAP_Missed_Call_History  0x11
#define EVT_PBAP_Received_Call_History  0x12
#define EVT_PBAP_Dialed_Call_History  0x13
#define EVT_PBAP_Combine_Call_History 0x14
#define EVT_Get_PB_By_AT_Cmd_Reply  0x15 //PhoneBookContacts
#define EVT_PBAP_Access_Finish  0x16
#define EVT_Read_Linked_Device_Information_Reply  0x17
#define EVT_Read_BTM_Version_Reply  0x18
#define EVT_Call_List_Report  0x19
#define EVT_AVRCP_Specific_Rsp  0x1A
#define EVT_BTM_Utility_Req 0x1B
#define EVT_Vendor_AT_Cmd_Reply 0x1C
#define EVT_Report_Vendor_AT_Event  0x1D
#define EVT_Read_Link_Status_Reply  0x1E
#define EVT_Read_Paired_Device_Record_Reply 0x1F
#define EVT_Read_Local_BD_Address_Reply 0x20
#define EVT_Read_Local_Device_Name_Reply  0x21
#define EVT_Report_SPP_iAP_Data 0x22
#define EVT_Report_Link_Back_Status 0x23
#define EVT_Ringtone_Finish_Indicate  0x24
#define EVT_User_Confrim_SSP_Req  0x25
#define EVT_Report_AVRCP_Vol_Ctrl 0x26
#define EVT_Report_Input_Signal_Level 0x27
#define EVT_Report_iAP_Info 0x28
#define EVT_REPORT_AVRCP_ABS_VOL_CTRL 0x29
#define EVT_Report_Voice_Prompt_Status  0x2A
#define EVT_Report_MAP_Data 0x2B
#define EVT_Security_Bonding_Res  0x2C
#define EVT_Report_Type_Codec 0x2D

#define A2DP_profile_signaling_channel_connected 0x01 //00000001b
#define A2DP_profile_stream_channel_connected 0x02 //00000010b
#define AVRCP_profile_connected 0x04 //b00000100
#define HF_profile_connected 0x08 //b000001000
#define SPP_connected 0x10 //b00010000



#endif /* IS2020_IS2020_H_ */
