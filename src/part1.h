#pragma once

#include <span>
#include "volumes.h"
#include "part1_defs.h"

namespace part_1
{

    inline auto get_vol_1_v() { return std::views::single(part_1_defs::vol_1_def); }
    inline auto get_vol_2_v() { return std::views::single(part_1_defs::vol_2_def); }
    inline auto get_vol_3_v() { return std::views::single(part_1_defs::vol_3_def); }
    inline auto get_part_1_v() { return part_1_defs::part_1_def; }
}
