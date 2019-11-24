/*
 * SYSTEM_EVENTS.h
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern QueueHandle_t xApplicationEvents;
extern QueueHandle_t xConsoleEvents;
extern QueueHandle_t xScreenEvents;
extern QueueHandle_t xBMCommands;

//event dispatchers
extern void postApplicationEvent(uint8_t event);
extern void postConsoleEvent(uint8_t event);
extern void postBMcommand(uint8_t event);



enum MYEVENTS {
	EVT_INVALID = 0,

	EVT_CALL_INBOUND,
	EVT_CALL_OUTBOUND,
	EVT_CALL_ANSWER,
	EVT_CALL_REJECT,
	EVT_CALL_FAILED,
	EVT_CALL_HANGUP,

	EVT_CMD_VOICEASSISTANT_START,
	EVT_CMD_VOICEASSISTANT_CLOSE,

	EVT_MUSIC_SCREEN,
	EVT_MUSIC_NEXT,
	EVT_MUSIC_PREVIOUS,
	EVT_MUSIC_FF,
	EVT_MUSIC_FR,
	EVT_MUSIC_PLAY,
	EVT_MUSIC_PAUSE,
	EVT_MUSIC_STOP,

	EVT_SETTINGS_PAIRING,
	EVT_SETTINGS_PAIRING_INIT,

	/*
	 * BTN mapping to volvo HU-,
	 */
	EVT_BTN_NEXT,
	EVT_BTN_PREV,
	EVT_BTN_SCAN,
	EVT_BTN_RND,
	EVT_BTN_1,
	EVT_BTN_2,
	EVT_BTN_3,
	EVT_BTN_4,
	EVT_BTN_5,
	EVT_BTN_6

};
