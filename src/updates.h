#pragma once

#include <map>
#include <string>

#include "date/date.h"
#include "volumes.h"

std::map<volume, date::sys_seconds> get_updated();
