#include "neighbour.h"
#include "common.h"
#include "utils.h"

int vapi_scan_neighours(int freq, vapi_neighbour_t* neighours)
{
    return 0;
}

int vapi_station_logoff(const u_char* mac)
{
    return -1;
}

FILE *vapi_fwupdate_open(void)
{
    return NULL;
}

int vapi_fwupdate_write(FILE *fp, const uint8_t* buf, uint16_t len)
{
    return 0;
}

void vapi_fwupdate_close(FILE *fp)
{
}

int vapi_upgrade_now(const void *image, int length)
{
    return 0;
}

int vapi_upgrade_bin(char *bin_name)
{
    return 0;
}

int vapi_load_config(int type, void *data, int data_len)
{
    return 0;
}

int vapi_save_config(int type, void *data, int data_len)
{
    return 0;
}

