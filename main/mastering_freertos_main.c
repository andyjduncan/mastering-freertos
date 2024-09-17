#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portable.h"

QueueHandle_t  xIntegerQueue;

static void isrHandler() {
    static uint32_t ulValueToSend = 0;

    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to
       pdFALSE as it will get set to pdTRUE inside the interrupt safe
       API function if a context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    xQueueSendToBackFromISR(xIntegerQueue, &ulValueToSend, &xHigherPriorityTaskWoken);
    ulValueToSend++;

    /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().
      If xHigherPriorityTaskWoken was set to pdTRUE inside
      xSemaphoreGiveFromISR() then calling portYIELD_FROM_ISR() will request
      a context switch. If xHigherPriorityTaskWoken is still pdFALSE then
      calling portYIELD_FROM_ISR() will have no effect. Unlike most FreeRTOS
      ports, the Windows port requires the ISR to return a value - the return
      statement is inside the Windows version of portYIELD_FROM_ISR(). */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void integerPrinter(void *pvParameters) {
    uint32_t receivedNumber;

    while (true) {
        xQueueReceive(xIntegerQueue, &receivedNumber, portMAX_DELAY);

        printf("Received %lu\n", receivedNumber);
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

    xIntegerQueue = xQueueCreate(10, sizeof(uint32_t));

    if (xIntegerQueue) {

        xTaskCreate( integerPrinter, "Integer", 2000, NULL, 2, NULL );

        gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);

        gpio_isr_handler_add(GPIO_NUM_6, isrHandler, NULL);

        gpio_set_intr_type(GPIO_NUM_6, GPIO_INTR_LOW_LEVEL);
        printf("Interrupt enabled\n");
    } else {
        printf("Queue failed to create\n");
    }
}