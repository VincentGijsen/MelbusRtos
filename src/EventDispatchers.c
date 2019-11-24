/*
 * EventDispatchers.c
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <SYSTEM_EVENTS.h>

#include <stdint.h>
//method to send event codes to application

void postApplicationEvent(uint8_t event) {
	uint8_t e = event;
	xQueueSend(xApplicationEvents, &e, 0);
}

void postConsoleEvent(uint8_t event) {
	uint8_t elem = event;
	xQueueSend(xConsoleEvents, &elem, 0);
}

void postBMcommand(uint8_t event){
	uint8_t elem = event;
	xQueueSend(xBMCommands, &elem, 0);
}

