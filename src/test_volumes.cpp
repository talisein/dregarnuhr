#include <optional>
#include "volumes.h"
#include "omnibus.h"
#include <boost/ut.hpp>


int main() {
    using namespace boost::ut;
    using namespace std::string_literals;
    "slug volume map"_test = [] {
        constexpr auto defined_volumes = std::views::transform(get_omnibus_definition_r(),
                                                               &std::ranges::range_value_t<decltype(get_omnibus_definition_r())>::name) | std::views::transform([](const auto& v) -> volume { return std::get<volume>(v); });

        for (auto v : defined_volumes) {
            expect(nothrow([&](){get_slug_from_volume(v);}));
            expect(eq(v, get_volume_from_slug(get_slug_from_volume(v))));
        }
    };
};
