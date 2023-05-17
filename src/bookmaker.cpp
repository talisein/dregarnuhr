#include <algorithm>
#include <array>
#include <ranges>
#include <span>
#include <set>
#include <string>
#include <fstream>

#include "bookmaker.h"
#include "volumes.h"
#include "log.h"
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "part5.h"
#include "omnibus.h"
#include "outcome/utils.hpp"
#include "outcome/try.hpp"
#include "jpeg.h"
#include "utils.h"
#include "png_reader.h"

namespace {
    using namespace std::literals;
    constexpr std::string_view USER_COVER_JPG_PATH {"Images/OverrideFrontCover.jpg"};
    constexpr std::string_view USER_COVER_XHTML_ID {"user_provided_cover.xhtml"};
    constexpr std::string_view USER_COVER_XHTML_PATH {"Text/OverrideFrontCover.xhtml"};
    constexpr std::string_view USER_COVER_XHTML_BODY {R"xml(<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en" xml:lang="en">
<head>
  <meta content="text/html; charset=UTF-8" http-equiv="default-style"/>
  <title>Ascendance of a Bookworm: Chronological Omnibus</title>
  <link href="../Styles/stylesheet.css" rel="stylesheet" type="text/css"/>
</head>

<body class="nomargin center">
  <section epub:type="cover">
    <img alt="Cover" class="cover" src="../Images/OverrideFrontCover.jpg"/>
  </section>
</body>
</html>)xml"};
    constexpr std::string_view USER_COVER_STYLESHEET_ID {"user_provided_cover_stylesheet.css"};
    constexpr std::string_view USER_COVER_STYLESHEET_PATH {"Styles/stylesheet.css"};
    constexpr std::string_view USER_COVER_STYLESHEET_BODY {R"css(body {
 line-height: 1.2em;
 font-size: 1em;
 overflow-wrap: break-word;
}

body.nomargin {
 margin: 0em;
 padding: 0em;
}

body.center {
 text-align: center;
}

img {
 max-width: 100%;
}

img.cover {
  text-indent: 0em;
  max-height: 98%;
  width: auto !important;
  clear: both;
})css"};

    constexpr auto defined_volumes = std::views::transform(get_omnibus_definition_r(),
                                                           &std::ranges::range_value_t<decltype(get_omnibus_definition_r())>::name) | std::views::transform([](const auto& v) -> volume { return std::get<volume>(v); });

    constexpr auto get_definition_view(omnibus o)
    {
        auto it = std::ranges::find(omnibus_defs::omnibus_arr,
                                    o,
                                    [](const auto& def) { return std::get<omnibus>(def.name); });
        if (it == std::ranges::end(omnibus_defs::omnibus_arr)) {
            return omnibus_defs::omnibus_def;
        }

        return *it;
    }

    constexpr auto get_definition_view(volume v)
    {
        auto it = std::ranges::find(omnibus_defs::omnibus_def,
                                    v,
                                    [](const auto& def) { return std::get<volume>(def.name); });
        if (it == std::ranges::end(omnibus_defs::omnibus_def)) {
            assert(false);
            return definition_view_t(std::views::single(omnibus_defs::omnibus_def.front()), v);

        }

        return definition_view_t(std::views::single(*it), v);
    }

    template<std::ranges::input_range R>
    auto get_filtered_defs(R&& defs, const epub::books_t& src_books, const epub::readers_t& src_readers)
    {
        return std::views::filter(std::forward<R>(defs), [&src_books, &src_readers](const volume_definition& def) {
            // Filter out sources we don't have available
            const auto& src_book_iter = src_books.find(def.vol);
            if (std::end(src_books) == src_book_iter) {
                return false;
            }

            const auto& src_reader = src_readers.at(def.vol);
            const auto& src_book = src_books.at(def.vol);
            const auto root = src_book.rootfile_path.substr(0, src_book.rootfile_path.find_first_of('/')+1);
            const auto src = utils::strcat(root, to_string_view(def.vol), '/', def.href);

            if (get_options()->size_filter) {
                const auto src_idx = src_reader->zip.locate_file(src.c_str()).value();
                const auto src_stat = src_reader->zip.stat(src_idx).value();
                if (src_stat.m_uncomp_size > get_options()->size_filter.value()) {
                    return false;
                }
            }

            if (get_options()->name_filter) {
                if (std::regex_search(src, get_options()->name_filter.value())) {
                    return false;
                }
            }
            return true;
        });
    }


    static std::filesystem::path
    get_new_filename(const std::string& basename)
    {
        int dups = 0;
        std::filesystem::path new_filename;

        do {
            std::string name;
            if (dups++ > 0) {
                name = utils::strcat(basename, " (", dups, ")", ".epub");
            } else {
                name = utils::strcat(basename, ".epub");
            }
            new_filename.assign(get_options()->output_dir);
            new_filename.append(name);
        } while (std::filesystem::exists(new_filename));
        return new_filename;
    }

    static std::filesystem::path
    get_new_filename(volume vol)
    {
        const auto v = to_string_view(vol);
        std::string base = utils::strcat(get_options()->prefix,
                                         get_options()->basename,
                                         "-part-"sv,
                                         v.substr(1,1),
                                         "-volume-"sv,
                                         v.substr(3,1),
                                         get_options()->suffix);
        return get_new_filename(base);
    }

    static std::filesystem::path
    get_new_filename(omnibus omnibus)
    {
        std::string_view o;
        switch (omnibus) {
            case omnibus::PART1: o = "-part-1"sv; break;
            case omnibus::PART2: o = "-part-2"sv; break;
            case omnibus::PART3: o = "-part-3"sv; break;
            case omnibus::PART4: o = "-part-4"sv; break;
            case omnibus::PART5: o = "-part-5"sv; break;
            case omnibus::ALL: break;
        };

        std::string base = utils::strcat(get_options()->prefix, get_options()->basename, o, get_options()->suffix);
        return get_new_filename(base);
    }

    static std::filesystem::path
    get_new_filename(const std::variant<omnibus, volume>& v)
    {
        return std::visit([]<typename T>(T&& arg) { return get_new_filename(std::forward<T>(arg)); }, v);
    }

    void cleanup_dangling(auto vol, const std::filesystem::path& filename) {
        std::error_code ec;
        bool exists = std::filesystem::exists(filename, ec);
        if (ec) {
            log_error("Failed to cleanup ", filename.string(), ": ", ec.message());
            return;
        }
        if (exists) {
            auto is_regular = std::filesystem::is_regular_file(filename, ec);
            if (ec) {
                log_error("Failed to create book ", vol, ". Dangling file ", filename, " is in unexpected state, got error: ", ec.message(), ". Clean it up yourself. This shouldn't happen!");
                return;
            }
            if (!is_regular) {
                log_error("Failed to create book ", vol, ". Dangling file ", filename, " is somehow not even a regular file. Something's gone very wrong, aborting.");
                return;
            }
            if (!std::filesystem::remove(filename, ec) || ec) {
                log_error("Failed to create book ", vol, ". Dangling file ", filename, " couldn't be cleaned up, error: ", ec.message());
                return;
            }
        }
    }
} // anonymous namespace


