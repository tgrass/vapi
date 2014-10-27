#ifndef __UTILS_H
#define __UTILS_H

#include "common.h"

BEGIN_EXTERN_C

int execute(const char *fmt, ...);

static inline int _zstr(const char *s)
{
    return !s || *s == '\0';
}

void debug(const char *fmt, ...);

int strmatch(const char *buf, const char *pattern);

int strnmatch(const char *buf, const char *pattern);

char *strmove(char src[]);

void parse_config_option(char *src, char **name, char **value);

#define ISNULL(p)   (!(p))

#define strlen_zero(x)  _zstr(x)

END_EXTERN_C

#endif
