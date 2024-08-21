#define CONFIG_FREERTOS_USE_IDLE_HOOK 1

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Declare a variable that will be incremented by the hook function.  */
volatile unsigned long ulIdleCycleCount = 0UL;

/*
 * Idle hook functions MUST be called vApplicationIdleHook(), take no
 * parameters, and return void.
 */
void vApplicationIdleHook( void ) {
    /* This hook function does nothing but increment a counter. */
    ulIdleCycleCount++;
}

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
        printf("%s running, %lu idle cycles\n", pcTaskName, ulIdleCycleCount);
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
            2,
            NULL,
            PRO_CPU_NUM
    );
}