namespace epub
{
    result<void>
    book_writer::start_book()
    {
        using namespace std::string_literals;
        static const std::string MIMETYPE { "mimetype"s };
        static const std::string METAINF { "META-INF/container.xml"s };
        // First item must be mimetype
        OUTCOME_TRY(writer.copy_from(base_reader->zip, MIMETYPE, MIMETYPE));
        basefiles.remove(MIMETYPE);
        // second item should be META-INF/container.xml
        OUTCOME_TRY(writer.copy_from(base_reader->zip, METAINF, METAINF));
        basefiles.remove(METAINF);
        // Next is the root file, but we need to create it from the definition
        const std::string rootfile_path = base_book.rootfile_path;
        basefiles.remove(base_book.rootfile_path);
        OUTCOME_TRY(const auto rootfile_buf, create_rootfile());
        OUTCOME_TRY(writer.add(rootfile_path, std::span<const char>(rootfile_buf), get_options()->compression_level));
        return outcome::success();
    }

    result<std::string>
    book_writer::get_ncx_id() const
    {
        auto it = std::ranges::find(definition, chapter_type::NCX, &volume_definition::get_chapter_type);
        if (definition.end() == it) {
            log_error("No NCX found in volume definition");
            return std::errc::invalid_argument;
        } else {
            return it->get_id();
        }
    }

