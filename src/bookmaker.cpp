#include <algorithm>
#include <array>
#include <ranges>
#include <span>
#include <set>
#include <string>

#include "bookmaker.h"
#include "volumes.h"
#include "log.h"
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "outcome/utils.hpp"
#include "outcome/try.hpp"
#include "jpeg.h"
#include "libxml++/validators/dtdvalidator.h"
#include "libxml++/validators/relaxngvalidator.h"
#include "dtd.h"

namespace {

    constinit std::array defined_volumes = std::to_array({
            volume::P1V1,
            volume::P1V2,
            volume::P1V3,
            volume::P2V1,
            volume::P2V2,
            volume::P2V3,
            volume::P2V4,
            volume::P3V1,
            volume::P3V2,
            volume::P3V3,
            volume::P3V4,
            volume::P3V5,
            volume::P4V1,
            volume::P4V2,
            volume::P4V3,
            volume::P4V4,
            volume::P4V5,
            volume::P4V6,
            volume::P4V7,
        });

    constexpr epub::definition_t get_definition_view(omnibus o)
    {
        switch (o)
        {
            case omnibus::PART1:
                return part_1::get_part_1();
            case omnibus::PART2:
                return part_2::get_part_2();
            case omnibus::PART3:
                return part_3::get_part_3();
            case omnibus::PART4:
                return part_4::get_part_4();
            case omnibus::ALL:
                assert(false);
                return part_3::get_part_3();
        }
        assert(false);
        return part_3::get_part_3();
    }

    constexpr epub::definition_t get_definition_view(volume v)
    {
        switch (v)
        {
            case volume::P1V1:
                return part_1::get_vol_1();
            case volume::P1V2:
                return part_1::get_vol_2();
            case volume::P1V3:
                return part_1::get_vol_3();
            case volume::P2V1:
                return part_2::get_vol_1();
            case volume::P2V2:
                return part_2::get_vol_2();
            case volume::P2V3:
                return part_2::get_vol_3();
            case volume::P2V4:
                return part_2::get_vol_4();
            case volume::P3V1:
                return part_3::get_vol_1();
            case volume::P3V2:
                return part_3::get_vol_2();
            case volume::P3V3:
                return part_3::get_vol_3();
            case volume::P3V4:
                return part_3::get_vol_4();
            case volume::P3V5:
                return part_3::get_vol_5();
            case volume::P4V1:
                return part_4::get_vol_1();
            case volume::P4V2:
                return part_4::get_vol_2();
            case volume::P4V3:
                return part_4::get_vol_3();
            case volume::P4V4:
                return part_4::get_vol_4();
            case volume::P4V5:
                return part_4::get_vol_5();
            case volume::P4V6:
                return part_4::get_vol_6();
            case volume::P4V7:
                return part_4::get_vol_7();
            case volume::P4V8: [[fallthrough]];
            case volume::FB1: [[fallthrough]];
            case volume::FB2: [[fallthrough]];
            case volume::RA1:
                assert(false);
              }
        assert(false);
        auto x = part_1::get_vol_1();
        return std::span(x.end(), x.end());
    }

