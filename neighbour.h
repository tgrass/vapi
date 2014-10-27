#ifndef __NEIGHHOUR_H
#define __NEIGHHOUR_H

#include "common.h"

BEGIN_EXTERN_C

#define VAPI_MAX_NEIGHBOURS     20
#define VAPI_FWUPDATE_PATH      "/tmp/"

typedef struct vapi_neighbour {
    char        essid[32];
    u_char      bssid[6];
    uint32_t    freq;
    uint32_t    channel;

    int         txpower;
    uint32_t    hwmode;
    uint32_t    htmode;
} vapi_neighbour_t;

int vapi_scan_neighours(int freq, vapi_neighbour_t *neighbours);

int vapi_station_logoff(const u_char *mac);

FILE *vapi_fwupdate_open(void);

int vapi_fwupdate_write(FILE *fp, const uint8_t *buf, uint16_t len);

void vapi_fwupdate_close(FILE *fp);

int vapi_upgrade_now(const void *image, int length);

int vapi_upgrade_bin(char *bin_name);

int vapi_load_config(int type, void *data, int data_len);

int vapi_save_config(int type, void *data, int data_len);


END_EXTERN_C

#endif
