#pragma once

#include <optional>
#include <string_view>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <span>
#include <map>
#include <memory>
#include <variant>
#include <ranges>

#include "outcome/result.hpp"
#include "ctre.hpp"

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
    P4V9,
    P5V1,
    P5V2,
    P5V3,
    FB1,
    FB2,
    FB3,
    RA1,
    SSC1,
    MP1V1,
    MP1V2,
    MP1V3,
    MP1V4,
    MP1V5,
    MP1V6,
    MP1V7,
    MP2V1,
    MP2V2,
    MP2V3,
    MP2V4,
    MP2V5,
    MP2V6,
    MP2V7,
    SSBDOVA1,    // Otto P1V3
    SSJBUNKO1,   // Benno P1V2
    SSTEASET,    // Ferdinand & Rozemyne P3V1
    SSWN1,       // Hartmut P3V1
    SSWN2,       // Charlotte P4V4
    SSDRAMACD2,  // Tulli P4V3
    SSDRAMACD3,  // Hartmut @ Name Swearing P4V7
    SSDRAMACD4,  // Ferdinand Hair Stick P4V9
    SSTOBBONUS1, // Raimund P4V6
    SSUP1        // Anastasius/Hannelore P5V1
};

// The order here determines the sort order
enum chapter_type {
  NCX = 0,
  STYLESHEET,
  IMAGE,
  FRONTMATTER,
  TOC,
  COVER,               /* Equivalent Comparison From Here */
  CHARACTERS,
  CHAPTER,
  MAP_RA_LIBRARY,
  AURELIA_FAMILY_TREE, /* ------------------------------- */
  MAP_EHRENFEST_CITY,
  MAP_EHRENFEST_DUCHY,
  MAP_YURGENSCHMIDT,
  TABLE_YURGENSCHMIDT_DUCHIES,
  AFTERWORD,
  MANGA,
  POLL,
  BONUS,
  SIGNUP,
  COPYRIGHT,
};

enum class chapter_uniqueness
{
    SINGLE,
    MULTIPLE
};

constexpr chapter_uniqueness
get_uniqueness(chapter_type c) {
    switch (c)
    {
        case TOC:
        case NCX:
        case MAP_EHRENFEST_CITY:
        case MAP_EHRENFEST_DUCHY:
        case MAP_YURGENSCHMIDT:
        case MAP_RA_LIBRARY:
        case TABLE_YURGENSCHMIDT_DUCHIES:
        case SIGNUP:
        case COPYRIGHT: return chapter_uniqueness::SINGLE;
        case COVER:
        case CHARACTERS:
        case CHAPTER:
        case IMAGE:
        case STYLESHEET:
        case MANGA:
        case AFTERWORD:
        case POLL:
        case BONUS:
        case AURELIA_FAMILY_TREE:
        case FRONTMATTER: return chapter_uniqueness::MULTIPLE;
    }
    throw std::logic_error("Couldn't determine chapter uniqueness");
}

enum class omnibus
{
    PART1,
    PART2,
    PART3,
    PART4,
    PART5,
    ALL
};

OUTCOME_V2_NAMESPACE::result<volume> identify_volume(std::string_view uid);
std::string_view to_string_view(volume vol);
std::string_view to_string_view(omnibus vol);

std::ostream& operator<<(std::ostream& os, volume v);
std::ostream& operator<<(std::ostream& os, omnibus v);
std::ostream& operator<<(std::ostream& os, const std::variant<omnibus, volume>& v);

