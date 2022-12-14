#pragma once

#include <span>
#include "volumes.h"
#include "part4_defs.h"

namespace part_4
{
    inline auto get_vol_1() { return part_4_defs::vol_1_def; }
    inline auto get_vol_2() { return part_4_defs::vol_2_def; }
    inline auto get_vol_3() { return part_4_defs::vol_3_def; }
    inline auto get_vol_4() { return part_4_defs::vol_4_def; }
    inline auto get_vol_5() { return part_4_defs::vol_5_def; }
    inline auto get_vol_6() { return part_4_defs::vol_6_def; }
    inline auto get_vol_7() { return part_4_defs::vol_7_def; }
    inline auto get_vol_8() { return part_4_defs::vol_9_def; }
    inline auto get_vol_9() { return part_4_defs::vol_9_def; }
    definition_span_view_t get_part_4();
    size_t get_reservation();
}
