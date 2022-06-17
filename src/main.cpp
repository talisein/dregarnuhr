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
    std::map<volume, std::unique_ptr<epub::book_reader>> book_readers;
    std::map<volume, epub::book> books;
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

    for (const auto& it : books ) {
        auto reader = book_readers.find(it.first);
        log_info("Found ", it.second.manifest.toc.title, ": ", reader->second->path);
    }

    OUTCOME_TRY(bookmaker::make_books(books, book_readers));
    return outcome::success();
}


[[noreturn]] void do_dump()
{
    auto reader = epub::book_reader(get_options()->input_file);

    auto res = reader.dump();
    if (!res) {
        std::system_error e(res.error());
        std::cerr << "Error: " << e.code() << ' ' << e.what() << std::endl;
    }
    auto book = res.value();
    using namespace std::string_view_literals;
    std::string dump_volume {"VTMP"};
    if (get_options()->dump_volume) {
        dump_volume = *get_options()->dump_volume;
    } else {
        do {
            std::stringstream ss;
            if (auto part_pos = book.manifest.toc.title.find("Part ");
                std::string::npos != part_pos)
            {
                ss << 'P' << book.manifest.toc.title.substr(part_pos + 5, 1);
            } else {
                break;
            }

            if (auto vol_pos = book.manifest.toc.title.find("Volume ");
                std::string::npos != vol_pos)
            {
                ss << 'V' << book.manifest.toc.title.substr(vol_pos + 7, 1);
                dump_volume = ss.str();
            } else {
                break;
            }
        } while (false);
    }
    std::cout << "{" << std::quoted(book.manifest.toc.dtb_uid) << "sv, volume::"sv
              <<  "VTMP" << "},\n";
    for (const auto& item : book.manifest.items) {
        std::cout << "{ volume::" << (get_options()->dump_volume ? *get_options()->dump_volume : "VREF"sv) << ", "
                  << std::quoted(item.id) <<  "sv, " <<  std::quoted(item.href) << "sv, "
                  << std::quoted(item.media_type) <<  "sv, ";
        if (item.toc_label) {
            std::cout << "std::make_optional<std::string_view>(" << std::quoted(*item.toc_label) << "sv)";
        } else {
            std::cout << "std::nullopt";
        }
        std::cout << ", " << std::boolalpha << item.in_spine <<  " },\n";
    }


    std::exit(EXIT_SUCCESS);
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
    }

    if (args::command::NORMAL == get_options()->command) {
        auto res = print_books(get_options()->input_dir);

        bool everything_done = !res.has_error();
        if (!everything_done) {
            if (get_options()->output_created) {
                std::error_code ec;
                if (std::filesystem::is_empty(get_options()->output_dir, ec) && !ec) {
                    log_verbose("No ebooks created successfully. Cleaning up created directory.");
                    fs::remove(get_options()->output_dir, ec);
                    if (ec) {
                        std::system_error e{ec};
                        log_error("Error: unable to delete empty output directory ", get_options()->output_dir, ": ", e.code(), ' ', e.what());
                    }
                }
            }
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
