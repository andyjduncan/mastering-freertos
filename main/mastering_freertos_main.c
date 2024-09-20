#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"

SemaphoreHandle_t  xMutex;

static void prvNewPrintString( const char *pcString ) {
    /* The mutex is created before the scheduler is started, so already exists
       by the time this task executes.
       Attempt to take the mutex, blocking indefinitely to wait for the mutex
       if it is not available straight away. The call to xSemaphoreTake() will
       only return when the mutex has been successfully obtained, so there is
       no need to check the function return value. If any other delay period
       was used then the code must check that xSemaphoreTake() returns pdTRUE
       before accessing the shared resource (which in this case is standard
       out). As noted earlier in this book, indefinite time outs are not
       recommended for production code. */
    xSemaphoreTake( xMutex, portMAX_DELAY );
    {
        /* The following line will only execute once the mutex has been
           successfully obtained. Standard out can be accessed freely now as
           only one task can have the mutex at any one time. */
        printf( "%s", pcString );
        fflush( stdout );
        /* The mutex MUST be given back! */
    }
    xSemaphoreGive( xMutex );
}

static void prvPrintTask( void *pvParameters ) {
    char *pcStringToPrint;
    const TickType_t xMaxBlockTimeTicks = 0x20;
    /* Two instances of this task are created. The string printed by the task
       is passed into the task using the task's parameter. The parameter is
       cast to the required type. */
    pcStringToPrint = ( char * ) pvParameters;
    while(true) {
        /* Print out the string using the newly defined function. */
        prvNewPrintString( pcStringToPrint );
        /* Wait a pseudo random time. Note that rand() is not necessarily
           reentrant, but in this case it does not really matter as the code
           does not care what value is returned. In a more secure application
           a version of rand() that is known to be reentrant should be used -
           or calls to rand() should be protected using a critical section. */
        vTaskDelay( ( rand() % xMaxBlockTimeTicks ) );
    }
}

void app_main() {
    /* Before a semaphore is used it must be explicitly created. In this
       example a mutex type semaphore is created. */
    xMutex = xSemaphoreCreateMutex();
    /* Check the semaphore was created successfully before creating the
       tasks. */
    if( xMutex != NULL ) {
        /* Create two instances of the tasks that write to stdout. The string
           they write is passed in to the task as the task's parameter. The
           tasks are created at different priorities so some pre-emption will
           occur. */
        xTaskCreate( prvPrintTask, "Print1", 2000,
                     "Task 1 ***************************************\n",
                     1, NULL );
        xTaskCreate( prvPrintTask, "Print2", 2000,
                     "Task 2 ---------------------------------------\n",
                     2, NULL );
    }
}