    result<std::string>
    book_writer::create_rootfile()
    {
        xmlpp::Document doc;
        xmlpp::ustring first_chapter_path;
        xmlpp::ustring toc_path;
        OUTCOME_TRY(auto idx, base_reader->zip.locate_file(base_book.rootfile_path.c_str()));
        OUTCOME_TRY(epub::file_reader f, base_reader->zip.extract_stream(idx));
        auto base_root = f.get_doc()->get_root_node();
        auto base_root_attrs = base_root->get_attributes();
        auto root = doc.create_root_node_by_import(f.get_doc()->get_root_node(), false);
        for (const auto& attr : base_root_attrs) {
            root->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
        }
        root->set_namespace_declaration(base_root->get_namespace_uri(), base_root->get_namespace_prefix());
        for (auto base_child : base_root->get_children()) {
            if (base_child->get_name() == "spine") {
                auto spine = utils::as_element(root->import_node(base_child, false));
                for (auto attr : base_child->get_first_child()->get_parent()->get_attributes()) {
                    if (attr->get_name() == "toc") {
                        OUTCOME_TRY(auto ncx_id, get_ncx_id());
                        spine->set_attribute(attr->get_name(),
                                             ncx_id,
                                             attr->get_namespace_prefix());
                    } else {
                        spine->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                    }
                }
                // Insert cover first
                if (get_options()->omnibus_type && get_options()->cover) {
                    auto itemref = spine->add_child_element("itemref");
                    itemref->set_attribute("idref", xmlpp::ustring(USER_COVER_XHTML_ID));
                }
                for (const auto &def : definition | std::views::filter(&volume_definition::in_spine)) {
                    auto itemref = spine->add_child_element("itemref");
                    itemref->set_attribute("idref", def.get_id());
                    auto src_book = src_books.find(def.vol);
                    auto src_iter = std::ranges::find(src_book->second.manifest.items, def.href, &manifest::item::href);
                    if (src_iter != std::end(src_book->second.manifest.items)) {
                        if (src_iter->spine_properties) {
                            auto v = src_iter->spine_properties.value();
//                            if (v != "page-spread-left" && v != "page_spread-right") {
                                itemref->set_attribute("properties", src_iter->spine_properties.value());
//                            }
                        }
                        if (src_iter->is_linear) {
//                            if (!src_iter->spine_properties || (src_iter->spine_properties &&
//                                                                src_iter->spine_properties.value() != "page-spread-left" &&
//                                                                src_iter->spine_properties.value() != "page-spread-right"))
//                            {
                                itemref->set_attribute("linear", "yes");
//                            }
                        }
                    }
                }
            } else if (base_child->get_name() == "metadata") {
                OUTCOME_TRY(const auto base_metadata, base_reader->get_metadata(f.get_doc()));
                auto metadata_node = utils::as_element(root->import_node(base_metadata, true));
                auto contributor = metadata_node->add_child_element("contributor", "dc");
                contributor->set_attribute("id", "contributor01");
                contributor->set_first_child_text("talisein");
                if (get_options()->omnibus_type) {
                    const xmlpp::Element::PrefixNsMap ns_map {{"dc", "http://purl.org/dc/elements/1.1/"}};
                    xmlpp::Node::NodeSet set = metadata_node->find("dc:title", ns_map);
                    if (!set.empty()) {
                        utils::as_element(set.front())->set_first_child_text(get_options()->title.value());
                    } else {
                        log_error("Can't find the metadata title..?");
                    }
                }
            } else if (base_child->get_name() == "manifest") {
                bool seen_cover = false;
                auto manifest = root->add_child_element("manifest");
                if (get_options()->omnibus_type && get_options()->cover) {
                    seen_cover = true;
                    {   auto item = manifest->add_child_element("item");
                        item->set_attribute("id", "user_provided_cover.jpg");
                        item->set_attribute("href", xmlpp::ustring(USER_COVER_JPG_PATH));
                        item->set_attribute("media-type", "image/jpeg");
                        item->set_attribute("properties", "cover-image"); }
                    {   auto item = manifest->add_child_element("item");
                        item->set_attribute("id", xmlpp::ustring(USER_COVER_XHTML_ID));
                        item->set_attribute("href", xmlpp::ustring(USER_COVER_XHTML_PATH));
                        item->set_attribute("media-type", "application/xhtml+xml"); }
                    {   auto item = manifest->add_child_element("item");
                        item->set_attribute("id", xmlpp::ustring(USER_COVER_STYLESHEET_ID));
                        item->set_attribute("href", xmlpp::ustring(USER_COVER_STYLESHEET_PATH));
                        item->set_attribute("media-type", "text/css"); }
                }
                for (const auto &def : definition) {
                    const auto src_book = src_books.find(def.vol);
                    auto item = manifest->add_child_element("item");
                    item->set_attribute("id", def.get_id());

                    if (def.vol == base_vol && def.href == base_book.manifest.toc_relpath) {
                        // toc.ncx will stay in OEBPS
                        item->set_attribute("href", xmlpp::ustring(def.href));
                    } else {
                        auto href { utils::xstrcat(to_string_view(def.vol), "/", def.href) };
                        item->set_attribute("href", href);
                        if (first_chapter_path.empty() && def.is_bodymatter()) {
                            first_chapter_path = href;
                        }
                        if (toc_path.empty() && chapter_type::TOC == def.get_chapter_type()) {
                            toc_path = href;
                        }
                    }

                    if (def.href.ends_with(".png") && !ctre::search<"jnovelclubCMYK.png">(def.href) && (get_options()->jpg_quality || get_options()->jpg_scale)) {
                        item->set_attribute("media-type", "image/jpeg");
                    } else {
                        item->set_attribute("media-type", xmlpp::ustring(def.mediatype));
                    }

                    // TODO: Maybe just use the source for most of the above properties
                    auto src_iter = std::ranges::find(src_book->second.manifest.items, def.href, &manifest::item::href);
                    if (src_iter != src_book->second.manifest.items.end() && src_iter->properties) {
                        if (src_iter->properties.value() != "cover-image" || !get_options()->omnibus_type.has_value()) {
                            item->set_attribute("properties", src_iter->properties.value());
                        } else if (src_iter->properties.value() == "cover-image" && get_options()->omnibus_type) {
                            if (!seen_cover) {
                                item->set_attribute("properties", src_iter->properties.value());
                                seen_cover = true;
                            }
                        }
                    }
                }
            } else if (base_child->get_name() == "guide") {
                auto guide = utils::import_attr(root, utils::as_element(base_child));
                auto add_ref = [&](const auto& type, const auto& title, auto&& href) {
                    auto text = guide->add_child_element("reference");
                    text->set_attribute("type", type);
                    text->set_attribute("title", title);
                    text->set_attribute("href", std::forward<decltype(href)>(href));
                };
                auto add_ref_to_first = [&](const auto &type, const auto& title, chapter_type chapter) {
                    if (auto iter = std::ranges::find(definition, chapter, &volume_definition::get_chapter_type);
                        iter != std::ranges::end(definition))
                    {
                        add_ref(type, title, utils::xstrcat(to_string_view(iter->vol), "/", iter->href));
                    }
                };
                add_ref_to_first("text", "Text", chapter_type::CHAPTER);
                add_ref("toc", "Table of Contents", toc_path);
                add_ref_to_first("copyright-page", "Copyright", chapter_type::SIGNUP);
                if (get_options()->cover) {
                    add_ref("cover", "Cover", xmlpp::ustring(USER_COVER_XHTML_PATH));
                } else {
                    add_ref_to_first("cover", "Cover", chapter_type::COVER);
                }
            } else {
                auto x = dynamic_cast<const xmlpp::TextNode*>(base_child);
                if (!x) {
                    log_info("Unexpected epub package node ", base_child->get_name());
                    root->import_node(base_child, true);
                }
            }
        }

        return doc.write_to_string_formatted();
    }

