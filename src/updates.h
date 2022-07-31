#pragma once

#include <map>
#include <string>

#include "volumes.h"
#include "config.h"

#if HAVE_CHRONO
#include <chrono>
using std::map<volume, std::chrono::sys_seconds> = update_map_t;
#else
#include "date/date.h"
using std::map<volume, date::sys_seconds> = update_map_t;
#endif

update_map_t get_updated();
