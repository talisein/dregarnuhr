#include <system_error>
#include <string_view>
#include <map>
#include <cassert>
#include "outcome/result.hpp"
#include "volumes.h"
#include "log.h"

namespace {
    using namespace std::string_view_literals;
    const std::map<std::string_view, volume> volume_map {
        {"9781718344631"sv, volume::FB1},
        {"9781718344648"sv, volume::FB2},
        {"9781718344655"sv, volume::FB3},
        {"9781718346734"sv, volume::RA1},
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
        {"9781718338005"sv, volume::MP1V1},
        {"9781718338029"sv, volume::MP1V2},
        {"9781718338043"sv, volume::MP1V3},
        {"9781718338067"sv, volume::MP1V4},
        {"9781718338081"sv, volume::MP1V5},
        {"9781718338104"sv, volume::MP1V6},
        {"9781718338128"sv, volume::MP1V7},
        {"9781718338142"sv, volume::MP2V1},
        {"9781718338166"sv, volume::MP2V2},
        {"9781718338180"sv, volume::MP2V3},
        {"9781718338203"sv, volume::MP2V4},
        {"9781718338227"sv, volume::MP2V5},
        {"9781718338241"sv, volume::MP2V6},
        {"96908895-C96B-4376-BBF8-6A25E96F72F3"sv, volume::SSBDOVA1},
        {"22BC23BD-BEB1-4F8F-9104-825FA616AD4C"sv, volume::SSJBUNKO1},
        {"DEB588FB-32EF-4512-8ADF-D8A5CB49D9CD"sv, volume::SSTEASET},
        {"5F99914F-5229-482E-A63D-D38002C442FD"sv, volume::SSWN1},
        {"94A7C755-6459-4B4A-A48C-9AEE66C3E58A"sv, volume::SSWN2},
        {"614AD11F-85BA-499A-9A35-74C4DBCCA288"sv, volume::SSDRAMACD2},
        {"E490F42A-CF20-476A-8327-59E034AE8928"sv, volume::SSDRAMACD3},
        {"EADEFA73-BCAD-4166-8745-0137BEAC38EC"sv, volume::SSDRAMACD4},
        {"7D41D170-FCB5-480A-82AD-D4F921AB5B61"sv, volume::SSTOBBONUS1},
        {"CE71D400-5EF9-4B8E-80C1-CED953C285C6"sv, volume::SSUP1},
    };

    const std::map<std::string_view, volume> slug_map {
        {"ascendance-of-a-bookworm-royal-academy-stories-first-year"sv, volume::RA1},
        {"ascendance-of-a-bookworm-fanbook-1"sv, volume::FB1},
        {"ascendance-of-a-bookworm-fanbook-2"sv, volume::FB2},
        {"ascendance-of-a-bookworm-fanbook-3"sv, volume::FB3},
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
    };

}

std::string_view to_string_view(volume vol)
{
    switch (vol) {
        case volume::P1V1: return "P1V1"sv;
        case volume::P1V2: return "P1V2"sv;
        case volume::P1V3: return "P1V3"sv;
        case volume::P2V1: return "P2V1"sv;
        case volume::P2V2: return "P2V2"sv;
        case volume::P2V3: return "P2V3"sv;
        case volume::P2V4: return "P2V4"sv;
        case volume::P3V1: return "P3V1"sv;
        case volume::P3V2: return "P3V2"sv;
        case volume::P3V3: return "P3V3"sv;
        case volume::P3V4: return "P3V4"sv;
        case volume::P3V5: return "P3V5"sv;
        case volume::P4V1: return "P4V1"sv;
        case volume::P4V2: return "P4V2"sv;
        case volume::P4V3: return "P4V3"sv;
        case volume::P4V4: return "P4V4"sv;
        case volume::P4V5: return "P4V5"sv;
        case volume::P4V6: return "P4V6"sv;
        case volume::P4V7: return "P4V7"sv;
        case volume::P4V8: return "P4V8"sv;
        case volume::P4V9: return "P4V9"sv;
        case volume::P5V1: return "P5V1"sv;
        case volume::P5V2: return "P5V2"sv;
        case volume::FB1:  return "FB1"sv;
        case volume::FB2:  return "FB2"sv;
        case volume::FB3:  return "FB3"sv;
        case volume::RA1:  return "RA1"sv;
        case volume::MP1V1:  return "M1V1"sv;
        case volume::MP1V2:  return "M1V2"sv;
        case volume::MP1V3:  return "M1V3"sv;
        case volume::MP1V4:  return "M1V4"sv;
        case volume::MP1V5:  return "M1V5"sv;
        case volume::MP1V6:  return "M1V6"sv;
        case volume::MP1V7:  return "M1V7"sv;
        case volume::MP2V1:  return "M2V1"sv;
        case volume::MP2V2:  return "M2V2"sv;
        case volume::MP2V3:  return "M2V3"sv;
        case volume::MP2V4:  return "M2V4"sv;
        case volume::MP2V5:  return "M2V5"sv;
        case volume::MP2V6:  return "M2V6"sv;
        case volume::SSBDOVA1: return "SSBDOVA1"sv;
        case volume::SSJBUNKO1: return "SSJBUNKO1"sv;
        case volume::SSTEASET: return "SSTEASET"sv;
        case volume::SSWN1: return "SSWN1"sv;
        case volume::SSWN2: return "SSWN2"sv;
        case volume::SSDRAMACD2: return "SSDRAMACD2"sv;
        case volume::SSDRAMACD3: return "SSDRAMACD3"sv;
        case volume::SSDRAMACD4: return "SSDRAMACD4"sv;
        case volume::SSTOBBONUS1: return "SSTOBBONUS1"sv;
        case volume::SSUP1: return "SSUP1"sv;
    }
    assert(false);
    return "UNKNOWN"sv;
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

OUTCOME_V2_NAMESPACE::result<volume>
identify_volume(std::string_view uid)
{
    auto iter = volume_map.find(uid);
    if (iter == volume_map.end()) {
        return std::errc::invalid_argument;
    }
    return iter->second;
}

std::ostream& operator<<(std::ostream& os, volume v)
{
    os << to_string_view(v);
    return os;
}

std::ostream& operator<<(std::ostream& os, const volume_definition& v)
{
    os << "{ volume::" << v.vol << ", " << std::quoted(v.href)
       << ", " << std::quoted(v.mediatype) << ", ";
    if (v.toc_label)
        os << std::quoted(v.toc_label.value());
    else
        os << "std::nullopt";
    os << ", " << std::boolalpha << v.in_spine << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::variant<omnibus, volume>& v)
{
    std::visit([&os](auto&& arg) { os << to_string_view(arg); }, v);
    return os;
}

std::ostream& operator<<(std::ostream& os, omnibus v)
{
    os << to_string_view(v);
    return os;
}

volume
get_volume_from_slug(std::string_view sv)
{
    auto it = slug_map.find(sv);
    if (it != slug_map.end()) {
        return it->second;
    }
    log_error("Unknown volume for slug ", sv);
    assert(false);
    return volume::P1V1;
}
