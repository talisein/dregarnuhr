#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>
#include "omnibus.h"
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "utils.h"


namespace
{

    std::vector<volume_definition> _make_omnibus()
    {
        std::array _omnibus_list = std::to_array<std::span<const volume_definition>>({
                part_1::get_part_1(), part_2::get_part_2(), part_3::get_part_3(), part_4::get_part_4()
            });

        std::vector<volume_definition> x;
        auto view { std::ranges::views::filter(std::ranges::join_view(_omnibus_list), utils::filter_chapter_stylesheet{}) };
        std::ranges::copy(view, std::back_inserter(x));
//        std::ranges::transform(view, std::back_inserter(x), utils::transform_unique_ids{string_set, "part4"});
        std::ranges::stable_sort(x);
        std::ranges::for_each(x, utils::foreach_label{}); // Must be done in order after sort
        return x;
    }

    const std::vector<volume_definition> _omnibus = _make_omnibus();


}


std::span<const volume_definition>
get_omnibus_definition()
{
    return std::span<const volume_definition>(_omnibus);
}
