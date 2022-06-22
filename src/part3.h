#pragma once

#include <array>
#include <span>
#include <ranges>
#include <string_view>
#include <optional>
#include "volumes.h"

namespace part_3
{

    std::span<const volume_definition> get_vol_1();
    std::span<const volume_definition> get_vol_2();
    std::span<const volume_definition> get_vol_3();
    std::span<const volume_definition> get_vol_4();
    std::span<const volume_definition> get_vol_5();
    std::span<const volume_definition> get_part_3();
}
