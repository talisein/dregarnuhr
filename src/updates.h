#pragma once

#include <map>
#include <string>

#include "volumes.h"
#include "config.h"

#if HAVE_CHRONO
#include <chrono>
using update_map_t = std::map<volume, std::chrono::sys_seconds>;
#else
#include "date/date.h"
using update_map_t = std::map<volume, date::sys_seconds>;
#endif

update_map_t get_updated();
