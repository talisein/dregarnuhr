#pragma once

#include <span>
#include "volumes.h"

namespace part_1
{
    std::span<const volume_definition> get_vol_1();
    std::span<const volume_definition> get_vol_2();
    std::span<const volume_definition> get_vol_3();
    std::span<const volume_definition> get_part_1();
}
