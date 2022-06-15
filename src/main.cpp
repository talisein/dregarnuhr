#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <system_error>

#include "args.h"
#include "epub.h"

namespace fs = std::filesystem;

void print_books(const fs::path& input_dir)
{
    const fs::path epub_ext{".epub"};
    for (auto const &dir_entry : fs::directory_iterator{input_dir}) {
        if (!dir_entry.is_regular_file()) continue;
        /*
        if (dir_entry.path().extension() == epub_ext) {
            try {
                zip::reader reader{dir_entry.path()};

                if (!reader.is_readable()) {
                    std::cerr << dir_entry.path() << " is not readable\n";
                    return;
                }

                auto res = reader.print_files();
                if (!res) {
                    std::cerr << "Failed to print files from "
                              << dir_entry.path()
                              << ": " << res.error().message() << '\n';
                }

                std::cout << std::endl << "Mimetype was: " << reader.get_mimetype().value() << std::endl;
                constexpr std::string_view expected_mimetype {"application/epub+zip"};
                std::cout << "Comparison is: " << (expected_mimetype == reader.get_mimetype().value()) << '\n';
            } catch (const std::system_error& e) {
                std::cerr << dir_entry.path()
                          << " was unable to be opened by zip library: "
                          << e.code() << ' ' << e.what() << '\n';
            }
        }*/
    }
}

int main(int argc, char* argv[])
{
    std::locale::global(std::locale(""));

    if (parse(argc, argv).has_failure()) {
        return EXIT_SUCCESS;
    }

    if (args::command::DUMP == options.command) {
        auto reader = epub::book_reader(options.input_file);
        auto res = reader.dump();
        if (!res) {
            std::system_error e(res.error());
            std::cerr << "Error: " << e.code() << ' ' << e.what() << std::endl;
        }
        return EXIT_SUCCESS;
    }

    //print_books(options.input_dir);

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
