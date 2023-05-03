#pragma once

#include <array>
#include <ranges>
#include <charconv>
#include <numeric>
#include <functional>
#include <concepts>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <string_view>
#include <set>
#include "libxml++/libxml++.h"
#include "libxml++/ustring.h"
#include "volumes.h"
#include "log.h"

namespace utils
{
    struct filter_unique_chapters
    {
        bool operator()(const volume_definition& def)
        {
            const auto chapter_type = def.get_chapter_type();
            if (get_uniqueness(chapter_type) == chapter_uniqueness::SINGLE) {
                if (chap_set.contains(chapter_type)) {
                    return false;
                } else {
                    chap_set.insert(chapter_type);
                    return true;
                }
            }
            return true;
        }

    private:
        std::set<chapter_type> chap_set;
    };

    struct filter_stylesheets
    {
        bool operator()(const volume_definition& def)
        {
            const auto chapter_type = def.get_chapter_type();
            if (chapter_type == STYLESHEET) {
                if (style_set.contains(def.vol)) {
                    return false;
                } else {
                    style_set.insert(def.vol);
                    return true;
                }
            }
            return true;
        }

    private:
        std::set<volume> style_set;
    };

    struct deduplicate_label
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



    template <size_t N, typename T, size_t... I>
    consteval auto
    vec_to_arr_impl(const T& v, std::index_sequence<I...>) -> std::array<typename T::value_type, N>
    {
        return { {v[I]...} };
    }

    template<size_t N, typename T>
    consteval auto
    vec_to_arr(const T& v) -> std::array<typename T::value_type, N>
    {
        return vec_to_arr_impl<N>(v, std::make_index_sequence<N>{});
    }

    template<std::ranges::input_range T>
    [[nodiscard]] std::vector<volume_definition>
    make_omnibus_def(T&& view)
    {
        std::vector<volume_definition> res;
        std::vector<volume_definition> workbook;
        filter_unique_chapters unique_filter; /* Stateful filters shouldn't be in std::views::filter */
        filter_stylesheets stylesheet_filter; /* Stateful filters shouldn't be in std::views::filter */
        deduplicate_label label_deduplicator;
        res.reserve(std::ranges::distance(view));

        /* Filter stylesheet */
        for (const auto &def : view) {
            if (stylesheet_filter(def))
                res.push_back(def);
        }

        /* Apply the unique_filter in reverse order, store to temp vector. */
        /* Note: views::reverse | views::filter | views::reverse doesn't work. */
        workbook.reserve(res.size());
        for (const auto& def : res | std::views::reverse) {
            if (unique_filter(def))
                workbook.push_back(def);
        }

        /* Now reverse the temp vector into our result vector. This dance is so
         * our stable sort gives the right result. */
        res.clear();
        std::ranges::copy(workbook | std::views::reverse, std::back_inserter(res));
        std::ranges::stable_sort(res, std::ranges::less());

        /* TODO23: In the future this could be a std::view::adjacent<2> */
        std::ranges::for_each(res, std::ref(label_deduplicator));

        return res;
    }

    template<typename T>
    constexpr std::size_t
    _count_sizes(const T& t)
    {
        using Type = std::remove_cvref_t<T>;
        if constexpr(std::is_same_v<Type, char>) {
            return 1ULL;
        } else if constexpr(std::is_same_v<Type, std::optional<std::string>> || std::is_same_v<Type, std::optional<std::string_view>>) {
            if (t.has_value())
                return std::size(*t);
            else
                return 0;
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
        } else if constexpr(std::is_same_v<Type, std::optional<std::string>> || std::is_same_v<Type, std::optional<std::string_view>>) {
            if (t.has_value())
                return std::size(*t) + _count_sizes(args...);
            else
                return _count_sizes(args...);
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
        } else if constexpr(std::is_same_v<Type, std::optional<std::string>> || std::is_same_v<Type, std::optional<std::string_view>>) {
            if (t.has_value())
                str.append(*t);
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
        } else if constexpr(std::is_same_v<Type, std::optional<std::string>> || std::is_same_v<Type, std::optional<std::string_view>>) {
            if (t.has_value())
                str.append(*t);
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

    template<typename Node>
    constexpr auto as_element(Node node) -> std::conditional_t<std::is_const_v<std::remove_pointer_t<Node>>, const xmlpp::Element*, xmlpp::Element*>
    {
        if constexpr (std::is_const_v<std::remove_pointer_t<Node>>) {
            return dynamic_cast<const xmlpp::Element*>(node);
        } else {
            return dynamic_cast<xmlpp::Element*>(node);
        }
    }

    inline auto to_element_filter() {
        return std::views::transform([](const xmlpp::Node* node) { return as_element(node); }) |
            std::views::filter([](const xmlpp::Element* element) { return nullptr != element; });
    }

    inline auto import_attr(xmlpp::Node* p, const xmlpp::Element* src) -> xmlpp::Element*
    {
        auto elem = as_element(p->import_node(src, false));
        for (auto&& attr : src->get_attributes()) {
            elem->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
        }
        return elem;
    }

    constexpr auto import_attr_except(xmlpp::Node* p, const xmlpp::Element* src, std::invocable<const xmlpp::Attribute*> auto&& func) -> xmlpp::Element*
    {
        auto elem = as_element(p->import_node(src, false));
        for (auto&& attr : src->get_attributes() | std::views::filter(std::forward<decltype(func)>(func))) {
            elem->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
        }
        return elem;
    }

    constexpr auto import_attr_except(xmlpp::Node* p, const xmlpp::Element* src, std::initializer_list<std::string_view>&& list) -> xmlpp::Element*
    {
        return import_attr_except(p, src,
            [list = std::move(list)](const xmlpp::Attribute* attr) -> bool
            {
                return std::ranges::end(list) == std::ranges::find_if(list, [name = attr->get_name()](auto&& list_elem) { return name == list_elem; });
            });
    }

    constexpr auto import_attr_except(xmlpp::Node* p, const xmlpp::Element* src, std::string_view&& attribute_name) -> xmlpp::Element*
    {
        return import_attr_except(p, src,
                                  [name = std::move(attribute_name)](const xmlpp::Attribute* attr) -> bool
                                  {
                                      return attr->get_name() != name;
                                  });
    }

    auto import_children_except(xmlpp::Node* p, const xmlpp::Node* src, std::invocable<const xmlpp::ustring&> auto&& func) -> void
    {
        for (const auto &child : src->get_children()) {
            if (std::invoke(func, child->get_name())) {
                p->import_node(child, true);
            }
        }
    }

    inline auto import_children_except(xmlpp::Node* p, const xmlpp::Node* src, std::initializer_list<std::string_view> list) -> void
    {
        import_children_except(p, src, [list = std::move(list)](const xmlpp::ustring &name) {
            return std::ranges::end(list) == std::ranges::find_if(list, [name](auto&& sv){ return name == sv; });
        });
    }

    inline auto import_children_except(xmlpp::Node* p, const xmlpp::Node* src, std::string_view&& sv) -> void
    {
        import_children_except(p, src, [sv = std::move(sv)](const xmlpp::ustring &name) {
            return name != sv;
        });
    }
}
