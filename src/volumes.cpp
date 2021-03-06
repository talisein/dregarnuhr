#include <string_view>
#include <map>
#include <regex>
#include <cassert>
#include "outcome/result.hpp"
#include "volumes.h"
#include "log.h"

namespace {
    using namespace std::string_view_literals;
    const std::map<std::string_view, volume> volume_map {
        {"9781718344631"sv, volume::FB1},
        {"9781718344648"sv, volume::FB2},
        {"9781718346017"sv, volume::P1V1},
        {"9781718346031"sv, volume::P1V2},
        {"9781718346055"sv, volume::P1V3},
        {"9781718346079"sv, volume::P2V1},
        {"9781718346093"sv, volume::P2V2},
        {"9781718346116"sv, volume::P2V3},
        {"9781718346130"sv, volume::P2V4},
        {"9781718346154"sv, volume::P3V1},
        {"9781718346178"sv, volume::P3V2},
        {"9781718346192"sv, volume::P3V3},
        {"9781718346215"sv, volume::P3V4},
        {"9781718346239"sv, volume::P3V5},
        {"9781718346253"sv, volume::P4V1},
        {"9781718346277"sv, volume::P4V2},
        {"9781718346291"sv, volume::P4V3},
        {"9781718346314"sv, volume::P4V4},
        {"9781718346338"sv, volume::P4V5},
        {"9781718346352"sv, volume::P4V6},
        {"9781718346376"sv, volume::P4V7},
        {"9781718338005"sv, volume::MP1V1},
        {"9781718338029"sv, volume::MP1V2},
        {"9781718338043"sv, volume::MP1V3},
        {"9781718338067"sv, volume::MP1V4},
        {"9781718338081"sv, volume::MP1V5},
        {"9781718338104"sv, volume::MP1V6},
        {"9781718338128"sv, volume::MP1V7},
        {"9781718338142"sv, volume::MP2V1},
        {"9781718338166"sv, volume::MP2V2},
        {"9781718338180"sv, volume::MP2V3},
        {"9781718338203"sv, volume::MP2V4},
        {"9781718338227"sv, volume::MP2V5},
    };

    const std::regex cover_regex                   {"cover.xhtml", std::regex_constants::icase};
    const std::regex frontmatter_regex             {"text/front", std::regex_constants::icase};
    const std::regex chapter_regex                 {"(xhtml/p-[0-9]*.xhtml|cover.xhtml|text/character|chapter|prologue|epilogue|x[0-9_]*.xhtml|text/[0-9]*.xhtml|text/insert|extra|side|temple)", std::regex_constants::icase};
    const std::regex map_ehrenfest_label_regex     {"Map of Ehrenfest", std::regex_constants::icase}; // can also use toc_label but this works for now
    const std::regex map_yurgenschmidt_label_regex {"Map of Yurgenschmidt", std::regex_constants::icase};
    const std::regex map_ehrenfest_href_regex      {"map.xhtml", std::regex_constants::icase}; // can also use toc_label but this works for now
    const std::regex map_yurgenschmidt_href_regex  {"map2.xhtml", std::regex_constants::icase};
    const std::regex family_tree_regex             {"tree.xhtml", std::regex_constants::icase};
    const std::regex afterword_regex               {"afterword.xhtml", std::regex_constants::icase};
    const std::regex manga_regex                   {"text/manga", std::regex_constants::icase};
    const std::regex poll_regex                    {"text/poll", std::regex_constants::icase};
    const std::regex bonus_regex                   {"text/bonus", std::regex_constants::icase};
    const std::regex copyright_regex               {"text/copyright", std::regex_constants::icase};
    const std::regex ncx_regex                     {"dtbncx", std::regex_constants::icase}; // mediatype
    const std::regex stylesheet_regex              {"css", std::regex_constants::icase};
    const std::regex image_regex                   {"image/", std::regex_constants::icase}; //mediatype
    const std::regex toc_regex                     {"toc.xhtml", std::regex_constants::icase};
    const std::regex characters_regex              {"text/character", std::regex_constants::icase};
    const std::regex signup_regex                  {"text/signup", std::regex_constants::icase};
}

std::string_view to_string_view(volume vol)
{
    switch (vol) {
        case volume::P1V1: return "P1V1"sv;
        case volume::P1V2: return "P1V2"sv;
        case volume::P1V3: return "P1V3"sv;
        case volume::P2V1: return "P2V1"sv;
        case volume::P2V2: return "P2V2"sv;
        case volume::P2V3: return "P2V3"sv;
        case volume::P2V4: return "P2V4"sv;
        case volume::P3V1: return "P3V1"sv;
        case volume::P3V2: return "P3V2"sv;
        case volume::P3V3: return "P3V3"sv;
        case volume::P3V4: return "P3V4"sv;
        case volume::P3V5: return "P3V5"sv;
        case volume::P4V1: return "P4V1"sv;
        case volume::P4V2: return "P4V2"sv;
        case volume::P4V3: return "P4V3"sv;
        case volume::P4V4: return "P4V4"sv;
        case volume::P4V5: return "P4V5"sv;
        case volume::P4V6: return "P4V6"sv;
        case volume::P4V7: return "P4V7"sv;
        case volume::P4V8: return "P4V8"sv;
        case volume::FB1:  return "FB1"sv;
        case volume::FB2:  return "FB2"sv;
        case volume::RA1:  return "RA1"sv;
        case volume::MP1V1:  return "M1V1"sv;
        case volume::MP1V2:  return "M1V2"sv;
        case volume::MP1V3:  return "M1V3"sv;
        case volume::MP1V4:  return "M1V4"sv;
        case volume::MP1V5:  return "M1V5"sv;
        case volume::MP1V6:  return "M1V6"sv;
        case volume::MP1V7:  return "M1V7"sv;
        case volume::MP2V1:  return "M2V1"sv;
        case volume::MP2V2:  return "M2V2"sv;
        case volume::MP2V3:  return "M2V3"sv;
        case volume::MP2V4:  return "M2V4"sv;
        case volume::MP2V5:  return "M2V5"sv;
    }
    assert(false);
    return "UNKNOWN"sv;
}


