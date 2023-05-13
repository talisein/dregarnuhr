#include "volume_definition.h"

std::ostream& operator<<(std::ostream& os, const volume_definition& v)
{
    os << "{ volume::" << v.vol << ", "
       << "/* " << v.type << " */ "
       << std::quoted(v.href)
       << ", " << std::quoted(v.mediatype) << ", ";
    if (v.toc_label)
        os << std::quoted(v.toc_label.value());
    else
        os << "std::nullopt";
    os << ", " << std::boolalpha << v.in_spine << " }";
    return os;
}
