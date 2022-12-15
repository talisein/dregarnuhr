#pragma once

#include <span>
#include "volumes.h"
#include "part5_defs.h"

namespace part_5
{
    inline auto get_vol_1() { return part_5_defs::vol_1_def; }

    definition_span_view_t get_part_5();
}