    result<void>
    book_writer::add_ncx()
    {
        auto def_iter = std::ranges::find(definition, chapter_type::NCX, &volume_definition::get_chapter_type);
        if (def_iter == std::ranges::end(definition)) {
            log_error("Couldn't find ncx");
            return std::errc::no_such_file_or_directory;
        }
        const auto& ncx_book = src_books.at(def_iter->vol);
        const auto toc_fullpath =  ncx_book.rootfile_path.substr(0,  ncx_book.rootfile_path.find_first_of('/')+1).append( ncx_book.manifest.toc_relpath);

        auto it = std::ranges::find(basefiles, toc_fullpath);
        if (it != basefiles.end()) {
            basefiles.remove(toc_fullpath);
        }

        OUTCOME_TRY(auto toc_buf, create_ncx(toc_fullpath));
        OUTCOME_TRY(writer.add(toc_fullpath, std::span<const char>(toc_buf), get_options()->compression_level));
        return outcome::success();
    }

    result<std::string>
    book_writer::create_ncx(const std::string& toc_fullpath)
    {
        static const auto volume_map = get_volume_map();
        xmlpp::Document doc;
        OUTCOME_TRY(auto idx, base_reader->zip.locate_file(toc_fullpath.c_str()));
        OUTCOME_TRY(epub::file_reader f, base_reader->zip.extract_stream(idx));
        const auto src_root = f.get_doc()->get_root_node();
        auto src_root_attrs = src_root->get_attributes();
        auto root = doc.create_root_node_by_import(src_root, false);
        for (const auto& attr : src_root_attrs) {
            root->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
        }
        root->set_namespace_declaration(src_root->get_namespace_uri(), src_root->get_namespace_prefix());
        for (const auto &child : src_root->get_children()) {
            if (child->get_name() == "navMap") continue;
            root->import_node(child, true);
        }
        auto navmap = root->add_child_element("navMap");

        auto make_nav_point = [point = int{1}](const auto &def, xmlpp::Element* parent,
                                               std::optional<xmlpp::ustring> label = std::nullopt,
                                               std::optional<xmlpp::ustring> href = std::nullopt)
            mutable -> xmlpp::Element*
        {
            auto res = parent->add_child_element("navPoint");
            res->set_attribute("id", utils::strcat("navPoint", point++));

            auto navLabel = res->add_child_element("navLabel");
            auto text = navLabel->add_child_element("text");
            if (label) {
                text->add_child_text(*label);
            } else {
                text->add_child_text(xmlpp::ustring(*def.toc_label));
            }
            auto content = res->add_child_element("content");
            std::string srcattr;
            if (href) {
                srcattr = *href;
            } else {
                srcattr = utils::strcat(to_string_view(def.vol), "/", def.href);
            }
            content->set_attribute("src", srcattr);

            return res;
        };

        if (get_options()->omnibus_type && get_options()->cover) {
            make_nav_point(*definition.begin(), navmap, xmlpp::ustring("Omnibus Cover"), xmlpp::ustring(USER_COVER_XHTML_PATH));
        }

        omnibus current_part = omnibus::PART1;
        volume current_volume = volume::FB1;
        xmlpp::Element *current_part_element = nullptr;
        xmlpp::Element *current_volume_element = nullptr;
        for (const auto& def : definition
                 | std::ranges::views::filter([](const auto& def) { return def.toc_label.has_value(); })) {
            if (get_options()->do_nested && def.is_bodymatter()) {
                auto [part, volume] = volume_map.at(def);
                if (!current_part_element || part != current_part) {
                    current_part = part;
                    current_volume = volume;
                    current_part_element = make_nav_point(def, navmap, xmlpp::ustring(to_string_view(part)));
                    current_volume_element = make_nav_point(def, current_part_element, xmlpp::ustring(to_string_view(volume)));
                }

                if (volume != current_volume) {
                    current_volume = volume;
                    current_volume_element = make_nav_point(def, current_part_element, xmlpp::ustring(to_string_view(volume)));
                }

                make_nav_point(def, current_volume_element);
            } else {
                make_nav_point(def, navmap);
            }
        }

        return doc.write_to_string_formatted();
    }

