#pragma once

#include <span>
#include <ranges>
#include "volumes.h"
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "part5.h"
#include "utils.h"

struct volume_map_comparator
{
    constexpr bool operator()(const volume_definition& left, const volume_definition& right) const {
        if (left.vol == right.vol) {
            return std::less<std::string_view>{}(left.href, right.href);
        }
        else return std::less<volume>{}(left.vol, right.vol);
    }
};
std::map<volume_definition, std::pair<omnibus, volume>, volume_map_comparator> get_volume_map();

namespace omnibus_defs
{
    constexpr std::array<definition_view_t<std::span<const definition_span_view_t>>, 5> omnibus_arr {
        part_1_defs::part_1_def,
        part_2_defs::part_2_def,
        part_3_defs::part_3_def,
        part_4_defs::part_4_def,
        part_5_defs::part_5_def
    };

    consteval auto view_to_arr()
    {
        constexpr auto v = std::views::join(omnibus_arr);
        constexpr auto sz = std::ranges::distance(v);
        std::vector<std::ranges::range_value_t<decltype(v)>> vec(std::ranges::begin(v),
                                                                 std::ranges::end(v));
        return utils::vec_to_arr<sz>(vec);
    }


    constexpr auto omnibus_flat_arr = view_to_arr();

    constexpr definition_view_t omnibus_def { std::span<const definition_span_view_t, std::dynamic_extent>(omnibus_flat_arr), omnibus::ALL };

}

constexpr inline auto get_omnibus_definition_r()
{
    return omnibus_defs::omnibus_def;
}