constexpr bool is_bodymatter(chapter_type chapter) noexcept
{
    /* Must update enum order */
    for (const chapter_type c : std::initializer_list<chapter_type>{
            COVER, CHARACTERS, CHAPTER, MAP_RA_LIBRARY, AURELIA_FAMILY_TREE})
    {
        if (c == chapter) {
            return true;
        }
    }

    return false;
}

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

    constexpr chapter_type
    evaluate_chapter_type() const {
        constexpr auto cover_regex                     = ctre::search<"cover.xhtml">;
        constexpr auto frontmatter_regex               = ctre::search<"text/front", ctre::case_insensitive>;
        constexpr auto chapter_regex                   = ctre::search<"(xhtml/p-[0-9]*.xhtml|chapter|prologue|epilogue|x[0-9_]*.xhtml|text/[0-9]*.xhtml|text/insert|extra|side|temple)", ctre::case_insensitive>;
        constexpr auto chapter2_regex                  = ctre::search<"text/content[0-9]+.xhtml", ctre::case_insensitive>;
        // ehrenfest city is frontmatter3.xhtml
        constexpr auto map_ehrenfest_city_label_regex  = ctre::search<"Map of Ehrenfest", ctre::case_insensitive>; // can also use toc_label but this works for now
        constexpr auto map_ehrenfest_duchy_label_regex = ctre::search<"Map of Ehrenfest Duchy", ctre::case_insensitive>; // can also use toc_label but this works for now
        constexpr auto map_yurgenschmidt_label_regex   = ctre::search<"Map of Yurgenschmidt", ctre::case_insensitive>;
        constexpr auto map_ehrenfest_href_regex        = ctre::search<"map.xhtml", ctre::case_insensitive>; // can also use toc_label but this works for now
        constexpr auto map_yurgenschmidt_href_regex    = ctre::search<"map2.xhtml", ctre::case_insensitive>;
        constexpr auto family_tree_regex               = ctre::search<"tree.xhtml", ctre::case_insensitive>;
        constexpr auto afterword_regex                 = ctre::search<"afterword.xhtml", ctre::case_insensitive>;
        constexpr auto manga_regex                     = ctre::search<"text/manga", ctre::case_insensitive>;
        constexpr auto poll_regex                      = ctre::search<"text/poll", ctre::case_insensitive>;
        constexpr auto bonus_regex                     = ctre::search<"text/bonus", ctre::case_insensitive>;
        constexpr auto copyright_regex                 = ctre::search<"text/copyright", ctre::case_insensitive>;
        constexpr auto ncx_regex                       = ctre::search<"dtbncx", ctre::case_insensitive>; // mediatype
        constexpr auto stylesheet_regex                = ctre::search<"css", ctre::case_insensitive>; // mediatype
        constexpr auto image_regex                     = ctre::search<"image/", ctre::case_insensitive>; // mediatype
        constexpr auto toc_regex                       = ctre::search<"toc.xhtml", ctre::case_insensitive>;
        constexpr auto characters_regex                = ctre::search<"text/character", ctre::case_insensitive>;
        constexpr auto signup_regex                    = ctre::search<"text/signup", ctre::case_insensitive>;

        if (image_regex(mediatype)) { // for the .jpg
            return IMAGE;
        }
        if (ncx_regex(mediatype)) {
            return NCX;
        }
        if (stylesheet_regex(mediatype)) {
            return STYLESHEET;
        }
        if (volume::FB3 == vol) { // must be before other maps
            if (map_ehrenfest_href_regex(href)) {
                return MAP_RA_LIBRARY;
            }
            if (toc_label && *toc_label == "Yurgenschmidt Duchies") {
                return TABLE_YURGENSCHMIDT_DUCHIES;
            }
        }
        if (chapter_regex(href)) {
            return CHAPTER;
        }
        if (cover_regex(href)) {
            return COVER;
        }

        if (chapter2_regex(href)) {
            return CHAPTER;
        }

        if (toc_label) {
            if (map_ehrenfest_duchy_label_regex(*toc_label)) {
                return MAP_EHRENFEST_DUCHY;
            }
            if (map_yurgenschmidt_label_regex(*toc_label)) {
                return MAP_YURGENSCHMIDT;
            }
            if (map_ehrenfest_city_label_regex(*toc_label)) {
                return MAP_EHRENFEST_CITY;
            }
        } else {
            if (map_ehrenfest_href_regex(href)) {
                return MAP_EHRENFEST_DUCHY;
            }
            if (map_yurgenschmidt_href_regex(href)) {
                return MAP_YURGENSCHMIDT;
            }
        }

        if (frontmatter_regex(href)) {
            return FRONTMATTER; // must be after maps
        }
        if (volume::P4V5 == vol && family_tree_regex(href)) {
            if (toc_label.has_value()) {
                return AURELIA_FAMILY_TREE;
            } else {
                return CHAPTER;
            }
        }
        if (afterword_regex(href)) {
            return AFTERWORD;
        }
        if (manga_regex(href)) {
            return MANGA;
        }
        if (poll_regex(href)) {
            return POLL;
        }
        if (bonus_regex(href)) {
            return BONUS;
        }
        if (copyright_regex(href)) {
            return COPYRIGHT;
        }
        if (toc_regex(href)) {
            return TOC;
        }
        if (characters_regex(href)) {
            return CHARACTERS;
        }
        if (signup_regex(href)) {
            return SIGNUP;
        }

        throw std::logic_error("Unable to identify chapter type");
    }

    template<typename StringHref, typename StringMedia, typename StringLabel>
    constexpr volume_definition(volume vol, StringHref&& href, StringMedia&& mediatype, StringLabel&& toc_label, bool in_spine)
        : vol(vol),
          href(std::forward<StringHref>(href)),
          mediatype(std::forward<StringMedia>(mediatype)),
          toc_label(std::forward<StringLabel>(toc_label)),
          in_spine(in_spine),
          type(evaluate_chapter_type()) {}
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
std::ostream& operator<<(std::ostream& os, chapter_type c);

volume get_volume_from_slug(std::string_view sv);
std::string_view get_slug_from_volume(volume vol);
