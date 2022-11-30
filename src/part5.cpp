#include <ranges>
#include <algorithm>
#include <vector>
#include <array>
#include <string_view>
#include <optional>
#include "part4.h"
#include "utils.h"

namespace
{
    using namespace std::string_view_literals;

    constexpr std::array vol_1 = std::to_array<volume_definition>({
{ volume::P5V1, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P5V1, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P5V1, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P5V1, "Images/Poll2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Poll1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Map2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Map.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Manga2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Manga1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert10.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Characters3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Characters2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Characters1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P5V1, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, "Cover"sv, true },
{ volume::P5V1, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, "Color Illustrations"sv, true },
{ volume::P5V1, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/characters1.xhtml"sv, "application/xhtml+xml"sv, "Characters"sv, true },
{ volume::P5V1, "Text/characters2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/characters3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, "Prologue"sv, true },
{ volume::P5V1, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/prologue_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter1.xhtml"sv, "application/xhtml+xml"sv, "The Children of the Former Veronica Faction"sv, true },
{ volume::P5V1, "Text/chapter2.xhtml"sv, "application/xhtml+xml"sv, "The Fellowship Gatherings (Third Year)"sv, true },
{ volume::P5V1, "Text/chapter3.xhtml"sv, "application/xhtml+xml"sv, "Passing Classes"sv, true },
{ volume::P5V1, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter3_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter4.xhtml"sv, "application/xhtml+xml"sv, "The New Librarian"sv, true },
{ volume::P5V1, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter4_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },

{ volume::P5V1, "Text/chapter23.xhtml"sv, "application/xhtml+xml"sv, "Finding Purpose and the Guardians of Knowledge"sv, true },

{ volume::P5V1, "Text/chapter5.xhtml"sv, "application/xhtml+xml"sv, "Practical: Divine Protections of the Gods"sv, true },
{ volume::P5V1, "Text/chapter6.xhtml"sv, "application/xhtml+xml"sv, "Music and Everyone’s Rituals"sv, true },
{ volume::P5V1, "Text/chapter7.xhtml"sv, "application/xhtml+xml"sv, "Discussing Divine Protections with Hirschur"sv, true },
{ volume::P5V1, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter7_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter8.xhtml"sv, "application/xhtml+xml"sv, "Beginning the Archduke Candidate Course"sv, true },
{ volume::P5V1, "Text/chapter9.xhtml"sv, "application/xhtml+xml"sv, "Dedication Whirling (Third Year)"sv, true },
{ volume::P5V1, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, "Hirschur’s Meeting with the Aub"sv, true },
{ volume::P5V1, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter10_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, "Researching Rituals and an Update on the Purge"sv, true },
{ volume::P5V1, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, "Finishing the Archduke Candidate Lessons"sv, true },
{ volume::P5V1, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, "Passing Professor Gundolf’s Classes"sv, true },
{ volume::P5V1, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, "Gretia’s Circumstances and Gathering Ingredients"sv, true },
{ volume::P5V1, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter14_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, "Professor Fraularm’s Class"sv, true },
{ volume::P5V1, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter15_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, "Hirschur’s Personal Librarian"sv, true },
{ volume::P5V1, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, "A Request from Royalty"sv, true },
{ volume::P5V1, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter17_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, "Tea Party for Bookworms"sv, true },
{ volume::P5V1, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, "Tea Party with Dunkelfelger"sv, true },
{ volume::P5V1, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter19_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, "Replies"sv, true },
{ volume::P5V1, "Text/insert10.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/chapter20_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },

{ volume::P5V1, "Text/chapter22.xhtml"sv, "application/xhtml+xml"sv, "Fantasy versus Reality"sv, true },

{ volume::P5V1, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, "Epilogue"sv, true },
{ volume::P5V1, "Text/map.xhtml"sv, "application/xhtml+xml"sv, "Map of Ehrenfest Duchy"sv, true },
{ volume::P5V1, "Text/map2.xhtml"sv, "application/xhtml+xml"sv, "Map of Yurgenschmidt"sv, true },
{ volume::P5V1, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, "Afterword"sv, true },
{ volume::P5V1, "Text/manga1.xhtml"sv, "application/xhtml+xml"sv, "A Comfy Life with My Family by You Shiina"sv, true },
{ volume::P5V1, "Text/manga2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/poll1.xhtml"sv, "application/xhtml+xml"sv, "The 3rd Ascendance of a Bookworm Character Poll!"sv, true },
{ volume::P5V1, "Text/poll2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, "Bonus High Resolution Illustrations"sv, true },
{ volume::P5V1, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P5V1, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, "About J-Novel Club"sv, true },
{ volume::P5V1, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, "Copyright"sv, true },
        });

} // namespace

constexpr auto _part_5_list{std::to_array<std::span<const volume_definition>>({
    std::span(vol_1),
})};

consteval auto _get_part5_view()
{
    constexpr std::ranges::join_view l{_part_5_list};
    constexpr auto sz = std::distance(l.begin(), l.end());
    std::vector<volume_definition> v(l.begin(),
                                     l.end());// { r.begin(), r.end() };
    return utils::vec_to_arr<sz>(v);
}

constexpr std::array _part_4_view = _get_part5_view();

const std::vector<volume_definition> _part_5 = utils::make_omnibus_def(_part_4_view, utils::calc_reservation(_part_5_list));


namespace part_5
{
    using namespace std::string_view_literals;

    std::span<const volume_definition>
    get_vol_1()
    {
        return vol_1;
    }

    std::span<const volume_definition>
    get_part_5()
    {
        return std::span<const volume_definition>(_part_5);
    }

    size_t
    get_reservation()
    {
        return _part_5.size();
    }

} // namespace part_5
