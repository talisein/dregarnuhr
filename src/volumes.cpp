#include <system_error>
#include <string_view>
#include <map>
#include <cassert>
#include "outcome/result.hpp"
#include "magic_enum.hpp"
#include "volumes.h"
#include "log.h"
#include "utils.h"

namespace {
    using namespace std::string_view_literals;

    const std::map<std::string_view, volume> slug_map {
        {"ascendance-of-a-bookworm-fanbook-1"sv, volume::FB1},
        {"ascendance-of-a-bookworm-fanbook-2"sv, volume::FB2},
        {"ascendance-of-a-bookworm-fanbook-3"sv, volume::FB3},
        {"ascendance-of-a-bookworm-royal-academy-stories-first-year"sv, volume::RA1},
        {"ascendance-of-a-bookworm-short-story-collection-volume-1"sv, volume::SSC1},
        {"ascendance-of-a-bookworm"sv, volume::P1V1},
        {"ascendance-of-a-bookworm-volume-2"sv, volume::P1V2},
        {"ascendance-of-a-bookworm-part-1-volume-3"sv, volume::P1V3},
        {"ascendance-of-a-bookworm-part-2-volume-1"sv, volume::P2V1},
        {"ascendance-of-a-bookworm-part-2-volume-2"sv, volume::P2V2},
        {"ascendance-of-a-bookworm-part-2-volume-3"sv, volume::P2V3},
        {"ascendance-of-a-bookworm-part-2-volume-4"sv, volume::P2V4},
        {"ascendance-of-a-bookworm-part-3-volume-1"sv, volume::P3V1},
        {"ascendance-of-a-bookworm-part-3-volume-2"sv, volume::P3V2},
        {"ascendance-of-a-bookworm-part-3-volume-3"sv, volume::P3V3},
        {"ascendance-of-a-bookworm-part-3-volume-4"sv, volume::P3V4},
        {"ascendance-of-a-bookworm-part-3-volume-5"sv, volume::P3V5},
        {"ascendance-of-a-bookworm-part-4-volume-1"sv, volume::P4V1},
        {"ascendance-of-a-bookworm-part-4-volume-2"sv, volume::P4V2},
        {"ascendance-of-a-bookworm-part-4-volume-3"sv, volume::P4V3},
        {"ascendance-of-a-bookworm-part-4-volume-4"sv, volume::P4V4},
        {"ascendance-of-a-bookworm-part-4-volume-5"sv, volume::P4V5},
        {"ascendance-of-a-bookworm-part-4-volume-6"sv, volume::P4V6},
        {"ascendance-of-a-bookworm-part-4-volume-7"sv, volume::P4V7},
        {"ascendance-of-a-bookworm-part-4-volume-8"sv, volume::P4V8},
        {"ascendance-of-a-bookworm-part-4-volume-9"sv, volume::P4V9},
        {"ascendance-of-a-bookworm-part-5-volume-1"sv, volume::P5V1},
        {"ascendance-of-a-bookworm-part-5-volume-2"sv, volume::P5V2},
        {"ascendance-of-a-bookworm-part-5-volume-3"sv, volume::P5V3},
        {"ascendance-of-a-bookworm-part-5-volume-4"sv, volume::P5V4},
    };

