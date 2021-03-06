#pragma once

#include <optional>
#include <string_view>
#include <iostream>
#include <iomanip>
#include <map>
#include <memory>

#include "outcome/result.hpp"
#include "epub.h"

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
    FB1,
    FB2,
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
  MAP_EHRENFEST,
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
    ALL
};

OUTCOME_V2_NAMESPACE::result<volume> identify_volume(std::string_view uid);
std::string_view to_string_view(volume vol);

std::ostream& operator<<(std::ostream& os, const volume& v);
std::ostream& operator<<(std::ostream& os, const omnibus& v);

struct volume_definition;
chapter_type get_chapter_type(const volume_definition& v);
struct volume_definition
{
    volume vol;
    std::string_view id;
    std::string_view href;
    std::string_view mediatype;
    std::optional<std::string_view> toc_label;
    bool in_spine;

    chapter_type get_chapter_type() const;

    bool is_in_spine(const std::map<volume, std::unique_ptr<epub::book_reader>>&) {
        return in_spine;
    }

    inline friend bool operator==(const volume_definition& lhs, const volume_definition& rhs)
    {
        return lhs.get_chapter_type() == rhs.get_chapter_type();
    }
    inline friend std::strong_ordering operator<=>(const volume_definition& lhs, const volume_definition& rhs)
    {
        return lhs.get_chapter_type() <=> rhs.get_chapter_type();
    }
};

chapter_uniqueness get_uniqueness(chapter_type c);

std::ostream& operator<<(std::ostream& os, const volume_definition& v);
