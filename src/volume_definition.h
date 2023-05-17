#pragma once

#include <string_view>
#include <optional>

#include "volumes.h"
#include "chapter_type.h"

struct volume_definition
{
    volume vol;
    std::string_view href;
    std::string_view mediatype;
    std::optional<std::string_view> toc_label;
    bool in_spine;
    chapter_type type;

    void print_id(std::ostream &os) const {
        auto pos = href.find_last_of('/');
        if (chapter_uniqueness::MULTIPLE == get_uniqueness(type)) {
            os << vol << '-';
        }
        std::string_view id;
        if (pos == std::string_view::npos) {
            id = href;
        } else {
            id = href.substr(pos+1);
        }

        if (chapter_type::MAP_EHRENFEST_CITY == type) {
            id = "city_map";
        } else if (chapter_type::MAP_EHRENFEST_DUCHY == type) {
            id = "duchy_map";
        } else if (chapter_type::MAP_YURGENSCHMIDT == type) {
            id = "country_map";
        } else if (chapter_type::TABLE_YURGENSCHMIDT_DUCHIES == type) {
            id = "duchies_table";
        }

        os << id;
    }

    std::string get_id() const {
        std::stringstream ss;
        print_id(ss);
        return ss.str();
    }

    constexpr bool is_bodymatter() const noexcept {
        return ::is_bodymatter(type);
    }

    constexpr chapter_type get_chapter_type() const noexcept { return type; };

    constexpr bool operator==(const volume_definition& other) const noexcept
    {
        return (type == other.get_chapter_type()) || (is_bodymatter() && other.is_bodymatter());
    }

    constexpr std::strong_ordering operator<=>(const volume_definition& other) const noexcept
    {
        if (is_bodymatter() && other.is_bodymatter())
            return std::strong_ordering::equivalent;

        return type <=> other.get_chapter_type();
    }

    template<typename StringHref, typename StringMedia, typename OptionalStringLabel>
    constexpr volume_definition(volume vol, StringHref&& href, StringMedia&& mediatype, OptionalStringLabel&& toc_label, bool in_spine)
        : vol(vol),
          href(std::forward<StringHref>(href)),
          mediatype(std::forward<StringMedia>(mediatype)),
          toc_label(std::forward<OptionalStringLabel>(toc_label)),
          in_spine(in_spine),
          type(evaluate_chapter_type(this->vol, this->mediatype, this->href, this->toc_label))
    { }
};



template<std::ranges::input_range R>
struct definition_view_t : public std::ranges::view_interface<definition_view_t<R>>
{
    constexpr definition_view_t(R&& _defs, omnibus o) : defs(std::forward<R>(_defs)), name(o) {};
    constexpr definition_view_t(R&& _defs, volume v) : defs(std::forward<R>(_defs)), name(v) {};

    R defs;
    std::variant<omnibus, volume> name;

    constexpr auto begin() const { return std::ranges::cbegin(defs); }
    constexpr auto end() const { return std::ranges::cend(defs); }
};

using definition_span_view_t = definition_view_t<std::span<const volume_definition>>;

std::ostream& operator<<(std::ostream& os, const volume_definition& v);
