#pragma once

#include <array>
#include <ranges>
#include <charconv>
#include <numeric>
#include <concepts>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <string_view>
#include <set>
#include "libxml++/ustring.h"
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

    template<std::ranges::sized_range T>
    [[nodiscard]] std::vector<volume_definition>
    make_omnibus_def(T&& view)
    {
        std::vector<volume_definition> res;
        res.reserve(std::ranges::size(view));
        std::ranges::remove_copy_if(view, std::back_inserter(res), utils::filter_chapter_stylesheet{});
        std::ranges::stable_sort(res);
        std::ranges::for_each(res, utils::foreach_label{});
        return res;
    }

    template<typename T>
    constexpr std::size_t
    _count_sizes(const T& t)
    {
        using Type = std::remove_cvref_t<T>;
        if constexpr(std::is_same_v<Type, char>) {
            return 1ULL;
        } else if constexpr(std::is_integral_v<Type>) {
            return std::max<size_t>(std::numeric_limits<Type>::digits10 + 1ULL + static_cast<size_t>(std::is_signed<Type>::value), 10ULL);
        } else {
            return std::size(t);
        }
    }

    template<typename T, typename... Stringlikes>
    constexpr std::size_t
    _count_sizes(const T& t, const Stringlikes&... args)
    {
        using Type = std::remove_cvref_t<T>;
        if constexpr(std::is_same_v<Type, char>) {
            return 1ULL + _count_sizes(args...);
        } else if constexpr(std::is_integral_v<Type>) {
            return std::max<size_t>(std::numeric_limits<Type>::digits10 + 1ULL + static_cast<size_t>(std::is_signed<Type>::value), 10ULL) + _count_sizes(args...);
        } else {
            return std::size(t) + _count_sizes(args...);
        }
    }

    template<typename String, typename T>
    constexpr String&
    _strconcat_append(String& str, T&& t)
    {
        using Type = std::remove_cvref_t<T>;
        if constexpr(std::is_same_v<Type, char>) {
            str.push_back(std::forward<T>(t));
        } else if constexpr(std::is_integral_v<Type>) {
            std::array<char, std::numeric_limits<Type>::digits10 + 1ULL + static_cast<size_t>(std::is_signed<Type>::value)> buf;
            auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), std::forward<T>(t));
            if (ec != std::errc()) {
                throw std::system_error(std::make_error_code(ec));
            }
            str.append(buf.data(), ptr);
        } else {
            str.append(std::forward<T>(t));
        }
        return str;
    }

    template<typename String, typename T, typename... Stringlikes>
    constexpr String&
    _strconcat_append(String& str, T&& t, Stringlikes&&... args)
    {
        using Type = std::remove_cvref_t<T>;
        if constexpr(std::is_same_v<Type, char>) {
            str.push_back(std::forward<T>(t));
        } else if constexpr(std::is_integral_v<Type>) {
            std::array<char, std::numeric_limits<Type>::digits10 + 1ULL + static_cast<size_t>(std::is_signed<Type>::value)> buf;
            auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), std::forward<T>(t));
            if (ec != std::errc()) {
                throw std::system_error(std::make_error_code(ec));
            }

            str.append(buf.data(), ptr);
        } else {
            str.append(std::forward<T>(t));
        }
        _strconcat_append(str, std::forward<Stringlikes>(args)...);
        return str;
    }

    template<typename String = std::string, typename... Stringlikes>
    [[nodiscard]] String
    strcat(Stringlikes&&... args)
    {
        String res;
        res.reserve(_count_sizes(args...));
        _strconcat_append(res, std::forward<Stringlikes>(args)...);
        return res;
    }

    template<typename... Stringlikes>
    [[nodiscard]] xmlpp::ustring
    xstrcat(Stringlikes&&... args)
    {
        return strcat<xmlpp::ustring>(std::forward<Stringlikes>(args)...);
    }

    template<std::integral To, std::integral From>
    [[nodiscard]] auto safe_int_cast(From from) -> std::remove_cvref_t<To> {
        using namespace std::string_view_literals;
        auto res = static_cast<std::remove_cvref_t<To>>(from);
        if (std::cmp_not_equal(res, from)) [[unlikely]] {
            std::string error_message = utils::strcat("Trying to cast from ", from, " to a type that would become ", res);
            if (std::cmp_greater(from, res)) {
                throw std::overflow_error(error_message);
            } else {
                throw std::range_error(error_message);
            }
        }
        return res;
    }

}
