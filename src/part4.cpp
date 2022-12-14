#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>
#include "part4.h"
#include "utils.h"

namespace
{
    constexpr auto _part_4_xxx = std::views::transform(part_4_defs::part_4_def.defs, &definition_span_view_t::defs);
    consteval auto _get_part4_view()
    {
        constexpr auto l = std::views::join(_part_4_xxx);
        constexpr auto sz = std::ranges::distance(l);
        std::vector<volume_definition> v(std::ranges::begin(l), std::ranges::end(l));
        return utils::vec_to_arr<sz>(v);
    }

    constexpr std::array _part_4_view = _get_part4_view();

    const std::vector<volume_definition> _part_4 = utils::make_omnibus_def(_part_4_view, utils::calc_reservation(_part_4_xxx));

} // namespace


namespace part_4
{
    using namespace std::string_view_literals;

    definition_span_view_t
    get_part_4()
    {
        return { std::span<const volume_definition>(_part_4), omnibus::PART4 };
    }

    size_t
    get_reservation()
    {
        return _part_4.size();
    }
}
