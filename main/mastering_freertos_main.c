#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

/* The periods assigned to the one-shot and auto-reload timers are 3.333
   second and half a second respectively. */
#define mainBACKLIGHT_TIMER_PERIOD pdMS_TO_TICKS( 2500 )

TimerHandle_t xBacklightTimer;

BaseType_t xSimulatedBacklightOn = pdFALSE;

static void prvBacklightTimerCallback(TimerHandle_t xTimer) {
    TickType_t xTimeNow = xTaskGetTickCount();
    /* The backlight timer expired, turn the backlight off. */
    xSimulatedBacklightOn = pdFALSE;
    /* Print the time at which the backlight was turned off. */
    printf("Timer expired, turning backlight OFF at time %lu\n", xTimeNow);
}

static void vKeyHitTask(void *pvParameters) {
    const TickType_t xShortDelay = pdMS_TO_TICKS(50);
    TickType_t xTimeNow;

    printf("Press button to turn the backlight on.\n");
/* Ideally an application would be event driven, and use an interrupt to
   process key presses. It is not practical to use keyboard interrupts
   when using the FreeRTOS Windows port, so this task is used to poll for
   a key press. */
    while (true) {
        /* Has a key been pressed? */
        int pressed = gpio_get_level(GPIO_NUM_6);

        if (pressed == 0) {
            /* A key has been pressed. Record the time. */
            xTimeNow = xTaskGetTickCount();
            if (xSimulatedBacklightOn == pdFALSE) {
                /* The backlight was off, so turn it on and print the time at
                   which it was turned on. */
                xSimulatedBacklightOn = pdTRUE;
                printf("Key pressed, turning backlight ON at time %lu\n",
                       xTimeNow);
            } else {
/* The backlight was already on, so print a message to say the
   timer is about to be reset and the time at which it was
   reset. */
                printf(
                        "Key pressed, resetting software timer at time %lu\n",
                        xTimeNow);
            }
            /* Reset the software timer. If the backlight was previously off,
   then this call will start the timer. If the backlight was
   previously on, then this call will restart the timer. A real
   application may read key presses in an interrupt. If this
   function was an interrupt service routine then
   xTimerResetFromISR() must be used instead of xTimerReset(). */
            xTimerReset(xBacklightTimer, xShortDelay);

        }
        vTaskDelay(pdMS_TO_TICKS(25));
    }
}

void app_main() {

    BaseType_t xTimerStarted;

    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the input pin
    io_conf.pin_bit_mask = (1ULL << GPIO_NUM_6);
    //enable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    xTaskCreate(
            vKeyHitTask,
            "Key Press",
            2000,
            NULL,
            1,
            NULL
            );

    xBacklightTimer = xTimerCreate("Backlight",
                                   mainBACKLIGHT_TIMER_PERIOD,
                                   pdFALSE,
            /* The timer's ID is initialized to NULL. */
                                 NULL,
/* prvTimerCallback() is used by both timers. */
                                 prvBacklightTimerCallback);
    if (xBacklightTimer != NULL) {
        /* Start the software timers, using a block time of 0 (no block time).
           The scheduler has not been started yet so any block time specified
           here would be ignored anyway. */
        xTimerStarted = xTimerStart(xBacklightTimer, 0);

        if (xTimerStarted == pdPASS) {
            printf("Started timer\n");
        } else {
            printf("Timer not started\n");
        }
    } else {
        printf("Timer not created\n");
    }
}