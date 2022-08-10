#pragma once

#include <numeric>
#include <concepts>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <string_view>
#include <set>
#include "volumes.h"
#include "log.h"

namespace utils
{
    struct filter_chapter_stylesheet
    {
        bool operator()(const volume_definition& def) {
            const auto chapter_type = def.get_chapter_type();
            if (get_uniqueness(chapter_type) == chapter_uniqueness::SINGLE ) {
                if (chap_set.contains(chapter_type)) {
                    return true;
                } else {
                    chap_set.insert(chapter_type);
                    return false;
                }
            } else {
                if (chapter_type == STYLESHEET) {
                    if (style_set.contains(def.vol)) {
                        return true;
                    } else {
                        style_set.insert(def.vol);
                        return false;
                    }
                } else {
                    return false;
                }
            }
        }
    private:
        std::set<chapter_type> chap_set;
        std::set<volume> style_set;
    };

    struct foreach_label
    {
        void operator()(volume_definition &def)
        {
            if (!prev_label) {
                prev_label = def.toc_label;
                return;
            }

            if (def.toc_label) {
                if (prev_label.value() == def.toc_label.value()) {
                    def.toc_label = std::nullopt;
                } else {
                    prev_label = def.toc_label;
                }
            }
        }
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

    template <size_t N, size_t... I>
    consteval std::array<volume_definition, N>
    vec_to_arr_impl(const std::vector<volume_definition>& v, std::index_sequence<I...>)
    {
        return { {v[I]...} };
    }

    template<size_t N>
    consteval auto vec_to_arr(const std::vector<volume_definition>& v)
    {
        return vec_to_arr_impl<N>(v, std::make_index_sequence<N>{});
    }



    template<std::ranges::input_range T>
    [[nodiscard]] std::vector<volume_definition>
    make_omnibus_def(T&& view, size_t reservation = 1000)
    {
        std::vector<volume_definition> res;
        res.reserve(reservation);
        std::ranges::remove_copy_if(view, std::back_inserter(res), utils::filter_chapter_stylesheet{});
        std::ranges::stable_sort(res);
        std::ranges::for_each(res, utils::foreach_label{});
        return res;
    }

    template<std::ranges::range R>
    constexpr size_t calc_reservation(R&& r)
    {
        auto op = [](size_t left, auto right) { return left + right.size(); };
        return std::accumulate(r.begin(), r.end(), 0ULL, op);
    }
}
