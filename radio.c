#include "common.h"
#include "radio.h"
#include "utils.h"
#include <glob.h>

static int vapi_get_iwinfo(const char *ifname, vapi_radio_t *radio);

int
vapi_get_radio(vapi_radio_t *radio)
{
    if (!radio)
        return -1;

    glob_t globbuf;
    char *p = NULL;
    int i = 0;

    glob("/sys/class/net/*", 0, NULL, &globbuf);

    for (i = 0; i < globbuf.gl_pathc; i++)
    {
        p = strrchr(globbuf.gl_pathv[i], '/');

        if (!p)
            continue;
        p++;

        if (strmatch(p, "ra") == 0 || strmatch(p, "wlan") == 0)
        {
            vapi_get_iwinfo(p, radio);
        }
    }
    globfree(&globbuf);

    return -1;
}

int vapi_set_radio(const vapi_radio_t *radio)
{
    if (ISNULL(radio))
    {
        return -1;
    }

    if (radio->enable == 0)
    {
        char buf[MAX_LINE] = { 0 };
        FILE *fp = fopen("/etc/config/wireless", "r");
        if (!fp)
        {
            return -1;
        }

        while (fgets(buf, MAX_LINE, fp) != NULL) {
            if (strstr(buf, "wifi-device") && strstr(buf, "config"))
            {
                char *p, *q;
                p = q = NULL;
                parse_config_option(buf, &p, &q);
                if (!q)
                {
                    fclose(fp);
                    return -1;
                }
                fclose(fp);
                execute("uci set wireless.%s.disabled=1", q);
                execute("uci commit wireless");
                execute("/etc/init.d/network restart");
                return 0;
            }
        }

        fclose(fp);
        return -1;
    }

    char buf[MAX_LINE] = { 0 };
    char *p, *q;
    p = q = NULL;
    FILE *fp = fopen("/etc/config/wireless", "r");
    if (!fp)
    {
        debug("fopen wireless: %s", strerror(errno));
        return -1;
    }

    while (fgets(buf, MAX_LINE, fp) != NULL) {
        if (strstr(buf, "config") && strstr(buf, "wifi-device"))
        {
            parse_config_option(buf, &p, &q);
            break;
        }
    }

    fclose(fp);
    if (!q)
        return -1;

    execute("uci set wireless.%s.disabled=0", q);

    if (radio->freq == 1)
    {
        if (radio->channel >= 1 && radio->channel <= 14)
            execute("uci set wireless.%s.channel=%d", q, radio->channel);
        else
            execute("uci set wireless.%s.channel=auto", q);
    }
    else if(radio->freq == 2)
    {
        switch(radio->channel)
        {
            case 34:
            case 36:
            case 38:
            case 40:
            case 42:
            case 44:
            case 46:
            case 48:
            case 52:
            case 56:
            case 60:
            case 64:
            case 100:
            case 104:
            case 108:
            case 112:
            case 116:
            case 120:
            case 124:
            case 128:
            case 132:
            case 136:
            case 140:
            case 149:
            case 153:
            case 157:
            case 161:
            case 165:
                execute("uci set wireless.%s.channel=%d", q, radio->channel);
                break;
            default:
                execute("uci set wireless.%s.channel=auto", q);
        }
    }
    else if(radio->freq == 3)
    {
        execute("uci set wireless.%s.channel=auto", q);
    }
    else
    {
        if (radio->channel == 0)
            execute("uci set wireless.%s.channel=auto", q);
        else
            execute("uci set wireless.%s.channel=%d", q, radio->channel);
    }

    if (radio->txpower == 255)
        execute("uci set wireless.%s.txpower=auto", q);
    else
        execute("uci set wireless.%s.txpower=%d", radio->txpower);


    switch(radio->htmode)
    {
        case 2:
            execute("uci set wireless.%s.htmode=HT40-", q);
            break;
        case 3:
            execute("uci set wireless.%s.htmode=20+40", q);
            break;
        case 4:
            execute("uci set wireless.%s.htmode=HT40", q);
            break;
        default:
            execute("uci set wireless.%s.htmode=HT20", q);
            break;
    }
    execute("uci commit wireless");
    execute("/etc/init.d/network restart");
    return 0;
}

