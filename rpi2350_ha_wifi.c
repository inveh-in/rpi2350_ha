/**
 * Copyright (c) inveh.
 *
 */
#include "rpi2350_ha_inf.h"
#include "rpi2350_ha_priv.h"

/* Value for the UDP protocol */
#define UDP_PORT 13400
#define BEACON_MSG_LEN_MAX 4    /* actual length would be +1 */
#define BEACON_TARGET "255.255.255.255"
#define BEACON_INTERVAL_MS 1000

/**
 * @brief Periodically checks the Wi-Fi link status and processes relevant events.
 */
static void wifi_task(void) {
    int status;

    switch (current_state) {
        case DEVICE_WIFI_LINK_TO_UP:
            status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
            printf("[WIFI] Link status: %d\n", status);
            if (status == CYW43_LINK_JOIN) {
                process_event(EVENT_WIFI_CONNECTED);
            } else if (status < 0) {
                process_event(EVENT_WIFI_ERROR);
            }
            break;
        case DEVICE_WIFI_LINK_UP:
            status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
            if (status == CYW43_LINK_NONET) {
                printf("[WIFI] Error: No matching SSID found\n");
                wifi_setting.ssid[0] = '\0';
                process_event(EVENT_WIFI_ERROR);
            } else if (status == CYW43_LINK_BADAUTH) {
                printf("[WIFI] Error: Authentication failure\n");
                wifi_setting.password[0] = '\0';
                process_event(EVENT_WIFI_ERROR);
            }

            if (status == CYW43_LINK_JOIN && (*(uint32_t *)&cyw43_state.netif[0].ip_addr) != 0) {
                char *ip_address = ip4addr_ntoa(&cyw43_state.netif[0].ip_addr);
                strcpy(wifi_setting.ip_address, ip_address);
                printf("[WIFI] Acquired IP address: %s\n", wifi_setting.ip_address);
                wifi_setting.link_status = 1;

                process_event(EVENT_IP_ACQUIRED);
            }
            break;
        case DEVICE_WIFI_LINK_CONNECTED:
            process_event(EVENT_WIFI_CONNECTED);
            break;
        case DEVICE_WIFI_LINK_DOWN:
            if (strlen(wifi_setting.ssid) > 0 && strlen(wifi_setting.password) > 0) {
                process_event(EVENT_WIFI_CONNECT);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Initializes the Wi-Fi functionality using the CYW43 driver.
 */
void rpi2350_ha_wifi_init(void) 
{
    cyw43_arch_enable_sta_mode();
}

void rpi2350_ha_wifi_proc(__unused void *params) 
{
        struct udp_pcb* pcb = udp_new();
    ip_addr_t addr;
    ipaddr_aton(BEACON_TARGET, &addr);

    int counter = 0;

    while (true) 
    {        
        wifi_task();
        
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

    cyw43_arch_deinit();
}