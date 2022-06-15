#include <iostream>
#include <filesystem>
#include <type_traits>
#include <source_location>
#include <cassert>
#include <libxml++/libxml++.h>
#include <outcome/try.hpp>
#include <outcome/utils.hpp>
#include "epub.h"
#include "log.h"
namespace
{
    constexpr bool verify_mimetype(std::string_view mimetype)
    {
        constexpr std::string_view expected_mimetype {"application/epub+zip"};
        return expected_mimetype == mimetype;
    }

    result<void> handle_xmlpp_exception(std::string_view filename, const std::source_location& src = std::source_location::current())
    {
        try {
            throw;
        } catch (xmlpp::internal_error &e) {
            log_error(filename, ": xml internal_error from ", src.function_name(), ":", src.line(), ": ", e.what());
            return std::errc::invalid_argument;
        } catch (xmlpp::validity_error &e) {

            return std::errc::invalid_argument;
        } catch (xmlpp::parse_error &e) {
            log_error(filename, ": xml parse_error from ",
                      src.function_name(), ":", src.line(), ": ", e.what());
            return std::errc::invalid_argument;
        } catch (xmlpp::exception &e) {
            log_error(filename, ": xml parse_error from ", src.function_name(), ":", src.line(), ": ", e.what());
            return std::errc::invalid_argument;
        } catch (std::exception &e) {
            log_error(filename, ": exception from ", src.function_name(), ":", src.line(), ": ", e.what());
            return outcome::error_from_exception();
        } catch (...) {
            log_error(filename, ": unexpected exception in", src.function_name(), ":", src.line());
            return outcome::error_from_exception();
        }
        assert(false); // unreachable
        return std::errc::invalid_argument;
    }

    template<typename T>
    result<xmlpp::Node::const_NodeSet> find_quiet(const xmlpp::ustring& xpath,
                                                  const xmlpp::Element::PrefixNsMap& map,
                                                  T ele,
                                                  const std::string &filename)
    {
        xmlpp::Node::const_NodeSet set;
        try {
            set = ele->find(xpath, map);
            if (set.size() == 0) {
                return std::errc::invalid_argument;
            }
            return set;
        } catch (...) {
            return handle_xmlpp_exception(filename).as_failure();
        };
    }

    template<typename T>
    result<xmlpp::Node::const_NodeSet> find(const xmlpp::ustring& xpath,
                                            const xmlpp::Element::PrefixNsMap& map,
                                            T ele,
                                            const std::string &filename)
    {
        xmlpp::Node::const_NodeSet set;
        try {
            set = ele->find(xpath, map);
            if (set.size() == 0) {
                log_error("Couldn't find ", xpath);
                return std::errc::invalid_argument;
            }
            return set;
        } catch (...) {
            return handle_xmlpp_exception(filename).as_failure();
        };
    }

    template<typename T>
    result<xmlpp::ustring> find_attr(const xmlpp::ustring& xpath,
                                     const xmlpp::Element::PrefixNsMap& map,
                                     const T ele,
                                     const std::string &filename)
    {
        OUTCOME_TRY(auto set, find(xpath, map, ele, filename));
        auto attr = dynamic_cast<const xmlpp::Attribute*>(set.front());
        if (!attr) {
            log_error("Not an attribute: ", xpath);
            return std::errc::invalid_argument;
        }
        return attr->get_value();
    }

    template<typename T>
    result<xmlpp::ustring> find_textnode(const xmlpp::ustring& xpath,
                                         const xmlpp::Element::PrefixNsMap& map,
                                         const T ele,
                                         const std::string &filename)
    {
        OUTCOME_TRY(auto set, find(xpath, map, ele, filename));
        auto textnode = dynamic_cast<const xmlpp::TextNode*>(set.front());
        if (!textnode) {log_error( "Not a textnode: ", xpath);
            return std::errc::invalid_argument;
        }
        return textnode->get_content();
    }

}

namespace epub
{
    using outcome::result;

    book_reader::book_reader(const fs::path& epub) :
        zip(epub)
    {
    }

    book_reader::book_reader(zip::reader&& zip) :
        zip(std::move(zip))
    {
    }

    result<std::string>
    book_reader::get_mimetype()
    {
        OUTCOME_TRY(auto idx, zip.locate_file("mimetype"));
        return zip.extract(idx);
    }

