#pragma once

#ifndef __SPECSTRINGS_STRICT_LEVEL
#define __SPECSTRINGS_STRICT_LEVEL 0
#endif

#include <easylogging++.h>

#define FOXY_TRACE LOG(TRACE)
#define FOXY_DEBUG LOG(DEBUG)
#define FOXY_INFO  LOG(INFO)
#define FOXY_WARN  LOG(WARNING)
#define FOXY_ERROR LOG(ERROR)
#define FOXY_FATAL LOG(FATAL)