OUTCOME_V2_NAMESPACE::result<volume>
identify_volume(std::string_view uid)
{
    auto iter = volume_map.find(uid);
    if (iter == volume_map.end()) {
        return std::errc::invalid_argument;
    }
    return iter->second;
}

std::ostream& operator<<(std::ostream& os, const volume& v)
{
    os << to_string_view(v);
    return os;
}

std::ostream& operator<<(std::ostream& os, const volume_definition& v)
{
    os << "{ volume::" << v.vol << ", " << std::quoted(v.id) << ", " << std::quoted(v.href)
       << ", " << std::quoted(v.mediatype) << ", ";
    if (v.toc_label)
        os << std::quoted(v.toc_label.value());
    else
        os << "std::nullopt";
    os << ", " << std::boolalpha << v.in_spine << " }";
    return os;
}

chapter_uniqueness
get_uniqueness(chapter_type c)
{
    switch (c)
    {
        case TOC:
        case NCX:
        case MAP_EHRENFEST:
        case MAP_YURGENSCHMIDT:
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
    assert(false);
    return chapter_uniqueness::MULTIPLE;
}

chapter_type
volume_definition::get_chapter_type() const
{
    if (std::regex_search(mediatype.begin(), mediatype.end(), image_regex)) {
        return IMAGE;
    }
    if (std::regex_search(mediatype.begin(), mediatype.end(), ncx_regex)) {
        return NCX;
    }
    if (std::regex_search(mediatype.begin(), mediatype.end(), stylesheet_regex)) {
        return STYLESHEET;
    }
    if (std::regex_search(href.begin(), href.end(), chapter_regex)) {
        return CHAPTER;
    }
    if (std::regex_search(href.begin(), href.end(), cover_regex)) {
        return COVER; // after chapter, so actually they are classified as chapter for now
    }

    if (toc_label) {
        if (std::regex_search(toc_label.value().begin(), toc_label.value().end(), map_ehrenfest_label_regex)) {
            return MAP_EHRENFEST;
        }
        if (std::regex_search(toc_label.value().begin(), toc_label.value().end(), map_yurgenschmidt_label_regex)) {
            return MAP_YURGENSCHMIDT;
        }
    } else {
        if (std::regex_search(href.begin(), href.end(), map_ehrenfest_href_regex)) {
            return MAP_EHRENFEST;
        }
        if (std::regex_search(href.begin(), href.end(), map_yurgenschmidt_href_regex)) {
            return MAP_YURGENSCHMIDT;
        }
    }

    if (std::regex_search(href.begin(), href.end(), frontmatter_regex)) {
        return FRONTMATTER; // must be after maps
    }
    if (std::regex_search(href.begin(), href.end(), family_tree_regex)) {
        return FAMILY_TREE;
    }
    if (std::regex_search(href.begin(), href.end(), afterword_regex)) {
        return AFTERWORD;
    }
    if (std::regex_search(href.begin(), href.end(), manga_regex)) {
        return MANGA;
    }
    if (std::regex_search(href.begin(), href.end(), poll_regex)) {
        return POLL;
    }
    if (std::regex_search(href.begin(), href.end(), bonus_regex)) {
        return BONUS;
    }
    if (std::regex_search(href.begin(), href.end(), copyright_regex)) {
        return COPYRIGHT;
    }
    if (std::regex_search(href.begin(), href.end(), toc_regex)) {
        return TOC;
    }
    if (std::regex_search(href.begin(), href.end(), characters_regex)) {
        return CHARACTERS; // after chapter, so actually they are classified as chapter for now
    }
    if (std::regex_search(href.begin(), href.end(), signup_regex)) {
        return SIGNUP;
    }

    log_error("Unclassified chapter type in ", vol, " for ", href);
    return CHAPTER;
}

std::ostream& operator<<(std::ostream& os, const omnibus& v)
{
    switch (v) {
        case omnibus::PART1:
            os << "Part 1";
            break;
        case omnibus::PART2:
            os << "Part 2";
            break;
        case omnibus::PART3:
            os << "Part 3";
            break;
        case omnibus::PART4:
            os << "Part 4";
            break;
        case omnibus::ALL:
            os << "All";
            break;
    }

    return os;
}
