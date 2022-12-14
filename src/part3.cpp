#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>

#include "part3.h"
#include "utils.h"

namespace
{
    using namespace std::string_view_literals;

    constexpr auto _part_3_xxx = std::views::transform(part_3_defs::part_3_def.defs, &definition_span_view_t::defs);

    consteval auto _get_part3_view()
    {
        constexpr auto l = std::views::join(_part_3_xxx);
        constexpr auto sz = std::ranges::distance(l);
        std::vector<volume_definition> v(std::ranges::begin(l), std::ranges::end(l));
        return utils::vec_to_arr<sz>(v);
    }

    constexpr std::array _part_3_view = _get_part3_view();

    const std::vector<volume_definition> _part_3 = utils::make_omnibus_def(_part_3_view, utils::calc_reservation(_part_3_xxx));
}


namespace part_3
{
    using namespace std::string_view_literals;

    definition_span_view_t
    get_part_3()
    {
        return { std::span<const volume_definition>(_part_3), omnibus::PART3 };
    }

    size_t
    get_reservation()
    {
        return _part_3.size();
    }
}
