#include <optional>
#include "utils.h"

#include <boost/ut.hpp>

template<> auto boost::ut::cfg<boost::ut::override> = boost::ut::runner<boost::ut::reporter<boost::ut::printer>>{};

int main() {
    using namespace boost::ut;
    using namespace std::string_literals;
    using namespace std::string_view_literals;

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

    "optional_strcat"_test = [] {
        std::optional<std::string> a = std::nullopt;
        std::optional<std::string> b{"okay"s};
        std::optional<std::string_view> c{"works"};

        expect(utils::strcat(a,b,c) == "okayworks"s);
        expect(utils::strcat(a,b,2,c) == "okay2works"s);
        expect(utils::strcat(a,b,a,2,c) == "okay2works"s);
        expect(utils::strcat(a,b,a,2,a,c) == "okay2works"s);
        expect(utils::strcat(a,b,2,a,c) == "okay2works"s);

    };

    "find_opt"_test = [] {
        using namespace std::string_view_literals;
        const char *arr[] = {"this", "is", "a", "test"};
        auto view = std::views::counted(arr, 4) | std::views::transform([](const char *p) { return std::string_view(p); });
        auto sv = utils::find_opt(view, "test"sv);
        expect(sv.has_value());
        expect("test"sv == *sv);
        sv = utils::find_opt(view, "meow"sv);
        expect(!sv.has_value());
    };

    "find_if_opt"_test = [] {
        using namespace std::string_view_literals;
        constexpr std::array<std::string_view, 4> arr {"this"sv, "is"sv, "a"sv, "test"sv};
        constexpr auto sv = utils::find_if_opt(arr, [](const auto &sv) { return sv.starts_with('t'); });
        expect(sv.has_value());
        expect(eq("this"sv, *sv));
        auto ssv = utils::find_if_opt(arr, [](const auto&){return false;});
        expect(!ssv.has_value());
    };

    "find_if_optarg"_test = [] {
        using namespace std::string_view_literals;
        constexpr std::array<std::string_view, 4> arr {"this"sv, "is=macaron"sv, "a"sv, "test"sv};
        constexpr auto sv = utils::find_if_optarg(arr, "is="sv);
        expect(sv.has_value());
        expect(eq("macaron"sv, *sv));
    };

    "import attr"_test = [] {
        xmlpp::Document doc { };
        auto root = doc.create_root_node("root");
        auto first = root->add_child_element("first");
        first->set_attribute("one", "1");
        first->set_attribute("two", "2");
        auto next = root->add_child_element("next");
        utils::import_attr(next, first);
        expect(doc.write_to_string_formatted() == R"xml(<?xml version="1.0" encoding="UTF-8"?>
<root>
  <first one="1" two="2"/>
  <next>
    <first one="1" two="2"/>
  </next>
</root>
)xml"s);
    };

    "import attr except many"_test = [] {
        xmlpp::Document doc { };
        auto root = doc.create_root_node("root");
        auto first = root->add_child_element("first");
        first->set_attribute("one", "1");
        first->set_attribute("two", "2");
        first->set_attribute("three", "3");
        auto next = root->add_child_element("next");
        utils::import_attr_except(next, first, {"two", "three"});
        expect(doc.write_to_string_formatted() == R"xml(<?xml version="1.0" encoding="UTF-8"?>
<root>
  <first one="1" two="2" three="3"/>
  <next>
    <first one="1"/>
  </next>
</root>
)xml"s);
    };

    "import attr except one"_test = [] {
        xmlpp::Document doc { };
        auto root = doc.create_root_node("root");
        auto first = root->add_child_element("first");
        first->set_attribute("one", "1");
        first->set_attribute("two", "2");
        auto next = root->add_child_element("next");
        utils::import_attr_except(next, first, "two");
        expect(doc.write_to_string_formatted() == R"xml(<?xml version="1.0" encoding="UTF-8"?>
<root>
  <first one="1" two="2"/>
  <next>
    <first one="1"/>
  </next>
</root>
)xml"s);
    };

    "import child except many"_test = [] {
        xmlpp::Document doc { };
        auto root = doc.create_root_node("root");
        auto first = root->add_child_element("first");
        first->set_attribute("one", "1");
        first->set_attribute("two", "2");
        first->add_child_element("hello");
        first->add_child_element("goodbye");
        first->add_child_element("goodnight");
        auto next = root->add_child_element("next");
        auto first_copy = utils::import_attr_except(next, first, {"two", "three"});
        utils::import_children_except(first_copy, first, {"hello", "goodnight"});
        expect(doc.write_to_string_formatted() == R"xml(<?xml version="1.0" encoding="UTF-8"?>
<root>
  <first one="1" two="2">
    <hello/>
    <goodbye/>
    <goodnight/>
  </first>
  <next>
    <first one="1">
      <goodbye/>
    </first>
  </next>
</root>
)xml"s);
    };

    "import child except one"_test = [] {
        xmlpp::Document doc { };
        auto root = doc.create_root_node("root");
        auto first = root->add_child_element("first");
        first->set_attribute("one", "1");
        first->set_attribute("two", "2");
        first->add_child_element("hello");
        first->add_child_element("goodbye");
        first->add_child_element("goodnight");
        auto next = root->add_child_element("next");
        auto first_copy = utils::import_attr_except(next, first, {"two", "three"});
        utils::import_children_except(first_copy, first, "goodbye");
        expect(doc.write_to_string_formatted() == R"xml(<?xml version="1.0" encoding="UTF-8"?>
<root>
  <first one="1" two="2">
    <hello/>
    <goodbye/>
    <goodnight/>
  </first>
  <next>
    <first one="1">
      <hello/>
      <goodnight/>
    </first>
  </next>
</root>
)xml"s);
    };
}
