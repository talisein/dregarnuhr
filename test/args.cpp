#include <vector>
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
        expect(get_options()->command == args::command_e::DUMP);
        expect(!res.has_value());
        expect(res.error() == std::errc::invalid_argument);
    };

    "normal_mode"_test = [] {
        const char *argv[] = {"dregarnuhr"};

        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(eq(get_options()->command, args::command_e::NORMAL));
        expect(!res.has_value());
        expect(res.error() == std::errc::invalid_argument);
    };


    "omnibus all"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(get_options()->omnibus_type.has_value());
        expect(get_options()->omnibus_type.value() == omnibus::ALL);
    } | std::vector<const char*>{"--omnibus", "--omnibus=", "--omnibus=all", "--omnibus=ALL", "--omnibus=AlL"};


    "omnibus part1"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(get_options()->omnibus_type.has_value());
        expect(eq(get_options()->omnibus_type.value(), omnibus::PART1));
    } | std::vector<const char*>{"--omnibus=part1", "--omnibus=Part1", "--omnibus=PART1"};

    "omnibus part2"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(get_options()->omnibus_type.has_value());
        expect(eq(get_options()->omnibus_type.value(), omnibus::PART2));
    } | std::vector<const char*>{"--omnibus=part2", "--omnibus=Part2", "--omnibus=PART2"};

    "omnibus part3"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(get_options()->omnibus_type.has_value());
        expect(eq(get_options()->omnibus_type.value(), omnibus::PART3));
    } | std::vector<const char*>{"--omnibus=part3", "--omnibus=Part3", "--omnibus=PART3"};

    "omnibus part4"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(get_options()->omnibus_type.has_value());
        expect(eq(get_options()->omnibus_type.value(), omnibus::PART4));
    } | std::vector<const char*>{"--omnibus=part4", "--omnibus=Part4", "--omnibus=PART4"};

    "omnibus part5"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(get_options()->omnibus_type.has_value());
        expect(eq(get_options()->omnibus_type.value(), omnibus::PART5));
    } | std::vector<const char*>{"--omnibus=part5", "--omnibus=Part5", "--omnibus=PART5"};

    "omnibus invalid"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(not res.has_value());
        expect(eq(res.error(), std::make_error_code(std::errc::invalid_argument)));
    } | std::vector<const char*>{"--omnibus=part6", "--omnibus=garbage", "--omnibus=Part"};

}
