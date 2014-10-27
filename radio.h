#ifndef __RADIO_H__
#define __RADIO_H__

#include <stdint.h>
#include "common.h"


BEGIN_EXTERN_C  

#define VAPI_RADIO_FLAG_MIN_RSSI        0x01

typedef struct vapi_radio {
    uint32_t    enable;
    uint32_t    freq;
    uint32_t    channel;
    uint32_t    txpower;
    uint32_t    hwmode;
    uint32_t    htmode;
    uint32_t    flags;
    int         min_rssi;
} vapi_radio_t;

int vapi_get_radio(vapi_radio_t *radio);

int vapi_set_radio(const vapi_radio_t *radio);

int vapi_get_radio_type(void);

END_EXTERN_C

#endif
