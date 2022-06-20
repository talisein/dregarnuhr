#include <string_view>
#include <libxml++/ustring.h>
#include <libxml++/dtd.h>
#include <libxml++/relaxngschema.h>
namespace DTD {

    xmlpp::ustring get_ncx();
    xmlpp::RelaxNGSchema get_package();
}
