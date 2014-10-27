#ifndef __QOS_H__
#define __QOS_H__

#include "common.h"
#include <net/if.h>
#include "wlan.h"

BEGIN_EXTERN_C


typedef struct vapi_wlan_if {
    uint32_t    id;
    u_char      wlanif[IFNAMSIZ];
} vapi_wlan_if_t;


typedef struct vapi_qos {
    uint32_t    total_up;
    uint32_t    total_down;
    uint32_t    vslan_up;
    uint32_t    vslan_down;
    uint32_t    home_percent;
} vapi_qos_t;

vapi_wlan_if_t wlanifs[VAPI_MAX_WLANS];

int vapi_wifi_start(vapi_wlan_if_t *wlanifs, int wlan_num);

int vapi_wifi_start_finish(void);

int vapi_wifi_stop_finish(void);

int vapi_qos_start(void);

int vapi_qos_stop(void);

int vapi_qos_set(const vapi_qos_t *qos);

END_EXTERN_C

#endif
