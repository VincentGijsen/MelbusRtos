/*
 * main-ScreenManager.c
 *
 *  Created on: Nov 4, 2019
 *      Author: vincent
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "IO.h"
#include "libopencm3/stm32/gpio.h"

#define SCREENLOOP_INTERVAL 1000

/*
 * Prototypes
 */
static void taskScreenManager( void *pvParameters );


static QueueHandle_t xQueue = NULL;

#define mainQUEUE_SEND_PARAMETER			( 0x1111UL )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

void mainScreenManager(void){

	xTaskCreate( taskScreenManager, "SCREEN", configMINIMAL_STACK_SIZE,
			( void * ) mainQUEUE_SEND_PARAMETER, mainQUEUE_SEND_TASK_PRIORITY, NULL );

}


static void taskScreenManager( void *pvParameters )
{
	TickType_t xNextWakeTime;
unsigned long ulReceivedValue;

	/* Check the task parameter is as expected. */
//	configASSERT( ( ( unsigned long ) pvParameters ) == 0x12 );
	xNextWakeTime = xTaskGetTickCount();
	for( ;; )
	{
		vTaskDelayUntil( &xNextWakeTime, SCREENLOOP_INTERVAL );
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		//xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );
		gpio_toggle(LED_PORT, LED_PIN);	/* LED on/off */

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */
		/*if( ulReceivedValue == 100UL )
		{
			//vParTestToggleLED( LED1 );
			ulReceivedValue = 0U;
		}
		*/
	}
}
