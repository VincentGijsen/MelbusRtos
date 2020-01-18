/*
 * MUSIC.c
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#include "IS2020.h"
#include "MUSIC.h"
#include "NMI.h"
#include <stdint.h>

void IS2020_MUSIC_CMD(uint8_t cmd, uint8_t devId) {

	switch (cmd) {
	uint8_t data[1] = { 0 };
case eMUSIC_NEXT:
	data[0] = MMI_next_song;
	IS2020_sendArrayInt(0x03, CMD_MMI_Action, devId, data);
	break;

case eMUSIC_PREVIOUS:
	data[0] = MMI_previous_song;
	IS2020_sendArrayInt(0x03, CMD_MMI_Action, devId, data);
	break;

case eMUSIC_PLAY:
	data[0] = Music_Control_PLAY;
	IS2020_sendArrayInt(0x03, CMD_Music_Control, devId, data);
	break;

case eMUSIC_PAUSE:
	data[0] = Music_Control_PAUSE;
	IS2020_sendArrayInt(0x03, CMD_Music_Control, devId, data);

	break;

default:

	break;

	}
}

