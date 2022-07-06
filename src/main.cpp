#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <system_error>

#include "outcome/result.hpp"
#include "outcome/try.hpp"
#include "args.h"
#include "epub.h"
#include "volumes.h"
#include "log.h"
#include "config.h"
#include "bookmaker.h"


namespace fs = std::filesystem;

result<void> print_books(const fs::path& input_dir)
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
            auto vol = identify_volume(book.value().manifest.toc.dtb_uid);
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

    for (const auto& it : book_readers ) {
        log_info("Found ", to_string_view(it.first), ": ", it.second->path);
    }
    if (books.end() == books.find(volume::FB1)) {
        log_info("Couldn't find Fanbook 1, so those chapters will be skipped in the new epubs.");
    }
    if (books.end() == books.find(volume::FB2)) {
        log_info("Couldn't find Fanbook 2, so those chapters will be skipped in the new epubs.");
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
    if (auto it = identify_volume(book.manifest.toc.dtb_uid); it.has_value()) {
        dump_volume = to_string_view(it.value());
    } else if (get_options()->dump_volume) {
        dump_volume = *get_options()->dump_volume;
    }
    std::cout << "{" << std::quoted(book.manifest.toc.dtb_uid) << "sv, volume::"sv
              <<  dump_volume << "},\n";
    for (const auto& item : book.manifest.items) {
        std::cout << "{ volume::" << dump_volume << ", "
                  << std::quoted(item.id) <<  "sv, " <<  std::quoted(item.href) << "sv, "
                  << std::quoted(item.media_type) <<  "sv, ";
        if (item.toc_label) {
            std::cout << std::quoted(*item.toc_label) << "sv";
        } else {
            std::cout << "std::nullopt";
        }
        std::cout << ", " << std::boolalpha << item.in_spine <<  " },\n";
    }
}

int main(int argc, char* argv[])
{
    #ifdef IS_WINDOWS
    std::locale::global(std::locale(".utf8"));
    #else
    std::locale::global(std::locale(""));
    #endif

    if (auto res = parse(argc, argv); res.has_failure()) {
        log_error("Failed to parse arguments: ", res.error().message());
        return EXIT_SUCCESS;
    }

    if (args::command::DUMP == get_options()->command) {
        do_dump();
    } else if (args::command::NORMAL == get_options()->command) {
        auto res = print_books(get_options()->input_dir);

        if (res.has_error()) {
            log_error("Failed to make books: ", res.error());
            if (get_options()->output_created) {
                std::error_code ec;
                if (std::filesystem::is_empty(get_options()->output_dir, ec) && !ec) {
                    log_verbose("No ebooks created successfully. Cleaning up created directory.");
                    fs::remove(get_options()->output_dir, ec);
                    if (ec) {
                        log_error("unable to delete empty output directory ", get_options()->output_dir, ": ", ec);
                    }
                }
            }
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
