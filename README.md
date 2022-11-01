# Dregarnuhr

Rearrange Ascendence of a Bookworm epubs into Chronological Order

[![CI](https://github.com/talisein/dregarnuhr/actions/workflows/main.yml/badge.svg)](https://github.com/talisein/dregarnuhr/actions/workflows/main.yml)
[![Copr build status](https://copr.fedorainfracloud.org/coprs/talisein/dregarnuhr/package/dregarnuhr/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/talisein/dregarnuhr/package/dregarnuhr/)

This program reads Asendence of a Bookworm epubs volumes, and creates new
volumes with the chapters rearranged into chronological order, as determined by
kuali on the [J-Novel
forums](https://forums.j-novel.club/topic/5036/alternate-reading-order). Incidently
we both created a program to do this task independently; you can find their
version at [mldriscoll/AOAB](https://github.com/mldriscoll/AOAB/). Their version
is a Windows-only application, but offers a more curated output. Dregarnuhr is
available for Windows, Mac, and Linux.

This program leverages [aoab-monitor](https://github.com/talisein/aoab-monitor)
to alert you if there are updates available for your epub files. FYI it will
fetch [updates.json](http://aoabmonitor.talinet.net/updates.json) from the web.

# Dependencies

- Running the Linux binary from Releases tab
  - OpenSSL, libxml2, libicu, liblzma, zlib, libstdc++
- Building from source
  - Meson
    - There is a meson wrap for all dependencies, so if you can install python,
      meson, ninja, and gcc >= 12 you should be good to go.
  - Ubuntu
    - If you are building on Ubuntu and want to avoid unnecessary meson wrap
      downloads, install these packages:
    - `libhowardhinnant-date-dev libcpp-httplib-dev libxml2-dev libjpeg-turbo8-dev meson ninja-build`
  - Fedora
    - You can install the copr package:
    ```
    # sudo dnf copr enable talisein/libxmlplusplus-5.0
    # sudo dnf copr enable talisein/dregarnuhr
    # sudo dnf install dregarnuhr
    ```

# Building

Its easiest to just grab a binary from the
[releases](https://github.com/talisein/dregarnuhr/releases) page. If you are
interested in building:

```
# meson setup build && ninja -C build
# # Invoke with:
# build/src/dregarnuhr ~/Documents/epub_in_dir ~/Documents/epub_out_dir
```

# Limitations (TODOs)

- If you make an omnibus, you need the first book.
  - Missing volumes after that are ok (obviously, they will not be included)
- I don't have a mac to test the mac version on. Let me know if it works.

# Examples

The simplest invocation of dregarnuhr takes an input directory with your
bookworm epubs, and an output directory name to generate the new epubs in. As
long as your epubs have a ".epub" extension, the metadata will be read to
identify the volume.

```
# dregarnuhr Documents/Myne Documents/Myne_out
Output directory "Documents/Myne_out" doesn't exist. Creating.
Found P1V1: "Documents/Myne/a-great-first-book.epub"
Found P1V2: "Documents/Myne/any-epub-name-works.epub"
Found P1V3: "Documents/Myne/ascendance-of-a-bookworm-part-1-volume-3.epub"
<snip>
Created chronologically ordered P1V1: "Documents/Myne_out/chronological-a-great-first-book.epub"
Created chronologically ordered P1V2: "Documents/Myne_out/chronological-any-epub-name-works.epub"
Created chronologically ordered P1V3: "Documents/Myne_out/chronological-ascendance-of-a-bookworm-part-1-volume-3.epub"
```

If you want to create a slimmed down set of volumes to read on your phone, you
can filter out the bonus illustrations and adjust the jpg quality. Note the name
filter is a case insensitive regex. When filtering out images, you should also
filter out the xhtml pages that reference them. This recipe produces epubs that
are about 3MiB large, a good improvement over the 30MiB originals.

```
# src/dregarnuhr --jpg-quality=75 --jpg-scale=2 '--filter=name=bonus[0-9].(jpg|xhtml)'  ~/Documents/Myne ~/Documents/Myne_out
```

## Omnibus

To create a single epub omnibus containing all your volumes:
```
# dregarnuhr --omnibus --prefix=omnibus- ~/Documents/Myne out
```

That creates a 650MiB file if you have everything from P1V1 to P4V7! If you want
a 50MiB slim version:
```
# dregarnuhr --omnibus --prefix=omnibus- --suffix=-slim --jpg-quality=75 --jpg-scale=2 "--filter=name=bonus[0-9].(jpg|xhtml)"   ~/Documents/Myne out
```

You can add a custom cover image. It must be a JPEG. An example is in the
covers/ directory of this repo. Custom covers are only for an omnibus.
```
dregarnuhr --omnibus --cover=cover.jpg ~/Documents/Myne out
```

## Other examples

If you invoke the program several times, output files are never overwritten.
```
# dregarnuhr Documents/Myne Documents/Myne_out
<snip>
Created chronologically ordered P1V1: "Documents/Myne_out/chronological-a-great-first-book (2).epub"
```

You can specify a prefix and/or a suffix for the generated filenames

```
# dregarnuhr --prefix=linear- --suffix=-dregarnuhr-edition Documents/Myne Documents/Myne_out
Info: Filename suffix set to "-dregarnuhr-edition"
Info: Filename prefix set to "linear-"
<snip>
Created chronologically ordered P1V1: "Documents/Myne_out/linear-a-great-first-book-dregarnuhr-edition.epub"
```

For safety, this program will automatically ignore any input that it had
previously produced.

```
# dregarnuhr out out2
Output directory "out2" doesn't exist. Creating.
Info: "out/chronological-ascendance-of-a-bookworm.epub" is produced by this program. Skipping!
Info: "out/chronological-a-great-first-book.epub" is produced by this program. Skipping!
Info: "out/chronological-a-great-first-book (2).epub" is produced by this program. Skipping!
Info: "out/chronological-a-great-first-book (3).epub" is produced by this program. Skipping!
Info: "out/chronological-any-epub-name-works.epub" is produced by this program. Skipping!
Info: "out/chronological-ascendance-of-a-bookworm-part-1-volume-3.epub" is produced by this program. Skipping!
Couldn't find Fanbook 1, so those chapters will be skipped in the new epubs.
Couldn't find Fanbook 2, so those chapters will be skipped in the new epubs.
Sorry, no books could be created.
```

## Bug reports

If it can't identify your volumes for some reason, pass the verbose flag and
share the output by creating an issue here on github, or tag me on the J-Novel
forums (@talisein).

```
# dregarnuhr Documents/Myne Documents/Myne_out --verbose
Verified input dir: Documents/Myne
Verified output dir: Documents/Myne_out
Info: Looking for epubs in Documents/Myne
Info: considering Documents/Myne/ascendance-of-a-bookworm-fanbook-1.epub
mimetype: application/epub+zip
META-INF/container.xml
META-INF/container.xml: rootfile: OEBPS/content.opf
OEBPS/content.opf: toc: ncx
OEBPS/content.opf: toc_href: toc.ncx, toc_path: OEBPS/toc.ncx
OEBPS/toc.ncx: dtb_uid: 9781718344631
OEBPS/toc.ncx: dtb_depth: 1
OEBPS/toc.ncx: title: Ascendance of a Bookworm: Official Fanbook Volume 1
Info: considering Documents/Myne/ascendance-of-a-bookworm-fanbook-2.epub
<snip>
```

Finally, this is completely unrelated to Ascendence of a Bookworm, but [Miku is
cute](https://youtu.be/GrH3OrZU6Ek) so maybe you'd like to take a moment out of
your day to appreciate her.