    result<epub::file_reader>
    book_reader::get_file_reader(const char* path)
    {
        OUTCOME_TRY(auto idx, zip.locate_file(path));
//        OUTCOME_TRY(auto raw, zip.extract_raw(idx));
        OUTCOME_TRY(auto stream, zip.extract_stream(idx));
        try {
            return std::move(stream);
        } catch (...) {
            return handle_xmlpp_exception(path).as_failure();
        }
    }

    result<epub::file_reader>
    book_reader::get_file_reader(const std::string& path)
    {
        return get_file_reader(path.c_str());
    }

    result<void>
    book_reader::dump()
    {
        auto mimetype = get_mimetype();
        if (!mimetype) {
            log_error( "couldn't find mimetype. Is this really an epub file?");
            return mimetype.as_failure();
        } else {
            if (!verify_mimetype(mimetype.value())) {
                log_error( "Found incorrect mimetype: ", mimetype.value(), ". Is this really an epub file?");
                return std::errc::invalid_argument;
            } else {
                log_verbose("mimetype: ", mimetype.value());
            }
        }

        OUTCOME_TRY(auto rootfile_path, dump_container());
        OUTCOME_TRY(auto manifest, dump_rootfile(rootfile_path));

        for (const auto& item : manifest.items) {
            log_verbose_ml("{ ", options.dump_volume, ", ",std::quoted(item.id), ", ", std::quoted(item.href), ", ", std::quoted(item.media_type), ", ");
            if (item.toc_label) {
                log_verbose_ml(std::quoted(*item.toc_label));
            } else {
                log_verbose_ml("std::nullopt");
            }
            log_verbose(", ", item.in_spine ? "true":"false", " },");
        }
        return outcome::success();
    }

    result<book_reader::manifest>
    book_reader::dump_rootfile(const std::string& rootfile_path)
    {
        auto rootfile = get_file_reader(rootfile_path);
        if (!rootfile) {
            log_error("rootfile ", rootfile_path, " couldn't be read");
            return rootfile.as_failure();
        }
        auto cdoc = rootfile.value().get_doc();
        auto root = cdoc->get_root_node();
        if (!root) {
            log_error(rootfile_path, " is empty");
            return std::errc::invalid_argument;
        }
        xmlpp::Element::PrefixNsMap map;
        map.insert({"opf", "http://www.idpf.org/2007/opf"});
        OUTCOME_TRY(auto tocncx, find_attr("/opf:package/opf:spine/@toc", map, root, rootfile_path));
        log_verbose(rootfile_path, ": toc: ", tocncx);

        using namespace std::string_literals;
        OUTCOME_TRY(auto toc_href, find_attr("/opf:package/opf:manifest/opf:item[@id='"s + tocncx + "']/@href"s , map, root, rootfile_path));

        std::string prefix = rootfile_path.substr(0, rootfile_path.find_last_of('/'));
        std::string toc_path = prefix + "/"s + toc_href;
        log_verbose(rootfile_path, ": toc_href: ", toc_href, ", toc_path: ", toc_path);

        OUTCOME_TRY(auto toc, dump_toc(prefix + "/"s + toc_href));

        manifest manifest;
        OUTCOME_TRY(auto items, find("/opf:package/opf:manifest/opf:item", map, root, rootfile_path));
        for (const auto &item : items) {
            OUTCOME_TRY(auto id, find_attr("@id", map, item, rootfile_path));
            OUTCOME_TRY(auto href, find_attr("@href", map, item, rootfile_path));
            OUTCOME_TRY(auto media_type, find_attr("@media-type", map, item, rootfile_path));

            auto itemref = find_quiet("/opf:package/opf:spine/opf:itemref[@idref='"s + id + "']", map, root, rootfile_path);
            if (toc.has_entry(href)) {
                manifest.items.emplace_back(id, href, media_type, toc.get_label(href), itemref.has_value());
            } else {
                manifest.items.emplace_back(id, href, media_type, std::nullopt, itemref.has_value());
            }
        }

        struct manifest sorted_manifest;
        for (const auto& item : manifest.items) {
            if (!item.in_spine)
                sorted_manifest.items.push_back(item);
        }
        OUTCOME_TRY(auto spine_items, find("/opf:package/opf:spine/opf:itemref/@idref", map, root, rootfile_path));
        for (const auto& item : spine_items) {
            auto id = dynamic_cast<const xmlpp::Attribute*>(item);
            if (!id) {
                log_error("Not an attribute: ", "idref");
                return std::errc::invalid_argument;
            }
            auto iter = std::ranges::find_if(manifest.items, [&id](const auto& i) { return i.id == id->get_value(); });
            if (iter != manifest.items.end()) {
                sorted_manifest.items.push_back(*iter);
            }
        }

        return sorted_manifest;
    }

