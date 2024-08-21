#define CONFIG_FREERTOS_USE_IDLE_HOOK 1

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Declare a variable of type QueueHandle_t. This is used to store the
   handle to the queue that is accessed by all three tasks. */
QueueHandle_t xQueue;

static void vSenderTask( void *pvParameters ) {
    int32_t lValueToSend;
    BaseType_t xStatus;
    /* Two instances of this task are created so the value that is sent to
       the queue is passed in via the task parameter - this way each instance
       can use a different value. The queue was created to hold values of type
       int32_t, so cast the parameter to the required type. */
    lValueToSend = ( int32_t ) pvParameters;
    /* As per most tasks, this task is implemented within an infinite loop. */
    while(true) {
        /* Send the value to the queue.
           The first parameter is the queue to which data is being sent. The
           queue was created before the scheduler was started, so before this
           task started to execute.
           The second parameter is the address of the data to be sent, in this
           case the address of lValueToSend.
           The third parameter is the Block time – the time the task should be
           kept in the Blocked state to wait for space to become available on
           the queue should the queue already be full. In this case a block
           time is not specified because the queue should never contain more
           than one item, and therefore never be full. */
        xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );
        if( xStatus != pdPASS ) {
            /* The send operation could not complete because the queue was full-
               this must be an error as the queue should never contain more than
               one item! */
            printf( "Could not send to the queue.\n" );
        }
    }
}

static void vReceiverTask( void *pvParameters ) {
    /* Declare the variable that will hold the values received from the
       queue. */
    int32_t lReceivedValue;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
    /* This task is also defined within an infinite loop. */
    while(true) {
        /* This call should always find the queue empty because this task will
           immediately remove any data that is written to the queue. */
        if( uxQueueMessagesWaiting( xQueue ) != 0 ) {
            printf( "Queue should have been empty!\n" );
        }
        /* Receive data from the queue.
           The first parameter is the queue from which data is to be received.
           The queue is created before the scheduler is started, and therefore
           before this task runs for the first time.
           The second parameter is the buffer into which the received data will
           be placed. In this case the buffer is simply the address of a
           variable that has the required size to hold the received data.
           The last parameter is the block time – the maximum amount of time
           that the task will remain in the Blocked state to wait for data to
           be available should the queue already be empty. */
        xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );
        if( xStatus == pdPASS ) {
            /* Data was successfully received from the queue, print out the
               received value. */
            printf( "Received = %li\n", lReceivedValue );
        }
        else
        {
            printf( "Could not receive from the queue.\n" );
        }
/* Data was not received from the queue even after waiting for
   100ms. This must be an error as the sending tasks are free
   running and will be continuously writing to the queue. */
    } }

void app_main() {

/* The queue is created to hold a maximum of 5 values, each of which is
       large enough to hold a variable of type int32_t. */
    xQueue = xQueueCreate( 5, sizeof( int32_t ) );

    if( xQueue != NULL ) {
        /* Create two instances of the task that will send to the queue. The
           task parameter is used to pass the value that the task will write
           to the queue, so one task will continuously write 100 to the queue
           while the other task will continuously write 200 to the queue. Both
           tasks are created at priority 1. */
        xTaskCreate(
                vSenderTask,
                "Sender1",
                2000,
                ( void * ) 100,
                1,
                NULL);
        xTaskCreate(
                vSenderTask,
                "Sender2",
                2000,
                ( void * ) 200,
                1,
                NULL);
        /* Create the task that will read from the queue. The task is created
           with priority 2, so above the priority of the sender tasks. */
        xTaskCreate(
                vReceiverTask,
                "Receiver",
                2000,
                NULL,
                2,
                NULL);
    }
    else
    {
        printf("Couldn't create queue");
    }
}