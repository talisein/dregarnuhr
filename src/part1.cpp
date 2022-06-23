#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>

#include "part1.h"
#include "utils.h"

namespace
{
    using namespace std::string_view_literals;

        constinit std::array vol_1 = std::to_array<volume_definition>({
{ volume::P1V1, "cover"sv, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Cover"sv), true },
{ volume::P1V1, "style"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::FB2, "fb2stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::FB1, "fb1stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V1, "ncx"sv, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P1V1, "Cover.jpg"sv, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "FrontMatter1.jpg"sv, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "FrontMatter3.jpg"sv, "Images/FrontMatter3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert1.jpg"sv, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert2.jpg"sv, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert3.jpg"sv, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert4.jpg"sv, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert5.jpg"sv, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert6.jpg"sv, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert7.jpg"sv, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert8.jpg"sv, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Insert9.jpg"sv, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "x37.jpg"sv, "Images/37.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "x38.jpg"sv, "Images/38.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "x39.jpg"sv, "Images/39.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "x40.jpg"sv, "Images/40.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "x41.jpg"sv, "Images/41.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "x42.jpg"sv, "Images/42.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "x43.jpg"sv, "Images/43.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "x44.jpg"sv, "Images/44.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB2, "Image2.jpg"sv, "Images/Image2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "FrontMatter2.jpg"sv, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Bonus1.jpg"sv, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "jnovelclubCMYK.png"sv, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P1V1, "Bonus2.jpg"sv, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "frontmatter1.xhtml"sv, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Color Illustrations"sv), true },
{ volume::P1V1, "frontmatter2.xhtml"sv, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "frontmatter3.xhtml"sv, "Text/frontmatter3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Map of Ehrenfest"sv), true },
{ volume::P1V1, "toc"sv, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "prologue.xhtml"sv, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Prologue"sv), true },
{ volume::P1V1, "extra2.xhtml"sv, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Unchanging Daily Life"sv), true },
{ volume::P1V1, "chapter01.xhtml"sv, "Text/chapter01.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A New Life"sv), true },
{ volume::P1V1, "chapter02.xhtml"sv, "Text/chapter02.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Exploring My Home"sv), true },
{ volume::P1V1, "insert1.xhtml"sv, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter021.xhtml"sv, "Text/chapter021.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter03.xhtml"sv, "Text/chapter03.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Exploring the City"sv), true },
{ volume::P1V1, "chapter04.xhtml"sv, "Text/chapter04.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Books: Unobtainable"sv), true },
{ volume::P1V1, "chapter05.xhtml"sv, "Text/chapter05.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Lifestyle Overhaul"sv), true },
{ volume::P1V1, "chapter06.xhtml"sv, "Text/chapter06.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Neighborhood Boys"sv), true },
{ volume::P1V1, "chapter07.xhtml"sv, "Text/chapter07.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Paper: Unobtainable"sv), true },
{ volume::P1V1, "insert2.xhtml"sv, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter071.xhtml"sv, "Text/chapter071.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter08.xhtml"sv, "Text/chapter08.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Learning to Respect Egyptian Culture"sv), true },
{ volume::P1V1, "chapter09.xhtml"sv, "Text/chapter09.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Winter Preparations"sv), true },
{ volume::P1V1, "chapter10.xhtml"sv, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Stone Slate Get!"sv), true },
{ volume::P1V1, "insert3.xhtml"sv, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter101.xhtml"sv, "Text/chapter101.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter11.xhtml"sv, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Beaten by Ancient Egyptians"sv), true },
{ volume::P1V1, "chapter12.xhtml"sv, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Sweet Taste of Winter"sv), true },
{ volume::P1V1, "insert4.xhtml"sv, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter121.xhtml"sv, "Text/chapter121.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter13.xhtml"sv, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Helping Out Otto"sv), true },
{ volume::P1V1, "chapter14.xhtml"sv, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Tuuli’s Hair Ornament"sv), true },
{ volume::P1V1, "insert5.xhtml"sv, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter141.xhtml"sv, "Text/chapter141.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB2,  "side1.xhtml"sv, "Text/side1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("One Winter Day"sv), true },
{ volume::FB2,  "x2.xhtml"sv, "Text/2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter15.xhtml"sv, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bring Me to The Forest"sv), true },
{ volume::P1V1, "chapter16.xhtml"sv, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Long Live Mesopotamian Culture"sv), true },
{ volume::P1V1, "insert6.xhtml"sv, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter161.xhtml"sv, "Text/chapter161.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1,  "x37.xhtml"sv, "Text/37.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("My Daughter’s about to Be a Criminal?! by Suzuka"sv), true },
{ volume::FB1, "x38.xhtml"sv, "Text/38.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "x39.xhtml"sv, "Text/39.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "x40.xhtml"sv, "Text/40.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "x41.xhtml"sv, "Text/41.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "x42.xhtml"sv, "Text/42.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "x43.xhtml"sv, "Text/43.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "x44.xhtml"sv, "Text/44.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter17.xhtml"sv, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Clay Tablets Won’t Work"sv), true },
{ volume::P1V1, "chapter18.xhtml"sv, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Tuuli’s Baptism"sv), true },
{ volume::P1V1, "chapter19.xhtml"sv, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("I Love You, Yellow River Culture"sv), true },
{ volume::P1V1, "insert7.xhtml"sv, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter191.xhtml"sv, "Text/chapter191.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter20.xhtml"sv, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("I Want Ink"sv), true },
{ volume::P1V1, "chapter21.xhtml"sv, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Intense Battle with Food"sv), true },
{ volume::P1V1, "chapter22.xhtml"sv, "Text/chapter22.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Mokkan and A Mysterious Fever"sv), true },
{ volume::P1V1, "extra1.xhtml"sv, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Day Without Myne"sv), true },
{ volume::P1V1, "chapter23.xhtml"sv, "Text/chapter23.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Road to a Meeting"sv), true },
{ volume::P1V1, "chapter24.xhtml"sv, "Text/chapter24.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Meeting with the Merchant"sv), true },
{ volume::P1V1, "insert8.xhtml"sv, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "chapter241.xhtml"sv, "Text/chapter241.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "epilogue.xhtml"sv, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Epilogue"sv), true },
{ volume::P1V1, "insert9.xhtml"sv, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "epilogue1.xhtml"sv, "Text/epilogue1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "afterword.xhtml"sv, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Afterword"sv), true },
{ volume::P1V1, "bonus1.xhtml"sv, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bonus High-Resolution Illustrations"sv), true },
{ volume::P1V1, "bonus2.xhtml"sv, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "signup.xhtml"sv, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("About J-Novel Club"sv), true },
{ volume::P1V1, "copyright.xhtml"sv, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Copyright"sv), true },
        });

    constinit std::array vol_2 = std::to_array<volume_definition>({
{ volume::P1V2, "ncx"sv, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P1V2, "stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V2, "FrontMatter3.jpg"sv, "Images/FrontMatter3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Cover.jpg"sv, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "FrontMatter1.jpg"sv, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "FrontMatter2.jpg"sv, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert1.jpg"sv, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert2.jpg"sv, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert3.jpg"sv, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert4.jpg"sv, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert5.jpg"sv, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert6.jpg"sv, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert7.jpg"sv, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert8.jpg"sv, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Insert9.jpg"sv, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Bonus1.jpg"sv, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "jnovelclubCMYK.png"sv, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P1V2, "Bonus2.jpg"sv, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "cover"sv, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Cover"sv), true },
{ volume::P1V2, "frontmatter1.xhtml"sv, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Color Illustrations"sv), true },
{ volume::P1V2, "frontmatter2.xhtml"sv, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "frontmatter3.xhtml"sv, "Text/frontmatter3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Map of Ehrenfest"sv), true },
{ volume::P1V2, "toc.xhtml"sv, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "prologue.xhtml"sv, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Prologue"sv), true },
{ volume::P1V2, "chapter01.xhtml"sv, "Text/chapter01.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Road to Washi"sv), true },
{ volume::P1V2, "insert1.xhtml"sv, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter01_1.xhtml"sv, "Text/chapter01_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter02.xhtml"sv, "Text/chapter02.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Invitation to the Otto Residence"sv), true },
{ volume::P1V2, "chapter03.xhtml"sv, "Text/chapter03.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Summons from Benno"sv), true },
{ volume::P1V2, "chapter04.xhtml"sv, "Text/chapter04.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Contract Magic"sv), true },
{ volume::P1V2, "insert2.xhtml"sv, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter04_1.xhtml"sv, "Text/chapter04_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter05.xhtml"sv, "Text/chapter05.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Lutz’s Most Important Job"sv), true },
{ volume::P1V2, "chapter06.xhtml"sv, "Text/chapter06.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Ordering Tools and Materials"sv), true },
{ volume::P1V2, "chapter07.xhtml"sv, "Text/chapter07.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Starting to Make Paper"sv), true },
{ volume::P1V2, "chapter08.xhtml"sv, "Text/chapter08.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Painful Mistakes"sv), true },
{ volume::P1V2, "insert3.xhtml"sv, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter08_1.xhtml"sv, "Text/chapter08_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter09.xhtml"sv, "Text/chapter09.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Lutz’s Myne"sv), true },
{ volume::P1V2, "insert4.xhtml"sv, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter10.xhtml"sv, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Completed Paper"sv), true },
{ volume::P1V2, "chapter11.xhtml"sv, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Merchants’ Guild"sv), true },
{ volume::P1V2, "chapter12.xhtml"sv, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Guildmaster and the Hair Ornament"sv), true },
{ volume::P1V2, "insert5.xhtml"sv, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter12_1.xhtml"sv, "Text/chapter12_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter13.xhtml"sv, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Guildmaster’s Granddaughter"sv), true },
{ volume::P1V2, "chapter14.xhtml"sv, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Freida’s Hair Ornaments"sv), true },
{ volume::P1V2, "chapter15.xhtml"sv, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Delivering the Hair Ornaments"sv), true },
{ volume::P1V2, "insert6.xhtml"sv, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter15_1.xhtml"sv, "Text/chapter15_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter16.xhtml"sv, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Winter Handiwork"sv), true },
{ volume::P1V2, "insert7.xhtml"sv, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter17.xhtml"sv, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Lutz’s Training Curriculum"sv), true },
{ volume::P1V2, "chapter18.xhtml"sv, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Why It Failed and How to Fix It"sv), true },
{ volume::P1V2, "chapter19.xhtml"sv, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Trombe Appears"sv), true },
{ volume::P1V2, "chapter20.xhtml"sv, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("We Went Ahead and Made Some"sv), true },
{ volume::P1V2, "insert8.xhtml"sv, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "chapter20_1.xhtml"sv, "Text/chapter20_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "extra2.xhtml"sv, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Gossiping by the Well"sv), true },
{ volume::P1V2, "chapter21.xhtml"sv, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Myne Collapses"sv), true },
{ volume::P1V2, "insert9.xhtml"sv, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "extra1.xhtml"sv, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Corinna’s Married Life"sv), true },
{ volume::P1V2, "epilogue.xhtml"sv, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Epilogue"sv), true },
{ volume::P1V2, "afterword.xhtml"sv, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Afterword"sv), true },
{ volume::P1V2, "bonus1.xhtml"sv, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bonus High Resolution Illustrations"sv), true },
{ volume::P1V2, "bonus2.xhtml"sv, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "signup.xhtml"sv, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("About J-Novel Club"sv), true },
{ volume::P1V2, "copyright.xhtml"sv, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Copyright"sv), true },
        });

constinit std::array vol_3 = std::to_array<volume_definition>({
        { volume::P1V3, "ncx"sv, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P1V3, "stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V3, "Cover.jpg"sv, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "FrontMatter1.jpg"sv, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "FrontMatter2.jpg"sv, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "FrontMatter3.jpg"sv, "Images/FrontMatter3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert1.jpg"sv, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert2.jpg"sv, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert3.jpg"sv, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert4.jpg"sv, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert5.jpg"sv, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert6.jpg"sv, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert7.jpg"sv, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert8.jpg"sv, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Insert9.jpg"sv, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Bonus1.jpg"sv, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "jnovelclubCMYK.png"sv, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P1V3, "Bonus2.jpg"sv, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "cover"sv, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Cover"sv), true },
{ volume::P1V3, "frontmatter1.xhtml"sv, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Color Illustrations"sv), true },
{ volume::P1V3, "frontmatter2.xhtml"sv, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "frontmatter3.xhtml"sv, "Text/frontmatter3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Map of Ehrenfest"sv), true },
{ volume::P1V3, "toc.xhtml"sv, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "prologue.xhtml"sv, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Prologue"sv), true },
{ volume::P1V3, "chapter01.xhtml"sv, "Text/chapter01.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Discussing the Devouring with Freida"sv), true },
{ volume::P1V3, "chapter02.xhtml"sv, "Text/chapter02.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Making Cake with Freida"sv), true },
{ volume::P1V3, "insert1.xhtml"sv, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter02_1.xhtml"sv, "Text/chapter02_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter03.xhtml"sv, "Text/chapter03.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Taking a Bath with Freida"sv), true },
{ volume::P1V3, "chapter04.xhtml"sv, "Text/chapter04.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Freida’s Baptism"sv), true },
{ volume::P1V3, "chapter05.xhtml"sv, "Text/chapter05.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Winter Begins"sv), true },
{ volume::P1V3, "chapter06.xhtml"sv, "Text/chapter06.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Completion of My Outfit and Hairpin"sv), true },
{ volume::P1V3, "chapter07.xhtml"sv, "Text/chapter07.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Lutz’s Tutor"sv), true },
{ volume::P1V3, "chapter08.xhtml"sv, "Text/chapter08.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Otto’s Counseling Room"sv), true },
{ volume::P1V3, "chapter09.xhtml"sv, "Text/chapter09.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Family Meeting"sv), true },
{ volume::P1V3, "insert2.xhtml"sv, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter09_1.xhtml"sv, "Text/chapter09_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter10.xhtml"sv, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Report to Lutz"sv), true },
{ volume::P1V3, "chapter11.xhtml"sv, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Back to Paper-Making"sv), true },
{ volume::P1V3, "chapter12.xhtml"sv, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Vested Interests"sv), true },
{ volume::P1V3, "chapter13.xhtml"sv, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Result of the Meeting with the Vested Interests"sv), true },
{ volume::P1V3, "insert3.xhtml"sv, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter13_1.xhtml"sv, "Text/chapter13_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter14.xhtml"sv, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Tools and Selecting a Workshop"sv), true },
{ volume::P1V3, "chapter15.xhtml"sv, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Preparing for Lutz’s Apprenticeship"sv), true },
{ volume::P1V3, "chapter16.xhtml"sv, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Contract with Freida"sv), true },
{ volume::P1V3, "chapter17.xhtml"sv, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("To the Baptism Ceremony"sv), true },
{ volume::P1V3, "insert4.xhtml"sv, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter17_1.xhtml"sv, "Text/chapter17_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter18.xhtml"sv, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Quiet Commotion"sv), true },
{ volume::P1V3, "chapter19.xhtml"sv, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Gated Paradise"sv), true },
{ volume::P1V3, "chapter20.xhtml"sv, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Rejection and Persuasion"sv), true },
{ volume::P1V3, "insert5.xhtml"sv, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter20_1.xhtml"sv, "Text/chapter20_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter21.xhtml"sv, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Benno’s Lecture"sv), true },
{ volume::P1V3, "chapter22.xhtml"sv, "Text/chapter22.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Contract Magic and Registering the Workshop"sv), true },
{ volume::P1V3, "chapter23.xhtml"sv, "Text/chapter23.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Strategy Meeting and the Temple"sv), true },
{ volume::P1V3, "chapter24.xhtml"sv, "Text/chapter24.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Confrontation"sv), true },
{ volume::P1V3, "insert6.xhtml"sv, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "chapter24_1.xhtml"sv, "Text/chapter24_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "epilogue.xhtml"sv, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Epilogue"sv), true },
{ volume::P1V3, "extra5.xhtml"sv, "Text/extra5.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Life of an Apprentice Merchant"sv), true },
{ volume::P1V3, "insert9.xhtml"sv, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "extra5_1.xhtml"sv, "Text/extra5_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "extra1.xhtml"sv, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Tuuli — Visiting Corinna"sv), true },
{ volume::P1V3, "insert7.xhtml"sv, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "extra1_1.xhtml"sv, "Text/extra1_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "extra2.xhtml"sv, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Leise — Confectionery Recipe"sv), true },
{ volume::P1V3, "extra3.xhtml"sv, "Text/extra3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Benno — Pound Cake Sampling"sv), true },
{ volume::P1V3, "insert8.xhtml"sv, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "extra3_1.xhtml"sv, "Text/extra3_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "extra4.xhtml"sv, "Text/extra4.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Mark — The Master and I"sv), true },
{ volume::P1V3, "extra6.xhtml"sv, "Text/extra6.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Roots of the Guildmaster’s Worries"sv), true },
{ volume::P1V3, "afterword.xhtml"sv, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Afterword"sv), true },
{ volume::P1V3, "bonus1.xhtml"sv, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bonus High Resolution Illustrations"sv), true },
{ volume::P1V3, "bonus2.xhtml"sv, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "signup.xhtml"sv, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("About J-Novel Club"sv), true },
{ volume::P1V3, "copyright.xhtml"sv, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Copyright"sv), true },

    });

    constexpr std::array _part_1_list = std::to_array<std::span<const volume_definition>>({
        std::span(vol_1), std::span(vol_2), std::span(vol_3)
    });

    std::set<std::string> string_set;

    std::vector<volume_definition> _make_part_1()
    {
        std::vector<volume_definition> x;
        x.reserve(vol_1.size() + vol_2.size() + vol_3.size());
        auto view { std::ranges::views::filter(std::ranges::join_view(_part_1_list), utils::filter_chapter_stylesheet{}) };
        std::ranges::transform(view, std::back_inserter(x), utils::transform_unique_ids{string_set, "part1"});
        std::ranges::stable_sort(x);
        std::ranges::for_each(x, utils::foreach_label{}); // Must be done in order after sort
        return x;
    }

    const std::vector<volume_definition> _part_1 = _make_part_1();
} // namespace

namespace part_1
{
    using namespace std::string_view_literals;

    std::span<const volume_definition>
    get_vol_1()
    {
        return std::span<const volume_definition>(vol_1);
    }

    std::span<const volume_definition>
    get_vol_2()
    {
        return std::span<const volume_definition>(vol_2);
    }

    std::span<const volume_definition>
    get_vol_3()
    {
        return std::span<const volume_definition>(vol_3);
    }

    std::span<const volume_definition>
    get_part_1()
    {
        return std::span<const volume_definition>(_part_1);
    }
}
