#define CONFIG_FREERTOS_USE_IDLE_HOOK 1

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t  xTask2Handle;

void vTask2( void * pvParameters ) {
    /*
     * Task 2 immediately deletes itself upon starting.
     * To do this it could call vTaskDelete() using NULL as the parameter.
     * For demonstration purposes, it instead calls vTaskDelete() with its own
     * task handle.
     */
    printf( "Task 2 is running and about to delete itself\n" );
    vTaskDelete( xTask2Handle );
}

void vTask1( void * pvParameters ) {
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    for( ;; ) {
        /* Print out the name of this task. */
        printf( "Task 1 is running\n" );
        /*
         * Create task 2 at a higher priority.
         * Pass the address of xTask2Handle as the pxCreatedTask parameter so
         * that xTaskCreate write the resulting task handle to that variable.
         */
        xTaskCreate( vTask2, "Task 2", 1000, NULL, 2, &xTask2Handle );
        /*
         * Task 2 has/had the higher priority. For Task 1 to reach here, Task 2
         * must have already executed and deleted itself.
         */
        vTaskDelay( xDelay100ms );
    }
}

void app_main() {
    xTaskCreate( vTask1, "Task 1", 1000, NULL, 1, NULL );

}