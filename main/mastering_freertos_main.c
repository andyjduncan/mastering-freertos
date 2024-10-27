#include <sys/cdefs.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"

/* Definitions for the event bits in the event group. */
#define mainFIRST_TASK_BIT ( 1UL << 0UL ) /* Event bit 0, set by the 1st task */
#define mainSECOND_TASK_BIT ( 1UL << 1UL ) /* Event bit 1, set by the 2nd task */
#define mainTHIRD_TASK_BIT ( 1UL << 2UL ) /* Event bit 2, set by the 3rd task */

EventGroupHandle_t  xEventGroup;

_Noreturn static void vSyncingTask( void *pvParameters ) {
    const TickType_t xMaxDelay = pdMS_TO_TICKS( 4000UL );
    const TickType_t xMinDelay = pdMS_TO_TICKS( 200UL );
    TickType_t xDelayTime;
    EventBits_t uxThisTasksSyncBit;
    const EventBits_t uxAllSyncBits = ( mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainTHIRD_TASK_BIT );
    /* Three instances of this task are created - each task uses a different
       event bit in the synchronization. The event bit to use is passed into
       each task instance using the task parameter. Store it in the
       uxThisTasksSyncBit variable. */
    uxThisTasksSyncBit = ( EventBits_t ) pvParameters;
    while(true) {
        /* Simulate this task taking some time to perform an action by delaying
           for a pseudo random time. This prevents all three instances of this
           task reaching the synchronization point at the same time, and so
           allows the example's behavior to be observed more easily. */
        xDelayTime = ( rand() % xMaxDelay ) + xMinDelay;
        vTaskDelay( xDelayTime );
        /* Print out a message to show this task has reached its synchronization
           point. pcTaskGetTaskName() is an API function that returns the name
           assigned to the task when the task was created. */
        printf("%lu: %s reached sync point\n", xTaskGetTickCount(), pcTaskGetName(NULL));
        /* Wait for all the tasks to have reached their respective
           synchronization points. */
        xEventGroupSync( /* The event group used to synchronize. */
                xEventGroup,
                /* The bit set by this task to indicate it has reached
                   the synchronization point. */
                uxThisTasksSyncBit,
                /* The bits to wait for, one bit for each task taking
                   part in the synchronization. */
                uxAllSyncBits,
                /* Wait indefinitely for all three tasks to reach the
                   synchronization point. */
                portMAX_DELAY );
        /* Print out a message to show this task has passed its synchronization
           point. As an indefinite delay was used the following line will only
           be executed after all the tasks reached their respective
           synchronization points. */
        printf("%lu: %s exited sync point\n", xTaskGetTickCount(), pcTaskGetName(NULL));
    }
}

void app_main() {
/* Before an event group can be used it must first be created. */
    xEventGroup = xEventGroupCreate();
    /* Create three instances of the task. Each task is given a different
       name, which is later printed out to give a visual indication of which
       task is executing. The event bit to use when the task reaches its
       synchronization point is passed into the task using the task parameter. */
    xTaskCreate(vSyncingTask, "Task 1", 2000, (void *) mainFIRST_TASK_BIT, 1, NULL );
    xTaskCreate(vSyncingTask, "Task 2", 2000, (void *) mainSECOND_TASK_BIT, 1, NULL );
    xTaskCreate(vSyncingTask, "Task 3", 2000, (void *) mainTHIRD_TASK_BIT, 1, NULL );
}