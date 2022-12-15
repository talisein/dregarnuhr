#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>

#include "part2.h"
#include "utils.h"

namespace
{
    using namespace std::string_view_literals;

    constexpr auto _part_2_xxx = std::views::transform(part_2_defs::part_2_def.defs, &definition_span_view_t::defs);

    consteval auto _get_part2_view()
    {
        constexpr auto l = std::views::join(_part_2_xxx);
        constexpr auto sz = std::ranges::distance(l);
        std::vector<volume_definition> v(std::ranges::begin(l), std::ranges::end(l));
        return utils::vec_to_arr<sz>(v);
    }

    constexpr std::array _part_2_view = _get_part2_view();

    const std::vector<volume_definition> _part_2 = utils::make_omnibus_def(_part_2_view);
} // namespace

namespace part_2
{
    using namespace std::string_view_literals;

    definition_span_view_t
    get_part_2()
    {
        return {std::span<const volume_definition>(_part_2), omnibus::PART2};
    }

    size_t
    get_reservation()
    {
        return _part_2.size();
    }
}
