#pragma once

#include <regex>

#include "epub.h"
#include "outcome/result.hpp"

class book_searcher
{
public:
    book_searcher(epub::books_t&& books,
                  epub::readers_t&& book_readers);

    OUTCOME_V2_NAMESPACE::result<std::vector<std::string>> search() const;

private:
    const std::regex whitespace;
    epub::books_t src_books;
    epub::readers_t src_readers;
};
