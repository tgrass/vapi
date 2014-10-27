#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include "common.h"
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>

BEGIN_EXTERN_C  


int vapi_get_wan_ip(struct in_addr *p);

int vapi_get_wan_mac(u_char *mac);

int vapi_get_lan_name(char *lan_name);

int vapi_get_hardware_version(char *buf, int buf_len);

int vapi_get_firmware_version(char *buf, int buf_len);

int vapi_get_cpu_usage(void);

int vapi_get_memory_usage(void);

static time_t g_cstime = (time_t)0;

time_t vapi_get_sys_time(void);

int vapi_set_sys_time(time_t t);

time_t vapi_get_uptime(void);

int vapi_get_vendor(char *buf, int buf_len);

int vapi_get_name(char *buf, int buf_len);

void vapi_reboot(void);

void vapi_open_shell(void);

END_EXTERN_C

#endif
