#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

#include "libxml++/nodes/element.h"
#include "libxml++/ustring.h"
#include "outcome/result.hpp"

#include "epub.h"
#include "volume_definition.h"
#include "volumes.h"
#include "zip.h"

class book_writer
{
public:
    using books_t = std::map<volume, const epub::book>;
    using readers_t = std::map<volume, std::unique_ptr<epub::book_reader>>;

private:
    book_writer(volume base_volume,
                const books_t& books,
                const readers_t& readers,
                std::variant<omnibus, volume> name);
public:

    book_writer(volume base_volume,
                const books_t& books,
                const readers_t& readers,
                std::variant<omnibus, volume> name,
                std::vector<volume_definition>&& defs);

    book_writer(volume base_volume,
                const books_t& books,
                const readers_t& readers,
                std::variant<omnibus, volume> name,
                std::ranges::input_range auto&& defs) :
        book_writer(base_volume, books, readers, name)
    {
        definition.assign(std::ranges::begin(defs), std::ranges::end(defs));
    }


    OUTCOME_V2_NAMESPACE::result<std::filesystem::path> make_book();

private:
    OUTCOME_V2_NAMESPACE::result<void> make_book_impl();
    OUTCOME_V2_NAMESPACE::result<std::string> create_ncx(const std::string& toc_fullpath);
    OUTCOME_V2_NAMESPACE::result<void> add_ncx();
    OUTCOME_V2_NAMESPACE::result<std::string> create_rootfile();
    OUTCOME_V2_NAMESPACE::result<void> start_book();
    void make_landmarks(xmlpp::Element *nav_node, const xmlpp::ustring& toc_path);
    void make_toc(xmlpp::Element *nav_node, const xmlpp::Element *src_nav);
    OUTCOME_V2_NAMESPACE::result<std::string> get_ncx_id() const;

private:
    using book_t = books_t::value_type::second_type;
    using reader_t = readers_t::value_type::second_type;

    volume base_vol;
    const books_t& src_books;
    const readers_t& src_readers;
    const book_t& base_book;
    const reader_t& base_reader;

    std::variant<omnibus, volume> name;
    std::vector<volume_definition> definition;
    std::filesystem::path filename;
    zip::writer writer;
    std::list<std::string> basefiles;
};
