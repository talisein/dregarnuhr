#include <system_error>
#include <string_view>
#include <map>
#include <cassert>
#include "outcome/result.hpp"
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
        case volume::P5V3: return "P5V3"sv;
        case volume::FB1:  return "FB1"sv;
        case volume::FB2:  return "FB2"sv;
        case volume::FB3:  return "FB3"sv;
        case volume::RA1:  return "RA1"sv;
        case volume::SSC1:  return "SSC1"sv;
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
        case volume::MP2V7:  return "M2V7"sv;
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
        case volume::SSUFTSS1: return "SSUFTSS1"sv;
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

std::ostream& operator<<(std::ostream& os, volume v)
{
    os << to_string_view(v);
    return os;
}

std::ostream& operator<<(std::ostream& os, const volume_definition& v)
{
    os << "{ volume::" << v.vol << ", "
       << "/* " << v.type << " */ "
       << std::quoted(v.href)
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

std::ostream& operator<<(std::ostream& os, chapter_type c)
{
    switch (c) {
        case NCX: os << "NCX"; break;
        case STYLESHEET: os << "STYLESHEET"; break;
        case IMAGE: os << "IMAGE"; break;
        case COVER: os << "COVER"; break;
        case FRONTMATTER: os << "FRONTMATTER"; break;
        case CHARACTERS: os << "CHARACTERS"; break;
        case TOC: os << "TOC"; break;
        case CHAPTER: os << "CHAPTER"; break;
        case MAP_RA_LIBRARY: os << "MAP_RA_LIBRARY"; break;
        case MAP_EHRENFEST_CITY: os << "MAP_EHRENFEST_CITY"; break;
        case MAP_EHRENFEST_DUCHY: os << "MAP_EHRENFEST_DUCHY"; break;
        case MAP_YURGENSCHMIDT: os << "MAP_YURGENSCHMIDT"; break;
        case TABLE_YURGENSCHMIDT_DUCHIES: os << "TABLE_YURGENSCHMIDT_DUCHIES"; break;
        case AURELIA_FAMILY_TREE: os << "AURELIA_FAMILY_TREE"; break;
        case AFTERWORD: os << "AFTERWORD"; break;
        case RECORDING_REPORT: os << "RECORDING_REPORT"; break;
        case MANGA: os << "MANGA"; break;
        case TOBOOKS_MANGA: os << "TOBOOKS_MANGA"; break;
        case POLL: os << "POLL"; break;
        case BONUS: os << "BONUS"; break;
        case SIGNUP: os << "SIGNUP"; break;
        case COPYRIGHT: os << "COPYRIGHT"; break;
    }
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
    for (const auto &p : slug_map) {
        if (p.second == vol)
            return p.first;
    }
    throw std::system_error(std::make_error_code(std::errc::argument_out_of_domain), utils::strcat("Invalid volume ", to_string_view(vol)));
}
