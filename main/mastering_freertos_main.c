#define CONFIG_FREERTOS_USE_IDLE_HOOK 1

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Declare a variable that is used to hold the handle of Task 2. */
TaskHandle_t xTask2Handle = NULL;

void vTask1(void *pvParameters) {
    UBaseType_t uxPriority;
    /*
     * This task will always run before Task 2 as it is created with the higher
     * priority. Neither Task 1 nor Task 2 ever block so both will always be in
     * either the Running or the Ready state.
     */
    /*
     * Query the priority at which this task is running - passing in NULL means
     * "return the calling task's priority".
*/
    uxPriority = uxTaskPriorityGet(NULL);
    for (;;) {
        /* Print out the name of this task. */
        printf("Task 1 is running\n");
        /*
         * Setting the Task 2 priority above the Task 1 priority will cause
         * Task 2 to immediately start running (as then Task 2 will have the
         * higher priority of the two created tasks). Note the use of the
         * handle to task 2 (xTask2Handle) in the call to vTaskPrioritySet().
         * Listing 4.25 shows how the handle was obtained.
         */
        printf("About to raise the Task 2 priority %i\n", uxPriority + 1);
        vTaskPrioritySet(xTask2Handle, (uxPriority + 1));
        /*
         * Task 1 will only run when it has a priority higher than Task 2.
         * Therefore, for this task to reach this point, Task 2 must already
         * have executed and set its priority back down to below the priority
         * of this task.
         */
    }
}

void vTask2(void *pvParameters) {
    UBaseType_t uxPriority;
    /*
     * Task 1 will always run before this task as Task 1 is created with the
     * higher priority. Neither Task 1 nor Task 2 ever block so will always be
     * in either the Running or the Ready state.
     *
     * Query the priority at which this task is running - passing in NULL means
     * "return the calling task's priority".
     */
    uxPriority = uxTaskPriorityGet(NULL);
    for (;;) {
        /*
         * For this task to reach this point Task 1 must have already run and
         * set the priority of this task higher than its own.
        */
        /* Print out the name of this task. */
        printf("Task 2 is running\n");
        /*
         * Set the priority of this task back down to its original value.
         * Passing in NULL as the task handle means "change the priority of the
         * calling task". Setting the priority below that of Task 1 will cause
         * Task 1 to immediately start running again – preempting this task.
         */
        printf("About to lower the Task 2 priority\n");
        vTaskPrioritySet(NULL, (uxPriority - 2));
    }
}

void app_main() {
    xTaskCreate(
            vTask2,
            "Task 2",
            2000,
            NULL,
            3,
            &xTask2Handle );

    xTaskCreate(
            vTask1,
            "Task 1",
            2000,
            NULL,
            2,
            NULL);

}