int vapi_get_radio_type(void)
{
    FILE *fp;
    glob_t globbuf;
    int i;
    int flag = -1;
    char *p, buf[MAX_BUF] = { 0 };
    glob("/sys/class/net/*", 0, NULL, &globbuf);

    for (i = 0; i < globbuf.gl_pathc; i++)
    {
        p = strrchr(globbuf.gl_pathv[i], '/');
        if (!p)
            continue;
        p++;
        if (strstr(p, "ra") || strstr(p, "wlan"))
        {
            flag = 1;
            snprintf(buf, sizeof(buf), "iwinfo %s info", p);
            break;
        }
    }

    globfree(&globbuf);

    if (flag < 1)
        return -1;

    fp = popen(buf, "r");
    if (!fp)
    {
        debug("popen: %s", strerror(errno));
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL){
        if ((p = strstr(buf, "Channel")))
        {
            p = strchr(p, ':');
            if (!p)
            {
                pclose(fp);
                return -1;
            }

            p++;

            while (*p == ' ')
                p++;

            i = 1;
            while (*(p+i) != ' ')
                i++;
            *(p+i) = 0;

            if (atoi(p) >=1 && atoi(p) <= 14)
            {
                pclose(fp);
                return 1;
            } else if (atoi(p) >= 34 && atoi(p) <= 165)
            {
                pclose(fp);
                return 2;
            }
            else if (atoi(p) <= 0)
            {
                pclose(fp);
                return -1;
            }
            else
            {
                pclose(fp);
                return 3;
            }
        }
    }

    pclose(fp);

    return -1;
}


static int vapi_get_iwinfo(const char *ifname, vapi_radio_t *radio)
{
    if (!radio)
    {
        return -1;
    }
    FILE *fp = NULL;
    char buf[MAX_BUF] = { 0 };
    int flag = -1;
    char *p, *q;
    p = q = NULL;
    int enable = -1;    /* Flags for radio enable or disabled; */

    radio->hwmode = 1;
    radio->flags = 0;

    fp = fopen("/etc/config/wireless", "r");
    if (!fp)
    {
        debug("fopen wireless: %s", strerror(errno));
        return -1;
    }

    while (fgets(buf, MAX_BUF, fp) != NULL) {
        if (strstr(buf, "config"))
        {
option:
            if (strstr(buf, "wifi-device"))
                flag = 1;
            else if (strstr(buf, "wifi-iface"))
                flag = 2;
            while (fgets(buf, MAX_BUF, fp) != NULL) {
                if (strstr(buf, "config")) {
                    if (strstr(buf, "wifi-device"))
                        flag = 1;
                    else if (strstr(buf, "wifi-iface"))
                        flag = 2;
                    goto option;
                } else if (strstr(buf, "option")) {
                    switch(flag)
                    {
                        case 1:
                            {
                                parse_config_option(buf, &p, &q);
                                if (strmatch(p, "channel") == 0)
                                {
                                    if (strmatch(q, "auto") == 0)
                                    {
                                        radio->channel = 0;
                                        radio->freq = 0;
                                    }
                                    else
                                    {
                                        radio->channel = atoi(q);
                                        if (radio->channel >= 1 && radio->channel <= 14)
                                            radio->freq = 1;
                                        else if (radio->channel >= 34 && radio->channel < 165)
                                            radio->freq = 2;
                                    }
                                }
                                else if (strmatch(p, "txpower") == 0)
                                {
                                    radio->txpower = atoi(q);
                                }
                                else if (strmatch(p, "ht") == 0)
                                {
                                    if (strcmp(q, "HT20") == 0)
                                        radio->htmode = 1;
                                    else if (strcmp(q, "HT40") == 0)
                                        radio->htmode = 4;
                                    else if (strcmp(q, "20+40") == 0)
                                        radio->htmode = 3;
                                    else 
                                        radio->htmode = 2;
                                }
                                else if (strmatch(p, "disabled") == 0)
                                {
                                    if (atoi(q) == 0)
                                        enable = 1;
                                    else
                                        enable = 0;
                                }
                                else if (strmatch(p, "enabled") == 0)
                                {
                                    if (atoi(q) == 0)
                                        enable = 0;
                                    else
                                        enable = 1;
                                }
                                p = NULL, q = NULL;
                            }
                            break;
                        case 2:
                            {
                            }
                            break;
                        default:
                            debug("Unkown config or optoins");
                            break;
                    }
                }
            }
        }
    }

    fclose(fp);

    if (flag < 0)
    {
        debug("Read wireless file error");
        return -1;
    }

    if (enable != 0)
        radio->enable = 1;

    return 0;
}
