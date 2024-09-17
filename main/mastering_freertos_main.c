#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portable.h"

SemaphoreHandle_t  xCountingSemaphore;

static void isrHandler() {

    BaseType_t xHigherPriorityTaskWoken;

    /* The xHigherPriorityTaskWoken parameter must be initialized to
       pdFALSE as it will get set to pdTRUE inside the interrupt safe
       API function if a context switch is required. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* 'Give' the semaphore to unblock the task, passing in the address of
       xHigherPriorityTaskWoken as the interrupt safe API function's
       pxHigherPriorityTaskWoken parameter. */
    xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken );

    /* Pass the xHigherPriorityTaskWoken value into portYIELD_FROM_ISR().
      If xHigherPriorityTaskWoken was set to pdTRUE inside
      xSemaphoreGiveFromISR() then calling portYIELD_FROM_ISR() will request
      a context switch. If xHigherPriorityTaskWoken is still pdFALSE then
      calling portYIELD_FROM_ISR() will have no effect. Unlike most FreeRTOS
      ports, the Windows port requires the ISR to return a value - the return
      statement is inside the Windows version of portYIELD_FROM_ISR(). */
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void vHandlerTask( void *pvParameters ) {
    /* xMaxExpectedBlockTime holds the maximum time expected between two
       interrupts. */
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(2000);

    const TickType_t processingTime = pdMS_TO_TICKS(500);
    /* As per most tasks, this task is implemented within an infinite loop. */
    while(true) {
        printf("Waiting for semaphore...\n");
        /* Use the semaphore to wait for the event. The semaphore was created
           before the scheduler was started, so before this task ran for the
           first time. The task blocks indefinitely, meaning this function
           call will only return once the semaphore has been successfully
           obtained - so there is no need to check the value returned by
           xSemaphoreTake(). */
        if (xSemaphoreTake(xCountingSemaphore, xMaxExpectedBlockTime ) == pdPASS) {
            /* To get here the event must have occurred. Process the event (in
               this Case, just print out a message). */
            printf( "Handler task - Processing event.\n" );
            vTaskDelay(processingTime);
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

    /* Before a semaphore is used it must be explicitly created. In this
       example a binary semaphore is created. */
    xCountingSemaphore = xSemaphoreCreateCounting(5, 0);

    /* Check the semaphore was created successfully. */
    if(xCountingSemaphore != NULL ) {
        /* Create the 'handler' task, which is the task to which interrupt
           processing is deferred. This is the task that will be synchronized
           with the interrupt. The handler task is created with a high priority
           to ensure it runs immediately after the interrupt exits. In this
           case a priority of 3 is chosen. */
        xTaskCreate( vHandlerTask, "Handler", 4000, NULL, 3, NULL );

        gpio_set_intr_type(GPIO_NUM_6, GPIO_INTR_LOW_LEVEL);
        printf("Interrupt enabled\n");
    }
}