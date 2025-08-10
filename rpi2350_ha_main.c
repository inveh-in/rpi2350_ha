/**
 * Copyright (c) inveh.
 *
 */
#include "rpi2350_ha_inf.h"
#include "rpi2350_ha_priv.h"
#include "rpi2350_ha_pub.h"

// Priorities of our threads - higher numbers are higher priority
#define CORE0_TASK_PRIORITY    ( tskIDLE_PRIORITY + 4UL )
#define CORE1_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2UL )

// Stack sizes of our threads in words (4 bytes)
#define CORE0_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define CORE1_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

// Turn the led on or off
void pico_set_led(bool led_on) {
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
}

// Perform initialisation
void rpi2350_ha_init(void) 
{
    int retVal;

    stdio_init_all();

    retVal = cyw43_arch_init();
    hard_assert(retVal == PICO_OK);

    rpi2350_ha_wifi_init();
    rpi2350_ha_ble_init();
}

void rpi2350_ha_core0_proc(__unused void *params) 
{
    rpi2350_ha_ble_proc(params);
}

void rpi2350_ha_core1_proc(__unused void *params) 
{
    rpi2350_ha_wifi_proc(params);
}

int main() {

    TaskHandle_t taskHandle_Core0;
    TaskHandle_t taskHandle_Core1;

    rpi2350_ha_init();

    // we must bind the main task to core0
    xTaskCreate(rpi2350_ha_core0_proc, "MainThread", CORE0_TASK_STACK_SIZE, NULL, CORE0_TASK_PRIORITY, &taskHandle_Core0);
    vTaskCoreAffinitySet(taskHandle_Core0, ( 1 << 0 ));

    // we must bind the main task to core1
    xTaskCreate(rpi2350_ha_core1_proc, "MainThread", CORE1_TASK_STACK_SIZE, NULL, CORE1_TASK_PRIORITY, &taskHandle_Core1);
    vTaskCoreAffinitySet(taskHandle_Core1, ( 1 << 1 ));

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    return 0;
}