#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdint.h>

#include "utils.h"
#include "system.h"

static int vapi_get_meminfo(const char *buf)
{
    char *p;
    p = strchr(buf, ':');
    if (!p)
    {
        debug("vapi_get_meminfo: Data format is error");
        return -1;
    }
    p++;
    while (*p == ' ' || *p == '\t')
        p++;
    return atoi(p);
}


int vapi_get_wan_ip(struct in_addr *ip)
{
    if (!ip)
    {
        debug("Parameter is NULL!");
        return -1;
    }
    struct ifreq if_data;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_PACKET, htons(0x8086))) < 0)
    {
        debug("socket(): %s", strerror(errno));
        return -1;
    }

    strcpy(if_data.ifr_name, "eth0.2");

    if (ioctl(sockfd, SIOCGIFADDR, &if_data) < 0)
    {
        debug("ioctl(): SIOCGIFADDR %s", strerror(errno));
        close(sockfd);
        return -1;
    }

    memcpy((void *)(&(ip->s_addr)), (void *)&if_data.ifr_addr.sa_data + 2, 4);
    close(sockfd);
    return 0;
}

int vapi_get_wan_mac(u_char *mac)
{
    if (!mac)
    {
        debug("Paramter is wrong, Mac is NULL!\n");
        return -1;
    }

    int i, s;
    struct ifreq ifr;
    char *hwaddr;

    strcpy(ifr.ifr_name, "eth0.2");

    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        debug("vapi_get_wan_mac socket: %s", strerror(errno));
        return -1;
    }

    if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0)
    {
        debug("vapi_get_wan_mac ioctl(SIOCGIFHWADDR): %s", strerror(errno));
        close(s);
        return -1;
    }

    hwaddr = ifr.ifr_hwaddr.sa_data;
    close(s);
    int co;
    for (co = 0; co < 6; co++)
    {
        mac[co] = hwaddr[co] & 0xFF;
    }
    return 0;
}

int vapi_get_lan_name(char *lan_name)
{
    return -1;
}


