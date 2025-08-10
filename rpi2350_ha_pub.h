#ifndef _RPI2350_HA_PUB_H
#define _RPI2350_HA_PUB_H

extern device_state_t current_state;
extern wifi_setting_t wifi_setting;
extern void process_event(device_event_t event);

extern void rpi2350_ha_ble_init(void);
extern void rpi2350_ha_ble_proc(__unused void *params);
extern void rpi2350_ha_wifi_init(void);
extern void rpi2350_ha_wifi_proc(__unused void *params);

#endif /* _RPI2350_HA_PUB_H */
