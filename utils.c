#define _GNU_SOURCE
#include "utils.h"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <ctype.h>

extern int daemon_debug;

int execute(const char *fmt, ...)
{
    int pid,
        status,
        rc;
    va_list ap;

    char cmd[MAX_LINE] = { 0 };
    va_start(ap, fmt);
    vsnprintf(cmd, MAX_LINE, fmt, ap);
    va_end(ap);

    const char *new_argv[4];
    new_argv[0] = "/bin/sh";
    new_argv[1] = "-c";
    new_argv[2] = cmd;
    new_argv[3] = NULL;

    pid = fork();
    if (pid == 0) {
        if (execvp("/bin/sh", (char * const *)new_argv) == -1)
            printf("execvp(): %s\n", strerror(errno));
        exit(1);
    } else if (pid < 0) {
        printf("fork error: %s\n", strerror(errno));
        exit(1);
    }

    rc = waitpid(pid, &status, 0);

    return (WEXITSTATUS(status));
}

void debug(const char *fmt, ...)
{
    va_list va;
    char buf[1024] = { 0 };
    time_t ts = time(NULL);

    if (!daemon_debug)
        return;

    fprintf(stdout, "[%.24s](%s:%d) ", ctime(&ts), __FILE__, __LINE__);
    va_start(va, fmt);
    vfprintf(stdout, fmt, va);
    va_end(va);
    fputc('\n', stdout);
}

int strmatch(const char *buf, const char *pattern)
{
    int len = strlen(pattern);
    if (len > strlen(buf))
        return -1;

    return memcmp(buf, pattern, len);
}

int strnmatch(const char *buf, const char *pattern)
{
    int len = strlen(pattern);
    if (len > strlen(buf))
        return -1;

    return strncasecmp(buf, pattern, len);
}

char *strmove(char src[])
{
    if (src[0] == '\0')
        return NULL;

    char *p, *q = NULL;
    p = q = src;

    while (*q == '\t' || *q == ' ')
        q++;
    p = q;
    q = src + (strlen(src)-1);
    while (*q == '\t' || *q == ' ')
        q--;
    *(q+1) = 0;
    strcpy(src, p);
    p = src;
    return p;
}

/* src: option name value OR config name value
 * name: a pointer to the name of src
 * value: a pointer to the value of src
 */

void parse_config_option(char *src, char **name, char **value)
{
    char *q;
    q = src;
    while (!isspace(*q))
    q++;
    while (isspace(*q))
        q++;
    *name = q;

    while (!isspace(*q))
         q++;
    if (*q == '\n')
    {
         *q = 0, *value = NULL;
         return;
    }
    *q = 0;
    q++;
    while (isspace(*q))
    {
         if (*q == '\n')
         {
              *value = NULL;
              return;
         }
         q++;
    }
    while (*q == '\'' || *q == '\"')
         q++;
    *value = q;
    while (!isspace(*q) && *q != '\'' && *q != '\"' && *q != '\n')
         q++;
    *q = 0;
}

