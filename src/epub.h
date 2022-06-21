#pragma once

#include <functional>
#include <optional>
#include <algorithm>
#include <system_error>
#include "libxml++/document.h"
#include "libxml++/parsers/domparser.h"
#include "outcome/result.hpp"
#include "zip.h"


namespace epub
{
    namespace fs = std::filesystem;
    namespace outcome = OUTCOME_V2_NAMESPACE;
    using outcome::result;

    struct toc {
        std::string title;
        std::string dtb_uid;
        std::string dtb_depth;

        struct toc_entry {
            std::string label;
            std::string content;
        };
        std::list<toc_entry> entries;

        inline bool has_entry(const std::string& content) const {
            return std::end(entries) != std::find_if(std::begin(entries), std::end(entries),
                                                     [&content](const toc_entry& e)
                                                     { return e.content == content; });
        }

        inline std::string get_label(const std::string& content) const {
            return std::find_if(std::begin(entries), std::end(entries),
                                [&content](const toc_entry& e)
                                { return e.content == content; })->label;
        }
    };

    struct manifest {
        std::string toc_relpath;
        struct toc toc;
        struct item {
            std::string id;
            std::string href;
            std::string media_type;
            std::optional<std::string> toc_label;
            std::optional<std::string> properties;
            std::optional<std::string> spine_properties;
            bool in_spine;
            bool is_linear;
        };

        std::list<item> items;
    };

    struct book {
        std::string mimetype;
        std::string rootfile_path;
        struct manifest manifest;
    };

    class file_reader
    {
    public:
        file_reader(std::basic_string<unsigned char> &&bytes);
        file_reader(zip::zipstreambuf &&in);
        const xmlpp::Document* get_doc() { return parser->get_document(); }
        void print_raw();

    private:
        std::optional<std::basic_string<unsigned char>> raw;
        std::optional<zip::zipstreambuf> streambuf;
        std::optional<std::istream> stream;
        std::unique_ptr<xmlpp::DomParser> parser;
    };

    class book_reader
    {
    public:
        book_reader(const fs::path& epub);
        book_reader(zip::reader &&zip);

        result<book> dump();
        result<std::string> get_mimetype();

        // TODO: Fix me, this method doesn't need to be in this class
        result<const xmlpp::Node*> get_metadata(const xmlpp::Document* doc);

        fs::path path;

        zip::reader zip;
    private:
        result<epub::file_reader> get_file_reader(const std::string& path);
        result<epub::file_reader> get_file_reader(const char* path);
        // Returns path to epub rootfile
        result<std::string> dump_container();
        result<manifest> dump_rootfile(const std::string& rootfile_path);
        result<toc> dump_toc(const std::string& toc_path);

    };


}
