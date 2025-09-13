/**
 * Copyright (c) inveh.
 *
 */
#include "rpi2350_ha_wifi_inf.h"
#include "rpi2350_ha_wifi_priv.h"
#include "rpi2350_ha_wifi_pub.h"

/* Value for the UDP protocol */
#define UDP_PORT 13400
#define BEACON_MSG_LEN_MAX 4    /* actual length would be +1 */
#define BEACON_TARGET "255.255.255.255"
#define BEACON_INTERVAL_MS 1000

wifi_device_state_t rpi2350_ha_wifi_st;
ip_addr_t addr;
struct udp_pcb* pcb;

/**
 * @brief Initializes the Wi-Fi functionality using the CYW43 driver.
 */
void rpi2350_ha_wifi_init(void) 
{
    rpi2350_ha_wifi_st = DEVICE_WIFI_START_UP;
}

void rpi2350_ha_wifi_1000ms() 
{    
    int rc;

    switch (rpi2350_ha_wifi_st)
    {
        case DEVICE_WIFI_START_UP:
        {
            if(rpi2350_ha_ble_st != 0)
            {     
                cyw43_arch_enable_sta_mode();   
                rc = cyw43_arch_wifi_connect_timeout_ms(rpi2350_ha_ble_ssid, rpi2350_ha_ble_password, CYW43_AUTH_WPA2_AES_PSK, 30000);
        
                if (rc != 0) 
                {
                    rpi2350_ha_wifi_st = DEVICE_WIFI_ERROR;      
                }
                else
                {
                    rpi2350_ha_wifi_st = DEVICE_WIFI_LINK_UDP;
                }

                pcb = udp_new();
                ipaddr_aton(BEACON_TARGET, &addr);
            }
        }
        break;

        case DEVICE_WIFI_LINK_UDP:
        {
            struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, BEACON_MSG_LEN_MAX+1, PBUF_RAM);
            char *req = (char *)p->payload;
            memset(req, 0, BEACON_MSG_LEN_MAX+1);
            snprintf(req, BEACON_MSG_LEN_MAX, "%d\n", 0x642126);
            err_t er = udp_sendto(pcb, p, &addr, UDP_PORT);
            pbuf_free(p);
            if (er != ERR_OK) 
            {
                printf("Failed to send UDP packet! error=%d\n", er);

                rpi2350_ha_wifi_st = DEVICE_WIFI_ERROR;
            } 
            else 
            {
                printf("Udp packet sent\n");

                rpi2350_ha_wifi_st = DEVICE_WIFI_LINK_TCPIP;
            }
        }
        break;

        case DEVICE_WIFI_LINK_TCPIP:
        {
            /* Establish TcpIp */
            cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
            cyw43_cb_tcpip_deinit(&cyw43_state, CYW43_ITF_STA);
            cyw43_cb_tcpip_deinit(&cyw43_state, CYW43_ITF_AP);
            cyw43_cb_tcpip_init(&cyw43_state, CYW43_ITF_STA);
            cyw43_cb_tcpip_init(&cyw43_state, CYW43_ITF_AP);

            rpi2350_ha_wifi_st = DEVICE_WIFI_RUNNING;
        }
        break;

        default: /* DEVICE_WIFI_ERROR */
        {
            /* This situation be never arrived */
            cyw43_arch_deinit();
        }
        break;
    }


}