#pragma once
#include <map>
#include <variant>
#include "epub.h"
#include "volume_definition.h"
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
}
