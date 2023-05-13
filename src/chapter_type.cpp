#include "chapter_type.h"

std::ostream& operator<<(std::ostream& os, chapter_type c)
{
    switch (c) {
        case chapter_type::NCX:                         os << "NCX";                         break;
        case chapter_type::STYLESHEET:                  os << "STYLESHEET";                  break;
        case chapter_type::IMAGE:                       os << "IMAGE";                       break;
        case chapter_type::COVER:                       os << "COVER";                       break;
        case chapter_type::FRONTMATTER:                 os << "FRONTMATTER";                 break;
        case chapter_type::CHARACTERS:                  os << "CHARACTERS";                  break;
        case chapter_type::TOC:                         os << "TOC";                         break;
        case chapter_type::CHAPTER:                     os << "CHAPTER";                     break;
        case chapter_type::MAP_RA_LIBRARY:              os << "MAP_RA_LIBRARY";              break;
        case chapter_type::MAP_EHRENFEST_CITY:          os << "MAP_EHRENFEST_CITY";          break;
        case chapter_type::MAP_EHRENFEST_DUCHY:         os << "MAP_EHRENFEST_DUCHY";         break;
        case chapter_type::MAP_YURGENSCHMIDT:           os << "MAP_YURGENSCHMIDT";           break;
        case chapter_type::TABLE_YURGENSCHMIDT_DUCHIES: os << "TABLE_YURGENSCHMIDT_DUCHIES"; break;
        case chapter_type::AURELIA_FAMILY_TREE:         os << "AURELIA_FAMILY_TREE";         break;
        case chapter_type::AFTERWORD:                   os << "AFTERWORD";                   break;
        case chapter_type::RECORDING_REPORT:            os << "RECORDING_REPORT";            break;
        case chapter_type::MANGA:                       os << "MANGA";                       break;
        case chapter_type::TOBOOKS_MANGA:               os << "TOBOOKS_MANGA";               break;
        case chapter_type::POLL:                        os << "POLL";                        break;
        case chapter_type::BONUS:                       os << "BONUS";                       break;
        case chapter_type::SIGNUP:                      os << "SIGNUP";                      break;
        case chapter_type::COPYRIGHT:                   os << "COPYRIGHT";                   break;
    }
    return os;
}
