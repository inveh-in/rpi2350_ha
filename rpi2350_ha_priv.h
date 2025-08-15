#ifndef _RPI2350_HA_PRIV_H
#define _RPI2350_HA_PRIV_H

/****************************************************************************************************/

// Declaration for Timer
struct timer_struct
{
  bool st_timer;
  unsigned long t_startTime;
};

//Start timer
void start_timer(timer_struct *timerX)
{
  if(timerX->st_timer == false)
  {
    timerX->t_startTime = millis();
    timerX->st_timer = true; 
  }
}

//Stop timer
void stop_timer(timer_struct *timerX)
{
  timerX->t_startTime = 0;
  timerX->st_timer = false;
}

//Get timer
unsigned long get_timer(timer_struct *timerX)
{
  return(millis() - timerX->t_startTime);
}
/****************************************************************************************************/
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
