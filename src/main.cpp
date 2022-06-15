#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <system_error>

#include "args.h"
#include "epub.h"
#include "volumes.h"


namespace fs = std::filesystem;

void print_books(const fs::path& input_dir)
{
    const fs::path epub_ext{".epub"};
    std::map<volume, std::unique_ptr<epub::book_reader>> book_readers;
    std::map<volume, epub::book> books;
    for (auto const &dir_entry : fs::directory_iterator{input_dir}) {
        if (!dir_entry.is_regular_file()) continue;
        if (dir_entry.path().extension() == epub_ext) {
            auto reader = std::make_unique<epub::book_reader>(dir_entry);
            auto book = reader->dump();
            if (book.has_failure()) continue;
            auto vol = identify_volume(book.value().manifest.toc.dtb_uid);
            if (vol.has_failure()) continue;
            book_readers.emplace(vol.value(), std::move(reader));
            books.emplace(vol.value(), std::move(book.value()));
        }
    }

    for (auto it : books) {
        std::cout << "Found " << it.second.manifest.toc.title << "\n";
    }
}

[[noreturn]] void do_dump()
{
    auto reader = epub::book_reader(options.input_file);

    auto res = reader.dump();
    if (!res) {
        std::system_error e(res.error());
        std::cerr << "Error: " << e.code() << ' ' << e.what() << std::endl;
    }
    auto book = res.value();
    using namespace std::string_view_literals;
    if (!options.dump_volume) {
        do {
            auto part_pos = book.manifest.toc.title.find("Part ");
            if (std::string::npos == part_pos) break;
            std::stringstream ss;
            ss << 'P' << book.manifest.toc.title.substr(part_pos + 5, 1);
            auto vol_pos = book.manifest.toc.title.find("Volume ");
            if (std::string::npos == vol_pos) break;
            ss << 'V' << book.manifest.toc.title.substr(vol_pos + 7, 1);
            options.dump_volume = ss.str();
        } while (false);
    }
    std::cout << "{" << std::quoted(book.manifest.toc.dtb_uid) << "sv, volume::"sv
              << (options.dump_volume ? *options.dump_volume : "VTMP"sv) << "},\n";
    for (const auto& item : book.manifest.items) {
        std::cout << "{ " << (options.dump_volume ? *options.dump_volume : "VREF"sv) << ", "
                  << std::quoted(item.id) <<  ", " <<  std::quoted(item.href) << ", "
                  << std::quoted(item.media_type) <<  ", ";
        if (item.toc_label) {
            std::cout << "std::make_optional<std::string_view>(" << std::quoted(*item.toc_label) << ")";
        } else {
            std::cout << "std::nullopt";
        }
        std::cout << ", " << std::boolalpha << item.in_spine <<  " },\n";
    }


    std::exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    #ifdef _WIN32
    std::locale::global(std::locale(".utf8"));
    #else
    std::locale::global(std::locale(""));
    #endif

    if (parse(argc, argv).has_failure()) {
        return EXIT_SUCCESS;
    }

    if (args::command::DUMP == options.command) {
        do_dump();
    }

    print_books(options.input_dir);

    bool everything_done = false;
    if (!everything_done) {
        if (options.output_created) {
            std::cerr << "ebooks not created successfully. Cleaning up created files.\n";
            std::error_code ec;
            fs::remove(options.output_dir, ec);
            if (ec) {
                std::system_error e{ec};
                std::cerr << "Error: unable to delete directory " << options.output_dir << ": " << e.code() << ' ' << e.what() << std::endl;
            }
        }
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
