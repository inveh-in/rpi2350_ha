/**
 * Copyright (c) inveh.
 *
 */
#include "rpi2350_ha_inf.h"
#include "rpi2350_ha_priv.h"
#include "rpi2350_ha_pub.h"

/* Value for the UDP protocol */
#define UDP_PORT 13400
#define BEACON_MSG_LEN_MAX 4    /* actual length would be +1 */
#define BEACON_TARGET "255.255.255.255"
#define BEACON_INTERVAL_MS 1000

/**
 * @brief Initializes the Wi-Fi functionality using the CYW43 driver.
 */
void rpi2350_ha_wifi_init(void) 
{
    if (cyw43_arch_init()) {
        panic("failed to initialize cyw43_arch\n");
    }
    cyw43_arch_enable_sta_mode();
}

void rpi2350_ha_wifi_10ms() 
{    
    struct udp_pcb* pcb = udp_new();
    ip_addr_t addr;
    ipaddr_aton(BEACON_TARGET, &addr);

    int counter = 0;

    while (true) 
    {                
        if(rpi2350_wifiEna_st)
        {
            struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, BEACON_MSG_LEN_MAX+1, PBUF_RAM);
            char *req = (char *)p->payload;
            memset(req, 0, BEACON_MSG_LEN_MAX+1);
            snprintf(req, BEACON_MSG_LEN_MAX, "%d\n", counter);
            err_t er = udp_sendto(pcb, p, &addr, UDP_PORT);
            pbuf_free(p);
            if (er != ERR_OK) {
                printf("Failed to send UDP packet! error=%d\n", er);
            } else {
                printf("Sent packet %d\n", counter);
                counter++;
            }

            cyw43_arch_poll();
            sleep_ms(BEACON_INTERVAL_MS);
        }
    }

    cyw43_arch_deinit();
}