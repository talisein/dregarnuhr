#include <spanstream>
#include <optional>
#include "args.h"

#include <boost/ut.hpp>


int main() {
    using namespace boost::ut;
    using namespace std::string_literals;

    "dump_mode"_test = [] {
        const char *argv[] = {"dregarnuhr", "--mode=dump"};

        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(get_options()->command == args::command::DUMP);
        expect(!res.has_value());
        expect(res.error() == std::errc::invalid_argument);
    };

    "dump_mode"_test = [] {
        const char *argv[] = {"dregarnuhr"};

        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(get_options()->command == args::command::NORMAL);
        expect(!res.has_value());
        expect(res.error() == std::errc::invalid_argument);
    };


}
