#include "interface.h"
#include "common.h"
#include "utils.h"
#include "system.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum WAN_TYPE{
    WAN_DHCP = 1,
    WAN_STATIC = 2,
    WAN_PPPOE = 3,
    WAN_PPTP = 4,
};

int vapi_log(int line, const char *log)
{
    return 0;
}

int vapi_log_load(char *buf, int line)
{
    return 0;
}

int vapi_factory(void)
{
    chdir("/");
    execute("killall dropbear uhttpd; sleep 1; mtd -r erase rootfs_data");
    return 0;
}

int vapi_add_br(const char *brname)
{
    return 0;
}

int vapi_del_br(const char *brname)
{
    return 0;
}

int vapi_add_br_if(const char *brname, const char *ifname)
{
    return -1;
}

int vapi_del_br_if(const char *brname, const char *ifname)
{
    return -1;
}

int vapi_add_wan_br_if(const char *ifname)
{
    return -1;
}

int vapi_del_wan_br_if(const char *ifname)
{
    return -1;
}

int vapi_add_lan_br_if(const char *ifname)
{
    return -1;
}

int vapi_dhcpserver_start(void)
{
    return execute("/etc/init.d/dnsmasq restart");
}

int vapi_set_wan_config(vapi_wan_config_t *config)
{
    if (!config)
    {
        debug("vapi_set_wan_config paramter is error");
        return -1;
    }

    if (config->type < WAN_DHCP || config->type > WAN_PPTP)
    {
        debug("Unknown wan config type");
        return -1;
    }

    FILE *fp = fopen("/etc/config/network", "r");
    if (!fp)
    {
        debug("fopen network error: %s", strerror(errno));
        return -1;
    }
    fclose(fp);
    
    switch(config->type)
    {
        case WAN_DHCP:
            {
                execute("uci delete network.wan");
                execute("uci set network.wan=interface");
                execute("uci set network.wan.ifname=eth0.2");
                execute("uci set network.wan.proto=dhcp");
                execute("uci commit network");
                execute("/etc/init.d/network restart");
                return 0;
            }

        case WAN_STATIC:
            {
                if (ISNULL(config->ip) || 
                        ISNULL(config->netmask) ||
                        ISNULL(config->gateway) ||
                        ISNULL(config->dns1))
                {
                    debug("Set static wan interface, lack of full paramters");
                    return -1;
                }

                execute("uci delete network.wan");
                execute("uci set network.wan=interface");
                execute("uci set network.wan.ifname=eth0.2");
                execute("uci set network.wan.proto=static");
                execute("uci set network.wan.ipaddr=\'%s\'", inet_ntoa(*(config->ip)));
                execute("uci set network.wan.netmask=\'%s\'", inet_ntoa(*(config->netmask)));
                execute("uci set network.wan.gateway=\'%s\'", inet_ntoa(*(config->gateway)));
                if (config->dns1 && ISNULL(config->dns2))
                    execute("uci set network.wan.dns=\'%s\'", inet_ntoa(*(config->dns1)));
                else
                    execute("uci set network.wan.dns=\'%s %s\'",
                            inet_ntoa(*(config->dns1)), inet_ntoa(*(config->dns2)));
                execute("uci commit network");
                execute("/etc/init.d/network restart");
                return 0;
            }
            break;
        case WAN_PPPOE:
            {
                if (strlen_zero(config->username) || strlen_zero(config->password))
                    return -1;

                strmove(config->username);
                strmove(config->password);
                execute("uci delete network.wan");
                execute("uci set network.wan=interface");
                execute("uci set network.wan.ifname=eth0.2");
                execute("uci set network.wan.proto=pppoe");
                execute("uci set network.wan.username=\'%s\'", config->username);
                execute("uci set network.wan.password=\'%s\'", config->password);
                if (config->timeout > 0)
                {
                    execute("uci set network.wan.timeout=%d", config->timeout);
                }
                execute("uci commit network");
                execute("/etc/init.d/network restart");
                return 0;
            }
            break;
        case WAN_PPTP:
            {
                if (strlen_zero(config->username) || strlen_zero(config->password))
                {
                    return -1;
                }
                strmove(config->username);
                strmove(config->password);
                execute("uci delete network.wan");
                execute("uci set network.wan=interface");
                execute("uci set network.wan.ifname=eth0.2");
                execute("uci set network.wan.proto=pptp");
                execute("uci set network.wan.username=\'%s\'", config->username);
                execute("uci set network.wan.password=\'%s\'", config->password);
                execute("uci commit network");
                execute("/etc/init.d/network restart");
                return 0;
            }
        default:
            return -1;
    }
    return -1;
}

