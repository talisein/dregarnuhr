#pragma once

#include <optional>
#include <string_view>
#include "outcome/result.hpp"

enum class volume
{
    P1V1,
    P1V2,
    P1V3,
    P2V1,
    P2V2,
    P2V3,
    P2V4,
    P3V1,
    P3V2,
    P3V3,
    P3V4,
    P3V5,
    P4V1,
    P4V2,
    P4V3,
    P4V4,
    P4V5,
    P4V6,
    P4V7,
    P4V8,
    FB1,
    FB2,
    RA1
};

OUTCOME_V2_NAMESPACE::result<volume> identify_volume(std::string_view uid);
std::string_view to_string_view(volume vol);

struct volume_definition
{
    volume vol;
    std::string_view id;
    std::string_view href;
    std::string_view mediatype;
    std::optional<std::string_view> toc_label;
    bool in_spine;
};