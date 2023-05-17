#include <optional>
#include "volumes.h"
#include "omnibus.h"
#include <boost/ut.hpp>
#include "magic_enum.hpp"

template<> auto boost::ut::cfg<boost::ut::override> = boost::ut::runner<boost::ut::reporter<boost::ut::printer>>{};

int main() {
    using namespace boost::ut;
    using namespace std::string_literals;

    "slug volume map"_test = [] {
        static constexpr auto defined_volumes = std::views::transform(get_omnibus_definition_r(),
                                                               &std::ranges::range_value_t<decltype(get_omnibus_definition_r())>::name) | std::views::transform([](const auto& v) -> volume { return std::get<volume>(v); });

        for (auto v : defined_volumes) {
            expect(nothrow([&](){get_slug_from_volume(v);}));
            expect(nothrow([&](){get_volume_from_slug(get_slug_from_volume(v));}));
            expect(that % v == get_volume_from_slug(get_slug_from_volume(v)));
        }
    };

    "uid volume map"_test = [] {
        magic_enum::enum_for_each<volume>([] (auto val) {
            constexpr volume v = decltype(val)::value;
            if (v == volume::UFTSS1) {
                auto res = get_uid_from_volume(v);
                expect(that % false == res.has_value());
            } else {
                auto uid_res = get_uid_from_volume(v);
                expect(uid_res.has_value());
                auto vol_res = get_volume_from_uid(uid_res.value());
                expect(vol_res.has_value());
                expect(that % v == vol_res.value());
            }
        });
    };
}