    result<book_reader::toc>
    book_reader::dump_toc(const std::string& toc_path)
    {
        OUTCOME_TRY(auto &&toc_file, get_file_reader(toc_path));
        toc toc;
        auto cdoc = toc_file.get_doc();
        auto root = cdoc->get_root_node();
        if (!root) {
            log_error(toc_path, " is empty");
            return std::errc::invalid_argument;
        }

        xmlpp::Element::PrefixNsMap map;
        map.insert({"dtb", "http://www.daisy.org/z3986/2005/ncx/"});

        // dtb_uid
        OUTCOME_TRY(toc.dtb_uid, find_attr("/dtb:ncx/dtb:head/dtb:meta[@name='dtb:uid']/@content", map, root, toc_path));

        // dtb_depth
        OUTCOME_TRY(toc.dtb_depth, find_attr("/dtb:ncx/dtb:head/dtb:meta[@name='dtb:depth']/@content", map, root, toc_path));

        // title
        OUTCOME_TRY(toc.title, find_textnode("/dtb:ncx/dtb:docTitle/dtb:text/text()", map, root, toc_path));

        log_verbose(toc_path, ": dtb_uid: ", toc.dtb_uid);
        log_verbose(toc_path, ": dtb_depth: ", toc.dtb_depth);
        log_verbose(toc_path, ": title: ", toc.title);

        OUTCOME_TRY(auto navpoint_set, find("/dtb:ncx/dtb:navMap/dtb:navPoint", map, root, toc_path));
        for (const auto &node : navpoint_set) {
            book_reader::toc::toc_entry entry;
            xmlpp::Node::const_NodeSet entryset;
            OUTCOME_TRY(entry.label, find_textnode("dtb:navLabel/dtb:text/text()", map, node, toc_path));
            OUTCOME_TRY(entry.content, find_attr("dtb:content/@src", map, node, toc_path));
            toc.entries.push_back(std::move(entry));
        }

        return toc;
    }

    result<std::string>
    book_reader::dump_container()
    {
        auto container = get_file_reader("META-INF/container.xml");
        if (!container) {
            log_error("couldn't find META-INF/container.xml. Is this really an epub file?");
            return container.as_failure();
        } else {
            log_verbose("META-INF/container.xml");
        }

        auto cdoc = container.value().get_doc();
        auto root = cdoc->get_root_node();
        if (!root) {
            log_error("container.xml is empty");
            return std::errc::invalid_argument;
        }

        try {
            xmlpp::Element::PrefixNsMap map;
            map.insert({"c", "urn:oasis:names:tc:opendocument:xmlns:container"});
            OUTCOME_TRY(auto content_path, find_attr("/c:container/c:rootfiles/c:rootfile/@full-path", map, root, "META-INF/container.xml"));
            log_verbose("META-INF/container.xml: rootfile: ", content_path);

            return content_path;
        } catch (...) {
            using namespace std::string_view_literals;
            return handle_xmlpp_exception("META-INF/container.xml"sv).as_failure();
        }

    }

    file_reader::file_reader(std::basic_string<unsigned char>&& bytes) :
        raw(std::move(bytes)),
        parser(std::make_unique<xmlpp::DomParser>())
    {
        parser->set_substitute_entities(true);
        parser->parse_memory_raw(raw->data(), raw->size());
    }

    file_reader::file_reader(zip::zipstreambuf &&in) :
        streambuf(std::make_optional<zip::zipstreambuf>(std::move(in))),
        stream(std::make_optional<std::istream>(std::addressof(streambuf.value()))),
        parser(std::make_unique<xmlpp::DomParser>())
    {
        parser->set_substitute_entities(true);
        parser->parse_stream(*stream);
    }

    void
    file_reader::print_raw()
    {
        if (raw.has_value())
            std::cout << raw->c_str() << std::endl;
        else {
            parser->get_document()->write_to_stream(std::cout);
        }
    }
}
