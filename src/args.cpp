#include <ranges>
#include <string>
#include <locale>
#include <vector>
#include <algorithm>
#include <iostream>
#include <system_error>
#include <cctype>
#include "outcome/try.hpp"
#include "outcome/utils.hpp"
#include "args.h"
#include "log.h"

struct args options;

const args* get_options() { return &options; }

namespace {
    using namespace std::string_view_literals;
    constinit std::string_view DEFAULT_PREFIX { "chronological-"sv };
    constinit std::string_view DEFAULT_OMNIBUS_PART1_TITLE { "Ascendence of a Bookworm: Part 1 Chronological Omnibus"sv };
    constinit std::string_view DEFAULT_OMNIBUS_PART2_TITLE { "Ascendence of a Bookworm: Part 2 Chronological Omnibus"sv };
    constinit std::string_view DEFAULT_OMNIBUS_PART3_TITLE { "Ascendence of a Bookworm: Part 3 Chronological Omnibus"sv };
    constinit std::string_view DEFAULT_OMNIBUS_PART4_TITLE { "Ascendence of a Bookworm: Part 4 Chronological Omnibus"sv };
    constinit std::string_view DEFAULT_OMNIBUS_TITLE { "Ascendence of a Bookworm: Chronological Omnibus"sv };
}

namespace {
    void usage(const char *prg)
    {
        log_info("Usage: ", fs::path(prg).filename().string(), " [options] input_dir output_dir\n\n",
                 "input_dir : Directory where Ascendence of a Bookworm .epub files reside.\n",
                 "output_dir: Directory to output new .epub files. Must not be input_dir.\n\n",
                 "Options:\n",
                 "--omnibus[=all]\t: Create an omnibus epub. If you only want part2, use --omnibus=part2\n"
                 "--title=\"XXX\"\t: Set a title. Only for omnibus.\n"
                 "--cover=XXX.jpg\t: Add a custom cover image. Only for omnibus.\n"
                 "--prefix=XXX\t: Created filenames will be prefixed with XXX. The default is ", std::quoted(DEFAULT_PREFIX), "\n",
                 "--suffix=XXX\t: Created filenames will be suffixed with XXX. The default is blank.\n",
                 "--verbose\t: Print a lot of debugging information\n",
                 "--jpg-scale=N\t: Scale jpg images down by 1/N, where N is between 1-16\n",
                 "--jpg-quality=N\t: Low jpg quality to n, where N is between 1-100\n"
                 "--mode=dump\t: Dump spine and toc data. Give a path to an epub file instead of a directory. This is mostly for development."
            );
    }

    inline result<void> handle_error(const std::error_code& ec)
    {
        if (ec) {
            std::system_error e{ec};
            log_error(e.code(), ' ', e.what());
            return ec;
        }

        return outcome::success();
    }

    result<void> verify_input_file(const fs::path& input_file)
    {
        std::error_code ec;

        if (!fs::exists(input_file, ec)) {
            OUTCOME_TRY(handle_error(ec));
            log_error("Input file ", input_file, " does not exist.\n");
            return std::errc::no_such_file_or_directory;
        }

        if (!fs::is_regular_file(input_file, ec) || ec) {
            OUTCOME_TRY(handle_error(ec));
            log_error("Input file ", input_file, " exists but is not a regular file.\n");
            return std::errc::invalid_argument;
        }

        return outcome::success();
    }

    result<void> verify_input_directory(const fs::path& input_dir)
    {
        std::error_code ec;

        if (!fs::exists(input_dir, ec)) {
            OUTCOME_TRY(handle_error(ec));
            log_error("Input directory ", input_dir, " does not exist.\n");
            return std::errc::no_such_file_or_directory;
        }

        if (!fs::is_directory(input_dir, ec) || ec) {
            OUTCOME_TRY(handle_error(ec));
            log_error("Input path ", input_dir, " exists but is not a directory.\n");
            return std::errc::not_a_directory;
        }

        return outcome::success();
    }

