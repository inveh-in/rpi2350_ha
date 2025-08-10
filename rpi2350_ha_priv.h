#ifndef _RPI2350_HA_PRIV_H
#define _RPI2350_HA_PRIV_H
typedef enum {
    DEVICE_START_UP = 0,
    DEVICE_WIFI_LINK_DOWN,
    DEVICE_WIFI_LINK_TO_UP,
    DEVICE_WIFI_LINK_UP,
    DEVICE_WIFI_LINK_CONNECTED,
    DEVICE_RUNNING,
    DEVICE_ERROR
} device_state_t;

typedef enum {
    EVENT_NONE = 0,
    EVENT_WIFI_CONFIGURED,
    EVENT_WIFI_CONNECT,
    EVENT_WIFI_CONNECTED,
    EVENT_WIFI_ERROR,
    EVENT_IP_ACQUIRED,
    EVENT_WIFI_DISCONNECTED,
    EVENT_ERROR_OCCURED
} device_event_t;

typedef struct {
    char ssid[33];
    char password[64];
    char ip_address[16];
    uint8_t link_status;
} wifi_setting_t;

typedef struct {
    char *data;
    size_t len;
    hci_con_handle_t *con_handle;
} notify_string_t;

#endif /* _RPI2350_HA_PRIV_H */
