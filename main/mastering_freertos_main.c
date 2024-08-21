#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

/* The periods assigned to the one-shot and auto-reload timers are 3.333
   second and half a second respectively. */
#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 3333 )
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 500 )

TimerHandle_t xAutoReloadTimer, xOneShotTimer;

static void prvTimerCallback( TimerHandle_t xTimer ) {
    TickType_t xTimeNow; uint32_t ulExecutionCount;
    /* A count of the number of times this software timer has expired is
       stored in the timer's ID. Obtain the ID, increment it, then save it as
       the new ID value. The ID is a void pointer, so is cast to a uint32_t. */
    ulExecutionCount = ( uint32_t ) pvTimerGetTimerID( xTimer ); ulExecutionCount++;
    vTimerSetTimerID( xTimer, ( void * ) ulExecutionCount );
    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();
    /* The handle of the one-shot timer was stored in xOneShotTimer when the
       timer was created. Compare the handle passed into this function with
       xOneShotTimer to determine if it was the one-shot or auto-reload timer
       that expired, then output a string to show the time at which the
       callback was executed. */
    if( xTimer == xOneShotTimer ) {
        printf( "One-shot timer callback executing %lu\n", xTimeNow );
    }
    else {
/* xTimer did not equal xOneShotTimer, so it must have been the
   auto-reload timer that expired. */
        printf("Auto-reload timer callback executing %lu\n", xTimeNow);
        if (ulExecutionCount == 5) {
            /* Stop the auto-reload timer after it has executed 5 times. This
               callback function executes in the context of the RTOS daemon
               task so must not call any functions that might place the daemon
               task into the Blocked state. Therefore a block time of 0 is
               used. */
            xTimerStop(xTimer, 0);
        }
    }
}
void app_main() {

    BaseType_t xTimer1Started, xTimer2Started;

    /* Create the one shot timer software timer, storing the handle in
   xOneShotTimer. */
    xOneShotTimer = xTimerCreate( "OneShot",
                                  mainONE_SHOT_TIMER_PERIOD,
                                  pdFALSE,
            /* The timer's ID is initialized to NULL. */
                                  NULL,
/* prvTimerCallback() is used by both timers. */
    prvTimerCallback );
/* Create the auto-reload software timer, storing the handle in
   xAutoReloadTimer */
    xAutoReloadTimer = xTimerCreate( "AutoReload",
                                     mainAUTO_RELOAD_TIMER_PERIOD,
                                     pdTRUE,
            /* The timer's ID is initialized to NULL. */
                                     NULL,
            /* prvTimerCallback() is used by both timers. */
                                     prvTimerCallback );
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