#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"

void vDeferredHandlingFunction(void *pvParameter1, uint32_t ulParameter2) {
    printf( "Handler task - Processing event %lu.\n", ulParameter2);
}

static void isrHandler() {
    static uint32_t ulParameterValue = 0;

    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to
       pdFALSE as it will get set to pdTRUE inside the interrupt safe
       API function if a context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Send a pointer to the interrupt's deferred handling function to the
       daemon task. The deferred handling function's pvParameter1 parameter
       is not used so just set to NULL. The deferred handling function's
       ulParameter2 parameter is used to pass a number that is incremented by
       one each time this interrupt handler executes. */
    xTimerPendFunctionCallFromISR( vDeferredHandlingFunction, /* Function to execute */
                                   NULL, /* Not used */
                                   ulParameterValue, /* Incrementing value. */
                                   &xHigherPriorityTaskWoken );
    ulParameterValue++;

    /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().
      If xHigherPriorityTaskWoken was set to pdTRUE inside
      xSemaphoreGiveFromISR() then calling portYIELD_FROM_ISR() will request
      a context switch. If xHigherPriorityTaskWoken is still pdFALSE then
      calling portYIELD_FROM_ISR() will have no effect. Unlike most FreeRTOS
      ports, the Windows port requires the ISR to return a value - the return
      statement is inside the Windows version of portYIELD_FROM_ISR(). */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
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

    gpio_set_intr_type(GPIO_NUM_6, GPIO_INTR_LOW_LEVEL);
    printf("Interrupt enabled\n");
}