#pragma once

#include <span>
#include "volumes.h"
#include "part2_defs.h"

namespace part_2
{
    inline auto get_vol_1() { return part_2_defs::vol_1_def; }
    inline auto get_vol_2() { return part_2_defs::vol_2_def; }
    inline auto get_vol_3() { return part_2_defs::vol_3_def; }
    inline auto get_vol_4() { return part_2_defs::vol_4_def; }
    definition_span_view_t get_part_2();
    size_t get_reservation();
}
