#include <iostream>
#include <filesystem>
#include <type_traits>
#include <source_location>
#include <cassert>
#include <chrono>
#include <libxml++/libxml++.h>
#include <outcome/try.hpp>
#include <outcome/utils.hpp>
#include "config.h"
#include "epub.h"
#include "log.h"
#include "utils.h"

#if !HAVE_CHRONO
#include "date/date.h"
#endif


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
            log_error(filename, ": xml validity_error from ", src.function_name(), ":", src.line(), ": ", e.what());
            return std::errc::invalid_argument;
        } catch (xmlpp::parse_error &e) {
            log_error(filename, ": xml parse_error from ",
                      src.function_name(), ":", src.line(), ": ", e.what());
            return std::errc::invalid_argument;
        } catch (xmlpp::exception &e) {
            log_error(filename, ": xml parse_error from ", src.function_name(), ":", src.line(), ": ", e.what());
            return std::errc::invalid_argument;
        } catch (std::system_error &e) {
            log_error(filename, ": exception from ", src.function_name(), ":", src.line(), ": ", e.what());
            return e.code();
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
                                                  T& ele,
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
                                            T& ele,
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
    result<xmlpp::ustring> find_attr_required(const xmlpp::ustring& xpath,
                                              const xmlpp::Element::PrefixNsMap& map,
                                              const T& ele,
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
    result<xmlpp::ustring> find_attr(const xmlpp::ustring& xpath,
                                     const xmlpp::Element::PrefixNsMap& map,
                                     const T& ele,
                                     const std::string &filename)
    {
        OUTCOME_TRY(auto set, find_quiet(xpath, map, ele, filename));
        auto attr = dynamic_cast<const xmlpp::Attribute*>(set.front());
        if (!attr) {
            return std::errc::invalid_argument;
        }
        return attr->get_value();
    }

    template<typename T>
    result<xmlpp::ustring> find_textnode(const xmlpp::ustring& xpath,
                                         const xmlpp::Element::PrefixNsMap& map,
                                         const T& ele,
                                         const std::string &filename)
    {
        OUTCOME_TRY(auto set, find(xpath, map, ele, filename));
        auto e = dynamic_cast<const xmlpp::Element*>(set.front());
        if (!e->has_child_text())
            return xmlpp::ustring();

        auto textnode = e->get_first_child_text();
        return textnode->get_content();
    }

}

namespace epub
{
    using outcome::result;

    book_reader::book_reader(const fs::path& epub) :
        path(epub),
        zip(epub)
    {
    }

    book_reader::book_reader(zip::reader&& inzip) :
        zip(std::move(inzip))
    {
    }

    result<std::string>
    book_reader::get_mimetype()
    {
        OUTCOME_TRY(auto idx, zip.locate_file("mimetype"));
        return zip.extract(idx);
    }

    result<epub::file_reader>
    book_reader::get_file_reader(const char* filepath)
    {
        OUTCOME_TRY(auto idx, zip.locate_file(filepath));
//        OUTCOME_TRY(auto raw, zip.extract_raw(idx));
        OUTCOME_TRY(auto stream, zip.extract_stream(idx));
        try {
            return stream;
        } catch (...) {
            return handle_xmlpp_exception(filepath).as_failure();
        }
    }

    result<epub::file_reader>
    book_reader::get_file_reader(const std::string& filepath)
    {
        return get_file_reader(filepath.c_str());
    }

    result<book>
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

        book book;
        book.mimetype = mimetype.value();

        OUTCOME_TRY(book.rootfile_path, dump_container());
        OUTCOME_TRY(book.manifest, dump_rootfile(book.rootfile_path));

        return book;
    }

    result<manifest>
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
        map.insert({"dc", "http://purl.org/dc/elements/1.1/"});
        // First, check if this is produced from this very program!
        auto is_dregarnuhr = find_quiet("/opf:package/opf:metadata/dc:contributor[text()='talisein']", map, root, rootfile_path);
        if (is_dregarnuhr.has_value()) {
            log_info("Info: ", path, " is produced by this program. Skipping!");
            return std::errc::invalid_argument;
        }

        // Get modified
        manifest manifest;
        auto modified_set = find_quiet("/opf:package/opf:metadata/opf:meta[@property='dcterms:modified']/text()", map, root, rootfile_path);
        if (modified_set.has_value()) {
            auto textnode = dynamic_cast<const xmlpp::TextNode*>(modified_set.value().front());
            if (textnode) {
                log_verbose("Modified: ", textnode->get_content());
                std::istringstream iss(textnode->get_content());
                static const std::basic_string<std::istringstream::char_type> format("%Y-%m-%dT%H:%M:%SZ");
                #if HAVE_CHRONO
                std::chrono::from_stream(iss, format.c_str(), manifest.modified);
                #else
                date::from_stream(iss, format.c_str(), manifest.modified);
                #endif
            }
        }

        OUTCOME_TRY(auto tocncx, find_attr_required("/opf:package/opf:spine/@toc", map, root, rootfile_path));
        log_verbose(rootfile_path, ": toc: ", tocncx);

        using namespace std::string_literals;
        OUTCOME_TRY(auto toc_href, find_attr_required("/opf:package/opf:manifest/opf:item[@id='"s + tocncx + "']/@href"s , map, root, rootfile_path));

        std::string prefix = rootfile_path.substr(0, rootfile_path.find_last_of('/'));
        std::string toc_path = prefix + "/"s + toc_href;
        log_verbose(rootfile_path, ": toc_href: ", toc_href, ", toc_path: ", toc_path);

        // Parse the spine's itemrefs
        OUTCOME_TRY(auto itemrefs, find("/opf:package/opf:spine/opf:itemref", map, root, rootfile_path));
        struct itemref {
            std::string idref;
            bool is_linear;
            std::optional<std::string> properties;
        };
        std::map<std::string, itemref> idrefs;
        std::vector<std::string> idref_order;
        for (const auto item : std::views::transform(itemrefs, [](const auto node) { return dynamic_cast<const xmlpp::Element*>(node); })) {
            itemref itemref;
            for (const auto attr : item->get_attributes()) {
                auto name = attr->get_name();
                if (name == "idref") {
                    itemref.idref = attr->get_value();
                } else if (name == "linear") {
                    itemref.is_linear = attr->get_value() == "yes";
                } else if (name == "properties") {
                    itemref.properties = attr->get_value();
                }
            }
            auto key = itemref.idref;
            idref_order.push_back(key);
            idrefs.try_emplace(std::move(key), std::move(itemref));
        }

        // Go through the manifest
        OUTCOME_TRY(manifest.toc, dump_toc(prefix + "/"s + toc_href));
        manifest.toc_relpath = toc_href;
        OUTCOME_TRY(auto items, find("/opf:package/opf:manifest/opf:item", map, root, rootfile_path));
        for (const auto &item : items) {
            OUTCOME_TRY(auto id, find_attr_required("@id", map, item, rootfile_path));
            OUTCOME_TRY(auto href, find_attr_required("@href", map, item, rootfile_path));
            OUTCOME_TRY(auto media_type, find_attr_required("@media-type", map, item, rootfile_path));
            auto props_node = find_attr("@properties", map, item, rootfile_path);
            std::optional<std::string> properties;
            std::optional<std::string> spine_properties;
            bool is_linear = false;
            bool in_spine = false;
            if (props_node) {
                properties = std::make_optional<std::string>(props_node.value());
            }
            if (auto iter = idrefs.find(id); std::end(idrefs) != iter) {
                const auto& idref = iter->second;
                in_spine = true;
                is_linear = idref.is_linear;
                spine_properties = idref.properties;
            }
            if (manifest.toc.has_entry(href)) {
                manifest.items.emplace_back(id, href, media_type, manifest.toc.get_label(href), properties, spine_properties, in_spine, is_linear);
            } else {
                manifest.items.emplace_back(id, href, media_type, std::nullopt, properties, spine_properties, in_spine, is_linear);
            }
        }

        struct manifest sorted_manifest(manifest);
        sorted_manifest.items.clear();
        for (const auto& item : manifest.items) {
            if (!item.in_spine)
                sorted_manifest.items.push_back(item);
        }
        for (const auto& id : idref_order) {
            auto iter = std::ranges::find(manifest.items, id, &manifest::item::id);
            if (iter != manifest.items.end()) {
                sorted_manifest.items.push_back(*iter);
            }
        }

        return sorted_manifest;
    }

    namespace {
        result<void>
        handle_navpoint(const xmlpp::Node* node, auto &entries) {
            toc::toc_entry entry;
            const auto label = dynamic_cast<const xmlpp::Element*>(node->get_first_child("navLabel"));
            if (label) {
                bool set = false;
                for (const auto &label_child : label->get_children()) {
                    auto text = dynamic_cast<const xmlpp::Element*>(label_child);
                    if (!text) continue;
                    entry.label = text->get_first_child_text()->get_content();
                    set = true;
                }
                if (!set) {
                    return std::errc::invalid_seek;
                }
            } else {
                return std::errc::invalid_seek;
            }

            const xmlpp::Node* content = node->get_first_child("content");
            if (content) {
                auto content_ele = dynamic_cast<const xmlpp::Element*>(content);
                xmlpp::Element::const_AttributeList attrs = content_ele->get_attributes();
                bool set = false;
                for (const xmlpp::Attribute* attr : attrs) {
                    if (attr->get_name() == "src") {
                        entry.content = attr->get_value();
                        set = true;
                    }
                }
                if (!set) {
                    return std::errc::invalid_seek;
                }
            } else {
                return std::errc::invalid_seek;
            }

            entries.push_back(entry);

            for (const auto &child_navpoint : node->get_children("navPoint")) {
                OUTCOME_TRY(handle_navpoint(child_navpoint, entries));
            }

            return outcome::success();
        }

    }
    result<toc>
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
        OUTCOME_TRY(toc.dtb_uid, find_attr_required("/dtb:ncx/dtb:head/dtb:meta[@name='dtb:uid']/@content", map, root, toc_path));

        // dtb_depth
        OUTCOME_TRY(toc.dtb_depth, find_attr_required("/dtb:ncx/dtb:head/dtb:meta[@name='dtb:depth']/@content", map, root, toc_path));

        // title
        OUTCOME_TRY(toc.title, find_textnode("/dtb:ncx/dtb:docTitle/dtb:text", map, root, toc_path));
        if (toc.title.empty()) {
            auto o = find_textnode("/dtb:ncx/dtb:navMap/dtb:navPoint/dtb:navLabel/dtb:text", map, root, toc_path);
            if (o) toc.title = o.value();
        }

        log_verbose(toc_path, ": dtb_uid: ", toc.dtb_uid);
        log_verbose(toc_path, ": dtb_depth: ", toc.dtb_depth);
        log_verbose(toc_path, ": title: ", toc.title);

        OUTCOME_TRY(auto navpoint_set, find("/dtb:ncx/dtb:navMap/dtb:navPoint", map, root, toc_path));
        for (const auto &node : navpoint_set) {
            OUTCOME_TRY(handle_navpoint(node, toc.entries));
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
            OUTCOME_TRY(auto content_path, find_attr_required("/c:container/c:rootfiles/c:rootfile/@full-path", map, root, "META-INF/container.xml"));
            log_verbose("META-INF/container.xml: rootfile: ", content_path);

            return content_path;
        } catch (...) {
            using namespace std::string_view_literals;
            return handle_xmlpp_exception("META-INF/container.xml"sv).as_failure();
        }

    }

    result<const xmlpp::Node*>
    book_reader::get_metadata(const xmlpp::Document* doc)
    {
        auto root = doc->get_root_node();
        if (!root) {
            log_error("container.xml is empty");
            return std::errc::invalid_argument;
        }

        xmlpp::Element::PrefixNsMap map;
        map.insert({"opf", "http://www.idpf.org/2007/opf"});
        OUTCOME_TRY(auto set, find("/opf:package/opf:metadata", map, root, "rootfile"));
        if (set.size() != 1) {
            log_error("More than 1 metadata?");
            return std::errc::invalid_argument;
        }
        return *set.begin();
    }

    file_reader::file_reader(std::basic_string<unsigned char>&& bytes) :
        raw(std::move(bytes)),
        parser(std::make_unique<xmlpp::DomParser>())
    {
        parser->set_substitute_entities(true);
        parser->parse_memory_raw(raw->data(), utils::safe_int_cast<xmlpp::Parser::size_type>(raw->size()));
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
        if (raw.has_value()) {
            log_info(raw->c_str());
        } else {
            log_info(parser->get_document()->write_to_string());
        }
    }
}