    auto get_filtered_defs(epub::definition_t defs, const epub::books_t& src_books, const epub::readers_t& src_readers)
    {
        return std::ranges::views::filter(defs, [&src_books, &src_readers](const volume_definition& def) {
            const auto& src_reader = src_readers.find(def.vol)->second;
            const auto& src_book = src_books.find(def.vol)->second;
            const auto root = src_book.rootfile_path.substr(0, src_book.rootfile_path.find_first_of('/')+1);
            auto src {root};
            src.append(def.href);

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
        std::stringstream ss;
        int dups = 0;
        std::filesystem::path new_filename;
        do {
            ss.str("");
            if (get_options()->prefix) ss << *get_options()->prefix;
            ss << base.stem().string();
            if (get_options()->suffix) ss << *get_options()->suffix;
            if (dups++ > 0)
                ss << " (" << dups << ")";
            ss << base.extension().string();
            new_filename.assign(get_options()->output_dir);
            new_filename.append(ss.view());
        } while (std::filesystem::exists(new_filename));
        return new_filename;
    }

    void cleanup_dangling(volume vol, const std::filesystem::path& filename) {
        std::error_code ec;
        bool exists = std::filesystem::exists(filename, ec);
        if (ec) {
            log_error("Failed to cleanup ", filename.string(), ": ", ec.message());
            return;
        }
        if (exists) {
            auto is_regular = std::filesystem::is_regular_file(filename, ec);
            if (ec) {
                log_error("Failed to create book ", to_string_view(vol), ". Dangling file ", filename, " is in unexpected state, got error: ", ec.message(), ". Clean it up yourself. This shouldn't happen!");
                return;
            }
            if (!is_regular) {
                log_error("Failed to create book ", to_string_view(vol), ". Dangling file ", filename, " is somehow not even a regular file. Something's gone very wrong, aborting.");
                return;
            }
            if (!std::filesystem::remove(filename, ec) || ec) {
                log_error("Failed to create book ", to_string_view(vol), ". Dangling file ", filename, " couldn't be cleaned up, error: ", ec.message());
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
        OUTCOME_TRY(writer.add(rootfile_path, std::span<const char>(rootfile_buf)));
        return outcome::success();
    }

    // TODO: filters, and missing volumes like fanbook, should be excluded in the rootfile and toc
    result<std::string>
    book_writer::create_rootfile()
    {
        xmlpp::Document doc;
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
                    spine->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                }
                for (const auto &def : get_filtered_defs(definition, src_books, src_readers) | std::ranges::views::filter([](const auto& x){return x.in_spine;})) {
                    auto itemref = spine->add_child_element("itemref");
                    itemref->set_attribute("idref", xmlpp::ustring(def.id));
                    //meow
                    auto src_book = src_books.find(def.vol);
                    auto src_iter = std::ranges::find_if(src_book->second.manifest.items, [&def](const auto &item) { return item.id == def.id; });
                    if (src_iter != std::end(src_book->second.manifest.items)) {
                        if (src_iter->spine_properties) {
                            itemref->set_attribute("properties", src_iter->spine_properties.value());
                        }
                        if (src_iter->is_linear) {
                            itemref->set_attribute("linear", "yes");
                        }
                    }
                }
            } else if (base_child->get_name() == "metadata") {
                OUTCOME_TRY(const auto base_metadata, base_reader->get_metadata(f.get_doc()));
                auto metadata_node = dynamic_cast<xmlpp::Element*>(root->import_node(base_metadata, true));
                auto contributor = metadata_node->add_child_element("contributor", "dc");
                contributor->set_attribute("id", "contributor01");
                contributor->set_first_child_text("talisein");
            } else if (base_child->get_name() == "manifest") {
                auto manifest = root->add_child_element("manifest");
                for (const auto &def : get_filtered_defs(definition, src_books, src_readers)) {
                    const auto src_book = src_books.find(def.vol);
                    auto item = manifest->add_child_element("item");
                    item->set_attribute("id", xmlpp::ustring(def.id));
                    // basebook just stay in normal place
                    if (def.vol == base_vol) {
                        // toc.ncx will stay in OEBPS
//                  if (def.href == base_book.manifest.toc_relpath) {
                        item->set_attribute("href", xmlpp::ustring(def.href));
                    } else {
                        xmlpp::ustring href(to_string_view(def.vol));
                        href.append("/").append(def.href);
                        item->set_attribute("href", href);
                    }

                    item->set_attribute("media-type", xmlpp::ustring(def.mediatype));

                    // TODO: Maybe just use the source for most of the above properties
                    auto src_iter = std::ranges::find_if(src_book->second.manifest.items, [&def](const auto &item) {
                            return def.id == item.id;
                        });
                    if (src_iter != src_book->second.manifest.items.end() && src_iter->properties) {
                        item->set_attribute("properties", src_iter->properties.value());
                    }
                }
            } else if (base_child->get_name() == "guide") {
                root->import_node(base_child, true);
            } else {
                auto x = dynamic_cast<const xmlpp::TextNode*>(base_child);
                if (!x) {
                    log_info("Unexpected epub package node ", base_child->get_name());
                    root->import_node(base_child, true);
                }
            }
        }

        // TODO: Validation
        return doc.write_to_string_formatted();
    }

    result<void>
    book_writer::add_ncx()
    {
        const auto toc_fullpath = base_book.rootfile_path.substr(0, base_book.rootfile_path.find_first_of('/')+1).append(base_book.manifest.toc_relpath);
        auto it = std::ranges::find(basefiles, toc_fullpath);
        if (it == basefiles.end()) { log_error("Couldn't find toc in original?"); return std::errc::no_such_file_or_directory; }
        basefiles.remove(toc_fullpath);
        OUTCOME_TRY(auto toc_buf, create_ncx(toc_fullpath));
        OUTCOME_TRY(writer.add(toc_fullpath, std::span<const char>(toc_buf)));
        return outcome::success();
    }

    result<std::string>
    book_writer::create_ncx(const std::string& toc_fullpath)
    {
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
        int point = 1;
        std::stringstream ss;
        for (const auto& def : get_filtered_defs(definition, src_books, src_readers)
                 | std::ranges::views::filter([](const auto& def) { return def.toc_label.has_value(); })) {
            auto navPoint = navmap->add_child_element("navPoint");
            ss.str("");
            ss << "navPoint" << point++;
            navPoint->set_attribute("id", ss.str());
            auto navLabel = navPoint->add_child_element("navLabel");
            auto text = navLabel->add_child_element("text");
            text->add_child_text(xmlpp::ustring(*def.toc_label));
            auto content = navPoint->add_child_element("content");
            ss.str("");
            if (vol == def.vol) {
                ss << def.href;
            } else {
                ss << to_string_view(def.vol) << "/" << def.href;
            }
            content->set_attribute("src", ss.str());
        }

        return doc.write_to_string_formatted();
    }

    result<void>
    book_writer::make_book_impl()
    {
        OUTCOME_TRY(start_book());
        for (const volume_definition& def : get_filtered_defs(definition, src_books, src_readers))
//                     | std::ranges::views::filter([&base_book](const volume_definition& def) { return def.href != base_book.manifest.toc_relpath; }))
        {
            if (def.href == base_book.manifest.toc_relpath) {
                OUTCOME_TRY(add_ncx());
                continue;
            }
            auto src_reader_iter = src_readers.find(def.vol);
            if (src_readers.end() == src_reader_iter) {
                log_verbose("Volume ", to_string_view(def.vol), " is unavailable. Skipping insertion.");
                continue;
            }
            const auto& src_reader = src_reader_iter->second;
            const auto& src_book = src_books.find(def.vol)->second;
            const auto& src_item = std::ranges::find_if(src_book.manifest.items, [&id = def.id](const auto &item) { return id == item.id; });
            std::optional<manifest::item> item;
            if (std::end(src_book.manifest.items) != src_item) {
                item = std::make_optional<manifest::item>(*src_item);
            }

            // Determine src and dst paths. dst will be e.g. OEBPS/FB1/Text. Basebook is just OEBPS/Text
            const auto root = base_book.rootfile_path.substr(0, base_book.rootfile_path.find_first_of('/')+1);
            auto src {root};
            src.append(def.href);
            auto dst {root};
            dst.append(to_string_view(def.vol));
            dst.append("/");
            dst.append(def.href);
            if (def.vol == vol) {
                dst.assign(src);
            }

            // Transformations
            if (src.ends_with(".jpg") && (get_options()->jpg_quality || get_options()->jpg_scale)) {
                OUTCOME_TRY(const auto idx, src_reader->zip.locate_file(src.c_str()));
                OUTCOME_TRY(const auto buf, src_reader->zip.extract_raw(idx));
                jpeg::compressor comp;
                jpeg::decompressor decomp {buf};
                OUTCOME_TRY(const auto outbuf, comp.compress_from(decomp, get_options()->jpg_quality, get_options()->jpg_scale));
                OUTCOME_TRY(writer.add(dst, outbuf));
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
                auto root = doc.create_root_node_by_import(src_root, false);
                root->set_namespace_declaration(src_root->get_namespace_uri());
                root->set_namespace_declaration("http://www.idpf.org/2007/ops", "epub");
                for (auto attr : src_root->get_attributes()) {
                    root->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                }
                auto dtd = src_parser.get_document()->get_internal_subset();
                doc.set_internal_subset(dtd->get_name(), dtd->get_external_id(), dtd->get_system_id());
                for (const auto& src_root_child : src_root->get_children()) {
                    if (src_root_child->get_name() == "head") {
                        root->import_node(src_root_child, true);
                    } else if (src_root_child->get_name() == "body") {
                        auto body = dynamic_cast<xmlpp::Element*>(root->import_node(src_root_child, false));
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
                                } else {
                                    body->import_node(src_iter, true);
                                }
                            }
                        }
                    }
                }
                auto str = doc.write_to_string();
                OUTCOME_TRY(writer.add(dst, std::span<const char>(str)));
                basefiles.remove(src);
                continue;
            }

