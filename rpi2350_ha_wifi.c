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
 * @brief Executes actions upon entering a specific device state.
 *
 * @param state The device state being entered.
 */
static void state_entry_action(device_state_t state) {
    switch (state) {
        case DEVICE_WIFI_LINK_TO_UP: {
            printf(
                "[STATE] Entering: DEVICE_WIFI_LINK_TO_UP, attempting Wi-Fi connection to SSID: "
                "%s\n",
                wifi_setting.ssid);
            rpi2350_wifiEna_st = 0;
            cyw43_arch_enable_sta_mode();
            int rc = cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
            rc = cyw43_arch_wifi_connect_async(wifi_setting.ssid, wifi_setting.password,
                                               CYW43_AUTH_WPA2_AES_PSK);
            if (rc != 0) {
                printf("[WIFI] Wi-Fi connect async failed, error code: %d\n", rc);
                process_event(EVENT_ERROR_OCCURED);
            }
            break;
        }

        case DEVICE_WIFI_LINK_CONNECTED: {
            printf("[STATE] Entering: DEVICE_WIFI_LINK_CONNECTED, Wi-Fi connected\n");
            rpi2350_wifiEna_st = 0;
            if (con_handle != HCI_CON_HANDLE_INVALID) {
                notify_string_t notify;
                notify.data = wifi_setting.ip_address;
                notify.len = strlen(wifi_setting.ip_address);
                notify.con_handle = &con_handle;
                btstack_context_callback_registration_t context_registration;
                context_registration.callback = &notify_ip_address_callback;
                context_registration.context = &notify;
                att_server_request_to_send_notification(&context_registration, con_handle);
            }
            break;
        }
        case DEVICE_WIFI_LINK_DOWN: {
            printf("[STATE] Entering: DEVICE_WIFI_LINK_DOWN\n");
            rpi2350_wifiEna_st = 0;
            cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
            cyw43_cb_tcpip_deinit(&cyw43_state, 0);
            cyw43_cb_tcpip_deinit(&cyw43_state, 1);
            cyw43_cb_tcpip_init(&cyw43_state, 0);
            cyw43_cb_tcpip_init(&cyw43_state, 1);
            wifi_setting.ip_address[0] = '\0';

            if (con_handle != HCI_CON_HANDLE_INVALID) {
                att_server_notify(con_handle, IP_ADDRESS_HANDLE,
                                  (uint8_t *)&wifi_setting.ip_address,
                                  strlen(wifi_setting.ip_address));
            }
            break;
        }

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

void rpi2350_ha_wifi_10ms() 
{    
    struct udp_pcb* pcb = udp_new();
    ip_addr_t addr;
    ipaddr_aton(BEACON_TARGET, &addr);

    int counter = 0;
               
    if(true)
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
    }

    /* This situation be never arrived
     * cyw43_arch_deinit(); */
}