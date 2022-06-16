#pragma once
#include <map>
#include "epub.h"
#include "volumes.h"

namespace bookmaker
{
    namespace outcome = OUTCOME_V2_NAMESPACE;
    using outcome::result;

    result<void> make_books(const std::map<volume, epub::book>& books,
                            const std::map<volume, std::unique_ptr<epub::book_reader>>& book_readers);

}
