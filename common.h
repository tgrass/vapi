#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
#define BEGIN_EXTERN_C      extern "C" {
#define END_EXTERN_C        }
#else
#define BEGIN_EXTERN_C  
#define END_EXTERN_C    
#endif

#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define MAX_LINE    1024
#define MAX_BUF     4096


#endif
