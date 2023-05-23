#include <expected>
#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <system_error>
#include <locale>
#include <utility>
#include <clocale>

#include "libxml/xmlerror.h"
#include "libxml++/libxml++.h"
#include "outcome/result.hpp"
#include "outcome/try.hpp"
#include "args.h"
#include "epub.h"
#include "volumes.h"
#include "log.h"
#include "config.h"
#include "bookmaker.h"
#include "updates.h"
#include "book_searcher.h"

#if HAVE_CHRONO
#include <format>
#else
#include "date/date.h"
#endif

#include "omnibus.h"

namespace fs = std::filesystem;

std::expected<std::pair<epub::readers_t, epub::books_t>, std::error_code>
search_input_directory(const fs::path& input_dir)
{
    const fs::path epub_ext{".epub"};
    epub::readers_t book_readers;
    epub::books_t books;
    log_verbose("Info: Looking for epubs in ", input_dir.string());
    for (auto const &dir_entry : fs::directory_iterator{input_dir}) {
        log_verbose("Info: considering ", dir_entry.path().string());
        if (!dir_entry.is_regular_file()) {
            log_verbose("Info: Not a regular file: ", dir_entry.path().string());
            continue;
        }
        if (dir_entry.path().extension() != epub_ext) {
            log_verbose("Info: ", std::quoted(dir_entry.path().string()),
                        " extension ", std::quoted(dir_entry.path().extension().string()),
                        " is not matching ", epub_ext.string());
            continue;
        } else {
            auto reader = std::make_unique<epub::book_reader>(dir_entry);
            auto book = reader->dump();
            if (book.has_failure()) {
                log_verbose("Info: Dump error for ", std::quoted(dir_entry.path().string()), ": ", book.error().message());
                continue;
            }
            auto vol = identify_volume(book.value());
            if (vol.has_failure()) {
                log_verbose("Info: Failed to identify ", std::quoted(dir_entry.path().string()),
                            ": Unknown uid ", std::quoted(book.value().manifest.toc.dtb_uid),
                            " with title: ", std::quoted(book.value().manifest.toc.title));
                continue;
            }
            book_readers.emplace(vol.value(), std::move(reader));
            books.emplace(vol.value(), std::move(book.value()));
        }
    }
    if (!books.empty()) {
        return std::make_pair(std::move(book_readers), std::move(books));
    } else {
        return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
    }
}

result<void> search_books(const fs::path& input_dir)
{
    auto search_res = search_input_directory(input_dir);
    if (!search_res.has_value()) {
        return search_res.error();
    }
    auto&& [book_readers, books] = search_res.value();

    book_searcher searcher {std::move(books), std::move(book_readers)};
    OUTCOME_TRY(searcher.search());

    return outcome::success();
}

result<void> print_books(const fs::path& input_dir)
{
    auto search_res = search_input_directory(input_dir);
    if (!search_res.has_value()) {
        return search_res.error();
    }
    auto&& [book_readers, books] = search_res.value();

    if (books.end() == books.find(volume::FB1)) {
        log_info("Couldn't find Fanbook 1, so those chapters will be skipped in the new epubs.");
    }
    if (books.end() == books.find(volume::FB2)) {
        log_info("Couldn't find Fanbook 2, so those chapters will be skipped in the new epubs.");
    }

    if (books.end() != books.find(volume::UFTSS1)) {
        // Prefer UFTSS1 over the others
        for (auto vol : {volume::SSJBUNKO1, volume::SSBDOVA1, volume::SSTEASET,
                volume::SSDRAMACD2, volume::SSTOBBONUS1, volume::SSDRAMACD3,
                volume::SSDRAMACD4})
        {
            books.erase(vol);
            book_readers.erase(vol);
        }
    }

    std::stringstream found_volumes_ss;
    bool is_first = true;
    for (const auto& it : book_readers) {
        log_verbose("Found ", to_string_view(it.first), ": ", it.second->path);
        if (!is_first) found_volumes_ss << ", ";
        is_first = false;
        found_volumes_ss << it.first;
    }
    log_info("Found ", found_volumes_ss.str());

    auto tags = get_updated();
    bool need_updates = false;
    for (const auto &it : books) {
        auto tag = tags.find(it.first);
        if (tag == tags.end()) {
            log_verbose(it.first, " has no entry in updates.json");
            continue;
        }
        auto jnc_updated = tag->second;
        auto epub_updated = it.second.manifest.modified;
        if ((jnc_updated - std::chrono::days(10)) > epub_updated) {
            #if HAVE_CHRONO
            using namespace std::chrono;
            log_info("UPDATE AVAILABLE: ", it.first, " has an updated epub as of ", std::format("{0:%c}", jnc_updated));
            log_verbose("Book updated: ", std::format("{0:%c}", epub_updated));
            #else
            log_info("UPDATE AVAILABLE: ", it.first, " has an updated epub as of ", date::format("%c", jnc_updated));
            log_verbose("Book updated: ", date::format("%c", epub_updated));
            #endif
            need_updates = true;
        }
    }

    if (need_updates) {
        log_info("Updates are available from JNC: https://j-novel.club/user");
    }

    epub::bookmaker bookmaker(std::move(books), std::move(book_readers));
    OUTCOME_TRY(bookmaker.make_books());
    return outcome::success();
}


