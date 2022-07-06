#pragma once

#include <concepts>
#include <stdexcept>
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
        transform_unique_ids(std::set<std::string>& set, std::string_view prefix) : m_set(set), m_prefix(prefix), unique(0) {}
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


    template<std::integral To, std::integral From>
    [[nodiscard]] auto safe_int_cast(From from) -> std::remove_cvref_t<To> {
        auto res = static_cast<std::remove_cvref_t<To>>(from);
        if (std::cmp_not_equal(res, from)) [[unlikely]] {
            std::stringstream ss;
            ss << "Trying to cast from " << from << " to a type that would become " << res;
            if (std::cmp_greater(from, res)) {
                throw std::overflow_error(ss.str());
            } else {
                throw std::range_error(ss.str());
            }
        }
        return res;
    }
}
