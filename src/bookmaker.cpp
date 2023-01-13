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
#include "libxml++/validators/relaxngvalidator.h"
#include "utils.h"

namespace {
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
            const auto src = utils::strcat(root, def.href);

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


    std::filesystem::path get_new_filename(const std::filesystem::path& base) {
        int dups = 0;
        std::filesystem::path new_filename;
        do {
            std::string name;
            if (dups++ > 0) {
                name = utils::strcat(get_options()->prefix, base.stem().string(), get_options()->suffix, " (", dups, ")", base.extension().string());
            } else {
                name = utils::strcat(get_options()->prefix, base.stem().string(), get_options()->suffix,                  base.extension().string());
            }
            new_filename.assign(get_options()->output_dir);
            new_filename.append(name);
        } while (std::filesystem::exists(new_filename));
        return new_filename;
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
    template<std::ranges::input_range R>
    result<void>
    book_writer<R>::start_book()
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

    template<std::ranges::input_range R>
    result<std::string>
    book_writer<R>::get_ncx_id() const
    {
        auto defs = get_filtered_defs(definition, src_books, src_readers);
        auto it = std::ranges::find(defs, NCX, &volume_definition::get_chapter_type);
        if (defs.end() == it) {
            log_error("No NCX found in volume definition");
            return std::errc::invalid_argument;
        } else {
            return it->get_id();
        }
    }

    template<std::ranges::input_range R>
    result<std::string>
    book_writer<R>::create_rootfile()
    {
        xmlpp::Document doc;
        xmlpp::ustring first_chapter_path;
        xmlpp::ustring toc_path;
        OUTCOME_TRY(const auto base_vol, identify_volume(base_book.manifest.toc.dtb_uid));
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
                auto spine = dynamic_cast<xmlpp::Element*>(root->import_node(base_child, false));
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
                for (const auto &def : get_filtered_defs(definition, src_books, src_readers) | std::ranges::views::filter(&volume_definition::in_spine)) {
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
                auto metadata_node = dynamic_cast<xmlpp::Element*>(root->import_node(base_metadata, true));
                auto contributor = metadata_node->add_child_element("contributor", "dc");
                contributor->set_attribute("id", "contributor01");
                contributor->set_first_child_text("talisein");
                if (get_options()->omnibus_type) {
                    const xmlpp::Element::PrefixNsMap ns_map {{"dc", "http://purl.org/dc/elements/1.1/"}};
                    xmlpp::Node::NodeSet set = metadata_node->find("dc:title", ns_map);
                    if (!set.empty()) {
                        dynamic_cast<xmlpp::Element*>(set.front())->set_first_child_text(get_options()->title.value());
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
                for (const auto &def : get_filtered_defs(definition, src_books, src_readers)) {
                    const auto src_book = src_books.find(def.vol);
                    auto item = manifest->add_child_element("item");
                    item->set_attribute("id", def.get_id());

                    if (def.vol == base_vol && def.href == base_book.manifest.toc_relpath) {
                        // toc.ncx will stay in OEBPS
                        item->set_attribute("href", xmlpp::ustring(def.href));
                    } else {
                        auto href { utils::xstrcat(to_string_view(def.vol), "/", def.href) };
                        item->set_attribute("href", href);
                        if (first_chapter_path.empty() && chapter_type::CHAPTER == def.get_chapter_type()) {
                            first_chapter_path = href;
                        }
                        if (toc_path.empty() && chapter_type::TOC == def.get_chapter_type()) {
                            toc_path = href;
                        }
                    }

                    item->set_attribute("media-type", xmlpp::ustring(def.mediatype));

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
                auto guide = dynamic_cast<xmlpp::Element*>(root->import_node(base_child, false));
                for (const auto attr : dynamic_cast<const xmlpp::Element*>(base_child)->get_attributes()) {
                    guide->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                }
                { auto text = guide->add_child_element("reference");
                    text->set_attribute("type", "text");
                    text->set_attribute("title", "Text");
                    text->set_attribute("href", first_chapter_path); }
                { auto text = guide->add_child_element("reference");
                    text->set_attribute("type", "toc");
                    text->set_attribute("title", "Table of Contents");
                    text->set_attribute("href", toc_path); }
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

    template<std::ranges::input_range R>
    result<void>
    book_writer<R>::add_ncx()
    {
        const auto toc_fullpath = base_book.rootfile_path.substr(0, base_book.rootfile_path.find_first_of('/')+1).append(base_book.manifest.toc_relpath);
        auto it = std::ranges::find(basefiles, toc_fullpath);
        if (it == basefiles.end()) { log_error("Couldn't find toc in original? Basebook ", vol, "  Wasn't at ", toc_fullpath); return std::errc::no_such_file_or_directory; }
        basefiles.remove(toc_fullpath);
        OUTCOME_TRY(auto toc_buf, create_ncx(toc_fullpath));
        OUTCOME_TRY(writer.add(toc_fullpath, std::span<const char>(toc_buf), get_options()->compression_level));
        return outcome::success();
    }

    template<std::ranges::input_range R>
    result<std::string>
    book_writer<R>::create_ncx(const std::string& toc_fullpath)
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

        auto make_nav_point = [vol = vol, point = int{1}](const auto &def, xmlpp::Element* parent,
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
        for (const auto& def : get_filtered_defs(definition, src_books, src_readers)
                 | std::ranges::views::filter([](const auto& def) { return def.toc_label.has_value(); })) {
            if (get_options()->do_nested && chapter_type::CHAPTER == def.get_chapter_type()) {
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

    template<std::ranges::input_range R>
    result<void>
    book_writer<R>::make_book_impl()
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
        for (const volume_definition& def : get_filtered_defs(definition, src_books, src_readers))
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
                        nav_root->import_node(src_root_child, true);
                    } else if (src_root_child->get_name() == "body") {
                        auto body = dynamic_cast<xmlpp::Element*>(nav_root->import_node(src_root_child, false));
                        for (const auto &src_iter : src_root_child->get_children()) {
                            if (src_iter->get_name() == "nav") {
                                auto srcattr = dynamic_cast<xmlpp::Element*>(src_iter)->get_attribute("type", "epub");
                                bool is_toc = srcattr->get_value() == "toc";
                                if (is_toc) {
                                    auto nav = dynamic_cast<xmlpp::Element*>(body->import_node(src_iter, false));
                                    for (const auto &attr : dynamic_cast<xmlpp::Element*>(src_iter)->get_attributes()) {
                                        nav->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                                    }
                                    make_toc(nav, dynamic_cast<xmlpp::Element*>(src_iter));
                                    log_verbose("Made toc");
                                } else {
                                    body->import_node(src_iter, true);
                                }
                            }
                        }
                    }
                }
                auto str = doc.write_to_string();
                OUTCOME_TRY(writer.add(dst, std::span<const char>(str), get_options()->compression_level));
                basefiles.remove(src);
                continue;
            }

            // No special case, just copy it.
            log_verbose("Copying ", src, " to ", dst);
            OUTCOME_TRY(writer.copy_from(src_reader->zip, src, dst));
            log_verbose("Copying ", src, " to ", dst, ": done");
            if (def.vol == vol) {
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
        for (auto src : basefiles | std::ranges::views::filter([this](auto const &basefile) { return std::end(expected_leftovers) == std::ranges::find(expected_leftovers, std::make_pair(vol, basefile)); })) {
            log_info("Warning: left over file in src ", to_string_view(vol), ": ", src);
        }
        OUTCOME_TRY(writer.finish());
        return outcome::success();
    }

    template<std::ranges::input_range R>
    void
    book_writer<R>::make_toc(xmlpp::Element *nav, const xmlpp::Element* src_nav)
    {
        static const auto volume_map = get_volume_map();

        for (const auto &child : src_nav->get_children()) {
            if (child->get_name() != "ol") {
                nav->import_node(child, true);
                continue;
            }
            auto root_ol = dynamic_cast<xmlpp::Element*>(nav->import_node(child, false));
            for (const auto& attr : dynamic_cast<const xmlpp::Element*>(child)->get_attributes()) {
                root_ol->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
            }

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
                return utils::xstrcat("../../", to_string_view(def.vol), "/", def.href);
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
                if (!set.empty() && def.vol == vol) {
                    auto src_li = dynamic_cast<const xmlpp::Element*>(set.front()->get_parent());
                    for (const auto &attr : src_li->get_attributes()) {
                        li->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                    }
                } else {
                    li->set_attribute("class", "toc-chapter");
                    li->set_attribute("id", utils::xstrcat("chrono-chapter"sv, idx++));
                }
                auto a = li->add_child_element("a");
                a->set_attribute("href", make_link(def));
                if (get_options()->omnibus_type) {
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
            for (const auto& def : get_filtered_defs(definition, src_books, src_readers) | std::ranges::views::filter([](const definition_t::value_type& def){ return def.toc_label.has_value(); }) ) {
                if (get_options()->do_nested && chapter_type::CHAPTER == def.get_chapter_type()) {
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

    template<std::ranges::input_range R>
    result<fs::path>
    book_writer<R>::make_book()
    {
        try {
            auto res = make_book_impl();
            if (res.has_failure()) {
                cleanup_dangling(definition.name, filename);
                return res.as_failure();
            }
        } catch (std::system_error& e) {
            cleanup_dangling(definition.name, filename);
            log_error("make_book: ", e.what());
            return e.code();
        } catch (std::exception& e) {
            cleanup_dangling(definition.name, filename);
            log_error("make_book: ", e.what());
            return outcome::error_from_exception();
        }
        return filename;
    }

    template<std::ranges::input_range R>
    book_writer<R>::book_writer(volume base_volume,
                                const books_t &books,
                                const readers_t &readers,
                                R def) :
        vol(base_volume),
        src_books(books),
        src_readers(readers),
        base_book(src_books.find(vol)->second),
        base_reader(src_readers.find(vol)->second),
        definition(def),
        filename(get_new_filename(base_reader->path)),
        writer(filename)
    {
        auto res = base_reader->zip.get_files();
        if (res) {
            basefiles = std::move(res.value());
        } else {
            std::system_error e(res.error());
            throw e;
        }
    }

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

    template<std::ranges::input_range R>
    result<void>
    bookmaker::make_books_impl(R&& view, std::variant<omnibus, volume> base)
    {
        if (std::holds_alternative<volume>(base)) {
            auto basebook = src_books.find(std::get<volume>(base));
            if (std::end(src_books) == basebook) {
                log_verbose("Info: ", std::get<volume>(base), " unavailable, skipping.");
                return std::errc::no_such_file_or_directory;
            }
        }

        auto x = std::views::filter(view,
                                    [&](const auto &def) {
                                        return src_books.contains(std::get<volume>(def.name));
                                    });

        if (std::ranges::empty(x)) {
            log_verbose("Info: No inputs to make ", base);
            return std::errc::no_such_file_or_directory;
        }


        try {
            auto res = std::visit(overloaded {
                        [&](omnibus o) -> result<fs::path> {

                            auto base_volume = std::get<volume>(x.begin()->name);
                            auto omnibus_def = utils::make_omnibus_def(std::views::join(x));
                            auto omnibus_def_view = definition_view_t(std::views::all(omnibus_def), o);

                            auto writer = book_writer<decltype(omnibus_def_view)>(base_volume, src_books, src_readers, omnibus_def_view);
                            return writer.make_book();
                        },
                        [&](volume) -> result<fs::path> {
                            auto base_volume = std::get<volume>(x.begin()->name);
                            auto writer = book_writer<decltype(x.front())>(base_volume, src_books, src_readers, x.front());
                            return writer.make_book();
                        }
                }, base);
            if (res.has_error()) {
                std::visit([&](auto&& arg) { log_error("Couldn't make ", arg, ": ", res.error().message(), ". Moving on..."); }, base);
                return res.as_failure();
            } else {
                std::visit([&](auto&& arg) { log_info("Created chronologically ordered ", arg, ": ", res.value() ); }, base);
                return outcome::success();
            }
        } catch (std::system_error& e) {
            std::visit([&](auto&& arg) { log_error("Couldn't make writer for ", arg, ": ", e.what(), ". Moving on..."); }, base);
            return e.code();
        } catch (std::exception& e) {
            std::visit([&](auto&& arg) { log_error("Couldn't make writer for ", arg, ": ", e.what(), ". Moving on..."); }, base);
            return outcome::error_from_exception();
        }
    }

    result<void>
    bookmaker::make_books()
    {
        int created_books = 0;

        if (get_options()->omnibus_type) {
            auto defs = get_definition_view(get_options()->omnibus_type.value());
            auto res = make_books_impl(std::move(defs), *get_options()->omnibus_type);
            if (res) {
                ++created_books;
            } else {
                log_info(res.error());
            }
        } else {
            for (volume defined_volume : defined_volumes) {
                auto res = make_books_impl(get_definition_view(defined_volume), defined_volume);
                if (res) {
                    ++created_books;
                }
            }
        }
        if (0 == created_books) {
            log_info("Sorry, no books could be created.");
            return std::errc::no_such_file_or_directory;
        }
        return outcome::success();
    }

    bookmaker::bookmaker(books_t&& books,
                         readers_t&& book_readers) :
            src_books(std::move(books)),
            src_readers(std::move(book_readers))
    {
    }
}
