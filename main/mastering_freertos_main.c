#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

/* The periods assigned to the one-shot and auto-reload timers are 3.333
   second and half a second respectively. */
#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 3333 )
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 500 )

static void prvOneShotTimerCallback( TimerHandle_t xTimer ) {
    TickType_t xTimeNow;
    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();
    /* Output a string to show the time at which the callback was executed. */
    printf( "One-shot timer callback executing %lu\n", xTimeNow );
    /* File scope variable. */
}

static void prvAutoReloadTimerCallback( TimerHandle_t xTimer ) {
    TickType_t xTimeNow;
    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();
    /* Output a string to show the time at which the callback was executed. */
    printf( "Auto-reload timer callback executing %lu\n", xTimeNow);
}

void app_main() {

    TimerHandle_t xAutoReloadTimer, xOneShotTimer;
    BaseType_t xTimer1Started, xTimer2Started;

    xOneShotTimer = xTimerCreate(
            /* Text name for the software timer - not used by FreeRTOS. */
            "OneShot",
            /* The software timer's period in ticks. */
            mainONE_SHOT_TIMER_PERIOD,
            /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
            pdFALSE,
            /* This example does not use the timer id. */
            0,
            /* Callback function to be used by the software timer being created. */
            prvOneShotTimerCallback );
/* Create the auto-reload timer, storing the handle to the created timer
   in xAutoReloadTimer. */
    xAutoReloadTimer = xTimerCreate(
            /* Text name for the software timer - not used by FreeRTOS. */
            "AutoReload",
            /* The software timer's period in ticks. */
            mainAUTO_RELOAD_TIMER_PERIOD,
            /* Setting uxAutoRealod to pdTRUE creates an auto-reload timer. */
            pdTRUE,
            /* This example does not use the timer id. */
            0,
            /* Callback function to be used by the software timer being created. */
            prvAutoReloadTimerCallback );

    if( ( xOneShotTimer != NULL ) && ( xAutoReloadTimer != NULL ) ) {
        /* Start the software timers, using a block time of 0 (no block time).
           The scheduler has not been started yet so any block time specified
           here would be ignored anyway. */
        xTimer1Started = xTimerStart(xOneShotTimer, 0);
        xTimer2Started = xTimerStart(xAutoReloadTimer, 0);

        if ((xTimer1Started == pdPASS) && (xTimer2Started == pdPASS)) {
            printf("Started timers\n");
        } else {
            printf("Timers not started\n");
        }
    } else {
        printf("Timers not created\n");
    }
}