    result<void>
    book_writer::make_book_impl()
    {
        OUTCOME_TRY(start_book());
        const auto root = base_book.rootfile_path.substr(0, base_book.rootfile_path.find_first_of('/')+1);
        // insert cover here too
        if (get_options()->omnibus_type && get_options()->cover) {
            auto dst { utils::strcat(root, USER_COVER_XHTML_PATH) };
            std::span<const char> span { USER_COVER_XHTML_BODY.data(), USER_COVER_XHTML_BODY.size() };
            OUTCOME_TRY(writer.add(dst, span, get_options()->compression_level));

            dst = utils::strcat(root, USER_COVER_JPG_PATH);
            try {
                std::ifstream cover_stream { get_options()->cover.value(), std::ios::in | std::ios::binary };
                cover_stream.exceptions(std::ios_base::badbit);
                OUTCOME_TRY(writer.add(dst, cover_stream, std::nullopt, std::filesystem::last_write_time(get_options()->cover.value())));
            } catch (std::system_error& e) {
                return e.code();
            } catch (std::exception& e) {
                log_error("Couldn't insert omnibus cover image: ", e.what());
                return outcome::error_from_exception();
            }

            dst = utils::strcat(root, USER_COVER_STYLESHEET_PATH);
            span = { USER_COVER_STYLESHEET_BODY.data(), USER_COVER_STYLESHEET_BODY.size() };
            OUTCOME_TRY(writer.add(dst, span, get_options()->compression_level));
        }
        for (const volume_definition& def : definition)
        {
            if (def.href == base_book.manifest.toc_relpath) {
                OUTCOME_TRY(add_ncx());
                log_verbose("Made ncx");
                continue;
            }
            auto src_reader_iter = src_readers.find(def.vol);
            if (src_readers.end() == src_reader_iter) {
                log_verbose("Volume ", to_string_view(def.vol), " is unavailable. Skipping insertion.");
                continue;
            }
            const auto& src_reader = src_reader_iter->second;
            const auto& src_book = src_books.find(def.vol)->second;
            const auto& src_item = std::ranges::find(src_book.manifest.items, def.href, &manifest::item::href);
            std::optional<manifest::item> item;
            if (std::end(src_book.manifest.items) != src_item) {
                item = std::make_optional<manifest::item>(*src_item);
            }

            // Determine src and dst paths. dst will be e.g. OEBPS/FB1/Text.
            const auto srcbook_root = src_book.rootfile_path.substr(0, src_book.rootfile_path.find_first_of('/')+1);
            const auto src = utils::strcat(srcbook_root, def.href);
            const auto dst = utils::strcat(root, to_string_view(def.vol), "/", def.href);

            // Transformations
            if (src.ends_with(".jpg") && (get_options()->jpg_quality || get_options()->jpg_scale)) {
                OUTCOME_TRY(const auto idx, src_reader->zip.locate_file(src.c_str()));
                OUTCOME_TRY(const auto buf, src_reader->zip.extract_raw(idx));
                jpeg::compressor comp;
                jpeg::decompressor decomp {buf};
                OUTCOME_TRY(const auto outbuf, comp.compress_from(decomp, get_options()->jpg_quality, get_options()->jpg_scale));
                OUTCOME_TRY(writer.add(dst, outbuf, std::nullopt));
                basefiles.remove(src);
                continue;
            }

            if (src.ends_with(".png") && !ctre::search<"jnovelclubCMYK.png">(src) && (get_options()->jpg_quality || get_options()->jpg_scale)) {
                OUTCOME_TRY(const auto idx, src_reader->zip.locate_file(src.c_str()));
                OUTCOME_TRY(const auto buf, src_reader->zip.extract_raw(idx));
                png::reader reader {buf};
                OUTCOME_TRY(auto unscaled_rgb_data, reader.read());
                auto scaled_rgb_data = unscaled_rgb_data;
                if (get_options()->jpg_scale) {
                    OUTCOME_TRY(auto b, reader.scale(get_options()->jpg_scale.value()));
                    scaled_rgb_data = b;
                }
                jpeg::compressor comp;
                OUTCOME_TRY(auto jpg_data, comp.compress_rgb(reader.get_width(), reader.get_height(), scaled_rgb_data, get_options()->jpg_quality));
//                const auto pos = dst.find(".png");
//                const auto renamed_dst = std::string(dst).replace(pos, 4, ".jpg");
                const auto to_percent = [](auto nom, auto denom) -> float { return 100.0f * static_cast<float>(nom) / static_cast<float>(denom); };
                log_verbose("PNG->JPG compression: ", dst, " From ", buf.size(), " to ", jpg_data.size_bytes(), ". ",
                            to_percent(jpg_data.size_bytes(), buf.size()), "%");
                OUTCOME_TRY(writer.add(dst, jpg_data, std::nullopt));
                basefiles.remove(src);
                continue;
            }

            // toc.xhtml needs to be rewritten
            if (item && item->properties.has_value() && item->properties.value() == "nav") {
                OUTCOME_TRY(const auto src_idx, src_reader->zip.locate_file(src.c_str()));
                OUTCOME_TRY(auto src_streambuf, src_reader->zip.extract_stream(src_idx));
                std::istream src_stream(&src_streambuf);
                xmlpp::DomParser src_parser;
                src_parser.parse_stream(src_stream);
                xmlpp::Document doc;
                auto src_root = src_parser.get_document()->get_root_node();
                auto nav_root = doc.create_root_node_by_import(src_root, false);
                nav_root->set_namespace_declaration(src_root->get_namespace_uri());
                nav_root->set_namespace_declaration("http://www.idpf.org/2007/ops", "epub");
                for (auto attr : src_root->get_attributes()) {
                    nav_root->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                }
                auto dtd = src_parser.get_document()->get_internal_subset();
                doc.set_internal_subset(dtd->get_name(), dtd->get_external_id(), dtd->get_system_id());
                for (const auto& src_root_child : src_root->get_children()) {
                    if (src_root_child->get_name() == "head") {
                        // Making the link to the stylesheet is currently
                        // unnecessary. If not for the title being wrong, we
                        // could just import the whole <head>...
                        auto head = utils::import_attr(nav_root, utils::as_element(src_root_child));
                        if (get_options()->title) {
                            utils::import_children_except(head, src_root_child, {"title", "link"});
                            auto title = head->add_child_element("title");
                            title->add_child_text(get_options()->title.value());
                        } else {
                            utils::import_children_except(head, src_root_child, "link");
                        }

                        auto src_link = utils::as_element(src_root_child->get_first_child("link"));
                        auto link = utils::import_attr_except(head, src_link, "href");
                        // Find the stylesheet from the volume with this toc
                        auto iter = std::ranges::find_if(definition, [vol = def.vol](auto&& def) {
                            return vol == def.vol && def.get_chapter_type() == chapter_type::STYLESHEET;
                        });

                        link->set_attribute("href", utils::xstrcat("../../", to_string_view(def.vol), "/", iter->href));
                    } else if (src_root_child->get_name() == "body") {
                        auto body = utils::as_element(nav_root->import_node(src_root_child, false));
                        for (const auto src_iter : src_root_child->get_children()) {
                            if (src_iter->get_name() == "nav") {
                                auto srcattr = utils::as_element(src_iter)->get_attribute("type", "epub");
                                bool is_toc = srcattr->get_value() == "toc";
                                if (is_toc) {
                                    auto nav = utils::as_element(body->import_node(src_iter, false));
                                    for (const auto &attr : utils::as_element(src_iter)->get_attributes()) {
                                        nav->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                                    }
                                    make_toc(nav, utils::as_element(src_iter));
                                    log_verbose("Made toc");
                                } else if (utils::as_element(src_iter)->get_attribute("type", "epub")->get_value() == "landmarks") {
                                    auto nav = utils::import_attr(body, utils::as_element(src_iter));
                                    make_landmarks(nav, utils::xstrcat("../../", to_string_view(def.vol), "/", def.href));
                                    log_verbose("Made landmarks");
                                } else {
                                    body->import_node(src_iter, true);
                                }
                            }
                        }
                    }
                }
                auto str = doc.write_to_string_formatted();
                OUTCOME_TRY(writer.add(dst, std::span<const char>(str), get_options()->compression_level));
                basefiles.remove(src);
                continue;
            }

            // No special case, just copy it.
            log_verbose("Copying ", src, " to ", dst);
            if (auto res = writer.copy_from(src_reader->zip, src, dst); !res) {
                log_error("Failed to copy ", to_string_view(def.vol), ":", src, ": ", res.error());
                return res;
            }
            log_verbose("Copying ", src, " to ", dst, ": done");
            if (def.vol == base_vol) {
                auto res = basefiles.remove(src);
                if (1 != res) [[unlikely]] {
                    log_error("Unexpectedly removed ", res, " sources named ", src);
                }
            }
            continue;
        }
        static const std::list<std::pair<volume, std::string>> expected_leftovers {
            std::make_pair(volume::P2V4, "OEBPS/Text/extra8.xhtml"),
            std::make_pair(volume::P4V2, "OEBPS/Text/chapter21.xhtml")
        };
        for (auto src : basefiles | std::ranges::views::filter([this](auto const &basefile) { return std::end(expected_leftovers) == std::ranges::find(expected_leftovers, std::make_pair(base_vol, basefile)); })) {
            log_info("Warning: left over file in src ", to_string_view(base_vol), ": ", src);
        }
        OUTCOME_TRY(writer.finish());
        return outcome::success();
    }

