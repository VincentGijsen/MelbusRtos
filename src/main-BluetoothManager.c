/*
 * main-BluetoothManager.c
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "SYSTEM_EVENTS.h"

#include "IO.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>

#include "IS2020/MUSIC.h"
#include "IS2020/IS2020.h"

/* Queues used to hold characters waiting to be transmitted - one queue per port. */
static QueueHandle_t xCharsForTxBM = { 0 };

/* Queues holding received characters - one queue per port. */
static QueueHandle_t xRxedCharsBM = { 0 };

/* Queues are used to hold characters that are waiting to be transmitted.  This
 constant sets the maximum number of characters that can be contained in such a
 queue at any one time. */
#define serTX_QUEUE_LENBM					( 50 )

/* Queues are used to hold characters that have been received but not yet
 processed.  This constant sets the maximum number of characters that can be
 contained in such a queue. */
#define serRX_QUEUE_LENBM					( 50)

/*Prototypes */
static void taskBMManager(void *pvParameters);
static void usart2_setup();
signed long xSendcharToBM(uint8_t data, TickType_t xBlockTime);

void writeSerial(uint8_t b){
	//xQueueSend(xCharsForTxBM, &b, 0);
	xSendcharToBM(b, 50);
}

void mainBM(void) {

	/* Create the queue of chars that are waiting to be sent to COM0. */
	xCharsForTxBM = xQueueCreate(serTX_QUEUE_LENBM, sizeof(char));

	/* Create the queue used to hold characters received from COM0. */
	xRxedCharsBM = xQueueCreate(serRX_QUEUE_LENBM, sizeof(char));
	usart2_setup();

#define PRIO ( tskIDLE_PRIORITY + 1 )

	xTaskCreate(taskBMManager, "BM", configMINIMAL_STACK_SIZE, (void *) 0, PRIO,
	NULL);

}

static uint8_t event[50] = { 0 };

static void taskBMManager(void *pvParameters) {
	TickType_t xNextWakeTime;

	//static char cls[] = { 27, '[', '2', 'J', 27, '[', 'H' }; /*terminal command to clear it */
	//char c = ' ';

	xNextWakeTime = xTaskGetTickCount();
	uint8_t awaiting_pkg_size = 0, tmp = 0;

	for (;;) {

		if ((awaiting_pkg_size == 0)
				&& (uxQueueMessagesWaiting(xRxedCharsBM) > 3)) {
			//check if first byte is STAR of msg
			uint8_t preamble = 0;
			if (xQueueReceive(xRxedCharsBM, &preamble, 20) == pdPASS) {
				//check-preamble
				if (preamble == 0xAA) {
					//event[0] = preamble;
					xQueueReceive(xRxedCharsBM, &tmp, 20);
					awaiting_pkg_size = tmp << 8;
					//event[1] = tmp;
					xQueueReceive(xRxedCharsBM, &tmp, 20);
					awaiting_pkg_size += tmp & 0xFF;
					//event[2] = tmp;

				}

			}
		}
		// check if all data has arived
		if (awaiting_pkg_size > 0
				&& uxQueueMessagesWaiting(xRxedCharsBM)
						>= awaiting_pkg_size + 1) {

			for (uint8_t x = 0; x < awaiting_pkg_size + 1; x++) {
				xQueueReceive(xRxedCharsBM, &tmp, 20);
				event[x] = tmp;
			}

			//shift event over to IS2020 driver
			//add +3 (0xaa + 2, +1 checksum)
			IS2020_decode(awaiting_pkg_size + 1, &event);

			//reset pkg-size, a
			awaiting_pkg_size = 0;


			/*
			 if (event[3] == 0x00) {
			 //got ack
			 tmp = event[0];
			 }
			 */

		}

		/*
		 * Handle commands from app
		 */
		uint8_t evt = 0;
		if (xQueueReceive(xBMCommands, &evt, 0) == pdPASS) {

			switch (evt) {
			case EVT_MUSIC_NEXT:
				IS2020_MUSIC_CMD(eMUSIC_NEXT,0, writeSerial);
				break;

			case EVT_MUSIC_PREVIOUS:

				break;

			case EVT_MUSIC_PAUSE:

				break;

			default:

				break;

			}
		}

	}

}


signed long xSendcharToBM(uint8_t data, TickType_t xBlockTime) {
	long lReturn;

	if ( xQueueSend( xCharsForTxBM, &data, xBlockTime ) == pdPASS) {
		lReturn = pdPASS;
		//USART_ITConfig( xUARTS[ lPort ], USART_IT_TXE, ENABLE );
		/* Enable transmit interrupt so it sends back the data. */
		USART_CR1(USART2) |= USART_CR1_TXEIE;
	} else {
		lReturn = pdFAIL;
	}

	return lReturn;
}


static void usart2_setup() {
	nvic_enable_irq(NVIC_USART2_IRQ);

	/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port B for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
	GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

	/* Setup GPIO pin GPIO_USART1_RE_RX on GPIO port B for receive. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
	GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART2, USART_MODE_TX_RX);

	/* Enable USART1 Receive interrupt. */
	USART_CR1(USART2) |= USART_CR1_RXNEIE;

	/* Finally enable the USART. */
	usart_enable(USART2);

}

void USART2_IRQHandler(void) {
	long xHigherPriorityTaskWoken = pdFALSE;
	static uint8_t data = 'A';

	/* Check if we were called because of RXNE. */
	if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0)
			&& ((USART_SR(USART2) & USART_SR_RXNE) != 0)) {

		/* Indicate that we got data. */
		//gpio_toggle(GPIOC, LED_PIN);
		/* Retrieve the data from the peripheral. */
		data = usart_recv(USART2);

		xQueueSendFromISR(xRxedCharsBM, &data, &xHigherPriorityTaskWoken);

	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(USART2) & USART_CR1_TXEIE) != 0)
			&& ((USART_SR(USART2) & USART_SR_TXE) != 0)) {

		/* Indicate that we are sending out data. */
		// gpio_toggle(GPIOA, GPIO7);
		/* Put data into the transmit register. */
		//usart_send(USART1, data);
		/* The interrupt was caused by the THR becoming empty.  Are there any
		 more characters to transmit? */
		if (xQueueReceiveFromISR(xCharsForTxBM, &data,
				&xHigherPriorityTaskWoken)) {
			/* A character was retrieved from the buffer so can be sent to the
			 THR now. */
			usart_send( USART2, data);
		} else {
			/* Disable the TXE interrupt as we don't need it anymore. */
			USART_CR1(USART2) &= ~USART_CR1_TXEIE;

			//USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
		}
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

}
