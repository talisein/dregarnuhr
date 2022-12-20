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
    FB1,
    FB2,
    FB3,
    RA1,
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
};

// The order here determines the sort order
enum chapter_type {
  NCX = 0,
  STYLESHEET,
  IMAGE,
  COVER,
  FRONTMATTER,
  CHARACTERS,
  TOC,
  CHAPTER,
  MAP_RA_LIBRARY,
  MAP_EHRENFEST_CITY,
  MAP_EHRENFEST_DUCHY,
  MAP_YURGENSCHMIDT,
  FAMILY_TREE,
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

struct volume_definition
{
    volume vol;
    std::string_view href;
    std::string_view mediatype;
    std::optional<std::string_view> toc_label;
    bool in_spine;

    void print_id(std::ostream &os) const {
        auto pos = href.find_last_of('/');
        os << vol << '-';
        if (pos == std::string_view::npos) {
            os << href;
        } else {
            os << href.substr(pos+1);
        }
    }

    std::string get_id() const {
        std::stringstream ss;
        print_id(ss);
        return ss.str();
    }

    chapter_type get_chapter_type() const;

    inline friend bool operator==(const volume_definition& lhs, const volume_definition& rhs)
    {
        return lhs.get_chapter_type() == rhs.get_chapter_type();
    }
    inline friend std::strong_ordering operator<=>(const volume_definition& lhs, const volume_definition& rhs)
    {
        return lhs.get_chapter_type() <=> rhs.get_chapter_type();
    }
};

template<std::ranges::input_range R>
struct definition_view_t : public std::ranges::view_interface<definition_view_t<R>>
{
    constexpr definition_view_t(R&& _defs, omnibus o) : defs(std::forward<R>(_defs)), name(o) {};
    constexpr definition_view_t(R&& _defs, volume v) : defs(std::forward<R>(_defs)), name(v) {};

    R defs;
    std::variant<omnibus, volume> name;

    auto begin() const { return defs.begin(); }
    auto end() const { return defs.end(); }
};

using definition_span_view_t = definition_view_t<std::span<const volume_definition>>;

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
        case FAMILY_TREE:
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
        case FRONTMATTER: return chapter_uniqueness::MULTIPLE;
    }
    return chapter_uniqueness::MULTIPLE;
}

std::ostream& operator<<(std::ostream& os, const volume_definition& v);

volume get_volume_from_slug(std::string_view sv);
