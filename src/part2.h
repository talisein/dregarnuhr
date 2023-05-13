#pragma once

#include "part2_defs.h"

namespace part_2
{
    inline auto get_vol_2_v() { return std::views::single(part_2_defs::vol_2_def); }
    inline auto get_vol_3_v() { return std::views::single(part_2_defs::vol_3_def); }
    inline auto get_vol_4_v() { return std::views::single(part_2_defs::vol_4_def); }
    inline auto get_part_2_v() { return part_2_defs::part_2_def; }
}
