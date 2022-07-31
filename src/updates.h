#pragma once

#include <map>
#include <chrono>
#include "volumes.h"

using update_map_t = std::map<volume, std::chrono::sys_seconds>;

update_map_t get_updated();
