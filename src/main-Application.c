/*
 * main-Application.c
 *
 *  Created on: Nov 10, 2019
 *      Author: vincent
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "SYSTEM_EVENTS.h"

#define MAX_COMMANDS_TO_QUEUE 10
//static QueueHandle_t xApplicationEvents = { 0 };

/*
 * prototypes
 */
void mainAppManager();
static void taskApplication(void *pvParameters);

void mainAppManager(void) {

#define QUEUE_BLOCK_TIME 200

#define QUEUE_PRIO ( tskIDLE_PRIORITY + 1 )

	xTaskCreate(taskApplication, "APP", configMINIMAL_STACK_SIZE, (void *) 0,
	QUEUE_PRIO, NULL);
}

static void taskApplication(void *pvParameters) {
	TickType_t xNextWakeTime;
	xNextWakeTime = xTaskGetTickCount();

	enum STATE {
		IDLE = 0, MUSIC, INBOUND_CALL, CALLING, VOICE_ASSIST
	};
	uint8_t playing = 0;

	enum STATE appState = IDLE;
	enum STATE lastState = IDLE;

	for (;;) {

		/*
		 * The main application state
		 */
		char evt = EVT_INVALID;

		if (xQueueReceive(xApplicationEvents, &evt, QUEUE_BLOCK_TIME) == pdPASS) {

			/*
			 * GLOBAL events
			 */
			switch (evt) {
			case EVT_PHONE_0_CONNECTED:
				postConsoleEvent(EVT_PHONE_0_CONNECTED);
				break;
			case EVT_PHONE_1_CONNECTED:
				postConsoleEvent(EVT_PHONE_1_CONNECTED);
				break;
			case EVT_PHONE_0_DISCONNECTED:
				postConsoleEvent(EVT_PHONE_0_DISCONNECTED);
				break;
			case EVT_PHONE_1_DISCONNECTED:
				postConsoleEvent(EVT_PHONE_1_DISCONNECTED);
				break;
			}

			switch (appState) {

			/*
			 * state specific
			 */
			case IDLE: {
				switch (evt) {
				case EVT_BTN_SCAN:
					//start playing
					appState = MUSIC;
					postConsoleEvent(EVT_MUSIC_SCREEN);
					postConsoleEvent(EVT_MUSIC_PLAY);
					//xQueueSend(xConsoleEvents, EVT_MUSIC_SCREEN, 0);
					//xQueueSend(xConsoleEvents, EVT_MUSIC_PLAY, 0);
					break;

				case EVT_BTN_1:
					//voice dail
					postConsoleEvent(EVT_CMD_VOICEASSISTANT_START);
					lastState = IDLE;
					appState = VOICE_ASSIST;
					break;

				default:
					//other keys
					break;
				}

			}
				//appstate = idle
				break;

			case VOICE_ASSIST: {
				switch (evt) {

				case EVT_BTN_1:
					postConsoleEvent(EVT_CMD_VOICEASSISTANT_START);
					break;

				case EVT_BTN_3:
					//voice dail
					postConsoleEvent(EVT_CMD_VOICEASSISTANT_CLOSE);
					appState = lastState;
					break;

				default:
					//nothing
					break;

				}
			}
				//voice assist
				break;

			case MUSIC: {
				switch (evt) {
				case EVT_BTN_NEXT:
					postConsoleEvent(EVT_MUSIC_NEXT);
					postBMcommand(EVT_MUSIC_NEXT);
					break;

				case EVT_BTN_PREV:
					postConsoleEvent(EVT_MUSIC_PREVIOUS);
					postBMcommand(EVT_MUSIC_PREVIOUS);
					break;

				case EVT_BTN_SCAN:
					if (playing) {
						postConsoleEvent(EVT_MUSIC_PAUSE);
						postBMcommand(EVT_MUSIC_PAUSE);
						//received back via BM as well
						playing = 0;
					} else {
						postConsoleEvent(EVT_MUSIC_PLAY);
						postBMcommand(EVT_MUSIC_PLAY);
						//received back via BM as well
						playing = 1;
					}

					break;

				case EVT_BTN_RND:
					//command never send by Hu, only for DBG purposes to emulate 'screen'
					postConsoleEvent(EVT_BTN_RND);
					break;

				case EVT_CALL_INBOUND:
					//inbound call,
					appState = INBOUND_CALL;
					lastState = MUSIC;
					break;

					/*
					 * These come from BT module
					 * DONT REPLY TO THESE by sending events to BM, as it creates a
					 * LOOP
					 */
				case EVT_MUSIC_PAUSE:
					postConsoleEvent(EVT_MUSIC_PAUSE);
					playing = 0;
					//postBMcommand(EV)
					break;

				case EVT_MUSIC_PLAY:
					postConsoleEvent(EVT_MUSIC_PLAY);
					playing = 1;
					break;

					/*
					 * DEBUG cmds
					 */

				case EVT_DGB_Q:
					postBMcommand(EVT_MUSIC_META);
					break;

				default:
					//nothing
					break;
				}
			}
				//appstate = music
				break;

			case INBOUND_CALL:
				//when inbound call is pending
				switch (evt) {
				case EVT_BTN_1:
					//answer
					postConsoleEvent(EVT_CALL_ANSWER);
					appState = CALLING;
					break;

				case EVT_BTN_3:
					//reject
					postConsoleEvent(EVT_CALL_REJECT);
					break;

				case EVT_CALL_FAILED:
					//resume last task
					appState = lastState;
				}

				//appstate = inbound_call
				break;

			case CALLING:
				switch (evt) {

				case EVT_BTN_3:
					//hangup
					postConsoleEvent(EVT_CALL_HANGUP);
					break;

				}
				break;

			}
		}

	}
}
