#pragma once

#include <initializer_list>
#include <optional>
#include <ostream>
#include <string_view>

#include "ctre.hpp"

#include "volumes.h"

// The order here determines the sort order
enum class chapter_type {
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
  MAP_ROYAL_ACADEMY,
  TABLE_YURGENSCHMIDT_DUCHIES,
  AFTERWORD,
  RECORDING_REPORT,
  TOBOOKS_MANGA,
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
        case chapter_type::TOC:
        case chapter_type::NCX:
        case chapter_type::MAP_EHRENFEST_CITY:
        case chapter_type::MAP_EHRENFEST_DUCHY:
        case chapter_type::MAP_YURGENSCHMIDT:
        case chapter_type::MAP_ROYAL_ACADEMY:
        case chapter_type::MAP_RA_LIBRARY:
        case chapter_type::TABLE_YURGENSCHMIDT_DUCHIES:
        case chapter_type::SIGNUP:
        case chapter_type::COPYRIGHT: return chapter_uniqueness::SINGLE;
        case chapter_type::COVER:
        case chapter_type::CHARACTERS:
        case chapter_type::CHAPTER:
        case chapter_type::IMAGE:
        case chapter_type::STYLESHEET:
        case chapter_type::MANGA:
        case chapter_type::TOBOOKS_MANGA:
        case chapter_type::AFTERWORD:
        case chapter_type::RECORDING_REPORT:
        case chapter_type::POLL:
        case chapter_type::BONUS:
        case chapter_type::AURELIA_FAMILY_TREE:
        case chapter_type::FRONTMATTER: return chapter_uniqueness::MULTIPLE;
    }
    throw std::logic_error("Couldn't determine chapter uniqueness");
}

constexpr bool
is_bodymatter(chapter_type chapter) noexcept
{
    /* Must update enum order */
    for (const chapter_type c : std::initializer_list<chapter_type>{
            chapter_type::COVER,
            chapter_type::CHARACTERS,
            chapter_type::CHAPTER,
            chapter_type::MAP_RA_LIBRARY,
            chapter_type::AURELIA_FAMILY_TREE})
    {
        if (c == chapter) {
            return true;
        }
    }

    return false;
}


std::ostream& operator<<(std::ostream& os, chapter_type c);

