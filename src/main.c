/*
 * main.c
 *
 *  Created on: Nov 4, 2019
 *      Author: vincent
 */
#include <stdint.h>
#include "libopencm3/stm32/rcc.h"
#include <libopencm3/stm32/gpio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* IO's
 *
 */
#include "IO.h"

/*
 * prototypes
 */

static void prvSetupHardware(void);

/*tasks*/
void mainConsoleManager(void);


 QueueHandle_t xApplicationEvents;
 QueueHandle_t xConsoleEvents;
 QueueHandle_t xScreenEvents;

#include "SYSTEM_EVENTS.h"

#define MAX_COMMANDS_TO_QUEUE 10




int main() {
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	prvSetupHardware();
	//start this task as it creates moste queues

	//setup queues
	xApplicationEvents = xQueueCreate(MAX_COMMANDS_TO_QUEUE, sizeof(char));
	xConsoleEvents = xQueueCreate(MAX_COMMANDS_TO_QUEUE, sizeof(char));
	xScreenEvents = xQueueCreate(MAX_COMMANDS_TO_QUEUE, sizeof(char));

	mainAppManager();

	mainScreenManager();
	mainConsoleManager();
	mainAppManager();

	vTaskStartScheduler();
	for (;;) {
		//
	}

}

static void prvSetupHardware(void) {

	/* LED */
	rcc_periph_clock_enable(RCC_GPIOC);

	gpio_set_mode(LED_PORT, GPIO_MODE_OUTPUT_50_MHZ,
	GPIO_CNF_OUTPUT_PUSHPULL, LED_PIN);

	/*usart port A*/
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART1);

}
void vApplicationMallocFailedHook(void) {
	/* vApplicationMallocFailedHook() will only be called if
	 configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	 function that will get called if a call to pvPortMalloc() fails.
	 pvPortMalloc() is called internally by the kernel whenever a task, queue,
	 timer or semaphore is created.  It is also called by various parts of the
	 demo application.  If heap_1.c or heap_2.c are used, then the size of the
	 heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	 FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	 to query the size of free heap space that remains (although it does not
	 provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void) {
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	 to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	 task.  It is essential that code added to this hook function never attempts
	 to block in any way (for example, call xQueueReceive() with a block time
	 specified, or call vTaskDelay()).  If the application makes use of the
	 vTaskDelete() API function (as this demo application does) then it is also
	 important that vApplicationIdleHook() is permitted to return to its calling
	 function, because it is the responsibility of the idle task to clean up
	 memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
	(void) pcTaskName;
	(void) pxTask;

	/* Run time stack overflow checking is performed if
	 configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	 function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void) {
	/* This function will be called by each tick interrupt if
	 configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	 added here, but the tick hook is called from an interrupt context, so
	 code must not attempt to block, and only the interrupt safe FreeRTOS API
	 functions can be used (those that end in FromISR()). */
}

