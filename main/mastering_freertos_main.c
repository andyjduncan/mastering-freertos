#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

volatile unsigned long mainDELAY_LOOP_COUNT = 1000000;

void vTaskFunction(void *pvParameters) {
    char *pcTaskName;

    TickType_t lastWakeTime = xTaskGetTickCount();

    const TickType_t loopDelay = pdMS_TO_TICKS(250);
    /*
     * The string to print out is passed in via the parameter. Cast this to a
     * character pointer.
     */
    pcTaskName = (char *) pvParameters;
    /* As per most tasks, this task is implemented in an infinite loop. */
    for (;;) {
        /* Print out the name of this task. */
        printf("%s running\n", pcTaskName);
        /* Delay for a period. */
        vTaskDelayUntil(&lastWakeTime, loopDelay);
    }
}

void app_main() {

    printf("Starting Task 1\n");
    xTaskCreatePinnedToCore(
            vTaskFunction,
            "Task 1",
            2000,
            (void *) "Task 1",
            1,
            NULL,
            PRO_CPU_NUM
    );

    printf("Starting Task 2\n");
    xTaskCreatePinnedToCore(
            vTaskFunction,
            "Task 2",
            2000,
            (void *) "Task 2",
            2,
            NULL,
            PRO_CPU_NUM
    );
}