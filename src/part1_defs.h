#pragma once

#include "volumes.h"

namespace part_1_defs
{
    using namespace std::string_view_literals;

        constexpr std::array vol_1 = std::to_array<volume_definition>({
{ volume::P1V1, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, "Cover"sv, true },
{ volume::P1V1, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::FB2, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::FB1, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V1, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P1V1, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/FrontMatter3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "Images/37.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "Images/38.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "Images/39.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "Images/40.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "Images/41.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "Images/42.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "Images/43.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB1, "Images/44.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::FB2, "Images/Image2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P1V1, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V1, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, "Color Illustrations"sv, true },
{ volume::P1V1, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/frontmatter3.xhtml"sv, "application/xhtml+xml"sv, "Map of Ehrenfest"sv, true },
{ volume::P1V1, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, "Prologue"sv, true },
{ volume::P1V1, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, "Unchanging Daily Life"sv, true },
{ volume::P1V1, "Text/chapter01.xhtml"sv, "application/xhtml+xml"sv, "A New Life"sv, true },
{ volume::P1V1, "Text/chapter02.xhtml"sv, "application/xhtml+xml"sv, "Exploring My Home"sv, true },
{ volume::P1V1, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter021.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter03.xhtml"sv, "application/xhtml+xml"sv, "Exploring the City"sv, true },
{ volume::P1V1, "Text/chapter04.xhtml"sv, "application/xhtml+xml"sv, "Books: Unobtainable"sv, true },
{ volume::P1V1, "Text/chapter05.xhtml"sv, "application/xhtml+xml"sv, "Lifestyle Overhaul"sv, true },
//{ volume::MP1V1, "page-150"sv, "xhtml/p-150.xhtml"sv, "application/xhtml+xml"sv, "Extra: Myne's Gotten Weird"sv, true },
{ volume::MP1V1, "xhtml/p-151.xhtml"sv, "application/xhtml+xml"sv, "Extra: Myne's Gotten Weird"sv, true },
{ volume::MP1V1, "xhtml/p-152.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V1, "xhtml/p-153.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V1, "xhtml/p-154.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V1, "xhtml/p-155.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V1, "page-img-150"sv, "image/i-150.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V1, "image/i-151.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V1, "image/i-152.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V1, "image/i-153.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V1, "image/i-154.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V1, "image/i-155.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V1, "style/fixed-layout.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V1, "Text/chapter06.xhtml"sv, "application/xhtml+xml"sv, "Neighborhood Boys"sv, true },
{ volume::P1V1, "Text/chapter07.xhtml"sv, "application/xhtml+xml"sv, "Paper: Unobtainable"sv, true },
{ volume::P1V1, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter071.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter08.xhtml"sv, "application/xhtml+xml"sv, "Learning to Respect Egyptian Culture"sv, true },
{ volume::P1V1, "Text/chapter09.xhtml"sv, "application/xhtml+xml"sv, "Winter Preparations"sv, true },
{ volume::P1V1, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, "Stone Slate Get!"sv, true },
{ volume::P1V1, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter101.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, "Beaten by Ancient Egyptians"sv, true },
{ volume::P1V1, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, "The Sweet Taste of Winter"sv, true },
{ volume::P1V1, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter121.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, "Helping Out Otto"sv, true },
{ volume::MP1V2, "xhtml/p-167.xhtml"sv, "application/xhtml+xml"sv, "Extra: Lutz and the Parue Tree"sv, true },
{ volume::MP1V2, "xhtml/p-168.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V2, "xhtml/p-169.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V2, "xhtml/p-170.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V2, "xhtml/p-171.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V2, "xhtml/p-172.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V2, "page-173"sv, "xhtml/p-173.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V2, "image/i-167.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V2, "image/i-168.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V2, "image/i-169.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V2, "image/i-170.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V2, "image/i-171.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V2, "image/i-172.jpg"sv, "image/jpeg"sv, std::nullopt, false },
//{ volume::MP1V2, "page-img-173"sv, "image/i-173.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V2, "style/fixed-layout.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V1, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, "Tuuli’s Hair Ornament"sv, true },
{ volume::P1V1, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter141.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB2, "Text/side1.xhtml"sv, "application/xhtml+xml"sv, "One Winter Day"sv, true },
{ volume::FB2, "Text/2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, "Bring Me to The Forest"sv, true },
{ volume::P1V1, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, "Long Live Mesopotamian Culture"sv, true },
{ volume::P1V1, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter161.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "Text/37.xhtml"sv, "application/xhtml+xml"sv, "My Daughter’s about to Be a Criminal?! by Suzuka"sv, true },
{ volume::FB1, "Text/38.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "Text/39.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "Text/40.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "Text/41.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "Text/42.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "Text/43.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::FB1, "Text/44.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, "Clay Tablets Won’t Work"sv, true },
{ volume::P1V1, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, "Tuuli’s Baptism"sv, true },
//{ volume::MP1V3, "page-183"sv, "xhtml/p-183.xhtml"sv, "application/xhtml+xml"sv, "Story: The Baptism Ceremony and the Hairpin"sv, true },
{ volume::MP1V3, "xhtml/p-184.xhtml"sv, "application/xhtml+xml"sv, "Story: The Baptism Ceremony and the Hairpin"sv, true },
{ volume::MP1V3, "xhtml/p-185.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V3, "xhtml/p-186.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V3, "xhtml/p-187.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V3, "xhtml/p-188.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V3, "xhtml/p-189.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V3, "xhtml/p-190.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V3, "page-img-183"sv, "image/i-183.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V3, "image/i-184.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V3, "image/i-185.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V3, "image/i-186.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V3, "image/i-187.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V3, "image/i-188.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V3, "image/i-189.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V3, "image/i-190.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V3, "style/fixed-layout.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V1, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, "I Love You, Yellow River Culture"sv, true },
{ volume::P1V1, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter191.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, "I Want Ink"sv, true },
{ volume::P1V1, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, "Intense Battle with Food"sv, true },
{ volume::P1V1, "Text/chapter22.xhtml"sv, "application/xhtml+xml"sv, "Mokkan and A Mysterious Fever"sv, true },
{ volume::P1V1, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, "A Day Without Myne"sv, true },
{ volume::P1V1, "Text/chapter23.xhtml"sv, "application/xhtml+xml"sv, "The Road to a Meeting"sv, true },
{ volume::P1V1, "Text/chapter24.xhtml"sv, "application/xhtml+xml"sv, "Meeting with the Merchant"sv, true },
{ volume::P1V1, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/chapter241.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, "Epilogue"sv, true },
{ volume::P1V1, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/epilogue1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, "Afterword"sv, true },
{ volume::P1V1, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, "Bonus High-Resolution Illustrations"sv, true },
{ volume::P1V1, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V1, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, "About J-Novel Club"sv, true },
{ volume::P1V1, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, "Copyright"sv, true },
        });

    constexpr std::array vol_2 = std::to_array<volume_definition>({
{ volume::P1V2, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P1V2, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V2, "Images/FrontMatter3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P1V2, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V2, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, "Cover"sv, true },
{ volume::P1V2, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, "Color Illustrations"sv, true },
{ volume::P1V2, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/frontmatter3.xhtml"sv, "application/xhtml+xml"sv, "Map of Ehrenfest"sv, true },
{ volume::P1V2, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, "Prologue"sv, true },
{ volume::P1V2, "Text/chapter01.xhtml"sv, "application/xhtml+xml"sv, "The Road to Washi"sv, true },
{ volume::P1V2, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter01_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter02.xhtml"sv, "application/xhtml+xml"sv, "Invitation to the Otto Residence"sv, true },
{ volume::P1V2, "Text/chapter03.xhtml"sv, "application/xhtml+xml"sv, "Summons from Benno"sv, true },
{ volume::P1V2, "Text/chapter04.xhtml"sv, "application/xhtml+xml"sv, "Contract Magic"sv, true },
{ volume::P1V2, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter04_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter05.xhtml"sv, "application/xhtml+xml"sv, "Lutz’s Most Important Job"sv, true },
{ volume::P1V2, "Text/chapter06.xhtml"sv, "application/xhtml+xml"sv, "Ordering Tools and Materials"sv, true },
//{ volume::MP1V4, "xhtml/p-165.xhtml"sv, "application/xhtml+xml"sv, "Story: Zeg the Apprentice Craftsman"sv, true },
{ volume::MP1V4, "xhtml/p-166.xhtml"sv, "application/xhtml+xml"sv, "Story: Zeg the Apprentice Craftsman"sv, true },
{ volume::MP1V4, "xhtml/p-167.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V4, "xhtml/p-168.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V4, "xhtml/p-169.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V4, "xhtml/p-170.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V4, "xhtml/p-171.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V4, "xhtml/p-172.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V4, "xhtml/p-173.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V4, "image/i-165.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "image/i-166.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "image/i-167.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "image/i-168.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "image/i-169.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "image/i-170.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "image/i-171.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "image/i-172.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "image/i-173.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V4, "style/fixed-layout.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V2, "Text/chapter07.xhtml"sv, "application/xhtml+xml"sv, "Starting to Make Paper"sv, true },
{ volume::P1V2, "Text/chapter08.xhtml"sv, "application/xhtml+xml"sv, "Painful Mistakes"sv, true },
{ volume::P1V2, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter08_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter09.xhtml"sv, "application/xhtml+xml"sv, "Lutz’s Myne"sv, true },
{ volume::P1V2, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, "Completed Paper"sv, true },
{ volume::P1V2, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, "The Merchants’ Guild"sv, true },
{ volume::P1V2, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, "The Guildmaster and the Hair Ornament"sv, true },
{ volume::P1V2, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter12_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, "The Guildmaster’s Granddaughter"sv, true },
{ volume::P1V2, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, "Freida’s Hair Ornaments"sv, true },
{ volume::P1V2, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, "Delivering the Hair Ornaments"sv, true },
{ volume::P1V2, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter15_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, "Winter Handiwork"sv, true },
{ volume::P1V2, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, "Lutz’s Training Curriculum"sv, true },
{ volume::P1V2, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, "Why It Failed and How to Fix It"sv, true },
//{ volume::MP1V5, "xhtml/p-175.xhtml"sv, "application/xhtml+xml"sv, "Story: Hairpins and the Meeting of Stores"sv, true },
{ volume::MP1V5, "xhtml/p-176.xhtml"sv, "application/xhtml+xml"sv, "Story: Hairpins and the Meeting of Stores"sv, true },
{ volume::MP1V5, "xhtml/p-177.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V5, "xhtml/p-178.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V5, "xhtml/p-179.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V5, "xhtml/p-180.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V5, "xhtml/p-181.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V5, "xhtml/p-182.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V5, "xhtml/p-183.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V5, "xhtml/p-184.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V5, "xhtml/p-185.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V5, "image/i-175.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-176.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-177.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-178.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-179.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-180.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-181.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-182.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-183.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-184.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "image/i-185.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V5, "style/fixed-layout.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V2, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, "A Trombe Appears"sv, true },
{ volume::P1V2, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, "We Went Ahead and Made Some"sv, true },
{ volume::P1V2, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/chapter20_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, "Gossiping by the Well"sv, true },
{ volume::P1V2, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, "Myne Collapses"sv, true },
{ volume::P1V2, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, "Corinna’s Married Life"sv, true },
{ volume::P1V2, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, "Epilogue"sv, true },
{ volume::P1V2, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, "Afterword"sv, true },
{ volume::P1V2, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, "Bonus High Resolution Illustrations"sv, true },
{ volume::P1V2, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V2, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, "About J-Novel Club"sv, true },
{ volume::P1V2, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, "Copyright"sv, true },
        });

constexpr std::array vol_3 = std::to_array<volume_definition>({
{ volume::P1V3, "toc.ncx"sv, "application/x-dtbncx+xml"sv, std::nullopt, false },
{ volume::P1V3, "Styles/stylesheet.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V3, "Images/Cover.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/FrontMatter1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/FrontMatter2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/FrontMatter3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert3.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert4.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert5.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert6.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert7.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert8.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Insert9.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/Bonus1.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Images/jnovelclubCMYK.png"sv, "image/png"sv, std::nullopt, false },
{ volume::P1V3, "Images/Bonus2.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Text/cover.xhtml"sv, "application/xhtml+xml"sv, "Cover"sv, true },
{ volume::P1V3, "Text/frontmatter1.xhtml"sv, "application/xhtml+xml"sv, "Color Illustrations"sv, true },
{ volume::P1V3, "Text/frontmatter2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/frontmatter3.xhtml"sv, "application/xhtml+xml"sv, "Map of Ehrenfest"sv, true },
{ volume::P1V3, "Text/toc.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/prologue.xhtml"sv, "application/xhtml+xml"sv, "Prologue"sv, true },
{ volume::P1V3, "Text/chapter01.xhtml"sv, "application/xhtml+xml"sv, "Discussing the Devouring with Freida"sv, true },
{ volume::P1V3, "Text/chapter02.xhtml"sv, "application/xhtml+xml"sv, "Making Cake with Freida"sv, true },
{ volume::P1V3, "Text/insert1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter02_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter03.xhtml"sv, "application/xhtml+xml"sv, "Taking a Bath with Freida"sv, true },
{ volume::P1V3, "Text/chapter04.xhtml"sv, "application/xhtml+xml"sv, "Freida’s Baptism"sv, true },
{ volume::P1V3, "Text/chapter05.xhtml"sv, "application/xhtml+xml"sv, "Winter Begins"sv, true },
{ volume::P1V3, "Text/chapter06.xhtml"sv, "application/xhtml+xml"sv, "Completion of My Outfit and Hairpin"sv, true },
{ volume::P1V3, "Text/chapter07.xhtml"sv, "application/xhtml+xml"sv, "Lutz’s Tutor"sv, true },
{ volume::P1V3, "Text/chapter08.xhtml"sv, "application/xhtml+xml"sv, "Otto’s Counseling Room"sv, true },
{ volume::P1V3, "Text/chapter09.xhtml"sv, "application/xhtml+xml"sv, "Family Meeting"sv, true },
{ volume::P1V3, "Text/insert2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter09_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter10.xhtml"sv, "application/xhtml+xml"sv, "Report to Lutz"sv, true },
{ volume::P1V3, "Text/chapter11.xhtml"sv, "application/xhtml+xml"sv, "Back to Paper-Making"sv, true },
{ volume::P1V3, "Text/chapter12.xhtml"sv, "application/xhtml+xml"sv, "Vested Interests"sv, true },
{ volume::P1V3, "Text/chapter13.xhtml"sv, "application/xhtml+xml"sv, "Result of the Meeting with the Vested Interests"sv, true },
{ volume::P1V3, "Text/insert3.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter13_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter14.xhtml"sv, "application/xhtml+xml"sv, "Tools and Selecting a Workshop"sv, true },
{ volume::MP1V6, "xhtml/p-171.xhtml"sv, "application/xhtml+xml"sv, "Extra: Some Time in the Woods"sv, true },
{ volume::MP1V6, "xhtml/p-172.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-173.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-174.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-175.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-176.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-177.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-178.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V6, "xhtml/p-179.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-180.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "image/i-171.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-172.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-173.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-174.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-175.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-176.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-177.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-178.jpg"sv, "image/jpeg"sv, std::nullopt, false },
//{ volume::MP1V6, "image/i-179.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-180.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "style/fixed-layout.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V3, "Text/chapter15.xhtml"sv, "application/xhtml+xml"sv, "Preparing for Lutz’s Apprenticeship"sv, true },
//{ volume::MP1V6, "xhtml/p-181.xhtml"sv, "application/xhtml+xml"sv, "Story: A Long-Awaited Reunion"sv, true },
{ volume::MP1V6, "xhtml/p-182.xhtml"sv, "application/xhtml+xml"sv, "Story: A Long-Awaited Reunion"sv, true },
{ volume::MP1V6, "xhtml/p-183.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-184.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-185.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-186.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-187.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-188.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V6, "xhtml/p-189.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V6, "image/i-181.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-182.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-183.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-184.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-185.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-186.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-187.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-188.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V6, "image/i-189.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::P1V3, "Text/chapter16.xhtml"sv, "application/xhtml+xml"sv, "Contract with Freida"sv, true },
{ volume::P1V3, "Text/chapter17.xhtml"sv, "application/xhtml+xml"sv, "To the Baptism Ceremony"sv, true },
{ volume::P1V3, "Text/insert4.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter17_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter18.xhtml"sv, "application/xhtml+xml"sv, "A Quiet Commotion"sv, true },
{ volume::P1V3, "Text/chapter19.xhtml"sv, "application/xhtml+xml"sv, "Gated Paradise"sv, true },
{ volume::P1V3, "Text/chapter20.xhtml"sv, "application/xhtml+xml"sv, "Rejection and Persuasion"sv, true },
{ volume::P1V3, "Text/insert5.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter20_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter21.xhtml"sv, "application/xhtml+xml"sv, "Benno’s Lecture"sv, true },
{ volume::P1V3, "Text/chapter22.xhtml"sv, "application/xhtml+xml"sv, "Contract Magic and Registering the Workshop"sv, true },
{ volume::P1V3, "Text/chapter23.xhtml"sv, "application/xhtml+xml"sv, "Strategy Meeting and the Temple"sv, true },
{ volume::P1V3, "Text/chapter24.xhtml"sv, "application/xhtml+xml"sv, "Confrontation"sv, true },
{ volume::P1V3, "Text/insert6.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/chapter24_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/epilogue.xhtml"sv, "application/xhtml+xml"sv, "Epilogue"sv, true },
{ volume::P1V3, "Text/extra5.xhtml"sv, "application/xhtml+xml"sv, "The Life of an Apprentice Merchant"sv, true },
{ volume::P1V3, "Text/insert9.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/extra5_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/extra1.xhtml"sv, "application/xhtml+xml"sv, "Tuuli — Visiting Corinna"sv, true },
{ volume::P1V3, "Text/insert7.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/extra1_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V7, "xhtml/p-209.xhtml"sv, "application/xhtml+xml"sv, "Story: As the Gilberta Company's Successor"sv, true },
{ volume::MP1V7, "xhtml/p-210.xhtml"sv, "application/xhtml+xml"sv, "Story: As the Gilberta Company's Successor"sv, true },
{ volume::MP1V7, "xhtml/p-211.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-212.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-213.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-214.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-215.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-216.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-217.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-218.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-219.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-220.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::MP1V7, "xhtml/p-221.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
//{ volume::MP1V7, "image/i-209.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-210.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-211.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-212.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-213.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-214.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-215.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-216.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-217.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-218.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-219.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-220.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "image/i-221.jpg"sv, "image/jpeg"sv, std::nullopt, false },
{ volume::MP1V7, "style/fixed-layout.css"sv, "text/css"sv, std::nullopt, false },
{ volume::P1V3, "Text/extra2.xhtml"sv, "application/xhtml+xml"sv, "Leise — Confectionery Recipe"sv, true },
{ volume::P1V3, "Text/extra3.xhtml"sv, "application/xhtml+xml"sv, "Benno — Pound Cake Sampling"sv, true },
{ volume::P1V3, "Text/insert8.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/extra3_1.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/extra4.xhtml"sv, "application/xhtml+xml"sv, "Mark — The Master and I"sv, true },
{ volume::P1V3, "Text/extra6.xhtml"sv, "application/xhtml+xml"sv, "The Roots of the Guildmaster’s Worries"sv, true },
{ volume::P1V3, "Text/afterword.xhtml"sv, "application/xhtml+xml"sv, "Afterword"sv, true },
{ volume::P1V3, "Text/bonus1.xhtml"sv, "application/xhtml+xml"sv, "Bonus High Resolution Illustrations"sv, true },
{ volume::P1V3, "Text/bonus2.xhtml"sv, "application/xhtml+xml"sv, std::nullopt, true },
{ volume::P1V3, "Text/signup.xhtml"sv, "application/xhtml+xml"sv, "About J-Novel Club"sv, true },
{ volume::P1V3, "Text/copyright.xhtml"sv, "application/xhtml+xml"sv, "Copyright"sv, true },
    });

    constexpr definition_view_t vol_1_def { std::span<const volume_definition, std::dynamic_extent>(vol_1), volume::P1V1 };
    constexpr definition_view_t vol_2_def { std::span<const volume_definition, std::dynamic_extent>(vol_2), volume::P1V2 };
    constexpr definition_view_t vol_3_def { std::span<const volume_definition, std::dynamic_extent>(vol_3), volume::P1V3 };
    constexpr std::array<definition_span_view_t, 3> part_1_arr{ { vol_1_def, vol_2_def, vol_3_def } };
    constexpr definition_view_t part_1_def { std::span<const definition_span_view_t, std::dynamic_extent>(part_1_arr), omnibus::PART1 };
}