constexpr chapter_type
evaluate_chapter_type(volume vol,
                      std::string_view mediatype,
                      std::string_view href,
                      std::optional<std::string_view> toc_label)
{
    constexpr auto cover_regex                     = ctre::search<"cover.xhtml">;
    constexpr auto frontmatter_regex               = ctre::search<"text/front", ctre::case_insensitive>;
    constexpr auto chapter_regex                   = ctre::search<"(xhtml/p-[0-9]*.xhtml|chapter|prologue|epilogue|x[0-9_]*.xhtml|text/[0-9]*.xhtml|text/insert|extra|side|temple)", ctre::case_insensitive>;
    constexpr auto chapter2_regex                  = ctre::search<"text/content[0-9]+.xhtml", ctre::case_insensitive>;
    // ehrenfest city is frontmatter3.xhtml
    constexpr auto map_ehrenfest_city_label_regex  = ctre::search<"Map of Ehrenfest", ctre::case_insensitive>; // can also use toc_label but this works for now
    constexpr auto map_ehrenfest_duchy_label_regex = ctre::search<"Map of Ehrenfest Duchy", ctre::case_insensitive>; // can also use toc_label but this works for now
    constexpr auto map_yurgenschmidt_label_regex   = ctre::search<"Map of Yurgenschmidt", ctre::case_insensitive>;
    constexpr auto map_royal_academy_label_regex   = ctre::search<"Map of the Royal Academy", ctre::case_insensitive>;
    constexpr auto map_ehrenfest_href_regex        = ctre::search<"map.xhtml", ctre::case_insensitive>; // can also use toc_label but this works for now
    constexpr auto map_yurgenschmidt_href_regex    = ctre::search<"map2.xhtml", ctre::case_insensitive>;
    constexpr auto map_royal_academy_href_regex    = ctre::search<"map3.xhtml", ctre::case_insensitive>;
    constexpr auto family_tree_regex               = ctre::search<"tree.xhtml", ctre::case_insensitive>;
    constexpr auto afterword_regex                 = ctre::search<"(afterword.xhtml|final[0-9][_]*[0-9]*)", ctre::case_insensitive>;
    constexpr auto manga_regex                     = ctre::search<"text/manga", ctre::case_insensitive>;
    constexpr auto poll_regex                      = ctre::search<"text/poll", ctre::case_insensitive>;
    constexpr auto bonus_regex                     = ctre::search<"text/bonus", ctre::case_insensitive>;
    constexpr auto copyright_regex                 = ctre::search<"text/copyright", ctre::case_insensitive>;
    constexpr auto ncx_regex                       = ctre::search<"dtbncx", ctre::case_insensitive>; // mediatype
    constexpr auto stylesheet_regex                = ctre::search<"css", ctre::case_insensitive>; // mediatype
    constexpr auto image_regex                     = ctre::search<"image/", ctre::case_insensitive>; // mediatype
    constexpr auto toc_regex                       = ctre::search<"(toc|nav).xhtml", ctre::case_insensitive>;
    constexpr auto characters_regex                = ctre::search<"text/character", ctre::case_insensitive>;
    constexpr auto signup_regex                    = ctre::search<"text/signup", ctre::case_insensitive>;

    if (image_regex(mediatype)) { // for the .jpg
        return chapter_type::IMAGE;
    }
    if (ncx_regex(mediatype)) {
        return chapter_type::NCX;
    }
    if (stylesheet_regex(mediatype)) {
        return chapter_type::STYLESHEET;
    }
    if (volume::FB3 == vol) { // must be before other maps
        if (map_ehrenfest_href_regex(href)) {
            return chapter_type::MAP_RA_LIBRARY;
        }
        if (toc_label && *toc_label == "Yurgenschmidt Duchies") {
            return chapter_type::TABLE_YURGENSCHMIDT_DUCHIES;
        }
    }

    if (volume::UFTSS1 == vol) {
        if (ctre::search<"/1.xhtml">(href))
            return chapter_type::FRONTMATTER;
        if (ctre::search<"/[0-9]-[0-9].xhtml">(href))
            return chapter_type::FRONTMATTER;
        if (ctre::search<"/[5-9].xhtml">(href))
            return chapter_type::RECORDING_REPORT;
        if (ctre::search<"/10-[0-9].xhtml">(href))
            return chapter_type::RECORDING_REPORT;
        if (ctre::search<"/11-[0-9].xhtml">(href))
            return chapter_type::CHAPTER;
        if (ctre::search<"/(12|13|14|15|16|17).xhtml">(href))
            return chapter_type::TOBOOKS_MANGA;
    }

    if (chapter_regex(href)) {
        return chapter_type::CHAPTER;
    }

    if (cover_regex(href)) {
        return chapter_type::COVER;
    }

    if (chapter2_regex(href)) {
        return chapter_type::CHAPTER;
    }

    if (toc_label) {
        if (map_ehrenfest_duchy_label_regex(*toc_label)) {
            return chapter_type::MAP_EHRENFEST_DUCHY;
        }
        if (map_yurgenschmidt_label_regex(*toc_label)) {
            return chapter_type::MAP_YURGENSCHMIDT;
        }
        if (map_ehrenfest_city_label_regex(*toc_label)) {
            return chapter_type::MAP_EHRENFEST_CITY;
        }
        if (map_royal_academy_label_regex(*toc_label)) {
            return chapter_type::MAP_ROYAL_ACADEMY;
        }
    } else {
        if (map_ehrenfest_href_regex(href)) {
            return chapter_type::MAP_EHRENFEST_DUCHY;
        }
        if (map_yurgenschmidt_href_regex(href)) {
            return chapter_type::MAP_YURGENSCHMIDT;
        }
        if (map_royal_academy_href_regex(href)) {
            return chapter_type::MAP_ROYAL_ACADEMY;
        }
    }

    if (frontmatter_regex(href)) {
        return chapter_type::FRONTMATTER; // must be after maps
    }
    if (volume::P4V5 == vol && family_tree_regex(href)) {
        if (toc_label.has_value()) {
            return chapter_type::AURELIA_FAMILY_TREE;
        } else {
            return chapter_type::CHAPTER;
        }
    }
    if (afterword_regex(href)) {
        return chapter_type::AFTERWORD;
    }
    if (manga_regex(href)) {
        return chapter_type::MANGA;
    }
    if (poll_regex(href)) {
        return chapter_type::POLL;
    }
    if (bonus_regex(href)) {
        return chapter_type::BONUS;
    }
    if (copyright_regex(href)) {
        return chapter_type::COPYRIGHT;
    }
    if (toc_regex(href)) {
        return chapter_type::TOC;
    }
    if (characters_regex(href)) {
        return chapter_type::CHARACTERS;
    }
    if (signup_regex(href)) {
        return chapter_type::SIGNUP;
    }

    throw std::logic_error("Unable to identify chapter type");
}
