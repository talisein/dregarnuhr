#include <algorithm>
#include <ranges>
#include <span>
#include <set>

#include "bookmaker.h"
#include "volumes.h"
#include "log.h"
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "outcome/utils.hpp"
#include "outcome/try.hpp"
#include "jpeglib.h"
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

    constexpr epub::definition_t get_definition_view(volume v)
    {
        switch (v)
        {
            case volume::P1V1:
                return std::span(part_1::vol_1.begin(), part_1::vol_1.end());
            case volume::P1V2:
                return std::span(part_1::vol_2.begin(), part_1::vol_2.end());
            case volume::P1V3:
                return std::span(part_1::vol_3.begin(), part_1::vol_3.end());
            case volume::P2V1:
                return std::span(part_2::vol_1.begin(), part_2::vol_1.end());
            case volume::P2V2:
                return std::span(part_2::vol_2.begin(), part_2::vol_2.end());
            case volume::P2V3:
                return std::span(part_2::vol_3.begin(), part_2::vol_3.end());
            case volume::P2V4:
                return std::span(part_2::vol_4.begin(), part_2::vol_4.end());
            case volume::P3V1:
                return std::span(part_3::vol_1.begin(), part_3::vol_1.end());
            case volume::P3V2:
                return std::span(part_3::vol_2.begin(), part_3::vol_2.end());
            case volume::P3V3:
                return std::span(part_3::vol_3.begin(), part_3::vol_3.end());
            case volume::P3V4:
                return std::span(part_3::vol_4.begin(), part_3::vol_4.end());
            case volume::P3V5:
                return std::span(part_3::vol_5.begin(), part_3::vol_5.end());
            case volume::P4V1:
                return std::span(part_4::vol_1.begin(), part_4::vol_1.end());
            case volume::P4V2:
                return std::span(part_4::vol_2.begin(), part_4::vol_2.end());
            case volume::P4V3:
                return std::span(part_4::vol_3.begin(), part_4::vol_3.end());
            case volume::P4V4:
                return std::span(part_4::vol_4.begin(), part_4::vol_4.end());
            case volume::P4V5:
                return std::span(part_4::vol_5.begin(), part_4::vol_5.end());
            case volume::P4V6:
                return std::span(part_4::vol_6.begin(), part_4::vol_6.end());
            case volume::P4V7:
                return std::span(part_4::vol_7.begin(), part_4::vol_7.end());
            case volume::P4V8:
                return std::span(part_4::vol_8.begin(), part_4::vol_8.end());
            case volume::FB1: [[fallthrough]];
            case volume::FB2: [[fallthrough]];
            case volume::RA1:
                assert(false);
              }
        assert(false);
        return std::span(part_1::vol_1.end(), part_1::vol_1.end());
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
        OUTCOME_TRY(auto rootfile_buf, create_rootfile());
        OUTCOME_TRY(writer.add(rootfile_path, rootfile_buf));
        return outcome::success();
    }

    // TODO: filters, and missing volumes like fanbook, should be excluded in the rootfile and toc
    result<std::string>
    book_writer::create_rootfile()
    {
        xmlpp::Document doc;
        OUTCOME_TRY(auto idx, base_reader->zip.locate_file(base_book.rootfile_path.c_str()));
        OUTCOME_TRY(epub::file_reader f, base_reader->zip.extract_stream(idx));
        OUTCOME_TRY(const auto metadata_src, base_reader->get_metadata(f.get_doc()));
        auto src_root = f.get_doc()->get_root_node();
        auto src_root_attrs = src_root->get_attributes();
        auto root = doc.create_root_node_by_import(f.get_doc()->get_root_node(), false);
        for (const auto& attr : src_root_attrs) {
            root->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
        }
        root->set_namespace_declaration(src_root->get_namespace_uri(), src_root->get_namespace_prefix());
        auto metadata_node = root->import_node(metadata_src, true);
        auto mynode = metadata_node->get_first_child()->get_parent()->add_child_element("contributor", "dc");
        mynode->set_attribute("id", "contributor01");
        mynode->set_first_child_text("talisein");
        auto manifest = root->add_child_element("manifest");
        for (const auto &it : definition) {
            auto item = manifest->add_child_element("item");
            item->set_attribute("id", xmlpp::ustring(it.id));

            // basebook just stay in normal place
            OUTCOME_TRY(auto base_vol, identify_volume(base_book.manifest.toc.dtb_uid));
            if (it.vol == base_vol) {
            // toc.ncx will stay in OEBPS
//            if (it.href == base_book.manifest.toc_relpath) {
                item->set_attribute("href", xmlpp::ustring(it.href));
            } else {
                xmlpp::ustring href(to_string_view(it.vol));
                href.append("/").append(it.href);
                item->set_attribute("href", href);
            }

            item->set_attribute("media-type", xmlpp::ustring(it.mediatype));
            // TODO: make generic
            if (it.id == "Cover.jpg") {
                item->set_attribute("properties", "cover-image");
            }
            if (it.id == "toc" || it.id == "toc.xhtml") {
                item->set_attribute("properties", "nav");
            }
        }

        auto src_root_children = src_root->get_children();
        for (auto child : src_root_children) {
            if (child->get_name() == "spine") {
                auto spine = dynamic_cast<xmlpp::Element*>(root->import_node(child, false));
                for (auto attr : child->get_first_child()->get_parent()->get_attributes()) {
                    spine->set_attribute(attr->get_name(), attr->get_value(), attr->get_namespace_prefix());
                }

                for (const auto &def : definition | std::ranges::views::filter([](const auto& x){return x.in_spine;})) {
                    auto itemref = spine->add_child_element("itemref");
                    itemref->set_attribute("idref", xmlpp::ustring(def.id));
                    if (def.id == "toc") {
                        itemref->set_attribute("linear", "yes");
                    }
                }
            } else if (child->get_name() == "guide") {
                root->import_node(child, true);
            }
        }
        xmlpp::RelaxNGValidator validator;
//        auto schema = DTD::get_package();
//        validator.set_schema(&schema, false);
//        validator.validate(&doc);
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
        OUTCOME_TRY(writer.add(toc_fullpath, toc_buf));
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
        for (const auto& def : definition
                 | std::ranges::views::filter([](const auto& def) { return def.toc_label.has_value(); })) {
            auto navPoint = navmap->add_child_element("navPoint");
            ss.str("");
            ss << "navPoint" << point++;
            navPoint->set_attribute("id", ss.str());
            auto navLabel = navPoint->add_child_element("navLabel");
            auto text = navLabel->add_child_element("text");
            text->add_child_text(xmlpp::ustring(*def.toc_label));
            auto content = navPoint->add_child_element("content");
            content->set_attribute("src", xmlpp::ustring(def.href));
        }

        return doc.write_to_string_formatted();
    }

    result<void>
    book_writer::make_book_impl()
    {
        OUTCOME_TRY(start_book());
        for (const volume_definition& def : definition)
//                     | std::ranges::views::filter([&base_book](const volume_definition& def) { return def.href != base_book.manifest.toc_relpath; }))
        {
            if (def.href == base_book.manifest.toc_relpath) {
                OUTCOME_TRY(add_ncx());
                continue;
            }
            auto reader_it = src_readers.find(def.vol);
            if (src_readers.end() == reader_it) {
                log_verbose("Volume ", to_string_view(def.vol), " is unavailable. Skipping insertion.");
                continue;
            }
            auto& reader = reader_it->second;
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
            if (get_options()->size_filter) {
                OUTCOME_TRY(const auto src_idx, reader->zip.locate_file(src.c_str()));
                OUTCOME_TRY(const auto src_stat, reader->zip.stat(src_idx));
                if (src_stat.m_uncomp_size > get_options()->size_filter.value()) {
                    log_info("Filter: ", to_string_view(def.vol), " ", src, " filtered for size ", src_stat.m_uncomp_size);
                    basefiles.remove(src);
                    continue;
                }
            }
            if (get_options()->name_filter) {
                if (std::regex_search(src, get_options()->name_filter.value())) {
                    log_info("Filter: ", to_string_view(def.vol), " ", src, " filtered for name");
                    basefiles.remove(src);
                    continue;
                  }
            }
            if (src.ends_with(".jpg") && get_options()->jpg_quality) {
                OUTCOME_TRY(const auto idx, reader->zip.locate_file(src.c_str()));
                OUTCOME_TRY(const auto buf, reader->zip.extract_raw(idx));
                struct jpeg_decompress_struct dinfo;
                struct jpeg_compress_struct cinfo;
                struct jpeg_error_mgr cjerr;
                struct jpeg_error_mgr djerr;
                cinfo.err = jpeg_std_error(&cjerr);
                dinfo.err = jpeg_std_error(&djerr);
                jpeg_create_compress(&cinfo);
                jpeg_create_decompress(&dinfo);
                jpeg_mem_src(&dinfo, buf.data(), buf.size());
                unsigned char* outbuf = nullptr;
                unsigned long outbuf_len = 0;
                jpeg_mem_dest(&cinfo, &outbuf, &outbuf_len);
                jpeg_read_header(&dinfo, true);
                jpeg_calc_output_dimensions(&dinfo);
                cinfo.image_width = dinfo.image_width;
                cinfo.image_height = dinfo.image_height;
                cinfo.input_components = dinfo.output_components;
                cinfo.in_color_space = dinfo.out_color_space;
                jpeg_set_defaults(&cinfo);
                jpeg_set_quality(&cinfo, get_options()->jpg_quality.value(), true);
                jpeg_start_decompress(&dinfo);
                jpeg_start_compress(&cinfo, true);
                auto row_stride = dinfo.output_width * dinfo.output_components;
                auto buffer = (*dinfo.mem->alloc_sarray) ((j_common_ptr)&dinfo, JPOOL_IMAGE, row_stride, 1);
                while (dinfo.output_scanline < dinfo.output_height) {
                    auto dim_read = jpeg_read_scanlines(&dinfo, buffer, 1);
                    auto dim_wrote [[maybe_unused]] = jpeg_write_scanlines(&cinfo, buffer, dim_read);
                }
                jpeg_finish_decompress(&dinfo);
                jpeg_finish_compress(&cinfo);
                jpeg_destroy_decompress(&dinfo);
                OUTCOME_TRY(writer.add(src, std::span<const char>(reinterpret_cast<char*>(outbuf), outbuf_len)));
                basefiles.remove(src);
                continue;
            }
            OUTCOME_TRY(writer.copy_from(reader->zip, src, dst));
            basefiles.remove(src);
            continue;
        }
        for (auto src : basefiles) {
            // P2V4extra8
            log_info("Warning: left over file in src: ", src, ". This is ok if it was moved to an earlier volume.");
        }
        OUTCOME_TRY(writer.finish());
        return outcome::success();
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

    result<void>
    bookmaker::make_books()
    {
        int created_books = 0;
        for (auto defined_volume : defined_volumes) {
            const definition_t view = get_definition_view(defined_volume);
            std::set<volume> located_inputs;
            for (const auto& def : view) {
                if (src_books.end() != src_books.find(def.vol))
                    located_inputs.insert(def.vol);
            }
            if (located_inputs.size() == 0) {
                log_verbose("Info: No inputs to make ", to_string_view(defined_volume));
                continue;
            }
            try {
                book_writer writer(defined_volume, src_books, src_readers, view);
                auto res = writer.make_book();
                if (res.has_error()) {
                    log_error("Couldn't make ", to_string_view(defined_volume), ": ", res.error().message(), ". Moving on...");
                } else {
                    log_info("Created chronologically ordered ", to_string_view(defined_volume), ": ", res.value() );
                    ++created_books;
                }
            } catch (std::exception& e) {
                log_error("Couldn't make writer for ", to_string_view(defined_volume), ": ", e.what(), ". Moving on...");
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
