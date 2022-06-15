#include <map>
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

OUTCOME_V2_NAMESPACE::result<volume>
identify_volume(std::string_view uid)
{
    auto iter = volume_map.find(uid);
    if (iter == volume_map.end()) {
        return std::errc::invalid_argument;
    }
    return iter->second;
}