    using namespace std::string_view_literals;
    const std::map<std::string_view, volume> uid_volume_map {
        {"9781718344631"sv, volume::FB1},
        {"9781718344648"sv, volume::FB2},
        {"9781718344655"sv, volume::FB3},
        {"9781718346734"sv, volume::RA1},
        {"9781718346758"sv, volume::SSC1},
        {"9781718346017"sv, volume::P1V1},
        {"9781718346031"sv, volume::P1V2},
        {"9781718346055"sv, volume::P1V3},
        {"9781718346079"sv, volume::P2V1},
        {"9781718346093"sv, volume::P2V2},
        {"9781718346116"sv, volume::P2V3},
        {"9781718346130"sv, volume::P2V4},
        {"9781718346154"sv, volume::P3V1},
        {"9781718346178"sv, volume::P3V2},
        {"9781718346192"sv, volume::P3V3},
        {"9781718346215"sv, volume::P3V4},
        {"9781718346239"sv, volume::P3V5},
        {"9781718346253"sv, volume::P4V1},
        {"9781718346277"sv, volume::P4V2},
        {"9781718346291"sv, volume::P4V3},
        {"9781718346314"sv, volume::P4V4},
        {"9781718346338"sv, volume::P4V5},
        {"9781718346352"sv, volume::P4V6},
        {"9781718346376"sv, volume::P4V7},
        {"9781718346390"sv, volume::P4V8},
        {"9781718346413"sv, volume::P4V9},
        {"9781718346437"sv, volume::P5V1},
        {"9781718346451"sv, volume::P5V2},
        {"9781718346475"sv, volume::P5V3},
        {"9781718346499"sv, volume::P5V4},
        {"9781718338005"sv, volume::M1V1},
        {"9781718338029"sv, volume::M1V2},
        {"9781718338043"sv, volume::M1V3},
        {"9781718338067"sv, volume::M1V4},
        {"9781718338081"sv, volume::M1V5},
        {"9781718338104"sv, volume::M1V6},
        {"9781718338128"sv, volume::M1V7},
        {"9781718338142"sv, volume::M2V1},
        {"9781718338166"sv, volume::M2V2},
        {"9781718338180"sv, volume::M2V3},
        {"9781718338203"sv, volume::M2V4},
        {"9781718338227"sv, volume::M2V5},
        {"9781718338241"sv, volume::M2V6},
        {"9781718338265"sv, volume::M2V7},
        {"96908895-C96B-4376-BBF8-6A25E96F72F3"sv, volume::SSBDOVA1},
        {"22BC23BD-BEB1-4F8F-9104-825FA616AD4C"sv, volume::SSJBUNKO1},
        {"DEB588FB-32EF-4512-8ADF-D8A5CB49D9CD"sv, volume::SSTEASET},
        {"614AD11F-85BA-499A-9A35-74C4DBCCA288"sv, volume::SSDRAMACD2},
        {"E490F42A-CF20-476A-8327-59E034AE8928"sv, volume::SSDRAMACD3},
        {"EADEFA73-BCAD-4166-8745-0137BEAC38EC"sv, volume::SSDRAMACD4},
        {"7D41D170-FCB5-480A-82AD-D4F921AB5B61"sv, volume::SSTOBBONUS1},
        {"CE71D400-5EF9-4B8E-80C1-CED953C285C6"sv, volume::SSUP1},
    };
}

std::string_view to_string_view(volume vol)
{
    return magic_enum::enum_name(vol);
}

std::string_view to_string_view(omnibus v)
{
    switch (v) {
        case omnibus::PART1: return "Part 1"sv;
        case omnibus::PART2: return "Part 2"sv;
        case omnibus::PART3: return "Part 3"sv;
        case omnibus::PART4: return "Part 4"sv;
        case omnibus::PART5: return "Part 5"sv;
        case omnibus::ALL: return "All"sv;
    }
    return "UnknownOmnibus"sv;
}

std::ostream& operator<<(std::ostream& os, volume v)
{
    return magic_enum::ostream_operators::operator<<(os, v);
}

std::ostream& operator<<(std::ostream& os, const std::variant<omnibus, volume>& v)
{
    std::visit([&os](auto&& arg) { os << arg; }, v);
    return os;
}

std::ostream& operator<<(std::ostream& os, omnibus o)
{
    os << to_string_view(o);
    return os;
}

volume
get_volume_from_slug(std::string_view sv)
{
    auto it = slug_map.find(sv);
    if (it != slug_map.end()) {
        return it->second;
    }
    throw std::system_error(std::make_error_code(std::errc::argument_out_of_domain), utils::strcat("Invalid volume slug ", sv));
}

std::string_view
get_slug_from_volume(volume vol)
{
    auto it = std::ranges::find(slug_map, vol, &std::add_const_t<decltype(slug_map)::value_type>::second);
    if (it != std::ranges::end(slug_map)) {
        return it->first;
    }

    throw std::system_error(std::make_error_code(std::errc::argument_out_of_domain), utils::strcat("Invalid volume ", to_string_view(vol)));
}

namespace {
}

outcome::result<volume>
get_volume_from_uid(std::string_view uid) noexcept
{
    auto it = uid_volume_map.find(uid);
    if (it != std::end(uid_volume_map)) {
        return it->second;
    }

    return std::errc::argument_out_of_domain;
}

outcome::result<std::string_view>
get_uid_from_volume(volume vol) noexcept
{
    auto it = std::ranges::find(uid_volume_map, vol, &std::add_const_t<decltype(uid_volume_map)::value_type>::second);
    if (it != std::ranges::end(uid_volume_map)) {
        return it->first;
    }

    return std::errc::argument_out_of_domain;
}
