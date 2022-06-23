#pragma once

#include <sstream>
#include <string_view>
#include <set>
#include "volumes.h"

namespace utils
{
    struct filter_chapter_stylesheet
    {
        bool operator()(const volume_definition& def);
    private:
        std::set<chapter_type> chap_set;
        std::set<volume> style_set;
    };

    struct transform_unique_ids
    {
        transform_unique_ids(std::set<std::string>& set, std::string_view prefix) : m_set(set), m_prefix(prefix) {}
        transform_unique_ids(const transform_unique_ids& t) : m_set(t.m_set), m_prefix(t.m_prefix), unique(t.unique), ss() {};
        volume_definition operator()(volume_definition def);

    private:
        std::set<std::string>& m_set;
        std::string_view m_prefix;
        int unique;
        std::stringstream ss;
    };

    struct foreach_label
    {
        void operator()(volume_definition &def);
    private:
        std::optional<std::string_view> prev_label;
    };
}
