#include <ranges>
#include <algorithm>
#include <map>

#include "omnibus.h"
#include "omnibus_defs.h"
#include "utils.h"

std::map<volume_definition, std::pair<omnibus, volume>, volume_map_comparator>
get_volume_map()
{
    std::map<volume_definition, std::pair<omnibus, volume>, volume_map_comparator> m;

    for (const auto& part : omnibus_defs::omnibus_arr) {
        auto o = std::get<omnibus>(part.name);
        for (const auto& vol : part.defs) {
            auto v = std::get<volume>(vol.name);
            auto pair = std::make_pair(o, v);
            for (const auto& def : vol.defs | std::views::filter(&volume_definition::is_bodymatter))
            {
                m.try_emplace(def, pair);
            }
        }
    }

    return m;
}