void do_dump()
{
    auto reader = epub::book_reader(get_options()->input_file);

    auto res = reader.dump();
    if (!res) {
        log_error(res.error());
        return;
    }
    auto book = res.value();
    using namespace std::string_view_literals;
    std::string dump_volume {"VTMP"};
    if (auto it = identify_volume(book); it.has_value()) {
        dump_volume = to_string_view(it.value());
    } else if (get_options()->dump_volume) {
        dump_volume = *get_options()->dump_volume;
    }
    std::cout << "{" << std::quoted(book.manifest.toc.dtb_uid) << "sv, volume::"sv
              <<  dump_volume << "},\n";
    for (const auto& item : book.manifest.items) {
        try {
            volume_definition v {identify_volume(book).value(),  item.href, item.media_type, item.toc_label, item.in_spine};
            std::cout << v << "," << std::endl;
        } catch (std::exception &) {
            std::cout << "{ volume::" << dump_volume << ", /* ??? */ "
                      <<  std::quoted(item.href) << "sv, "
                      << std::quoted(item.media_type) <<  "sv, ";
            if (item.toc_label) {
                std::cout << std::quoted(*item.toc_label) << "sv";
            } else {
                std::cout << "std::nullopt";
            }
            std::cout << ", " << std::boolalpha << item.in_spine <<  " },\n";
        }
    }
}

int main(int argc, char* argv[])
{
    try {
        if (IS_WINDOWS) {
            std::locale::global(std::locale(".utf8"));
        } else {
            std::locale::global(std::locale(""));
        }
    } catch (std::exception &e) {
        // MacOS has issues
        std::setlocale(LC_ALL, "");
        log_info("Warning: Couldn't set locale, but we will continue: ", e.what());
    }

    if (auto res = parse(argc, argv); !res.has_value()) {
        log_error("Failed to parse arguments: ", res.error());
        return EXIT_SUCCESS;
    }

    if (args::command_e::DUMP == get_options()->command) {
        do_dump();
    } else if (args::command_e::NORMAL == get_options()->command) {
        auto res = print_books(get_options()->input_dir);

        if (res.has_error()) {
            if (get_options()->output_created) {
                std::error_code ec;
                if (std::filesystem::is_empty(get_options()->output_dir, ec) && !ec) {
                    log_verbose("No ebooks created. Cleaning up created empty directory.");
                    fs::remove(get_options()->output_dir, ec);
                    if (ec != std::error_code{}) {
                        log_error("unable to delete empty output directory ", get_options()->output_dir, ": ", ec);
                    }
                }
            }
            return EXIT_SUCCESS;
        }
    } else if (args::command_e::SEARCH == get_options()->command) {
        auto res = search_books(get_options()->input_dir);

        if (res.has_error()) {
            log_error("Failed to search books: ", res.error());
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
