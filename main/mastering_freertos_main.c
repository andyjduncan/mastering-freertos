#include <sys/cdefs.h>
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"

TaskHandle_t handlerTask;

static void isrHandler() {
    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to
       pdFALSE as it will get set to pdTRUE inside the interrupt safe
       API function if a context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR(handlerTask, &xHigherPriorityTaskWoken);
    vTaskNotifyGiveFromISR(handlerTask, &xHigherPriorityTaskWoken);
    vTaskNotifyGiveFromISR(handlerTask, &xHigherPriorityTaskWoken);

    /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().
      If xHigherPriorityTaskWoken was set to pdTRUE inside
      xSemaphoreGiveFromISR() then calling portYIELD_FROM_ISR() will request
      a context switch. If xHigherPriorityTaskWoken is still pdFALSE then
      calling portYIELD_FROM_ISR() will have no effect. Unlike most FreeRTOS
      ports, the Windows port requires the ISR to return a value - the return
      statement is inside the Windows version of portYIELD_FROM_ISR(). */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

_Noreturn static void vHandlerTask( void *pvParameters ) {
    /* xMaxExpectedBlockTime holds the maximum time expected between two
       interrupts. */
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(2000);

    uint32_t eventsToProcess;

    /* As per most tasks, this task is implemented within an infinite loop. */
    while(true) {
        printf("Waiting for events...\n");

        eventsToProcess = ulTaskNotifyTake(pdTRUE, xMaxExpectedBlockTime);

        if (eventsToProcess != 0) {
            while (eventsToProcess > 0) {
                printf("Processing event...\n");
                eventsToProcess--;
            }
        } else {
            printf("Handler task - Timeout waiting for event.\n");
        }
    }
}

void app_main() {
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

    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);

    gpio_isr_handler_add(GPIO_NUM_6, isrHandler, NULL);

    xTaskCreate(vHandlerTask, "Handler", 4000, NULL, 3, &handlerTask);

    if (handlerTask != NULL) {
        gpio_set_intr_type(GPIO_NUM_6, GPIO_INTR_LOW_LEVEL);
        printf("Interrupt enabled\n");
    }
}