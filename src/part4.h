#pragma once

#include "part4_defs.h"

namespace part_4
{
    inline auto get_vol_1_v() { return std::views::single(part_4_defs::vol_1_def); }
    inline auto get_vol_2_v() { return std::views::single(part_4_defs::vol_2_def); }
    inline auto get_vol_3_v() { return std::views::single(part_4_defs::vol_3_def); }
    inline auto get_vol_4_v() { return std::views::single(part_4_defs::vol_4_def); }
    inline auto get_vol_5_v() { return std::views::single(part_4_defs::vol_5_def); }
    inline auto get_vol_6_v() { return std::views::single(part_4_defs::vol_6_def); }
    inline auto get_vol_7_v() { return std::views::single(part_4_defs::vol_7_def); }
    inline auto get_vol_8_v() { return std::views::single(part_4_defs::vol_8_def); }
    inline auto get_vol_9_v() { return std::views::single(part_4_defs::vol_9_def); }
    inline auto get_part_3_v() { return part_4_defs::part_4_def; }
}
