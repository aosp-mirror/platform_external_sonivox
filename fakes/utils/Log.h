#pragma once

#define ALOGE(...)
#define ALOGV(...)

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

#if defined(__APPLE__) && defined (__APPLE_CC__)
#include <sys/types.h>
typedef __int64_t off64_t;
#endif
