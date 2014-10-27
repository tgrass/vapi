#ifndef __WALN_H
#define __WALN_H

#include "common.h"
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>

BEGIN_EXTERN_C

#define VAPI_MAX_WLANS          16
#define VAPI_MAX_WLAN_INFOS     16


typedef struct vapi_wlan {
    uint32_t    id;
    uint32_t    hidden;
    uint32_t    max_stations;
    char        essid[32];
    uint32_t    encryt_mode;

    u_char      encryt_key[32];
    struct in_addr radius_ip;
    u_int16_t   radius_port;
    u_int16_t   freq;
} vapi_wlan_t;

typedef struct vapi_wan_info {
    char        essid[32];
    u_char      bssid[6];
    uint32_t    hidden;
    uint32_t    encryt_mode;
    uint32_t    current_stations;
    uint64_t    recv_packets;
    uint64_t    recv_gbytes;
    uint64_t    recv_bytes;
    uint64_t    send_packets;
    uint64_t    send_gbytes;
    uint64_t    send_bytes;
    uint32_t    freq;
    uint32_t    channel;

    int         txpower;
    uint32_t    hwmode;
    uint32_t    htmode;
    char        ifname[32];
} vapi_wlan_info_t;

int vapi_add_wlan(const vapi_wlan_t *wlan);

int vapi_del_wlan(uint32_t id);

int vapi_set_wlan(const vapi_wlan_t *wlan);

int vapi_get_all_wlans(vapi_wlan_info_t *wlan_infos);

int vapi_del_all_wlans(void);



END_EXTERN_C

#endif
