#pragma once

#include <span>
#include "volumes.h"

namespace part_5
{
    std::span<const volume_definition> get_vol_1();

    std::span<const volume_definition> get_part_5();
    size_t get_reservation();
}
