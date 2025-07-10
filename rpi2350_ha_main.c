/**
 * Copyright (c) inveh.
 *
 */
#include "rpi2350_ha_main_inf.h"

/* Private definition */
#define LED_DELAY_MS 1000

// Priorities of our threads - higher numbers are higher priority
#define CORE0_TASK_PRIORITY    ( tskIDLE_PRIORITY + 4UL )
#define CORE1_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2UL )

// Stack sizes of our threads in words (4 bytes)
#define CORE0_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define CORE1_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

// Perform initialisation
int rpi2350_ha_init(void) 
{
    int retVal;

    stdio_init_all();
    retVal = cyw43_arch_init();

    return retVal;
}

// Turn the led on or off
void pico_set_led(bool led_on) {
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
}

void rpi2350_ha_core0_proc(__unused void *params) 
{
    while (true) 
    {
        pico_set_led(true);
        busy_wait_ms(LED_DELAY_MS);
        pico_set_led(false);
        busy_wait_ms(LED_DELAY_MS);

        printf("1st worker is on core %d\n", portGET_CORE_ID());
    }
}

void rpi2350_ha_core1_proc(__unused void *params) 
{
    while (true) 
    {
        printf("2nd worker is on core %d\n", portGET_CORE_ID());
        busy_wait_ms(LED_DELAY_MS);
    }
}

int main() {

    TaskHandle_t taskHandle_Core0;
    TaskHandle_t taskHandle_Core1;

    hard_assert(rpi2350_ha_init() == PICO_OK);

    // we must bind the main task to core0
    xTaskCreate(rpi2350_ha_core0_proc, "MainThread", CORE0_TASK_STACK_SIZE, NULL, CORE0_TASK_PRIORITY, &taskHandle_Core0);
    vTaskCoreAffinitySet(taskHandle_Core0, 0);

    // we must bind the main task to core1
    xTaskCreate(rpi2350_ha_core1_proc, "MainThread", CORE1_TASK_STACK_SIZE, NULL, CORE1_TASK_PRIORITY, &taskHandle_Core1);
    vTaskCoreAffinitySet(taskHandle_Core1, 1);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    return 0;
}