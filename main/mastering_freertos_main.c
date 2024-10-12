#include <sys/dirent.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"

/* Definitions for the event bits in the event group. */
#define mainFIRST_TASK_BIT ( 1UL << 0UL )   /* Event bit 0, set by a task */
#define mainSECOND_TASK_BIT ( 1UL << 1UL )  /* Event bit 1, set by a task */
#define mainISR_BIT ( 1UL << 2UL )          /* Event bit 2, set by an ISR */

EventGroupHandle_t  xEventGroup;

_Noreturn static void vEventBitSettingTask( void *pvParameters ) {
    const TickType_t xDelay200ms = pdMS_TO_TICKS( 200UL );
    while(true) {
        /* Delay for a short while before starting the next loop. */
        vTaskDelay( xDelay200ms );
        /* Print out a message to say event bit 0 is about to be set by the
           task, then set event bit 0. */
        printf( "Bit setting task -\t about to set bit 0.\n" );
        xEventGroupSetBits( xEventGroup, mainFIRST_TASK_BIT );
        /* Delay for a short while before setting the other bit. */
        vTaskDelay( xDelay200ms );
        /* Print out a message to say event bit 1 is about to be set by the
           task, then set event bit 1. */
        printf( "Bit setting task -\t about to set bit 1.\n" );
        xEventGroupSetBits( xEventGroup, mainSECOND_TASK_BIT );
    }
}

void vPrintStringFromDaemonTask(void * stringToPrint, uint32_t numParam) {
    printf("%s", (char *)stringToPrint);
}

void vApplicationTickHook( void ) {
    static const char *pcString = "Bit setting ISR -\t about to set bit 2.\n";
    static int iCount = 0;
    /* Print out a message every 500 ms. The message is not written out
       directly, but sent to the gatekeeper task. */
    iCount++;
    if( iCount >= pdMS_TO_TICKS(500) ) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        /* Print out a message to say bit 2 is about to be set. Messages cannot
            be printed from an ISR, so defer the actual output to the RTOS daemon
            task by pending a function call to run in the context of the RTOS
            daemon task. */
        xTimerPendFunctionCallFromISR( vPrintStringFromDaemonTask, ( void * ) pcString,
                                       0,
                                       &xHigherPriorityTaskWoken );

        /* Set bit 2 in the event group. */
        xEventGroupSetBitsFromISR( xEventGroup,
                                   mainISR_BIT,
                                   &xHigherPriorityTaskWoken );
        /* Reset the count */
        iCount = 0;
    }
}

_Noreturn static void vEventBitReadingTask( void *pvParameters ) {
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = (mainFIRST_TASK_BIT |
                                        mainSECOND_TASK_BIT |
                                        mainISR_BIT);
    while (true) {
/* Block to wait for event bits to become set within the event
   group. */
        xEventGroupValue = xEventGroupWaitBits(
                /* The event group to read */
                xEventGroup,
                /* Bits to test */
                xBitsToWaitFor,
                /* Clear bits on exit if the
                   unblock condition is met */
                pdTRUE,
                /* Don't wait for all bits. This
                   parameter is set to pdTRUE for
                   second execution. */
                pdFALSE,
                /* Don't time out. */
                portMAX_DELAY);
/* Print a message for each bit that was set. */
        if ((xEventGroupValue & mainFIRST_TASK_BIT) != 0) {
            printf("Bit reading task -\t Event bit 0 was set\n");
        }
        if ((xEventGroupValue & mainSECOND_TASK_BIT) != 0) {
            printf("Bit reading task -\t Event bit 1 was set\n");
        }
        if ((xEventGroupValue & mainISR_BIT) != 0) {
            printf("Bit reading task -\t Event bit 2 was set\n");
        }
    }
}

void app_main() {
    /* Before an event group can be used it must first be created. */
    xEventGroup = xEventGroupCreate();
    /* Create the task that sets event bits in the event group. */
    xTaskCreate( vEventBitSettingTask, "Bit Setter", 1000, NULL, 1, NULL );
    /* Create the task that waits for event bits to get set in the event
       group. */
    xTaskCreate( vEventBitReadingTask, "Bit Reader", 1000, NULL, 2, NULL );
}