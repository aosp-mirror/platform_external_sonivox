#pragma once

#define ALOGE(...)
#define ALOGV(...)

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

#include <sys/types.h>

#ifndef off64_t
typedef off_t off64_t;
#endif
