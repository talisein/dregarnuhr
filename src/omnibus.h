#pragma once

#include <span>
#include <ranges>
#include "volume_definition.h"

struct volume_map_comparator
{
    constexpr bool operator()(const volume_definition& left, const volume_definition& right) const {
        if (left.vol == right.vol) {
            return std::less<std::string_view>{}(left.href, right.href);
        }
        else return std::less<volume>{}(left.vol, right.vol);
    }
};
std::map<volume_definition, std::pair<omnibus, volume>, volume_map_comparator> get_volume_map();