int vapi_get_hardware_version(char *buf, int buf_len)
{
    if (!buf || buf_len <= 0)
    {
        debug("FATAL error: buffer is not enogh!");
        return -1;
    }

    FILE *fp = NULL;
    char buffer[1024] = { 0 };

    fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
    {
        debug("Get data error: %s", strerror(errno));
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strmatch(buffer, "system type") == 0)
        {
            char *p = strchr(buffer, ':');
            char *q = NULL;
            int len = 0;
            if (!p) 
            {
                debug("File format error!");
                fclose(fp);
                return -1;
            }
            p++;
            while ((*p == ' ') || (*p == '\t'))
                p++;
            q = p;
            len = strlen(p);
            while ((*q != '\n') && (*q != '\0') && ((q-p) <= len))
                q++;
            if (*q == '\n')
                *q = '\0';
            strncpy(buf, p, buf_len);
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -1;
}


int vapi_get_firmware_version(char *buf, int buf_len)
{
    if (!buf || buf_len <= 0)
    {
        debug("Buffer is not enough full!");
        return -1;
    }

    FILE *fp = NULL;
    char buffer[1024] = { 0 };

    fp = fopen("/etc/openwrt_release", "r");
    if (!fp)
    {
        debug("Get data from /etc/openwrt_release error: %s", strerror(errno));
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strmatch(buffer, "DISTRIB_DESCRIPTION") == 0) {
            char *p, *q;
            int len = 0;
            p = strchr(buffer, '=');
            if (!p)
            {
                debug("Data format error");
                fclose(fp);
                return -1;
            }
            p++;
            while (*p == '\'' || *p == '\"' || *p == ' ')
                p++;
            q = p;
            len = strlen(p);
            while (*q != '\'' && *q != '\"' && *q != '\n' && *q != '\0' && (q-p) <= len)
                q++;
            if (*q == '\'' || *q == '\"' || *q == '\n')
                *q = '\0';
            strncpy(buf, p, buf_len);
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return -1;
}

int vapi_get_cpu_usage(void)
{
    FILE *fp = NULL;
    char buf[1024] = { 0 };
    char *p, *q;
    unsigned int total = 0;
    unsigned int cpu[10] = { 0 }; 
    double tmp = 0;
    p = q = NULL;

    fp = fopen("/proc/stat", "r");
    if (!fp)
    {
        debug("vapi_get_cpu_usage error: %s", strerror(errno));
        return -1;
    }

    if (fgets(buf, sizeof(buf), fp) != NULL) {
        if (strmatch(buf, "cpu") != 0)
        {
            fclose(fp);
            return -1;
        }

        int i = 0;
        p = q = buf;
        while (*p != ' ' && *p != '\t')
            p++;
        while (*p == ' ' || *p == '\t')
            p++;
        q = p;
        while (*q != ' ')
            q++;
        *q = '\0';
        cpu[0] = strtoul(p, NULL, 10);
        total += cpu[0];
        p = ++q;
        for (i=0; i < 7; i++)
        {
            if (*q == '\n' || *q == '\0')
                break;
            while (*q != ' ')
                q++;
            *q = '\0';
            cpu[i+1] = strtoul(p, NULL, 10);
            total += cpu[i+1];
            p = ++q;
        }

        fclose(fp);

        return ((int)round((total - cpu[3]) * 1.0 * 100 / total));
    }

    fclose(fp);
    return -1;
}

int vapi_get_memory_usage(void)
{
    FILE *fp = NULL;
    int total = -1;
    int available = -1;
    int buffers = -1;
    int free = -1;
    char buf[200] = { 0 };
    char *p;

    fp = fopen("/proc/meminfo", "r");
    if (!fp)
    {
        debug("Open file error: %s", strerror(errno));
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        if (total > 0 && available > -1 && buffers > -1)
            break;
        if (strnmatch(buf, "MemTotal") == 0)
        {
            total = vapi_get_meminfo(buf);
        } else if (strnmatch(buf, "MemAvailable") == 0)
        {
            available = vapi_get_meminfo(buf);
        } else if (strnmatch(buf, "Buffers") == 0)
            buffers = vapi_get_meminfo(buf);
        else if (strnmatch(buf, "MemFree") == 0)
            free = vapi_get_meminfo(buf);
    }

    fclose(fp);

    if (total <= 0 || buffers < 0)
        return -1;

    if (available < buffers && free < 0)
        return -1;

    if (available > 0)
        return ((int)round((available - buffers) * 1.0 * 100 / total));
    else
        return ((int)round((total - free) * 1.0 * 100 / total));
}

time_t
vapi_get_sys_time(void)
{
    return (g_cstime + time(NULL));
}

int vapi_set_sys_time(time_t t)
{
    g_cstime = t - time(NULL);
    return 0;
}

time_t
vapi_get_uptime(void)
{
    FILE *fp = NULL;
    time_t uptime;
    char *p = NULL;
    char buf[100] = { 0 };

    fp = fopen("/proc/uptime", "r");
    if (!fp)
    {
        debug("vapi_get_uptime: %s", strerror(errno));
        return 0;
    }

    fgets(buf, sizeof(buf), fp);
    p = buf;
    while (*p != '.' && *p != '\0' && *p != ' ')
        p++;
    *p = 0;

    uptime = (time_t)strtoul(buf, NULL, 10);
    return uptime;
}

int
vapi_get_vendor(char *buf, int buf_len)
{
    return -1;
}

int vapi_get_name(char *buf, int buf_len)
{
    if (!buf || buf_len < 0)
        return -1;

    FILE *fp = popen("uname", "r");
    if (!fp)
    {
        debug("popen uname error: %s", strerror(errno));
        return -1;
    }

    if (fgets(buf, buf_len, fp) != NULL)
    {
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = 0;
        pclose(fp);
        return 0;
    }

    pclose(fp);
    return -1;
}

void vapi_reboot(void)
{
    execute("reboot");
}

void vapi_open_shell(void)
{
    return;
}
