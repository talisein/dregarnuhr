# Dregarnuhr

Rearrange Ascendence of a Bookworm epubs into Chronological Order

[![CI](https://github.com/talisein/dregarnuhr/actions/workflows/main.yml/badge.svg)](https://github.com/talisein/dregarnuhr/actions/workflows/main.yml)

Currently only dumping is supported:

```
# dregarnuhr --mode=dump P1V1 ascendence-of-a-bookwork.epub
```

The output is along the lines of the below.

```
OEBPS/content.opf: toc: ncx
OEBPS/content.opf: toc_href: toc.ncx, toc_path: OEBPS/toc.ncx
OEBPS/toc.ncx: dtb_uid: 9781718346017
OEBPS/toc.ncx: dtb_depth: 1
OEBPS/toc.ncx: title: Ascendance of a Bookworm: Part 1 Daughter of a Soldier Volume 1
{ P1V1, "style", "Styles/stylesheet.css", "text/css", std::nullopt, false },
{ P1V1, "ncx", "toc.ncx", "application/x-dtbncx+xml", std::nullopt, false },
{ P1V1, "Cover.jpg", "Images/Cover.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "FrontMatter1.jpg", "Images/FrontMatter1.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "FrontMatter3.jpg", "Images/FrontMatter3.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert1.jpg", "Images/Insert1.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert2.jpg", "Images/Insert2.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert3.jpg", "Images/Insert3.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert4.jpg", "Images/Insert4.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert5.jpg", "Images/Insert5.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert6.jpg", "Images/Insert6.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert7.jpg", "Images/Insert7.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert8.jpg", "Images/Insert8.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Insert9.jpg", "Images/Insert9.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "FrontMatter2.jpg", "Images/FrontMatter2.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "Bonus1.jpg", "Images/Bonus1.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "jnovelclubCMYK.png", "Images/jnovelclubCMYK.png", "image/png", std::nullopt, false },
{ P1V1, "Bonus2.jpg", "Images/Bonus2.jpg", "image/jpeg", std::nullopt, false },
{ P1V1, "cover", "Text/cover.xhtml", "application/xhtml+xml", "Cover", true },
{ P1V1, "frontmatter1.xhtml", "Text/frontmatter1.xhtml", "application/xhtml+xml", "Color Illustrations", true },
{ P1V1, "frontmatter2.xhtml", "Text/frontmatter2.xhtml", "application/xhtml+xml", std::nullopt, true },
{ P1V1, "frontmatter3.xhtml", "Text/frontmatter3.xhtml", "application/xhtml+xml", "Map of Ehrenfest", true },
{ P1V1, "toc", "Text/toc.xhtml", "application/xhtml+xml", std::nullopt, true },
{ P1V1, "prologue.xhtml", "Text/prologue.xhtml", "application/xhtml+xml", "Prologue", true },
{ P1V1, "chapter01.xhtml", "Text/chapter01.xhtml", "application/xhtml+xml", "A New Life", true },
{ P1V1, "chapter02.xhtml", "Text/chapter02.xhtml", "application/xhtml+xml", "Exploring My Home", true },
{ P1V1, "insert1.xhtml", "Text/insert1.xhtml", "application/xhtml+xml", std::nullopt, true },
{ P1V1, "chapter021.xhtml", "Text/chapter021.xhtml", "application/xhtml+xml", std::nullopt, true },
{ P1V1, "chapter03.xhtml", "Text/chapter03.xhtml", "application/xhtml+xml", "Exploring the City", true },
{ P1V1, "chapter04.xhtml", "Text/chapter04.xhtml", "application/xhtml+xml", "Books: Unobtainable", true },
{ P1V1, "chapter05.xhtml", "Text/chapter05.xhtml", "application/xhtml+xml", "Lifestyle Overhaul", true },
{ P1V1, "chapter06.xhtml", "Text/chapter06.xhtml", "application/xhtml+xml", "Neighborhood Boys", true },
```

The idea is to use this to initialize a struct for a chronological order, e.g.

```c++
struct reversed {
  enum volume { P1V1, FB1 };
  string id;
  string path;
  string type;
  optional<string> toc_text;
  bool in_spine;
};

vector<reversed> reversed_p1v1 = {
{ P1V1, "chapter05.xhtml", "Text/chapter05.xhtml", "application/xhtml+xml", "Lifestyle Overhaul", true },
{ P1V1, "chapter04.xhtml", "Text/chapter04.xhtml", "application/xhtml+xml", "Books: Unobtainable", true },
{ P1V1, "chapter03.xhtml", "Text/chapter03.xhtml", "application/xhtml+xml", "Exploring the City", true },
{ FB1, "x37.xhtml", "Text/37.xhtml", "application/xhtml+xml", "My Daughter’s about to Be a Criminal?! by Suzuka", true },
{ P1V1, "chapter021.xhtml", "Text/chapter021.xhtml", "application/xhtml+xml", std::nullopt, true },
{ P1V1, "insert1.xhtml", "Text/insert1.xhtml", "application/xhtml+xml", std::nullopt, true },
}
```

Then its just a matter of walking through the struct and spitting out new xml
for the new P1V1 rootfile and toc, and copying the chapters into a new zipfile.