namespace {
    using namespace std::string_view_literals;
    const std::map<std::string_view, volume> volume_map {
        {"9781718344631"sv, volume::FB1},
        {"9781718344648"sv, volume::FB2},
        {"9781718344655"sv, volume::FB3},
        {"9781718346734"sv, volume::RA1},
        {"9781718346758"sv, volume::SSC1},
        {"9781718346017"sv, volume::P1V1},
        {"9781718346031"sv, volume::P1V2},
        {"9781718346055"sv, volume::P1V3},
        {"9781718346079"sv, volume::P2V1},
        {"9781718346093"sv, volume::P2V2},
        {"9781718346116"sv, volume::P2V3},
        {"9781718346130"sv, volume::P2V4},
        {"9781718346154"sv, volume::P3V1},
        {"9781718346178"sv, volume::P3V2},
        {"9781718346192"sv, volume::P3V3},
        {"9781718346215"sv, volume::P3V4},
        {"9781718346239"sv, volume::P3V5},
        {"9781718346253"sv, volume::P4V1},
        {"9781718346277"sv, volume::P4V2},
        {"9781718346291"sv, volume::P4V3},
        {"9781718346314"sv, volume::P4V4},
        {"9781718346338"sv, volume::P4V5},
        {"9781718346352"sv, volume::P4V6},
        {"9781718346376"sv, volume::P4V7},
        {"9781718346390"sv, volume::P4V8},
        {"9781718346413"sv, volume::P4V9},
        {"9781718346437"sv, volume::P5V1},
        {"9781718346451"sv, volume::P5V2},
        {"9781718346475"sv, volume::P5V3},
        {"9781718338005"sv, volume::MP1V1},
        {"9781718338029"sv, volume::MP1V2},
        {"9781718338043"sv, volume::MP1V3},
        {"9781718338067"sv, volume::MP1V4},
        {"9781718338081"sv, volume::MP1V5},
        {"9781718338104"sv, volume::MP1V6},
        {"9781718338128"sv, volume::MP1V7},
        {"9781718338142"sv, volume::MP2V1},
        {"9781718338166"sv, volume::MP2V2},
        {"9781718338180"sv, volume::MP2V3},
        {"9781718338203"sv, volume::MP2V4},
        {"9781718338227"sv, volume::MP2V5},
        {"9781718338241"sv, volume::MP2V6},
        {"9781718338265"sv, volume::MP2V7},
        {"96908895-C96B-4376-BBF8-6A25E96F72F3"sv, volume::SSBDOVA1},
        {"22BC23BD-BEB1-4F8F-9104-825FA616AD4C"sv, volume::SSJBUNKO1},
        {"DEB588FB-32EF-4512-8ADF-D8A5CB49D9CD"sv, volume::SSTEASET},
        {"5F99914F-5229-482E-A63D-D38002C442FD"sv, volume::SSWN1},
        {"94A7C755-6459-4B4A-A48C-9AEE66C3E58A"sv, volume::SSWN2},
        {"614AD11F-85BA-499A-9A35-74C4DBCCA288"sv, volume::SSDRAMACD2},
        {"E490F42A-CF20-476A-8327-59E034AE8928"sv, volume::SSDRAMACD3},
        {"EADEFA73-BCAD-4166-8745-0137BEAC38EC"sv, volume::SSDRAMACD4},
        {"7D41D170-FCB5-480A-82AD-D4F921AB5B61"sv, volume::SSTOBBONUS1},
        {"CE71D400-5EF9-4B8E-80C1-CED953C285C6"sv, volume::SSUP1},
    };
}

result<volume>
epub::identify_volume(const epub::book& book)
{
    std::string_view uid = book.manifest.toc.dtb_uid;
    auto iter = volume_map.find(uid);
    if (iter == volume_map.end()) {
        return std::errc::invalid_argument;
    }

    if (volume::SSDRAMACD3 == iter->second) {
        if (book.manifest.toc.title == "Ascendance of a Bookworm: Unofficial Fan Translated Side Stories Vol. 1") {
            return volume::SSUFTSS1;
        } else if (book.manifest.toc.title == "Ascendance of a Bookworm P4V7 Drama CD 3 SS: Witnessing the Name Swearing") {
            return volume::SSDRAMACD3;
        } else {
            log_error("Unexpected title: ", book.manifest.toc.title);
            return std::errc::invalid_argument;
        }
    } else {
        return iter->second;
    }
}
