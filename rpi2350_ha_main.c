/**
 * Copyright (c) inveh.
 *
 */
#include "rpi2350_ha_main_inf.h"

#define LED_DELAY_MS 1000

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

int main() {

    hard_assert(rpi2350_ha_init() == PICO_OK);

    while (true) 
    {
        pico_set_led(true);
        sleep_ms(LED_DELAY_MS);
        pico_set_led(false);
        sleep_ms(LED_DELAY_MS);
        printf("Hello, world!\n");
    }
}