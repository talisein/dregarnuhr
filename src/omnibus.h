#pragma once

#include <span>
#include "volumes.h"
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "part5.h"


std::span<const volume_definition> get_omnibus_definition();


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

    constexpr auto omnibus_flat_view = std::views::transform(omnibus_arr, &decltype(omnibus_arr)::value_type::defs)
        | std::views::join
        | std::views::transform(&definition_span_view_t::defs)
        | std::views::join;
}
