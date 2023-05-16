#include "chapter_type.h"
#include "magic_enum.hpp"

std::ostream& operator<<(std::ostream& os, chapter_type c)
{
    os << magic_enum::enum_name(c);
    return os;
}