            // No special case, just copy it.
            OUTCOME_TRY(writer.copy_from(src_reader->zip, src, dst));
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

    void
    book_writer::make_toc(xmlpp::Element *nav, const xmlpp::Element* src_nav)
    {
        for (const auto &child : src_nav->get_children()) {
            if (child->get_name() != "ol") {
                nav->import_node(child, true);
                continue;
            }
            auto ol = dynamic_cast<xmlpp::Element*>(nav->import_node(child, false));
            for (const auto& attr : dynamic_cast<const xmlpp::Element*>(child)->get_attributes()) {
                ol->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
            }
            auto idx = 1;
            std::stringstream ss;
            for (const auto& def : get_filtered_defs(definition, src_books, src_readers) | std::ranges::views::filter([](const definition_t::value_type& def){ return def.toc_label.has_value(); }) ) {
                xmlpp::Element::PrefixNsMap map;
                map.insert({"html", "http://www.w3.org/1999/xhtml"});
                using namespace std::string_literals;
                xmlpp::Node::const_NodeSet set = src_nav->find("html:ol/html:li/html:a[@href='../"s + std::string(def.href) + "']"s, map);
                if (set.empty()) { // Sometimes it is "chapter1.html" instead of "../Text/chapter1.html"
                    set = src_nav->find("html:ol/html:li/html:a[@href='"s + std::string(def.href.substr(def.href.find_last_of('/')+1)) + "']"s, map);
                }
                auto li = ol->add_child_element("li");
                if (!set.empty() && def.vol == vol) {
                    auto src_li = dynamic_cast<const xmlpp::Element*>(set.front()->get_parent());
                    for (const auto &attr : src_li->get_attributes()) {
                        li->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                    }
                } else {
                    ss.str("");
                    ss << "chrono-chapter" << idx++;
                    li->set_attribute("class", "toc-chapter");
                    li->set_attribute("id", ss.str());
                }
                auto a = li->add_child_element("a");
                ss.str("");
                if (def.vol == vol) { // basevol has same path
                    ss << "../" << def.href;
                } else {
                    ss << "../" << to_string_view(def.vol) << "/" << def.href;
                }
                a->set_attribute("href", ss.str());
                a->add_child_text(xmlpp::ustring(def.toc_label.value()));
            }
        }
    }

