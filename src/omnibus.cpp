#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>
#include "omnibus.h"
#include "utils.h"

namespace
{
    consteval auto _get_omnibus_view()
    {
        constexpr auto sz = std::ranges::distance(omnibus_defs::omnibus_flat_view);
        std::vector<volume_definition> v(std::ranges::begin(omnibus_defs::omnibus_flat_view), std::ranges::end(omnibus_defs::omnibus_flat_view));
        return utils::vec_to_arr<sz>(v);
    }

    constexpr std::array _omnibus_view = _get_omnibus_view();

    const std::vector<volume_definition> _omnibus = utils::make_omnibus_def(_omnibus_view);
}

definition_span_view_t
get_omnibus_definition()
{
    return { std::span<const volume_definition>(_omnibus), omnibus::ALL };
}

std::map<volume_definition, std::pair<omnibus, volume>, volume_map_comparator>
get_volume_map()
{
    std::map<volume_definition, std::pair<omnibus, volume>, volume_map_comparator> m;

    for (const auto& part : omnibus_defs::omnibus_arr) {
        auto o = std::get<omnibus>(part.name);
        for (const auto& vol : part.defs) {
            auto v = std::get<volume>(vol.name);
            auto pair = std::make_pair(o, v);
            for (const auto& def : vol.defs |
                     std::views::filter([](const auto &d) { return d.get_chapter_type() == chapter_type::CHAPTER;})) {
                m.try_emplace(def, pair);
            }
        }
    }

    return m;
}
