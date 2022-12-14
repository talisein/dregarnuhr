#pragma once

#include <span>
#include "volumes.h"
#include "part1_defs.h"

namespace part_1
{
    inline auto get_vol_1() { return part_1_defs::vol_1_def; }
    inline auto get_vol_2() { return part_1_defs::vol_2_def; }
    inline auto get_vol_3() { return part_1_defs::vol_3_def; }
    definition_span_view_t get_part_1();
    size_t get_reservation();
}