    void
    book_writer::make_landmarks(xmlpp::Element *nav, const xmlpp::ustring& toc_path)
    {
        auto h1 = nav->add_child_element("h1");
        h1->add_child_text("Landmarks");
        auto ol = nav->add_child_element("ol");

        auto add_landmark_path = [&](const auto& type, const auto& title, const auto& path) {
            auto li = ol->add_child_element("li");
            auto a = li->add_child_element("a");
            a->set_attribute("type", type, "epub");
            a->set_attribute("href", path);
            a->add_child_text(title);
        };

        auto add_landmark = [&](const auto& type, const auto& title, const auto& def_iter) {
            add_landmark_path(type, title, utils::xstrcat("../../", to_string_view(def_iter->vol), "/", def_iter->href));
        };

        auto add_landmark_to_first = [&](const auto& type, const auto& title, chapter_type chapter) {
            if (auto iter = std::ranges::find(definition, chapter, &volume_definition::get_chapter_type);
                iter != std::ranges::end(definition)) {
                add_landmark(type, title, iter);
            }
        };

        add_landmark_to_first("frontmatter", "Color Images", chapter_type::FRONTMATTER);

        add_landmark_to_first("bodymatter", "Body", chapter_type::CHAPTER);


        if (get_options()->cover) {
            add_landmark_path("cover", "Cover", utils::xstrcat("../../", USER_COVER_XHTML_PATH));
        } else {
            add_landmark_to_first("cover", "Cover", chapter_type::COVER);
        }

        add_landmark_path("toc", "Table of Contents", toc_path);

        bool marked_afterword = false;
        bool marked_imprint = false;
        bool marked_copyright = false;
        if (auto it = std::ranges::find_first_of(definition,
                                                 std::initializer_list<chapter_type>{
                                                     chapter_type::MAP_EHRENFEST_CITY,
                                                     chapter_type::MAP_EHRENFEST_DUCHY,
                                                     chapter_type::MAP_YURGENSCHMIDT,
                                                     chapter_type::TABLE_YURGENSCHMIDT_DUCHIES,
                                                     chapter_type::AFTERWORD,
                                                     chapter_type::MANGA,
                                                     chapter_type::POLL,
                                                     chapter_type::SIGNUP,
                                                     chapter_type::COPYRIGHT,
                                                 },
                                                 std::ranges::equal_to{}, &volume_definition::get_chapter_type);
            it != std::ranges::end(definition))
        {
            if (it->get_chapter_type() == chapter_type::AFTERWORD) {
                add_landmark("backmatter afterword", "Backmatter", it);
                marked_afterword = true;
            } else if (it->get_chapter_type() == chapter_type::SIGNUP) {
                add_landmark("backmatter imprint", "Backmatter", it);
                marked_imprint = true;
            } else if (it->get_chapter_type() == chapter_type::COPYRIGHT) {
                add_landmark("backmatter copyright-page", "Backmatter", it);
                marked_copyright = true;
            } else {
                add_landmark("backmatter", "Backmatter", it);
            }
        }

        if (auto it = std::ranges::find(definition, chapter_type::AFTERWORD, &volume_definition::get_chapter_type);
            !marked_afterword && it != std::ranges::end(definition))
        {
            add_landmark("afterword", "Afterword", it);
        }

        if (auto it = std::ranges::find(definition, chapter_type::SIGNUP, &volume_definition::get_chapter_type);
            !marked_imprint && it != std::ranges::end(definition))
        {
            add_landmark("imprint", "Imprint", it);
        }

        if (auto it = std::ranges::find(definition, chapter_type::COPYRIGHT, &volume_definition::get_chapter_type);
            !marked_copyright && it != std::ranges::end(definition))
        {
            add_landmark("copyright-page", "Copyright", it);
        }

    }

