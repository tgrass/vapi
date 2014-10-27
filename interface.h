#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "common.h"

BEGIN_EXTERN_C

#define VAPI_LOG_LINE_MAX_BYTES     128
#define VAPI_LOG_MAX_LINES          1024

typedef struct vapi_wan_config {
    int             type;
    struct in_addr  *ip;
    struct in_addr  *netmask;
    struct in_addr  *gateway;
    struct in_addr  *dns1;
    struct in_addr  *dns2;
    char            username[64];
    char            password[128];
    int             redial_mode;
    int             timeout;
    int             mtu;
    int             enable_defaultroute;
    int             enable_vpn;
    struct in_addr  *pptp_ip;
    struct in_addr  *pptp_netmask;
    struct in_addr  *pptp_serverip;
    int             vlan_id;
    int             radio;
    int             auth_type;
    char            auth_password[32];
    int             hwmode;
    int             htmode;
    int             txpower;
    uint8_t         bssid[6];
} vapi_wan_config_t;

int vapi_log(int line, const char *log);

int  vapi_log_load(char *buf, int line);

int vapi_factory(void);

int vapi_add_br(const char *brname);

int vapi_del_br(const char *brname);

int vapi_add_br_if(const char *brname, const char *ifname);

int vapi_del_br_if(const char *brname, const char *ifname);

int vapi_add_wan_br_if(const char *ifname);

int vapi_del_wan_br_if(const char *ifname);

int vapi_add_lan_br_if(const char *ifname);

int vapi_del_lan_br_if(const char *ifname);

int vapi_dhcpserver_start(void);

int vapi_set_wan_config(vapi_wan_config_t *config);

int vapi_get_wan_config(vapi_wan_config_t *config);

int vapi_get_wan_vlan_id(void);

int vapi_add_vlan_if(int vid, char *ifname, int ifname_len);

int vapi_del_vlan_if(int vid);

int vapi_get_vlan_if_name(int vid, char *ifname, int ifname_len);

int vapi_iface_down(const char *ifname);

int vapi_iface_up(const char *ifname);

void vapi_wan_down(void);

int vapi_init(void);

int vapi_set_brlan_ip(struct in_addr *ipaddr);

void vapi_set_brlandhcp_config(int cfgid);

void vapi_set_firewall_nat(char *lan_ip);

void vapi_brlanipcfg_init(void);

END_EXTERN_C

#endif

