#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"

QueueHandle_t  xPrintQueue;

static void prvStdioGatekeeperTask( void *pvParameters ) {
    char *pcMessageToPrint;
    /* This is the only task that is allowed to write to standard out. Any
       other task wanting to write a string to the output does not access
       standard out directly, but instead sends the string to this task. As
       only this task accesses standard out there are no mutual exclusion or
       serialization issues to consider within the implementation of the task
       itself. */
    for( ;; ) {
        /* Wait for a message to arrive. An indefinite block time is specified
           so there is no need to check the return value – the function will
           only return when a message has been successfully received. */
        xQueueReceive( xPrintQueue, &pcMessageToPrint, portMAX_DELAY );
        /* Output the received string. */
        printf( "%s", pcMessageToPrint );
        fflush( stdout );
        /* Loop back to wait for the next message. */
    }
}

static void prvPrintTask( void *pvParameters ) {
    char *pcStringToPrint;
    const TickType_t xMaxBlockTimeTicks = 0x20;
    /* Two instances of this task are created. The string printed by the task
       is passed into the task using the task's parameter. The parameter is
       cast to the required type. */
    pcStringToPrint = ( char * ) pvParameters;
    while(true) {
        /* Print out the string, not directly, but instead by passing a pointer
           to the string to the gatekeeper task via a queue. The queue is
           created before the scheduler is started so will already exist by the
           time this task executes for the first time. A block time is not
           specified because there should always be space in the queue. */
        xQueueSendToBack( xPrintQueue, &pcStringToPrint, 0 );

        /* Wait a pseudo random time. Note that rand() is not necessarily
           reentrant, but in this case it does not really matter as the code
           does not care what value is returned. In a more secure application
           a version of rand() that is known to be reentrant should be used -
           or calls to rand() should be protected using a critical section. */
        vTaskDelay( ( rand() % xMaxBlockTimeTicks ) );
    }
}

void vApplicationTickHook( void ) {
    char *stringToPrint = "Message printed from the tick hook interrupt ##############\n";
    static int iCount = 0;
    /* Print out a message every 200 ticks. The message is not written out
       directly, but sent to the gatekeeper task. */
    iCount++;
    if( iCount >= 200 ) {
        /* As xQueueSendToFrontFromISR() is being called from the tick hook, it
           is not necessary to use the xHigherPriorityTaskWoken parameter (the
           third parameter), and the parameter is set to NULL. */
        xQueueSendToFrontFromISR( xPrintQueue,
                                  &stringToPrint,
                                  NULL );
        /* Reset the count ready to print out the string again in 200 ticks
           time. */
        iCount = 0;
    }
}

void app_main() {
    /* Before a queue is used it must be explicitly created. The queue is
       created to hold a maximum of 5 character pointers. */
    xPrintQueue = xQueueCreate( 5, sizeof( char * ) );

    /* Check the queue was created successfully. */
    if( xPrintQueue != NULL ) {
        /* Create two instances of the tasks that send messages to the
           gatekeeper. The index to the string the task uses is passed to the
           task via the task parameter (the 4th parameter to xTaskCreate()).
           The tasks are created at different priorities so the higher priority
           task will occasionally preempt the lower priority task. */
        xTaskCreate( prvPrintTask, "Print1", 1000, "Task 1 ***************************************\n", 1, NULL );
        xTaskCreate( prvPrintTask, "Print2", 1000, "Task 2 ---------------------------------------\n", 2, NULL );
        /* Create the gatekeeper task. This is the only task that is permitted
           to directly access standard out. */
        xTaskCreate( prvStdioGatekeeperTask, "Gatekeeper", 2000, NULL, 0, NULL );
        printf("Created tasks\n");
    } else {
        printf("Failed to create queue\n");
    }
}