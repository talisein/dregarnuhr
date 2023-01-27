#include <vector>
#include <spanstream>
#include <optional>

#include <boost/ut.hpp>
#include "miniz.h"

#include "args.h"
#include "utils.h"


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

    "compression level small"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(res.has_value());
        expect(get_options()->compression_level.has_value());
        expect(eq(*get_options()->compression_level, static_cast<mz_uint>(MZ_BEST_COMPRESSION)));
    } | std::vector<const char*>{"--compression-level=smallest", "--compression-level=SMALLEST", "--compression-level=smalLest"};

    "compression level fast"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(res.has_value());
        expect(get_options()->compression_level.has_value());
        expect(eq(*get_options()->compression_level, static_cast<mz_uint>(MZ_BEST_SPEED)));
    } | std::vector<const char*>{"--compression-level=fastest", "--compression-level=FASTEST", "--compression-level=Fastest"};

    "compression level fast"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg.first, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(res.has_value());
        expect(get_options()->compression_level.has_value());
        expect(eq(*get_options()->compression_level, arg.second));
    } | std::vector<std::pair<const char*, mz_uint>>{
        {"--compression-level=0", 0}, {"--compression-level=1", 1}, {"--compression-level=8", 8},
        {"--compression-level=10", 10}, {"--compression-level=100", 10}, {"--compression-level=999999999", 10},
    };

    "compression level range"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(not res.has_value());
        expect(eq(res.error(), std::make_error_code(std::errc::result_out_of_range)));
    } | std::vector<const char*>{"--compression-level=9999999999999"};

    "compression level invalid"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(not res.has_value());
        expect(eq(res.error(), std::make_error_code(std::errc::invalid_argument)));
    } | std::vector<const char*> {
        "--compression-level=-1", "--compression-level=-10000000000000", "--compression-level=smallest1",
        "--compression-level=6abc", "--compression-level=abc5", "--compression-level=123ok"
    };

    "size filter"_test = [] (const auto& arg) {
        const char *argv[] = {"dregarnuhr", arg.first, "..", "."};
        auto res = parse(sizeof(argv)/sizeof(argv[0]), const_cast<char**>(argv));
        expect(res.has_value());
        expect(get_options()->size_filter.has_value());
        expect(eq(*get_options()->size_filter, arg.second));
    } | std::vector<std::pair<const char*, std::size_t>>{
        {"--filter=size=123", 123ULL}, {"--filter=size=456:name=[abc]", 456ULL}, {"--filter=name=wow:size=789", 789ULL},
    };
}