    result<fs::path>
    book_writer::make_book()
    {
        try {
            auto res = make_book_impl();
            if (res.has_failure()) {
                cleanup_dangling(vol, filename);
                return res.as_failure();
            }
        } catch (std::exception& e) {
            cleanup_dangling(vol, filename);
            log_error("make_book: ", e.what());
            return outcome::error_from_exception();
        }
        return filename;
    }

    book_writer::book_writer(volume volume,
                             const books_t &books,
                             const readers_t &readers,
                             definition_t def) :
        vol(volume),
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
    // changes must be mirrored above
    result<void>
    bookmaker::make_books_impl(definition_t view, std::variant<volume, omnibus> base)
    {
        std::set<volume> located_inputs;
        for (const auto& def : view) {
            if (src_books.end() != src_books.find(def.vol))
                located_inputs.insert(def.vol);
        }

        if (located_inputs.size() == 0) {
            std::visit([](auto&& arg) { log_verbose("Info: No inputs to make ", arg); }, base);
            return std::errc::no_such_file_or_directory;
        }

        try {
            auto res = std::visit(overloaded {
                    [&](omnibus o) { book_writer writer(view.begin()->vol, src_books, src_readers, view); return writer.make_book(); },
                    [&](volume v) { book_writer writer(v, src_books, src_readers, view); return writer.make_book();}
                }, base);
            if (res.has_error()) {
                std::visit([&](auto&& arg) { log_error("Couldn't make ", arg, ": ", res.error().message(), ". Moving on..."); }, base);
                return res.as_failure();
            } else {
                std::visit([&](auto&& arg) { log_info("Created chronologically ordered ", arg, ": ", res.value() ); }, base);
                return outcome::success();
            }
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
            auto res = make_books_impl(get_definition_view(get_options()->omnibus_type.value()), *get_options()->omnibus_type);
            if (res) {
                ++created_books;
            }
        } else {
            for (auto defined_volume : defined_volumes) {
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
