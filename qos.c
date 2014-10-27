#include "qos.h"
#include "common.h"
#include "utils.h"

int vapi_wifi_start(vapi_wlan_if_t *wlanifs, int wlan_num)
{
    return 0;
}

int vapi_wifi_start_finish(void)
{
    return -1;
}

int vapi_wifi_stop(void)
{
    return -1;
}

int vapi_wifi_stop_finish(void)
{
    return 0;
}

int vapi_qos_start(void)
{
    int ret = execute("/etc/init.d/qos restart");
    return ret;
}

int vapi_qos_stop(void)
{
    int ret = execute("/etc/init.d/qos stop");
    return ret;
}

int vapi_qos_set(const vapi_qos_t *qos)
{
    return 0;
}

