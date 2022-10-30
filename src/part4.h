#pragma once

#include <span>
#include "volumes.h"

namespace part_4
{
    std::span<const volume_definition> get_vol_1();
    std::span<const volume_definition> get_vol_2();
    std::span<const volume_definition> get_vol_3();
    std::span<const volume_definition> get_vol_4();
    std::span<const volume_definition> get_vol_5();
    std::span<const volume_definition> get_vol_6();
    std::span<const volume_definition> get_vol_7();
    std::span<const volume_definition> get_vol_8();
    std::span<const volume_definition> get_vol_9();
    std::span<const volume_definition> get_part_4();
    size_t get_reservation();
}