    void
    book_writer::make_toc(xmlpp::Element *nav, const xmlpp::Element* src_nav)
    {
        static const auto volume_map = get_volume_map();

        for (const auto child : src_nav->get_children() | utils::to_element_filter()) {
            if (child->get_name() != "ol") {
                nav->import_node(child, true);
                continue;
            }

            auto root_ol = utils::import_attr(nav, child);
            // Insert cover here too
            if (get_options()->omnibus_type && get_options()->cover) {
                auto li = root_ol->add_child_element("li");
                li->set_attribute("class", "toc-front");
                li->set_attribute("id", "toc-omnibus-cover");
                auto a = li->add_child_element("a");
                a->set_attribute("href", utils::xstrcat("../../", USER_COVER_XHTML_PATH));
                a->add_child_text("Omnibus Cover");
            }

            constexpr auto make_link = [](const volume_definition& def) -> xmlpp::ustring {
                auto src = def.href;
//                if (src.ends_with(".png") && !ctre::search<"jnovelclubCMYK.png">(src) && (get_options()->jpg_quality || get_options()->jpg_scale)) {
//                    const auto pos = src.find(".png");
//                    src = std::string(src).replace(pos, 4, ".jpg");
//                };
                return utils::xstrcat("../../", to_string_view(def.vol), "/", src);
            };

            const auto ns_map = xmlpp::Element::PrefixNsMap{{"html", "http://www.w3.org/1999/xhtml"}};
            int idx = 1;
            auto make_li = [&](xmlpp::Element* parent, const volume_definition& def) -> xmlpp::Element*
            {
                using namespace std::string_literals;
                using namespace std::string_view_literals;
                xmlpp::Node::const_NodeSet set = src_nav->find("html:ol/html:li/html:a[@href='../"s + std::string(def.href) + "']"s, ns_map);
                if (set.empty()) { // Sometimes it is "chapter1.html" instead of "../Text/chapter1.html"
                    set = src_nav->find("html:ol/html:li/html:a[@href='"s + std::string(def.href.substr(def.href.find_last_of('/')+1)) + "']"s, ns_map);
                }
                auto li = parent->add_child_element("li");
                if (!set.empty() && def.vol == base_vol) {
                    auto src_li = utils::as_element(set.front()->get_parent());
                    for (const auto &attr : src_li->get_attributes()) {
                        li->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                    }
                } else {
                    li->set_attribute("class", "toc-chapter");
                    li->set_attribute("id", utils::xstrcat("chrono-chapter"sv, idx++));
                }
                auto a = li->add_child_element("a");
                a->set_attribute("href", make_link(def));
                if (get_options()->omnibus_type && def.is_bodymatter()) {
                    a->add_child_text(utils::xstrcat(to_string_view(def.vol), ": ", def.toc_label.value()));
                } else {
                    a->add_child_text(xmlpp::ustring(def.toc_label.value()));
                }

                return li;
            };


            auto make_named_ol = [root_ol](xmlpp::Element* parent, xmlpp::ustring title, xmlpp::ustring link) -> xmlpp::Element* {
                auto li = parent->add_child_element("li");
                li->set_attribute("style", "margin-top: 16px;");
//                auto span = li->add_child_element("span");
//                span->set_attribute("style", "font-weight:bold");
//                span->add_child_text(title);
                auto a = li->add_child_element("a");
                a->set_attribute("href", link);
                a->add_child_text(title);
                auto ol = li->add_child_element("ol");
                for (const auto &attr : root_ol->get_attributes()) {
                    ol->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                }
                return ol;
            };

            omnibus current_part = omnibus::PART1;
            volume current_volume = volume::FB1;
            xmlpp::Element* current_part_ol = nullptr;
            xmlpp::Element* current_volume_ol = nullptr;
            for (const auto& def : definition | std::ranges::views::filter([](const definition_t::value_type& def){ return def.toc_label.has_value(); }) ) {
                if (get_options()->do_nested && def.is_bodymatter()) {
                    auto [part, volume] = volume_map.at(def);
                    if (!current_part_ol || part != current_part) {
                        current_part = part;
                        current_volume = volume;
                        current_part_ol = make_named_ol(root_ol, xmlpp::ustring(to_string_view(part)), make_link(def));
                        current_volume_ol = make_named_ol(current_part_ol, xmlpp::ustring(to_string_view(volume)), make_link(def));
                    }

                    if (volume != current_volume) {
                        current_volume = volume;
                        current_volume_ol = make_named_ol(current_part_ol, xmlpp::ustring(to_string_view(volume)), make_link(def));
                    }

                    make_li(current_volume_ol, def);
                } else {
                    make_li(root_ol, def);
                }
            }
        }
    }

