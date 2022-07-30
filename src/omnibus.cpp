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
    const auto _omnibus_list { std::to_array({
                part_1::get_part_1(),
                part_2::get_part_2(),
                part_3::get_part_3(),
                part_4::get_part_4()
            })
    };

    const std::ranges::join_view _omnibus_view { _omnibus_list };

    const std::vector<volume_definition> _omnibus = utils::make_omnibus_def(_omnibus_view,
                                                                            part_1::get_reservation() +
                                                                            part_2::get_reservation() +
                                                                            part_3::get_reservation() +
                                                                            part_4::get_reservation());

}


std::span<const volume_definition>
get_omnibus_definition()
{
    return std::span<const volume_definition>(_omnibus);
}