int vapi_get_wan_config(vapi_wan_config_t *config)
{
    if (ISNULL(config))
    {
        debug("Wan config is NULL");
        return -1;
    }

    FILE *fp;
    char buf[MAX_LINE] = { 0 };

    fp = popen("uci -P /var/state show network.wan", "r");
    if (!fp)
    {
        debug("vapi_get_wan_config error: %s", strerror(errno));
        return -1;
    }

    config->type = -1;

    while(fgets(buf, MAX_LINE, fp) != NULL) {
        if (strstr(buf, "proto"))
        {
            char *p = strchr(buf, '=');
            if (!p)
                return -1;
            p++;
            if (strmatch(p, "dhcp") == 0)
                config->type = WAN_DHCP;
            else if (strmatch(p, "pppoe") == 0)
                config->type = WAN_PPPOE;
            else if (strmatch(p, "static") == 0)
                config->type = WAN_STATIC;
            else if (strmatch(p, "pptp") == 0)
                config->type = WAN_PPTP;
            else 
                return -1;

            break;
        }
    }

    pclose(fp);

    switch(config->type)
    {
        case WAN_DHCP:
            {
                vapi_get_wan_mac((u_char*)config->bssid);
            }
            return 0;
        case WAN_STATIC:
            {
                if (config->ip)
                {
                    free(config->ip);
                    config->ip = NULL;
                }
                if (config->netmask)
                {
                    free(config->netmask);
                    config->netmask = NULL;
                }
                if (config->gateway)
                {
                    free(config->gateway);
                    config->gateway = NULL;
                }
                if (config->dns1)
                {
                    free(config->dns1);
                    config->dns1 = NULL;
                }

                if (config->dns2)
                {
                    free(config->dns2);
                    config->dns2 = NULL;
                }

                FILE *fp = popen("uci show network.wan", "r");
                char buf[MAX_LINE] = { 0 };
                char *p;
                if (!fp)
                    return -1;
                while (fgets(buf, MAX_LINE, fp) != NULL) {
                    p = strchr(buf, '=');
                    if (!p)
                        continue;
                    p++;
                    if (strstr(buf, "ipaddr"))
                    {
                        config->ip = (struct in_addr*)malloc(sizeof(struct in_addr));
                        if (ISNULL(config->ip))
                            exit(-1);
                        inet_aton(p, config->ip);
                    } 
                    else if (strstr(buf, "netmask"))
                    {
                        config->netmask = (struct in_addr*)malloc(sizeof(struct in_addr));
                        if (ISNULL(config->netmask))
                            exit(-1);
                        inet_aton(p, config->netmask);
                    }
                    else if (strstr(buf, "gateway"))
                    {
                        config->gateway = (struct in_addr*)malloc(sizeof(struct in_addr));
                        if (ISNULL(config->gateway))
                            exit(-1);
                        inet_aton(p, config->gateway);
                    }
                    else if (strstr(buf, "dns"))
                    {
                        char *q = p;
                        char tmp;
                        while(*q != ' ' && *q != '\t' && *q != '\n' &&  *q != '\r')
                            q++;

                        tmp = *q;
                        *q = 0;
                        
                        config->dns1 = (struct in_addr*)malloc(sizeof(struct in_addr));
                        if (ISNULL(config->dns1))
                            exit(-1);
                        inet_aton(p, config->dns1);
                        if (tmp == '\n' || tmp == 0)
                            continue;
                        q++;
                        while (*q == ' ' || *q == '\t')
                            q++;
                        if (*q == '\n' ||*q == 0)
                            continue;
                        p = q;
                        while (!isspace(*q))
                            q++;
                        *q = 0;
                        config->dns2 = (struct in_addr*)malloc(sizeof(struct in_addr));
                        if (ISNULL(config->dns2))
                            continue;
                        inet_aton(p, config->dns2);
                    }
                }
                pclose(fp);
            }
            return 0;
        case WAN_PPPOE:
        case WAN_PPTP:
            {
                FILE *fp;
                char buf[MAX_BUF] = { 0 };
                char *p, *q;
                bzero(config->username, sizeof(config->username));
                bzero(config->password, sizeof(config->password));
                fp = fopen("/etc/config/network", "r");
                if (!fp)
                    return -1;
                while (fgets(buf, MAX_BUF, fp) != NULL) {
                    if (strstr(buf, "config") && strstr(buf, "interface") && strstr(buf, "wan")){
                        while (fgets(buf, MAX_BUF, fp) != NULL) {
                            if (strstr(buf, "config"))
                                break;
                            char *p, *q;
                            p = q = NULL;
                            parse_config_option(buf, &p, &q);
                            if (p && q)
                            {
                                if (strstr(p, "username"))
                                    strcpy(config->username, q);
                                if (strstr(p, "password"))
                                    strcpy(config->password, q);
                            }
                            p = q = NULL;
                        }
                        break;
                    }
                }
                fclose(fp);

                if (strlen_zero(config->username) || strlen_zero(config->password))
                    return -1;
            }
            return 0;
        default:
            return -1;
    }

    return -1;
}

int vapi_get_wan_vlan_id(void)
{
    return -1;
}

int vapi_add_vlan_if(int vid, char *ifname, int ifname_len)
{
    return 0;
}

int vapi_del_vlan_if(int vid)
{
    return -1;
}

int vapi_get_vlan_if_name(int vid, char *ifname, int ifname_len)
{
    return 0;
}

int vapi_iface_down(const char *ifname)
{
    return -1;
}

int vapi_iface_up(const char *ifname)
{
    return -1;
}

void vapi_wan_down(void)
{
}

int vapi_init(void)
{
    return 0;
}

int vapi_set_brlan_ip(struct in_addr *ipaddr)
{
    char *ip = inet_ntoa(*ipaddr);
    if (!ip)
    {
        debug("Illigal ip address!");
        return -1;
    }

    if (strmatch(ip, "0.0.0.0") == 0 || 
            strmatch(ip, "127.0.0.1") == 0 ||
            strmatch(ip, "255.255.255.255") == 0)
    {
        debug("Illigal ip address!");
        return -1;
    }

    execute("uci set network.lan.ipaddr=\'%s\'", ip);
    execute("uci commit network");
    execute("/etc/init.d/network restart");

    return 0;
}

void vapi_set_brlandhcp_config(int cfgid)
{
    switch(cfgid)
    {
        case 0:
            return;
        case 1:
            return;
        default:
            return;
    }
    return;
}

void vapi_set_firewall_nat(char *lan_ip)
{
    return;
}

/*
 * Default br-lan ipv4 setting, dhcp and nat services initial function
 * Use 10.0.0.1 as the default setting
 * VAPI inner function
 */

void vapi_brlanipcfg_init(void)
{
    execute("uci set network.lan.ipaddr=10.0.0.1");
    execute("uci commit network");
    execute("/etc/init.d/network restart");
    return;
}

