#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

volatile unsigned long mainDELAY_LOOP_COUNT = 1000000;

void vTask1(void * pvParameters )
{
    /* ulCount is declared volatile to ensure it is not optimized out. */
    volatile unsigned long ulCount;
    for( ;; )
    {
        /* Print out the name of the current task task. */
        printf( "Task 1 is running\n" );
        /* Delay for a period. */
        for( ulCount = 0; ulCount < mainDELAY_LOOP_COUNT; ulCount++ )
        {
            /*
            * This loop is just a very crude delay implementation. There is
            * nothing to do in here. Later examples will replace this crude
            * loop with a proper delay/sleep function.
            */
        }
    }
}

void vTask2( void * pvParameters )
{
    /* ulCount is declared volatile to ensure it is not optimized out. */
    volatile unsigned long ulCount;
    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ;; )
    {
        /* Print out the name of this task. */
        printf( "Task 2 is running\n" );
        /* Delay for a period. */
        for( ulCount = 0; ulCount < mainDELAY_LOOP_COUNT; ulCount++ )
        {
            /*
            * This loop is just a very crude delay implementation. There is
            * nothing to do in here. Later examples will replace this crude
            * loop with a proper delay/sleep function.
            */
        }
    }
}

void app_main() {

    xTaskCreatePinnedToCore(
            vTask1,
            "Task 1",
            2000,
            NULL,
            2,
            NULL,
            PRO_CPU_NUM
            );

    xTaskCreatePinnedToCore(
            vTask2,
            "Task 2",
            2000,
            NULL,
            2,
            NULL,
            PRO_CPU_NUM
    );
}