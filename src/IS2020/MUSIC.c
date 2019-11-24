/*
 * MUSIC.c
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#include "IS2020.h"
#include "MUSIC.h"
#include <stdint.h>

void IS2020_MUSIC_CMD(uint8_t cmd, uint8_t devId, void  funct()){

	switch(cmd){

	case eMUSIC_NEXT:
		IS2020_sendArrayInt(0x03, CMD_Music_Control, devId, Music_Control_FFW, funct  );
		break;

	case eMUSIC_PREVIOUS:

		break;

	case eMUSIC_PLAY:

		break;

	case eMUSIC_PAUSE:

		break;

	default:

		break;



	}
}


