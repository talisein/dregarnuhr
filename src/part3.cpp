#include <set>
#include <vector>
#include <algorithm>
#include "part3.h"

namespace
{
    using namespace std::string_view_literals;

    constexpr std::array vol_1 = std::to_array<const volume_definition>({
{ volume::P3V1, "ncx"sv, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P3V1, "stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P3V1, "jnovelclubCMYK.png"sv, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P3V1, "Bonus1.jpg"sv, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Bonus2.jpg"sv, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Characters1.jpg"sv, "Images/Characters1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Characters2.jpg"sv, "Images/Characters2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Cover.jpg"sv, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Extra1.jpg"sv, "Images/Extra1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "FrontMatter1.jpg"sv, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "FrontMatter2.jpg"sv, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert1.jpg"sv, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert2.jpg"sv, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert3.jpg"sv, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert4.jpg"sv, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert5.jpg"sv, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert6.jpg"sv, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert7.jpg"sv, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert8.jpg"sv, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Insert9.jpg"sv, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Manga1.jpg"sv, "Images/Manga1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Manga2.jpg"sv, "Images/Manga2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Extra2.jpg"sv, "Images/Extra2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "Map.jpg"sv, "Images/Map.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V1, "cover"sv, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Cover"sv), true },
{ volume::P3V1, "frontmatter1.xhtml"sv, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Color Illustrations"sv), true },
{ volume::P3V1, "frontmatter2.xhtml"sv, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "map.xhtml"sv, "Text/map.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Map of Ehrenfest Duchy"sv), true },
{ volume::P3V1, "characters1.xhtml"sv, "Text/characters1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Characters"sv), true },
{ volume::P3V1, "characters2.xhtml"sv, "Text/characters2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "toc.xhtml"sv, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "prologue.xhtml"sv, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Prologue"sv), true },
{ volume::P3V1, "chapter1.xhtml"sv, "Text/chapter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Examination Results and the Noble’s Quarter"sv), true },
{ volume::P3V1, "chapter2.xhtml"sv, "Text/chapter2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Preparing for the Baptism Ceremony"sv), true },
{ volume::P3V1, "insert1.xhtml"sv, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter2_1.xhtml"sv, "Text/chapter2_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "extra1.xhtml"sv, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Being My Little Sister’s Knight"sv), true },
{ volume::P3V1, "chapter3.xhtml"sv, "Text/chapter3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Noble’s Baptism Ceremony"sv), true },
{ volume::P3V1, "chapter4.xhtml"sv, "Text/chapter4.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Adoption"sv), true },
{ volume::P3V1, "insert2.xhtml"sv, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter4_1.xhtml"sv, "Text/chapter4_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter5.xhtml"sv, "Text/chapter5.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Inauguration Ceremony"sv), true },
{ volume::P3V1, "chapter6.xhtml"sv, "Text/chapter6.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Reunited at Last"sv), true },
{ volume::P3V1, "insert3.xhtml"sv, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter6_1.xhtml"sv, "Text/chapter6_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter7.xhtml"sv, "Text/chapter7.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("How to Make Fluffy Bread"sv), true },
{ volume::P3V1, "chapter8.xhtml"sv, "Text/chapter8.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Starbind Ceremony in the Lower City"sv), true },
{ volume::P3V1, "insert4.xhtml"sv, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter9.xhtml"sv, "Text/chapter9.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Archduke’s Castle"sv), true },
{ volume::P3V1, "chapter10.xhtml"sv, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Starbind Ceremony in the Noble’s Quarter"sv), true },
{ volume::P3V1, "chapter11.xhtml"sv, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Archduke and the Italian Restaurant"sv), true },
{ volume::P3V1, "chapter12.xhtml"sv, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Making a Monastery"sv), true },
{ volume::P3V1, "insert5.xhtml"sv, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter12_1.xhtml"sv, "Text/chapter12_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "extra2.xhtml"sv, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("One Stressed-Out Chef"sv), true },
{ volume::P3V1, "chapter13.xhtml"sv, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("How to Gather Donations"sv), true },
{ volume::P3V1, "insert6.xhtml"sv, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter13_1.xhtml"sv, "Text/chapter13_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter14.xhtml"sv, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("My First Magic Training Regimen"sv), true },
{ volume::P3V1, "chapter15.xhtml"sv, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Working Toward Wax Stencils"sv), true },
{ volume::P3V1, "chapter16.xhtml"sv, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("An Illustration of Ferdinand"sv), true },
{ volume::P3V1, "chapter17.xhtml"sv, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Johann and Zack"sv), true },
{ volume::P3V1, "insert7.xhtml"sv, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter17_1.xhtml"sv, "Text/chapter17_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter18.xhtml"sv, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Elvira and Lamprecht Attack"sv), true },
{ volume::P3V1, "chapter19.xhtml"sv, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Finishing My Highbeast and the Wax Stencils"sv), true },
{ volume::P3V1, "insert8.xhtml"sv, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter19_1.xhtml"sv, "Text/chapter19_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "chapter20.xhtml"sv, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Harspiel Concert"sv), true },
{ volume::P3V1, "epilogue.xhtml"sv, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Epilogue"sv), true },
{ volume::P3V1, "insert9.xhtml"sv, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "epilogue_1.xhtml"sv, "Text/epilogue_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB2, "side5.xhtml"sv, "Text/side5.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Tea Party with My Older Sister"sv), true },
{ volume::FB2, "fb2stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P3V1, "afterword.xhtml"sv, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Afterword"sv), true },
{ volume::P3V1, "manga1.xhtml"sv, "Text/manga1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Coming Out of Nowhere Again, It’s the End of Volume Bonuses! by You Shiina"sv), true },
{ volume::P3V1, "manga2.xhtml"sv, "Text/manga2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "poll1.xhtml"sv, "Text/poll1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The 1st Ascendance of a Bookworm Character Poll!"sv), true },
{ volume::P3V1, "poll2.xhtml"sv, "Text/poll2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "bonus1.xhtml"sv, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bonus High Resolution Illustrations"sv), true },
{ volume::P3V1, "bonus2.xhtml"sv, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V1, "signup.xhtml"sv, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("About J-Novel Club"sv), true },
{ volume::P3V1, "copyright.xhtml"sv, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Copyright"sv), true },
    });


    constexpr std::array vol_2 = std::to_array<const volume_definition>({
{ volume::P3V2, "ncx"sv, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P3V2, "stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P3V2, "jnovelclubCMYK.png"sv, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P3V2, "Bonus1.jpg"sv, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Bonus2.jpg"sv, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Characters1.jpg"sv, "Images/Characters1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Characters2.jpg"sv, "Images/Characters2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Cover.jpg"sv, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "FrontMatter1.jpg"sv, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "FrontMatter2.jpg"sv, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert1.jpg"sv, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert2.jpg"sv, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert3.jpg"sv, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert4.jpg"sv, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert5.jpg"sv, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert6.jpg"sv, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert7.jpg"sv, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert8.jpg"sv, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Insert9.jpg"sv, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Manga1.jpg"sv, "Images/Manga1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Manga2.jpg"sv, "Images/Manga2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Characters3.jpg"sv, "Images/Characters3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "Map.jpg"sv, "Images/Map.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V2, "cover"sv, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Cover"sv), true },
{ volume::P3V2, "frontmatter1.xhtml"sv, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Color Illustrations"sv), true },
{ volume::P3V2, "frontmatter2.xhtml"sv, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "map.xhtml"sv, "Text/map.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Map of Ehrenfest Duchy"sv), true },
{ volume::P3V2, "characters1.xhtml"sv, "Text/characters1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Characters"sv), true },
{ volume::P3V2, "characters2.xhtml"sv, "Text/characters2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "characters3.xhtml"sv, "Text/characters3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "toc.xhtml"sv, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "prologue.xhtml"sv, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Prologue"sv), true },
{ volume::P3V2, "insert1.xhtml"sv, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "prologue_1.xhtml"sv, "Text/prologue_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter1.xhtml"sv, "Text/chapter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Discussing the Harvest Festival"sv), true },
{ volume::P3V2, "chapter2.xhtml"sv, "Text/chapter2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Hasse’s Monastery"sv), true },
{ volume::P3V2, "chapter3.xhtml"sv, "Text/chapter3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The New Orphans"sv), true },
{ volume::P3V2, "insert2.xhtml"sv, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter3_1.xhtml"sv, "Text/chapter3_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter4.xhtml"sv, "Text/chapter4.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Orphan’s Treatment and Investigating the City"sv), true },
{ volume::P3V2, "chapter5.xhtml"sv, "Text/chapter5.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Monastery’s Barrier"sv), true },
{ volume::P3V2, "chapter6.xhtml"sv, "Text/chapter6.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A New Task and Winter Preparations"sv), true },
{ volume::P3V2, "insert3.xhtml"sv, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter6_1.xhtml"sv, "Text/chapter6_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter7.xhtml"sv, "Text/chapter7.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Opening the Italian Restaurant"sv), true },
{ volume::P3V2, "chapter8.xhtml"sv, "Text/chapter8.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Discussing How to Improve Hasse"sv), true },
{ volume::P3V2, "insert4.xhtml"sv, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter8_1.xhtml"sv, "Text/chapter8_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter9.xhtml"sv, "Text/chapter9.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Switching Places"sv), true },
{ volume::P3V2, "insert5.xhtml"sv, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter9_1.xhtml"sv, "Text/chapter9_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "extra1.xhtml"sv, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Wilfried’s Day as the High Bishop"sv), true },
{ volume::P3V2, "insert9.xhtml"sv, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "extra1_1.xhtml"sv, "Text/extra1_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "extra3.xhtml"sv, "Text/extra3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Justus’s Grand Lower City Infiltration"sv), true },
{ volume::P3V2, "chapter10.xhtml"sv, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Preparing for the Harvest Festival"sv), true },
{ volume::P3V2, "chapter11.xhtml"sv, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Hasse’s Contract"sv), true },
{ volume::P3V2, "chapter12.xhtml"sv, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Starting Merchant Activities"sv), true },
{ volume::P3V2, "chapter13.xhtml"sv, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Hasse’s Harvest Festival"sv), true },
{ volume::P3V2, "insert6.xhtml"sv, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter14.xhtml"sv, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Harvest Festival"sv), true },
{ volume::P3V2, "chapter15.xhtml"sv, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Night of Schutzaria"sv), true },
{ volume::P3V2, "insert7.xhtml"sv, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter15_1.xhtml"sv, "Text/chapter15_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter16.xhtml"sv, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Aftermath"sv), true },
{ volume::P3V2, "insert8.xhtml"sv, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "chapter16_1.xhtml"sv, "Text/chapter16_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "extra2.xhtml"sv, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Hasse’s Orphans"sv), true },
{ volume::P3V2, "chapter17.xhtml"sv, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("My Winter Preparations"sv), true },
{ volume::P3V2, "epilogue.xhtml"sv, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Epilogue"sv), true },
{ volume::P3V2, "afterword.xhtml"sv, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Afterword"sv), true },
{ volume::P3V2, "manga1.xhtml"sv, "Text/manga1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Comfy Life with My Family by You Shiina"sv), true },
{ volume::P3V2, "manga2.xhtml"sv, "Text/manga2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "bonus1.xhtml"sv, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bonus High Resolution Illustrations"sv), true },
{ volume::P3V2, "bonus2.xhtml"sv, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V2, "signup.xhtml"sv, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("About J-Novel Club"sv), true },
{ volume::P3V2, "copyright.xhtml"sv, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Copyright"sv), true },
        });


constexpr std::array vol_3 = std::to_array<const volume_definition>({
{ volume::P3V3, "ncx"sv, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P3V3, "stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P3V3, "jnovelclubCMYK.png"sv, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P3V3, "Bonus1.jpg"sv, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Bonus2.jpg"sv, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Characters1.jpg"sv, "Images/Characters1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Characters2.jpg"sv, "Images/Characters2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Cover.jpg"sv, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "FrontMatter1.jpg"sv, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "FrontMatter2.jpg"sv, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert1.jpg"sv, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert2.jpg"sv, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert3.jpg"sv, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert4.jpg"sv, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert5.jpg"sv, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert6.jpg"sv, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert7.jpg"sv, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert8.jpg"sv, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert9.jpg"sv, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Manga1.jpg"sv, "Images/Manga1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Manga2.jpg"sv, "Images/Manga2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Map.jpg"sv, "Images/Map.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Characters3.jpg"sv, "Images/Characters3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "Insert10.jpg"sv, "Images/Insert10.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V3, "cover"sv, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Cover"sv), true },
{ volume::P3V3, "frontmatter1.xhtml"sv, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Color Illustrations"sv), true },
{ volume::P3V3, "frontmatter2.xhtml"sv, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "map.xhtml"sv, "Text/map.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Map of Ehrenfest Duchy"sv), true },
{ volume::P3V3, "characters1.xhtml"sv, "Text/characters1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Characters"sv), true },
{ volume::P3V3, "characters2.xhtml"sv, "Text/characters2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "characters3.xhtml"sv, "Text/characters3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "toc.xhtml"sv, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "prologue.xhtml"sv, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Prologue"sv), true },
{ volume::P3V3, "chapter1.xhtml"sv, "Text/chapter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Ingo and Improving the Printing Press"sv), true },
{ volume::P3V3, "chapter2.xhtml"sv, "Text/chapter2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Gathering of the Gutenbergs"sv), true },
{ volume::P3V3, "insert1.xhtml"sv, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter2_1.xhtml"sv, "Text/chapter2_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "extra2.xhtml"sv, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The High Bishop’s Exclusive Business"sv), true },
{ volume::P3V3, "chapter3.xhtml"sv, "Text/chapter3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Start of Winter Socializing"sv), true },
{ volume::P3V3, "chapter4.xhtml"sv, "Text/chapter4.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Baptism Ceremony and Our Winter Debut"sv), true },
{ volume::P3V3, "insert2.xhtml"sv, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter4_1.xhtml"sv, "Text/chapter4_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter5.xhtml"sv, "Text/chapter5.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Class for Kids"sv), true },
{ volume::P3V3, "insert3.xhtml"sv, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter5_1.xhtml"sv, "Text/chapter5_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter6.xhtml"sv, "Text/chapter6.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Tea Party"sv), true },
{ volume::P3V3, "chapter7.xhtml"sv, "Text/chapter7.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Dedication Ritual (Take Two)"sv), true },
{ volume::P3V3, "extra1.xhtml"sv, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Winter Debut and the Playroom"sv), true },
{ volume::P3V3, "chapter8.xhtml"sv, "Text/chapter8.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Gathering the Winter Ingredient"sv), true },
{ volume::P3V3, "chapter9.xhtml"sv, "Text/chapter9.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Fighting the Schnesturm"sv), true },
{ volume::P3V3, "insert4.xhtml"sv, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter9_1.xhtml"sv, "Text/chapter9_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter10.xhtml"sv, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("To the End of Winter"sv), true },
{ volume::P3V3, "chapter11.xhtml"sv, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Selling the Materials"sv), true },
{ volume::P3V3, "chapter12.xhtml"sv, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Angelica and the Coming of Spring"sv), true },
{ volume::P3V3, "insert5.xhtml"sv, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter12_1.xhtml"sv, "Text/chapter12_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter13.xhtml"sv, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Upcoming Spring Prayer"sv), true },
{ volume::P3V3, "insert6.xhtml"sv, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter13_1.xhtml"sv, "Text/chapter13_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter14.xhtml"sv, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Hasse’s Punishment"sv), true },
{ volume::P3V3, "chapter15.xhtml"sv, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Door of Judgment"sv), true },
{ volume::P3V3, "chapter16.xhtml"sv, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Execution"sv), true },
{ volume::P3V3, "insert7.xhtml"sv, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter16_1.xhtml"sv, "Text/chapter16_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter17.xhtml"sv, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Spring Ingredient and Discussing Spring Prayer"sv), true },
{ volume::P3V3, "chapter18.xhtml"sv, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Goddesses’ Bath"sv), true },
{ volume::P3V3, "insert8.xhtml"sv, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter18_1.xhtml"sv, "Text/chapter18_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter19.xhtml"sv, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Night of Flutrane"sv), true },
{ volume::P3V3, "chapter20.xhtml"sv, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The End of Spring Prayer"sv), true },
{ volume::P3V3, "insert9.xhtml"sv, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "chapter20_1.xhtml"sv, "Text/chapter20_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "epilogue.xhtml"sv, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Epilogue"sv), true },
{ volume::P3V3, "afterword.xhtml"sv, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Afterword"sv), true },
{ volume::P3V3, "manga1.xhtml"sv, "Text/manga1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Comfy Life with My Family by You Shiina"sv), true },
{ volume::P3V3, "manga2.xhtml"sv, "Text/manga2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "insert10.xhtml"sv, "Text/insert10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Jureve Ingredients"sv), true },
{ volume::P3V3, "bonus1.xhtml"sv, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bonus High Resolution Illustrations"sv), true },
{ volume::P3V3, "bonus2.xhtml"sv, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V3, "signup.xhtml"sv, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("About J-Novel Club"sv), true },
{ volume::P3V3, "copyright.xhtml"sv, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Copyright"sv), true },

        });


constexpr std::array vol_4 = std::to_array<const volume_definition>({
{ volume::P3V4, "ncx"sv, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P3V4, "stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P3V4, "jnovelclubCMYK.png"sv, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P3V4, "Bonus1.jpg"sv, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Bonus2.jpg"sv, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Characters1.jpg"sv, "Images/Characters1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Characters2.jpg"sv, "Images/Characters2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Characters3.jpg"sv, "Images/Characters3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "FrontMatter1.jpg"sv, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "FrontMatter2.jpg"sv, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert1.jpg"sv, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert2.jpg"sv, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert3.jpg"sv, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert4.jpg"sv, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert5.jpg"sv, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert6.jpg"sv, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert7.jpg"sv, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert8.jpg"sv, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert9.jpg"sv, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Manga1.jpg"sv, "Images/Manga1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Manga2.jpg"sv, "Images/Manga2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Map.jpg"sv, "Images/Map.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Cover.jpg"sv, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "Insert10.jpg"sv, "Images/Insert10.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V4, "cover"sv, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Cover"sv), true },
{ volume::P3V4, "frontmatter1.xhtml"sv, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Color Illustrations"sv), true },
{ volume::P3V4, "frontmatter2.xhtml"sv, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "map.xhtml"sv, "Text/map.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Map of Ehrenfest Duchy"sv), true },
{ volume::P3V4, "characters1.xhtml"sv, "Text/characters1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Characters"sv), true },
{ volume::P3V4, "characters2.xhtml"sv, "Text/characters2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "characters3.xhtml"sv, "Text/characters3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "toc.xhtml"sv, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "prologue.xhtml"sv, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Prologue"sv), true },
{ volume::P3V4, "chapter1.xhtml"sv, "Text/chapter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A New Dress"sv), true },
{ volume::P3V4, "chapter2.xhtml"sv, "Text/chapter2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The New Printing Press’s Trial Run"sv), true },
{ volume::P3V4, "insert1.xhtml"sv, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter2_1.xhtml"sv, "Text/chapter2_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter3.xhtml"sv, "Text/chapter3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Benno’s Request"sv), true },
{ volume::P3V4, "chapter4.xhtml"sv, "Text/chapter4.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Staying Home During the Archduke Conference"sv), true },
{ volume::P3V4, "chapter5.xhtml"sv, "Text/chapter5.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Payment and Unveiling the Dress"sv), true },
{ volume::P3V4, "chapter6.xhtml"sv, "Text/chapter6.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Angelica’s Manablade"sv), true },
{ volume::P3V4, "insert2.xhtml"sv, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter6_1.xhtml"sv, "Text/chapter6_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter7.xhtml"sv, "Text/chapter7.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Let’s Print More Stuff"sv), true },
{ volume::P3V4, "chapter8.xhtml"sv, "Text/chapter8.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Meeting with the Plantin Company"sv), true },
{ volume::P3V4, "insert3.xhtml"sv, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter8_1.xhtml"sv, "Text/chapter8_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter9.xhtml"sv, "Text/chapter9.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Archducal Couple Returns"sv), true },
{ volume::P3V4, "chapter10.xhtml"sv, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Dress Debut and Ferdinand’s Return"sv), true },
{ volume::P3V4, "insert4.xhtml"sv, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter10_1.xhtml"sv, "Text/chapter10_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter11.xhtml"sv, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Lunch Meeting and Business Day"sv), true },
{ volume::P3V4, "chapter12.xhtml"sv, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Heading to Illgner"sv), true },
{ volume::P3V4, "chapter13.xhtml"sv, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Illgner’s Brigitte"sv), true },
{ volume::P3V4, "insert5.xhtml"sv, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter13_1.xhtml"sv, "Text/chapter13_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter21.xhtml"sv, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Damuel’s Proposal"sv), true },
{ volume::P3V4, "insert9.xhtml"sv, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter21_1.xhtml"sv, "Text/chapter21_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter14.xhtml"sv, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Mount Lohenberg"sv), true },
{ volume::P3V4, "chapter15.xhtml"sv, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Riesefalke Egg"sv), true },
{ volume::P3V4, "insert6.xhtml"sv, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter15_1.xhtml"sv, "Text/chapter15_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter16.xhtml"sv, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Hand Pumps"sv), true },
{ volume::P3V4, "insert7.xhtml"sv, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter16_1.xhtml"sv, "Text/chapter16_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter17.xhtml"sv, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Georgine’s Visit"sv), true },
{ volume::P3V4, "insert8.xhtml"sv, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter17_1.xhtml"sv, "Text/chapter17_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "chapter18.xhtml"sv, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Dirk’s Mana and Submission Contract"sv), true },
{ volume::P3V4, "chapter22.xhtml"sv, "Text/chapter22.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Staying in Illgner"sv), true },
{ volume::P3V4, "chapter19.xhtml"sv, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Georgine Departs"sv), true },
{ volume::P3V4, "chapter20.xhtml"sv, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Tea Party"sv), true },
{ volume::P3V4, "epilogue.xhtml"sv, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Epilogue"sv), true },
{ volume::P3V4, "afterword.xhtml"sv, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Afterword"sv), true },
{ volume::P3V4, "manga1.xhtml"sv, "Text/manga1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Comfy Life with My Family by You Shiina"sv), true },
{ volume::P3V4, "manga2.xhtml"sv, "Text/manga2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "insert10.xhtml"sv, "Text/insert10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Jureve Ingredients"sv), true },
{ volume::P3V4, "bonus1.xhtml"sv, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bonus High Resolution Illustrations"sv), true },
{ volume::P3V4, "bonus2.xhtml"sv, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V4, "signup.xhtml"sv, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("About J-Novel Club"sv), true },
{ volume::P3V4, "copyright.xhtml"sv, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Copyright"sv), true },
        });

constexpr std::array vol_5 = std::to_array<const volume_definition>({
{ volume::P3V5, "ncx"sv, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P3V5, "stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P3V5, "jnovelclubCMYK.png"sv, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P3V5, "Map.jpg"sv, "Images/Map.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Bonus1.jpg"sv, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Bonus2.jpg"sv, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Characters1.jpg"sv, "Images/Characters1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Characters2.jpg"sv, "Images/Characters2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Characters3.jpg"sv, "Images/Characters3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Cover.jpg"sv, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "FrontMatter1.jpg"sv, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "FrontMatter2.jpg"sv, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert1.jpg"sv, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert2.jpg"sv, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert3.jpg"sv, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert4.jpg"sv, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert5.jpg"sv, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert6.jpg"sv, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert7.jpg"sv, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert8.jpg"sv, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert9.jpg"sv, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Insert10.jpg"sv, "Images/Insert10.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Manga2.jpg"sv, "Images/Manga2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "Manga1.jpg"sv, "Images/Manga1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P3V5, "cover"sv, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Cover"sv), true },
{ volume::P3V5, "frontmatter1.xhtml"sv, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Color Illustrations"sv), true },
{ volume::P3V5, "frontmatter2.xhtml"sv, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "map.xhtml"sv, "Text/map.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Map of Ehrenfest Duchy"sv), true },
{ volume::P3V5, "characters1.xhtml"sv, "Text/characters1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Characters"sv), true },
{ volume::P3V5, "characters2.xhtml"sv, "Text/characters2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "characters3.xhtml"sv, "Text/characters3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "toc.xhtml"sv, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "prologue.xhtml"sv, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Prologue"sv), true },
{ volume::P3V5, "insert1.xhtml"sv, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "prologue_1.xhtml"sv, "Text/prologue_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter1.xhtml"sv, "Text/chapter1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The New Orphans and Operation Grimm"sv), true },
{ volume::P3V5, "chapter2.xhtml"sv, "Text/chapter2.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Hasse and the Gray Priests"sv), true },
{ volume::P3V5, "chapter3.xhtml"sv, "Text/chapter3.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Ruelle Gathering (Take Two)"sv), true },
{ volume::P3V5, "chapter4.xhtml"sv, "Text/chapter4.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Damuel’s Growth"sv), true },
{ volume::P3V5, "insert2.xhtml"sv, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter4_1.xhtml"sv, "Text/chapter4_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter5.xhtml"sv, "Text/chapter5.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Conditions for Compressing Mana"sv), true },
{ volume::P3V5, "chapter6.xhtml"sv, "Text/chapter6.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Illgner’s Harvest Festival"sv), true },
{ volume::P3V5, "chapter7.xhtml"sv, "Text/chapter7.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("My First-Ever Little Sister"sv), true },
{ volume::P3V5, "insert3.xhtml"sv, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter7_1.xhtml"sv, "Text/chapter7_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter8.xhtml"sv, "Text/chapter8.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Wilfried’s Actions"sv), true },
{ volume::P3V5, "insert4.xhtml"sv, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter8_1.xhtml"sv, "Text/chapter8_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter9.xhtml"sv, "Text/chapter9.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Wilfried’s Punishment"sv), true },
{ volume::FB2, "side6.xhtml"sv, "Text/side6.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Feelings of Panic"sv), true },
{ volume::FB2, "fb2stylesheet.css"sv, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P3V5, "chapter10.xhtml"sv, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Jureve and Mana Compression"sv), true },
{ volume::P3V5, "chapter11.xhtml"sv, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Charlotte’s Baptism Ceremony"sv), true },
{ volume::P3V5, "chapter12.xhtml"sv, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Kidnapped Daughter"sv), true },
{ volume::P3V5, "chapter13.xhtml"sv, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Rescue"sv), true },
{ volume::P3V5, "insert5.xhtml"sv, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter13_1.xhtml"sv, "Text/chapter13_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter14.xhtml"sv, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("And So, the Future"sv), true },
{ volume::P3V5, "chapter15.xhtml"sv, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Grandfather on the Day of the Baptism"sv), true },
{ volume::P3V5, "chapter16.xhtml"sv, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("In Place of My Older Sister"sv), true },
{ volume::P3V5, "insert6.xhtml"sv, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter16_1.xhtml"sv, "Text/chapter16_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter17.xhtml"sv, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Two Marriages"sv), true },
{ volume::P3V5, "chapter20.xhtml"sv, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("The Laynoble Guard Knight"sv), true },
{ volume::P3V5, "chapter21.xhtml"sv, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("One Handful of a Chef"sv), true },
{ volume::P3V5, "chapter18.xhtml"sv, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("No Rest for Us"sv), true },
{ volume::P3V5, "insert7.xhtml"sv, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter18_1.xhtml"sv, "Text/chapter18_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "insert8.xhtml"sv, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter18_2.xhtml"sv, "Text/chapter18_2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter19.xhtml"sv, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Meanwhile at the Temple"sv), true },
{ volume::P3V5, "insert9.xhtml"sv, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "chapter19_1.xhtml"sv, "Text/chapter19_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "epilogue.xhtml"sv, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Epilogue"sv), true },
{ volume::P3V5, "afterword.xhtml"sv, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Afterword"sv), true },
{ volume::P3V5, "manga1.xhtml"sv, "Text/manga1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("A Comfy Life with My Family by You Shiina"sv), true },
{ volume::P3V5, "manga2.xhtml"sv, "Text/manga2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "insert10.xhtml"sv, "Text/insert10.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Jureve Ingredients"sv), true },
{ volume::P3V5, "bonus1.xhtml"sv, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Bonus High Resolution Illustrations"sv), true },
{ volume::P3V5, "bonus2.xhtml"sv, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P3V5, "signup.xhtml"sv, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("About J-Novel Club"sv), true },
{ volume::P3V5, "copyright.xhtml"sv, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, std::make_optional<std::string_view>("Copyright"sv), true },
        });

    constexpr std::array _part_3_list = std::to_array<std::span<const volume_definition>>({
        std::span(vol_1), std::span(vol_2), std::span(vol_3), std::span(vol_4), std::span(vol_5)
    });

    auto _part_3_view { std::ranges::views::filter(std::ranges::join_view(_part_3_list), [chap_set = std::set<chapter_type>{}, style_set = std::set<volume>{}](const volume_definition& def) mutable {
    if (get_uniqueness(def.get_chapter_type()) == chapter_uniqueness::SINGLE ) {
        if (chap_set.contains(def.get_chapter_type())) {
            return false;
        } else {
            chap_set.insert(def.get_chapter_type());
            return true;
        }
    } else {
        if (def.get_chapter_type() == STYLESHEET) {
            if (style_set.contains(def.vol)) {
                return false;
            } else {
                style_set.insert(def.vol);
                return true;
            }
        } else {
            return true;
        }
    }
}) };

    std::vector<volume_definition> _make_part_3()
    {
        std::vector<volume_definition> x;
        x.reserve(vol_1.size() + vol_2.size() + vol_3.size() + vol_4.size() + vol_5.size());
        std::ranges::copy(std::begin(_part_3_view), std::end(_part_3_view), std::back_inserter(x));
        std::ranges::stable_sort(x);
        std::optional<std::string_view> prev_label = std::nullopt;
        for (auto &def : x) {
            if (!prev_label) {
                prev_label = def.toc_label;
                continue;
            } else {
                if (def.toc_label) {
                    if (prev_label.value() == def.toc_label.value()) {
                        def.toc_label = std::nullopt;
                    } else {
                        prev_label = def.toc_label;
                    }
                }
            }
        }
       return x;
    }

    const std::vector<volume_definition> _part_3 = _make_part_3();
}


namespace part_3
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
    get_vol_4()
    {
        return std::span<const volume_definition>(vol_4);
    }

    std::span<const volume_definition>
    get_vol_5()
    {
        return std::span<const volume_definition>(vol_5);
    }


    std::span<const volume_definition>
    get_part_3()
    {
        return std::span<const volume_definition>(_part_3);
    }
}
