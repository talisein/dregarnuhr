#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>

#include "part1.h"
#include "utils.h"

namespace
{
    using namespace std::string_view_literals;

    constexpr auto _part_1_xxx = std::views::transform(part_1_defs::part_1_def.defs, &definition_span_view_t::defs);

    consteval auto _get_part1_view()
    {
        constexpr auto l = std::views::join(_part_1_xxx);
        constexpr auto sz = std::ranges::distance(l);
        std::vector<volume_definition> v(std::ranges::begin(l), std::ranges::end(l));
        return utils::vec_to_arr<sz>(v);
    }

    constexpr std::array _part_1_view = _get_part1_view();

    const std::vector<volume_definition> _part_1 = utils::make_omnibus_def(_part_1_view, utils::calc_reservation(_part_1_xxx));

} // namespace

namespace part_1
{
    using namespace std::string_view_literals;

    definition_span_view_t
    get_part_1()
    {
        return {std::span<const volume_definition>(_part_1), omnibus::PART1};
    }

    size_t
    get_reservation()
    {
        return _part_1.size();
    }

}
