#include "utils.h"

#include <boost/ut.hpp>


int main() {
    using namespace boost::ut;
    using namespace std::string_literals;

    "simple_int_cast"_test = [] {
        expect(1_c == utils::safe_int_cast<unsigned char>(1));
        expect(throws<std::overflow_error>([] { (void)utils::safe_int_cast<unsigned char>(513); }));
        expect(throws<std::range_error>([] { (void)utils::safe_int_cast<unsigned char>(-1); }));
    };

    "simple_strcat"_test = [] {
        expect(utils::strcat("abc", "def") == "abcdef"s);
        expect(utils::strcat(1, "def") == "1def"s);
        expect(utils::strcat("abc", 1) == "abc1"s);

        constexpr std::string_view longstr { "1234567890123456" };
        auto i = 2147486647;
        auto l = 4147486647;
        auto ui = 4047486647U;
        auto ul = 10047486647UL;
        auto bigl = 9'223'372'036'854'775'807L;
        auto bigul = 18'446'744'073'709'551'615UL;
        short s = 16000;
        unsigned short us = 30000;
        std::uint8_t ub = 0xFF;
        std::int8_t b = 0x7E;

        expect(utils::strcat(longstr, "-", i) == "1234567890123456-2147486647"s);
        expect(utils::strcat(longstr, "-", l) == "1234567890123456-4147486647"s);
        expect(utils::strcat(longstr, "-", ui) == "1234567890123456-4047486647"s);
        expect(utils::strcat(longstr, "-", ul) == "1234567890123456-10047486647"s);
        expect(utils::strcat(longstr, "-", bigl) == "1234567890123456-9223372036854775807"s);
        expect(utils::strcat(longstr, "-", bigul) == "1234567890123456-18446744073709551615"s);
        expect(utils::strcat(longstr, "-", s) == "1234567890123456-16000"s);
        expect(utils::strcat(longstr, "-", us) == "1234567890123456-30000"s);
        expect(utils::strcat(longstr, "-", ub) == "1234567890123456-255"s);
        expect(utils::strcat(longstr, "-", b) == "1234567890123456-126"s);
        expect(utils::strcat(longstr, "-", -23045798234059) == "1234567890123456--23045798234059"s);
        expect(utils::strcat(longstr, '-', 0) == "1234567890123456-0"s);
        expect(utils::strcat(longstr, '-', -1) == "1234567890123456--1"s);
        expect(utils::strcat(longstr, "-", i, '-',  l,"-",  ui,"-",  ul, "-"s, bigul, '-', bigul + 1) == "1234567890123456-2147486647-4147486647-4047486647-10047486647-18446744073709551615-0"s);

    };
}
