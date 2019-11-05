#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "IO.h"
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>

/*
 * Prototypes
 */
static void taskConsoleManager(void *pvParameters);
static void usart_setup(void);
signed long xSerialGetChar(signed char *pcRxedChar, TickType_t xBlockTime);
long lSerialPutString(const char * const pcString, unsigned long ulStringLength);
signed long xSerialPutChar(uint8_t data, TickType_t xBlockTime);

/*
 * Queues
 */

/* Queues used to hold characters waiting to be transmitted - one queue per port. */
static QueueHandle_t xCharsForTx = { 0 };

/* Queues holding received characters - one queue per port. */
static QueueHandle_t xRxedChars = { 0 };

/* Queues are used to hold characters that are waiting to be transmitted.  This
constant sets the maximum number of characters that can be contained in such a
queue at any one time. */
#define serTX_QUEUE_LEN					( 10 )

/* Queues are used to hold characters that have been received but not yet
processed.  This constant sets the maximum number of characters that can be
contained in such a queue. */
#define serRX_QUEUE_LEN					( 10 )

/* The maximum amount of time that calls to lSerialPutString() should wait for
 there to be space to post each character to the queue of characters waiting
 transmission.  NOTE!  This is the time to wait per character - not the time to
 wait for the entire string. */
#define serPUT_STRING_CHAR_DELAY		( 5 / portTICK_PERIOD_MS )

void mainConsoleManager(void) {

	usart_setup();

	/* Create the queue of chars that are waiting to be sent to COM0. */
	xCharsForTx = xQueueCreate(serTX_QUEUE_LEN, sizeof(char));

	/* Create the queue used to hold characters received from COM0. */
	xRxedChars = xQueueCreate(serRX_QUEUE_LEN, sizeof(char));

#define PRIO ( tskIDLE_PRIORITY + 1 )

	xTaskCreate(taskConsoleManager, "console", configMINIMAL_STACK_SIZE,
			(void *) 0, PRIO, NULL);

}

static void taskConsoleManager(void *pvParameters) {

	TickType_t xNextWakeTime;

	static char cls[] = { 27, '[', '2','J', 27, '[', 'H'};
	char data[1] = { ' '};
	data[0] = 'h';
	data[1] = ' ';

	xNextWakeTime = xTaskGetTickCount();
	for (;;) {
		if (xSerialGetChar(&data, 1) == pdPASS) {
			lSerialPutString(cls, 7);
			//vTaskDelayUntil( &xNextWakeTime, 100 );

			lSerialPutString(&data, 1);
			gpio_toggle(GPIOC, LED_PIN);

		}
	}

}

static void usart_setup() {
	/*
	 Enable the USART1 interrupt. */
	nvic_enable_irq(NVIC_USART1_IRQ);

	/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port B for transmit. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
	GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup GPIO pin GPIO_USART1_RE_RX on GPIO port B for receive. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
	GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);

	/* Enable USART1 Receive interrupt. */
	USART_CR1(USART1) |= USART_CR1_RXNEIE;

	/* Finally enable the USART. */
	usart_enable(USART1);

}

signed long xSerialGetChar(signed char *pcRxedChar, TickType_t xBlockTime) {
	long lReturn = pdFAIL;

	if (xQueueReceive(xRxedChars, pcRxedChar, xBlockTime) == pdPASS) {
		lReturn = pdPASS;
	}

	return lReturn;
}

long lSerialPutString(const char * const pcString, unsigned long ulStringLength) {
	long lReturn;
	unsigned long ul;

	lReturn = pdPASS;

	for (ul = 0; ul < ulStringLength; ul++) {
		if ( xQueueSend(xCharsForTx, &(pcString[ul]),
				serPUT_STRING_CHAR_DELAY) != pdPASS) {
			/* Cannot fit any more in the queue.  Try turning the Tx on to
			 clear some space. */
			USART_CR1(USART1) |= USART_CR1_TXEIE;

			vTaskDelay( serPUT_STRING_CHAR_DELAY);

			/* Go back and try again. */
			continue;
		}

	}
	USART_CR1(USART1) |= USART_CR1_TXEIE;

	return lReturn;
}

signed long xSerialPutChar(uint8_t data, TickType_t xBlockTime) {
	long lReturn;

	if ( xQueueSend( xCharsForTx, &data, xBlockTime ) == pdPASS) {
		lReturn = pdPASS;
		//USART_ITConfig( xUARTS[ lPort ], USART_IT_TXE, ENABLE );
		/* Enable transmit interrupt so it sends back the data. */
		USART_CR1(USART1) |= USART_CR1_TXEIE;
	} else {
		lReturn = pdFAIL;
	}

	return lReturn;
}

void USART1_IRQHandler(void) {
	long xHigherPriorityTaskWoken = pdFALSE;
	static uint8_t data = 'A';

	/* Check if we were called because of RXNE. */
	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0)
			&& ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {

		/* Indicate that we got data. */
		gpio_toggle(GPIOC, LED_PIN);

		/* Retrieve the data from the peripheral. */
		data = usart_recv(USART1);

		xQueueSendFromISR(xRxedChars, &data, &xHigherPriorityTaskWoken);

	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(USART1) & USART_CR1_TXEIE) != 0)
			&& ((USART_SR(USART1) & USART_SR_TXE) != 0)) {

		/* Indicate that we are sending out data. */
		// gpio_toggle(GPIOA, GPIO7);
		/* Put data into the transmit register. */
		//usart_send(USART1, data);
		/* The interrupt was caused by the THR becoming empty.  Are there any
		 more characters to transmit? */
		if (xQueueReceiveFromISR(xCharsForTx, &data,
				&xHigherPriorityTaskWoken)) {
			/* A character was retrieved from the buffer so can be sent to the
			 THR now. */
			usart_send( USART1, data);
		} else {
			/* Disable the TXE interrupt as we don't need it anymore. */
			USART_CR1(USART1) &= ~USART_CR1_TXEIE;

			//USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
		}
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

}

