/**
 * Copyright (c) inveh.
 *
 */
#include "rpi2350_ha_inf.h"
#include "rpi2350_ha_priv.h"

/* Private definition */
#define LED_DELAY_MS 1000

// Priorities of our threads - higher numbers are higher priority
#define CORE0_TASK_PRIORITY    ( tskIDLE_PRIORITY + 4UL )
#define CORE1_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2UL )

// Stack sizes of our threads in words (4 bytes)
#define CORE0_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define CORE1_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

/* Value for the UDP protocol */
#define UDP_PORT 13400
#define BEACON_MSG_LEN_MAX 127
#define BEACON_TARGET "255.255.255.255"
#define BEACON_INTERVAL_MS 1000

// Turn the led on or off
void pico_set_led(bool led_on) {
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
}

// Perform initialisation
int rpi2350_ha_init(void) 
{
    int retVal;

    stdio_init_all();
    retVal = cyw43_arch_init();

    return retVal;
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
    struct udp_pcb* pcb = udp_new();

    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) 
    {
        printf("failed to connect.\n");
        return 1;
    } 
    else 
    {
        printf("Connected.\n");
    }

    ip_addr_t addr;
    ipaddr_aton(BEACON_TARGET, &addr);

    int counter = 0;

    while (true) 
    {
        //printf("2nd worker is on core %d\n", portGET_CORE_ID());
        
        struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, BEACON_MSG_LEN_MAX+1, PBUF_RAM);
        char *req = (char *)p->payload;
        memset(req, 0, BEACON_MSG_LEN_MAX+1);
        snprintf(req, BEACON_MSG_LEN_MAX, "%d\n", counter);
        err_t er = udp_sendto(pcb, p, &addr, UDP_PORT);
        pbuf_free(p);
        if (er != ERR_OK) {
            printf("Failed to send UDP packet! error=%d", er);
        } else {
            printf("Sent packet %d\n", counter);
            counter++;
        }

        // Note in practice for this simple UDP transmitter,
        // the end result for both background and poll is the same

#if PICO_CYW43_ARCH_POLL
        // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
        // main loop (not from a timer) to check for Wi-Fi driver or lwIP work that needs to be done.
        cyw43_arch_poll();
        sleep_ms(BEACON_INTERVAL_MS);
#else
        // if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP work
        // is done via interrupt in the background. This sleep is just an example of some (blocking)
        // work you might be doing.
        sleep_ms(BEACON_INTERVAL_MS);
#endif
    }

    cyw43_arch_deinit();
}

int main() {

    TaskHandle_t taskHandle_Core0;
    TaskHandle_t taskHandle_Core1;

    hard_assert(rpi2350_ha_init() == PICO_OK);

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