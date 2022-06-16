#include <string_view>
#include <map>
#include <cassert>
#include "outcome/result.hpp"
#include "volumes.h"

namespace {
    using namespace std::string_view_literals;
    const std::map<std::string_view, volume> volume_map {
        {"9781718344631"sv, volume::FB1},
        {"9781718344648"sv, volume::FB2},
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
        case volume::FB1:  return "FB1"sv;
        case volume::FB2:  return "FB2"sv;
        case volume::RA1:  return "RA1"sv;
    }
    assert(false);
    return "UNKNOWN"sv;
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
