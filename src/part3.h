#pragma once

#include "part3_defs.h"

namespace part_3
{
    inline auto get_vol_1_v() { return std::views::single(part_3_defs::vol_1_def); }
    inline auto get_vol_2_v() { return std::views::single(part_3_defs::vol_2_def); }
    inline auto get_vol_3_v() { return std::views::single(part_3_defs::vol_3_def); }
    inline auto get_vol_4_v() { return std::views::single(part_3_defs::vol_4_def); }
    inline auto get_vol_5_v() { return std::views::single(part_3_defs::vol_5_def); }
    inline auto get_part_3_v() { return part_3_defs::part_3_def; }
}
