#pragma once
#include <map>
#include <variant>
#include "epub.h"
#include "volumes.h"

namespace epub
{
    namespace outcome = OUTCOME_V2_NAMESPACE;
    namespace fs = std::filesystem;
    using outcome::result;
    using definition_t = std::span<const volume_definition>;
    using books_t = std::map<volume, const epub::book>;
    using readers_t = std::map<volume, std::unique_ptr<epub::book_reader>>;

    class bookmaker
    {
    public:
        bookmaker(books_t&& books,
                  readers_t&& book_readers);

        result<void> make_books();
    private:
        template<std::ranges::input_range R>
        result<void> make_books_impl(R&& def, std::variant<omnibus, volume> base);

    result<void>
    make_book_impl(volume vol,
                   zip::writer &writer,
                   const epub::book& base_book,
                   const std::unique_ptr<epub::book_reader>& base_reader,
                   const definition_t &definition);

    result<fs::path> make_book(volume vol,
                               const definition_t& definition);

    private:
        books_t src_books;
        readers_t src_readers;
    };

    template<std::ranges::input_range R>
    class book_writer
    {
    public:
        book_writer(volume base_volume,
                    const books_t& books,
                    const readers_t& readers,
                    R definition);

        result<fs::path> make_book();

    private:
        result<void> make_book_impl();
        result<std::string> create_ncx(const std::string& toc_fullpath);
        result<void> add_ncx();
        result<std::string> create_rootfile();
        result<void> start_book();
        void make_toc(xmlpp::Element *nav_node, const xmlpp::Element *src_nav);
        result<std::string> get_ncx_id() const;
    private:
        using book_t = books_t::value_type::second_type;
        using reader_t = readers_t::value_type::second_type;

        volume vol;
        const books_t& src_books;
        const readers_t& src_readers;
        const book_t& base_book;
        const reader_t& base_reader;
        std::remove_cvref_t<R> definition;
        fs::path filename;
        zip::writer writer;
        std::list<std::string> basefiles;
    };

}