    result<void> verify_output_directory(const fs::path& input_dir, const fs::path& output_dir)
    {
        options.output_created = false;
        std::error_code ec;

        bool output_exists = fs::exists(output_dir, ec);
        OUTCOME_TRY(handle_error(ec));

        if (!output_exists) {
            log_info("Output directory ", output_dir, " doesn't exist. Creating.");
            options.output_created = fs::create_directory(output_dir, ec);
            if (ec) {
                std::system_error e{ec};
                log_error("Failed to create output directory ", output_dir, ": ", e.code(), ' ', e.what());
                return ec;
            } else {
                log_verbose("Info: created directory ", output_dir.string());
            }
        } else {
            if (!fs::is_directory(output_dir, ec) || ec) {
                OUTCOME_TRY(handle_error(ec));
                log_error("Output directory ", output_dir, " exists but isn't a directory.\n");
                return std::errc::not_a_directory;
            }

            if (fs::equivalent(input_dir, output_dir, ec)) {
                log_error("Input and output directories are the same. This isn't allowed.\n");
                return std::errc::invalid_argument;
            }
            OUTCOME_TRY(handle_error(ec));
        }

        return outcome::success();
    }
} // anonymous namespace


result<void>
parse(int argc, char **argv)
{
    using namespace std::string_view_literals;
    using namespace std::ranges;
    const std::vector<std::string_view> args_sv { argv + 1, argv + argc};

    if (argc < 3) {
        usage(argv[0]);
        return outcome::failure(std::errc::invalid_argument);
    }
    options.command = args::command::NORMAL;
    auto args_options { views::filter(args_sv, [](const auto &sv) { return sv.starts_with("--"sv); }) };
    auto args_files { views::filter(args_sv, [](const auto &sv) { return !sv.starts_with("--"sv); }) };
    if (find (args_options, "--mode=dump"sv) != args_options.end()) {
        log_info("MODE=dump");
        options.command = args::command::DUMP;
        options.verbose = true;
    }
    if (find (args_options, "--verbose"sv) != args_options.end()) {
        options.verbose = true;
    }
    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--suffix="sv); }); it != args_options.end()) {
        auto pos = it->find("="sv);
        options.suffix = std::make_optional<std::string>(it->substr(pos+1));
        log_info("Info: Filename suffix set to ", std::quoted(*options.suffix));
    }
    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--prefix="sv); }); it != args_options.end()) {
        auto pos = it->find("="sv);
        options.prefix = std::make_optional<std::string>(it->substr(pos+1));
        if (options.prefix->empty() && (!options.suffix || options.suffix->empty())) {
            log_error("No suffix and empty prefix defined. This would create files with the same name as the input; that will make things too confusing!");
            return std::errc::invalid_argument;
        }
        log_info("Info: Filename prefix set to ", std::quoted(*options.prefix));
    }

    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--filter="sv); }); it != args_options.end()) {
        const auto filters = it->substr(it->find("="sv)+1);
        // Size
        if (const auto pos = filters.find("size="sv); std::string::npos != pos) {
            auto colon_pos = filters.find(":", pos);
            auto cnt = std::string::npos;
            if (std::string::npos != colon_pos) {
                cnt = colon_pos - pos - 5;
            }
            options.size_filter = std::stoull(std::string(filters.substr(pos + 5, cnt)));
            log_info("Size filter: ", *options.size_filter);
        }
        // Name
        if (const auto pos = filters.find("name="sv); std::string::npos != pos) {
            try {
                auto colon_pos = filters.find(":", pos);
                auto cnt = std::string::npos;
                if (std::string::npos != colon_pos) {
                    cnt = colon_pos - pos - 5;
                }
                options.name_filter = std::make_optional<std::regex>(std::string(filters.substr(pos + 5, cnt)), std::regex_constants::icase);
            } catch (std::system_error &e) {
                log_error("Failed to prepare regex name filter: ", e.what());
                return e.code();
            } catch (std::exception &e) {
                log_error("Failed to prepare regex name filter: ", e.what());
                return outcome::error_from_exception();
            }
        }
    }
    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--jpg-quality="sv); }); it != args_options.end()) {
        auto pos = it->find("="sv);
        std::string quality(it->substr(pos+1));
        auto i = std::stoi(quality);
        options.jpg_quality = std::make_optional<int>( i );
        log_info("JPG quality will be ", options.jpg_quality.value());
    }
    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--jpg-scale="sv); }); it != args_options.end()) {
        auto pos = it->find("="sv);
        std::string scale(it->substr(pos+1));
        auto i = std::stoi(scale);
        options.jpg_scale = std::make_optional<int>( i );
        log_info("JPG scale will be ", options.jpg_scale.value());
    }
    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--title="sv); }); it != args_options.end()) {
        auto pos = it->find("="sv);
        options.title = std::make_optional<std::string>(it->substr(pos+1));
        if (options.title.value().size() == 0) {
            log_error("Setting a blank title seems like a mistake");
            return std::errc::invalid_argument;
        }
        log_info("Title set to ", std::quoted(options.title.value()));
    }
    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--omnibus"sv); }); it != args_options.end()) {
        auto pos = it->find("="sv);
        if (std::string_view::npos == pos) {
            options.omnibus_type = omnibus::ALL;
        } else {
            auto locale = std::locale();
            std::string type (it->substr(pos+1));
            std::ranges::transform(it->substr(pos+1), type.begin(), [&locale](const auto c){ return std::tolower(c, locale); });
            if (type == "part1") {
                options.omnibus_type = omnibus::PART1;
            } else if (type == "part2") {
                options.omnibus_type = omnibus::PART2;
            } else if (type == "part3") {
                options.omnibus_type = omnibus::PART3;
            } else if (type == "part4") {
                options.omnibus_type = omnibus::PART4;
            } else if (type == "all") {
                options.omnibus_type = omnibus::ALL;
            } else {
                log_error("Unrecognized omnibus value: ", type);
                return std::errc::invalid_argument;
            }
            log_info("Omnibus ", *options.omnibus_type, " selected.");
        }
        if (!options.title) {
            switch (options.omnibus_type.value()) {
                case omnibus::PART1:
                    options.title = std::make_optional<std::string>(DEFAULT_OMNIBUS_PART1_TITLE);
                    break;
                case omnibus::PART2:
                    options.title = std::make_optional<std::string>(DEFAULT_OMNIBUS_PART2_TITLE);
                    break;
                case omnibus::PART3:
                    options.title = std::make_optional<std::string>(DEFAULT_OMNIBUS_PART3_TITLE);
                    break;
                case omnibus::PART4:
                    options.title = std::make_optional<std::string>(DEFAULT_OMNIBUS_PART4_TITLE);
                    break;
                case omnibus::ALL:
                    options.title = std::make_optional<std::string>(DEFAULT_OMNIBUS_TITLE);
                    break;
            }
            log_info("Omnibus title defaulted to: ", options.title.value());
        }
    }
    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--cover="sv); }); it != args_options.end()) {
        auto pos = it->find("="sv);
        auto path = fs::path(it->substr(pos+1));
        std::error_code ec;
        if (!fs::exists(path, ec) || ec) {
            log_error("Cover ", path, " doesn't exist");
            if (ec) return ec;
            return std::errc::invalid_argument;
        }
        auto status = fs::status(path, ec);
        if (ec) {
            log_error("Cover ", path, " can't be read: ", ec.message());
        }
        if (status.type() != fs::file_type::regular) {
            log_error("Cover ", path, " isn't a regular file");
            if (ec) return ec;
            return std::errc::invalid_argument;
        }
        const fs::path JPG {".jpg"};
        if (path.extension() != JPG) {
            log_error("Cover ", path, " isn't a .jpg");
            return std::errc::invalid_argument;
        }
        options.cover = path;
    }

    if (!options.prefix && !options.suffix) {
        options.prefix = std::make_optional<std::string>(DEFAULT_PREFIX);
    }



    if (args::command::DUMP == options.command) {
        const auto num_files = std::distance(args_files.begin(), args_files.end());
        if (num_files == 0) {
            usage(argv[0]);
            return outcome::failure(std::errc::invalid_argument);
        }
        auto it = args_files.begin();

        if (2 == num_files) {
            options.dump_volume = *it++;
            options.input_file = *it;
        } else {
            options.input_file = *it;
        }

        OUTCOME_TRY(verify_input_file(options.input_file));
    }

    if (args::command::NORMAL == options.command) {
        if (std::distance(args_files.begin(), args_files.end()) != 2) {
            usage(argv[0]);
            return outcome::failure(std::errc::invalid_argument);
        }

        auto it = args_files.begin();
        options.input_dir = *it++;
        options.output_dir = *it;

        OUTCOME_TRY(verify_input_directory(options.input_dir));
        log_verbose("Verified input dir: ", options.input_dir.string());
        OUTCOME_TRY(verify_output_directory(options.input_dir, options.output_dir));
        log_verbose("Verified output dir: ", options.output_dir.string());
    }
    return outcome::success();
}
