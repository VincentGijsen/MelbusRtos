/*
 * main-BluetoothManager.c
 *
 *  Created on: Nov 24, 2019
 *      Author: vincent
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "SYSTEM_EVENTS.h"
#include "QueueLimits.h"
#include "IO.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>

#include "IS2020/MUSIC.h"
#include "IS2020/IS2020.h"

//SemaphoreHandle_t xSemaphorePkgsOut;
static uint8_t flagPkgOut = false;
/* Queues used to hold characters waiting to be transmitted - one queue per port. */
static QueueHandle_t xQueueBluetoothModuleTX = { 0 };

/* Queues holding received characters - one queue per port. */
static QueueHandle_t xQueueBluetoothModuleRX = { 0 };




/*Prototypes */
static void taskBMManager(void *pvParameters);
static void usart2_setup();
signed long xSendcharToBM(uint8_t data, TickType_t xBlockTime);


void mainBM(void) {

	/* Create the queue of chars that are waiting to be sent to COM0. */
	xQueueBluetoothModuleTX = xQueueCreate(QUEUELIMIT_BLUETOOTH_MODULE_TX, sizeof(char));

	/* Create the queue used to hold characters received from COM0. */
	xQueueBluetoothModuleRX = xQueueCreate(QUEUELIMIT_BLUETOOTH_MODULE_RX, sizeof(char));
	usart2_setup();

#define PRIO ( tskIDLE_PRIORITY + 1 )
	unsigned short STACK_SIZE = configMINIMAL_STACK_SIZE + 60;
	xTaskCreate(taskBMManager, "BM", STACK_SIZE, (void *) 0, PRIO,
	NULL);

}

static void taskBMManager(void *pvParameters) {
	TickType_t xNextWakeTime;

	xNextWakeTime = xTaskGetTickCount();
	//IS2020_setIO(writeSerial, checkOutActivate);
	IS2020_setQueues(xQueueBluetoothModuleRX, xQueueBluetoothModuleTX);

	for (;;) {

		//	IS2020_getnextEvent();

		/*
		 * Handle commands from app
		 */
		uint8_t evt = 0;
		if (xQueueReceive(xBMCommands, &evt, 0) == pdPASS) {

			switch (evt) {
				case EVT_MUSIC_NEXT:
					IS2020_MUSIC_CMD(eMUSIC_NEXT, 0);
					break;

				case EVT_MUSIC_PREVIOUS:
					IS2020_MUSIC_CMD(eMUSIC_PREVIOUS, 0);
					//vTaskDelay(500);
					//IS2020_AVRCP_DISPLAYABLE_CHARS(writeSerial);
					//vTaskDelay(500);
					//IS2020_AVRCP_REGISTER_ALL_EVENTS(0);

					break;

				case EVT_MUSIC_PAUSE:
					IS2020_MUSIC_CMD(eMUSIC_PAUSE, 0);
					break;

				case EVT_MUSIC_PLAY:
					IS2020_MUSIC_CMD(eMUSIC_PLAY, 0);
					break;

				case EVT_MUSIC_META:
					//IS2020_AVRCP_DISPLAYABLE_CHARS(writeSerial);
					//vTaskDelay(100);
					//usb_vcp_send_strn("meta\n",5);
					IS2020_AVRCP_META();
					//IS2020_AVRCP_GETPLAY_STATUS(writeSerial);
					break;

				case EVT_PHONE_0_CONNECTED:
					//query info
					IS2020_read_link_status(0);
					postApplicationEvent(EVT_PHONE_0_CONNECTED);
					IS2020_AVRCP_REGISTER_ALL_EVENTS(0);

					break;

				case EVT_PHONE_0_DISCONNECTED:
					postApplicationEvent(EVT_PHONE_0_DISCONNECTED);
					break;

				case EVT_PHONE_1_CONNECTED:
					IS2020_read_link_status(1);
					postApplicationEvent(EVT_PHONE_1_CONNECTED);
					break;

				case EVT_PHONE_1_DISCONNECTED:
					postApplicationEvent(EVT_PHONE_1_DISCONNECTED);
					break;

				case EVT_BATTERY_STATUS_CHANGED:
					//do someghing
					break;

				case EVT_ROAMING_CHANGED:
					//do something
					break;

				case EVT_SIGNAL_STRENGHT_CHANGED:
					//do something
				{

				}
					break;

				default:

					break;

			}
		}

		/*
		 * Check if TXbuffer has data, ifso, flush to Bluetooth-receiver
		 */
		if (uxQueueMessagesWaiting(xQueueBluetoothModuleTX) > 0) {
			//pass start-command
			USART_CR1(USART2) |= USART_CR1_TXEIE;
		}

		/*
		 * check for new events
		 */
		IS2020_getnextEvent();
	}

}
/*
 signed long xSendcharToBM(uint8_t data, TickType_t xBlockTime) {
 long lReturn;

 if ( xQueueSend( xCharsForTxBM, &data, xBlockTime ) == pdPASS) {
 lReturn = pdPASS;
 //USART_ITConfig( xUARTS[ lPort ], USART_IT_TXE, ENABLE );
 /// Enable transmit interrupt so it sends back the data.
 } else {
 lReturn = pdFAIL;
 }

 return lReturn;
 }
 */

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
	if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) && ((USART_SR(USART2) & USART_SR_RXNE) != 0)) {

		/* Indicate that we got data. */
		//gpio_toggle(GPIOC, LED_PIN);
		/* Retrieve the data from the peripheral. */
		data = usart_recv(USART2);
		//prevent pushing to much
		if (uxQueueMessagesWaitingFromISR(xQueueBluetoothModuleRX) < (QUEUELIMIT_BLUETOOTH_MODULE_RX - 1)) {
			xQueueSendFromISR(xQueueBluetoothModuleRX, &data, &xHigherPriorityTaskWoken);
		}

	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(USART2) & USART_CR1_TXEIE) != 0) && ((USART_SR(USART2) & USART_SR_TXE) != 0)) {

		/* Indicate that we are sending out data. */
		// gpio_toggle(GPIOA, GPIO7);
		/* Put data into the transmit register. */
		//usart_send(USART1, data);
		/* The interrupt was caused by the THR becoming empty.  Are there any
		 more characters to transmit? */
		if (xQueueReceiveFromISR(xQueueBluetoothModuleTX, &data, &xHigherPriorityTaskWoken)) {
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
