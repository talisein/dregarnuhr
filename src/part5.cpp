#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>
#include "part5.h"
#include "utils.h"

namespace {

    constexpr auto _part_5_xxx = std::views::transform(part_5_defs::part_5_def.defs, &definition_span_view_t::defs);

    consteval auto _get_part5_view()
    {
        constexpr auto l = std::views::join(_part_5_xxx);
        constexpr auto sz = std::ranges::distance(l);
        std::vector<volume_definition> v(std::ranges::begin(l), std::ranges::end(l));
        return utils::vec_to_arr<sz>(v);
    }

    constexpr std::array _part_5_view = _get_part5_view();

    const std::vector<volume_definition> _part_5 = utils::make_omnibus_def(_part_5_view);
}

namespace part_5
{
    using namespace std::string_view_literals;

    definition_span_view_t
    get_part_5()
    {
        return { std::span<const volume_definition>(_part_5), omnibus::PART5 };
    }

} // namespace part_5
