#include <algorithm>
#include <set>
#include <ranges>
#include "bookmaker.h"
#include "volumes.h"
#include "log.h"
#include "part1.h"


namespace {

    constinit std::array defined_volumes = std::to_array({
            volume::P1V1,
            volume::P1V2,
            volume::P1V3,
        });

    constexpr auto get_definition_view(volume v)
    {
        switch (v)
        {
            case volume::P1V1:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P1V2:
                return std::ranges::subrange(part_1::vol_2);
            case volume::P1V3:
                return std::ranges::subrange(part_1::vol_3);
            case volume::P2V1:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P2V2:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P2V3:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P2V4:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P3V1:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P3V2:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P3V3:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P3V4:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P3V5:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P4V1:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P4V2:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P4V3:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P4V4:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P4V5:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P4V6:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P4V7:
                return std::ranges::subrange(part_1::vol_1);
            case volume::P4V8:
                return std::ranges::subrange(part_1::vol_1);
            case volume::FB1:
                return std::ranges::subrange(part_1::vol_1);
            case volume::FB2:
                return std::ranges::subrange(part_1::vol_1);
            case volume::RA1:
                return std::ranges::subrange(part_1::vol_1);
              }
        assert(false);
    }


} // anonymous namespace


namespace bookmaker
{

    result<void> make_book(const std::map<volume, epub::book>& books,
                           const std::map<volume, std::unique_ptr<epub::book_reader>>& book_readers,
                           const auto& definition)
    {
        return std::errc::function_not_supported;
    }

    result<void> make_books(const std::map<volume, epub::book>& books,
                            const std::map<volume, std::unique_ptr<epub::book_reader>>& book_readers)
    {
        int created_books = 0;
        for (auto defined_volume : defined_volumes) {
            const auto view = get_definition_view(defined_volume);
            log_info("vol ", to_string_view(defined_volume), " has ", view.size(), " elements.");
            std::set<volume> located_inputs;
            for (const auto& def : view) {
                if (books.end() != books.find(def.vol))
                    located_inputs.insert(def.vol);
            }
            if (located_inputs.size() == 0) {
                log_verbose("Info: No inputs to make ", to_string_view(defined_volume));
                continue;
            }
            log_info("Creating chronologically ordered ", to_string_view(defined_volume));
            auto res = make_book(books, book_readers, view);
            if (res.has_error()) {
                log_error("Couldn't make ", to_string_view(defined_volume), ": ", res.error().message(), ". Moving on...");
            } else {
                log_verbose("Created chronologically ordered ", to_string_view(defined_volume), " success!");
                ++created_books;
            }
        }

        if (0 == created_books)
            return std::errc::no_such_file_or_directory;
        return outcome::success();
    }


}