    result<fs::path>
    book_writer::make_book()
    {
        try {
            auto res = make_book_impl();
            if (res.has_failure()) {
                cleanup_dangling(name, filename);
                return res.as_failure();
            }
        } catch (std::system_error& e) {
            cleanup_dangling(name, filename);
            log_error("make_book: ", e.what());
            return e.code();
        } catch (std::exception& e) {
            cleanup_dangling(name, filename);
            log_error("make_book: ", e.what());
            return outcome::error_from_exception();
        }
        return filename;
    }

    book_writer::book_writer(volume base_volume,
                             const books_t &books,
                             const readers_t &readers,
                             std::variant<omnibus, volume> name,
                             std::ranges::input_range auto&& definition) :
        base_vol(base_volume),
        src_books(books),
        src_readers(readers),
        base_book(src_books.find(base_vol)->second),
        base_reader(src_readers.find(base_vol)->second),
        name(name),
        definition(std::ranges::begin(definition), std::ranges::end(definition)),
        filename(get_new_filename(name)),
        writer(filename)
    {
        auto res = base_reader->zip.get_files();
        if (res) {
            basefiles = std::move(res.value());
        } else {
            throw std::system_error(res.error());
        }
    }

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template<std::ranges::input_range R>
    result<void>
    bookmaker::make_books_impl(R&& view, std::variant<omnibus, volume> book)
    {
        if (std::holds_alternative<volume>(book)) {
            if (!src_books.contains(std::get<volume>(book))) {
                log_verbose("Info: ", std::get<volume>(book), " unavailable, skipping.");
                return std::errc::no_such_file_or_directory;
            }
        }

        if (std::ranges::none_of(view, [&](const auto &def) {
            return src_books.contains(std::get<volume>(def.name));
        })) {
            log_verbose("Info: No inputs to make ", book);
            return std::errc::no_such_file_or_directory;
        }

        try {
            auto res = std::visit(overloaded {
                        [&](omnibus o) -> result<fs::path> {
                            // x is a view of a list of (defs, volume_name) definiition_view_t
                            auto omnibus_def = utils::make_omnibus_def(get_filtered_defs(std::views::join(view), src_books, src_readers));
                            //auto first_chapter = std::ranges::find(omnibus_def, CHAPTER, &volume_definition::get_chapter_type);
                            auto base_volume = omnibus_def.front().vol;
                            auto writer = book_writer(base_volume, src_books, src_readers, o, omnibus_def);
                            return writer.make_book();
                        },
                            [&](volume v) -> result<fs::path> {
                            // x is a filter(views::single) of a (defs, volume_name) definiition_view_t
                            auto volume_def = get_filtered_defs(view.front(), src_books, src_readers);
                            auto writer = book_writer(v, src_books, src_readers, v, volume_def);
                            return writer.make_book();
                        }
                }, book);
            if (res.has_error()) {
                std::visit(overloaded {
                        [&](omnibus o) {
                            log_error("Couldn't make omnibus ", o, ": ", res.error().message());
                        },
                        [&](volume v) {
                            log_error("Couldn't make ", v, ": ", res.error().message(), ". Moving on...");
                        }
                }, book);
                return res.as_failure();
            } else {
                std::visit([&](auto&& arg) { log_info("Created chronologically ordered ", arg, ": ", res.value() ); }, book);
                return outcome::success();
            }
        } catch (std::system_error& e) {
            std::visit([&](auto&& arg) { log_error("Couldn't make writer for ", arg, ": ", e.what()); }, book);
            return e.code();
        } catch (...) {
            std::visit([&](auto&& arg) { log_error("Couldn't make writer for ", arg); }, book);
            return outcome::error_from_exception();
        }
    }

    result<void>
    bookmaker::make_books()
    {
        int created_books = 0;

        if (get_options()->omnibus_type) {
            auto defs = get_definition_view(get_options()->omnibus_type.value());
            OUTCOME_TRY(make_books_impl(std::move(defs), *get_options()->omnibus_type));
            ++created_books;
        } else {
            for (volume defined_volume : defined_volumes) {
                OUTCOME_TRY(make_books_impl(get_definition_view(defined_volume), defined_volume));
                ++created_books;
            }
        }

        log_info("Created ", created_books, " new books.");
        return outcome::success();
    }

    bookmaker::bookmaker(books_t&& books,
                         readers_t&& book_readers) :
            src_books(std::move(books)),
            src_readers(std::move(book_readers))
    {
    }
}
