#pragma once

#include <map>
#include <string>
#include <chrono>

#include "date/date.h"
#include "volumes.h"
#include "config.h"

#if HAVE_CHRONO
using std::map<volume, std::chrono::sys_seconds> = update_map_t;
#else
using std::map<volume, date::sys_seconds> = update_map_t;
#endif

update_map_t get_updated();
