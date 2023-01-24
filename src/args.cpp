#include <ranges>
#include <string>
#include <locale>
#include <vector>
#include <algorithm>
#include <iostream>
#include <system_error>
#include <cctype>
#include "ctre.hpp"
#include "config.h"
#include "args.h"
#include "log.h"
#include "miniz.h"
#include "updates.h"
#include "utils.h"

struct args options;

const args* get_options() { return &options; }

namespace {
    using namespace std::string_view_literals;
    constexpr std::string_view DEFAULT_PREFIX { "chronological-"sv };
    constexpr std::string_view DEFAULT_OMNIBUS_PART1_TITLE { "Ascendence of a Bookworm: Part 1 Chronological Omnibus"sv };
    constexpr std::string_view DEFAULT_OMNIBUS_PART2_TITLE { "Ascendence of a Bookworm: Part 2 Chronological Omnibus"sv };
    constexpr std::string_view DEFAULT_OMNIBUS_PART3_TITLE { "Ascendence of a Bookworm: Part 3 Chronological Omnibus"sv };
    constexpr std::string_view DEFAULT_OMNIBUS_PART4_TITLE { "Ascendence of a Bookworm: Part 4 Chronological Omnibus"sv };
    constexpr std::string_view DEFAULT_OMNIBUS_PART5_TITLE { "Ascendence of a Bookworm: Part 5 Chronological Omnibus"sv };
    constexpr std::string_view DEFAULT_OMNIBUS_TITLE { "Ascendence of a Bookworm: Chronological Omnibus"sv };
    constexpr mz_uint DEFAULT_COMPRESSION_LEVEL = MZ_DEFAULT_LEVEL;
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
                 "--compression-level=[0-10,fastest,smallest]\t: Set compression level; only for generated files. The default is ", DEFAULT_COMPRESSION_LEVEL, '\n',
                 "--no-nested\t: Skip creating a nested NCX (table of contents) for omnibus outputs.\n"
                 "--mode=dump\t: Dump spine and toc data. Give a path to an epub file instead of a directory. This is mostly for development."
            );
    }

    std::expected<void, std::error_code> verify_input_file(const fs::path& input_file)
    {
        if (std::error_code ec; !fs::exists(input_file, ec) || ec) {
            log_error("Input file ", input_file, " does not exist. ", ec);
            if (ec)
                return std::unexpected(ec);
            else
                return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }

        if (std::error_code ec; !fs::is_regular_file(input_file, ec) || ec) {
            log_error("Input file ", input_file, " exists but is not a regular file. ", ec);
            if (ec)
                return std::unexpected(ec);
            else
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }

        return {};
    }

    std::expected<void, std::error_code> verify_input_directory(const fs::path& input_dir)
    {
        if (std::error_code ec; !fs::exists(input_dir, ec) || ec) {
            log_error("Input directory ", input_dir, " does not exist. ", ec);
            if (ec)
                return std::unexpected(ec);
            else
                return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }

        if (std::error_code ec; !fs::is_directory(input_dir, ec) || ec) {
            log_error("Input path ", input_dir, " exists but is not a directory. ", ec);
            if (ec)
                return std::unexpected(ec);
            else
                return std::unexpected(std::make_error_code(std::errc::not_a_directory));
        }

        return {};
    }

    std::expected<void, std::error_code> verify_output_directory(const fs::path& input_dir, const fs::path& output_dir)
    {
        options.output_created = false;
        std::error_code ec;

        bool output_exists = fs::exists(output_dir, ec);
        if (ec) {
            log_error("Output directory couldn't be checked: ", ec);
            return std::unexpected(ec);
        }

        if (!output_exists) {
            log_info("Output directory ", output_dir, " doesn't exist. Creating.");
            options.output_created = fs::create_directory(output_dir, ec);
            if (ec) {
                log_error("Failed to create output directory ", output_dir, ": ", ec);
                return std::unexpected(ec);
            } else {
                log_verbose("Info: created directory ", output_dir.string());
            }
        } else {
            if (!fs::is_directory(output_dir, ec) || ec) {
                log_error("Output directory ", output_dir, " exists but isn't a directory. ", ec);
                if (ec)
                    return std::unexpected(ec);
                else
                    return std::unexpected(std::make_error_code(std::errc::not_a_directory));
            }

            if (fs::equivalent(input_dir, output_dir, ec)) {
                log_error("Input and output directories are the same. This isn't allowed.");
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
            if (ec) {
                log_error("While comparing input and output directories: ", ec);
                return std::unexpected(ec);
            }
        }

        return {};
    }
} // anonymous namespace


std::expected<void, std::error_code>
parse(int argc, char** argv)
{
    using namespace std::string_view_literals;
    using namespace std::ranges;
    auto args_view = std::views::counted(argv, argc) | std::views::drop(1) | std::views::transform([](const char *p) -> std::string_view { return std::string_view(p); });
    std::vector<std::string_view> args_sv(begin(args_view), end(args_view));
//    auto args_sv = args_view;

    options.command = args::command_e::NORMAL;
    if (argc < 2) {
        usage(argv[0]);
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
    auto args_options { views::filter(args_sv, [](const auto &sv) { return sv.starts_with("--"sv); }) };
    auto args_files { views::filter(args_sv, [](const auto &sv) { return !sv.starts_with("--"sv); }) };
    if (find (args_options, "--mode=dump"sv) != args_options.end()) {
        log_info("MODE=dump");
        options.command = args::command_e::DUMP;
        options.verbose = true;
    }
    if (find (args_options, "--verbose"sv) != args_options.end()) {
        options.verbose = true;
    }

    options.suffix = utils::find_if_optarg<std::string>(args_options, "--suffix="sv, std::identity{});
    if (options.suffix) log_info("Filename suffix set to ", std::quoted(*options.suffix));

    options.prefix = utils::find_if_optarg<std::string>(args_options, "--prefix="sv);

    if (options.prefix) {
        log_info("Filename prefix set to ", std::quoted(*options.prefix));
        if (options.prefix->empty() && (!options.suffix || options.suffix->empty())) {
            log_error("No suffix and empty prefix defined. This would create files with the same name as the input; that will make things too confusing!");
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
    }

    const auto filters = utils::find_if_optarg(args_options, "--filter="sv);

    if (filters) {
        // Size
        if (const auto pos = filters->find("size="sv); std::string::npos != pos) {
            auto colon_pos = filters->find(':', pos);
            auto cnt = std::string::npos;
            if (std::string_view::npos != colon_pos) {
                cnt = colon_pos - pos - 5;
            }
            options.size_filter = std::stoull(std::string(filters->substr(pos + 5, cnt)));
            log_info("Size filter: ", *options.size_filter);
        }
        // Name
        if (const auto pos = filters->find("name="sv); std::string::npos != pos) {
            try {
                auto colon_pos = filters->find(":", pos);
                auto cnt = std::string::npos;
                if (std::string::npos != colon_pos) {
                    cnt = colon_pos - pos - 5;
                }
                options.name_filter = std::make_optional<std::regex>(std::string(filters->substr(pos + 5, cnt)), std::regex_constants::icase);
            } catch (std::system_error &e) {
                log_error("Failed to prepare regex name filter: ", e.what());
                return std::unexpected(e.code());
            } catch (std::exception &e) {
                log_error("Failed to prepare regex name filter: ", e.what());
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
            }
        }
    }

    constexpr auto to_int = [](std::string_view s) -> std::optional<int> {
        if (int value; std::from_chars(std::to_address(s.begin()), std::to_address(s.end()), value).ec == std::errc{})
            return value;
        else
            return std::nullopt;
    };

    options.jpg_quality = utils::find_if_optarg(args_options, "--jpg-quality"sv).and_then(to_int);
    options.jpg_scale = utils::find_if_optarg(args_options, "--jpg-scale"sv).and_then(to_int);
    options.title = utils::find_if_optarg(args_options, "--title=");

    options.title = utils::find_if_optarg(args_options, "--title="sv);
    if (options.title) {
        if (options.title.value().size() == 0) {
            log_error("Setting a blank title seems like a mistake");
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        log_info("Title set to ", std::quoted(options.title.value()));
    }

    if (auto opt = utils::find_if_opt(args_options, [](const auto sv) { return sv.starts_with("--omnibus"sv); }); opt) {
        auto pos = opt->find('=');
        if (std::string_view::npos == pos) {
            options.omnibus_type = omnibus::ALL;
        } else {
            const auto type {opt->substr(pos+1)};
            if (ctre::match<"part1", ctre::case_insensitive>(type)) {
                options.omnibus_type = omnibus::PART1;
            } else if (ctre::match<"part2", ctre::case_insensitive>(type)) {
                options.omnibus_type = omnibus::PART2;
            } else if (ctre::match<"part3", ctre::case_insensitive>(type)) {
                options.omnibus_type = omnibus::PART3;
            } else if (ctre::match<"part4", ctre::case_insensitive>(type)) {
                options.omnibus_type = omnibus::PART4;
            } else if (ctre::match<"part5", ctre::case_insensitive>(type)) {
                options.omnibus_type = omnibus::PART5;
            } else if (ctre::match<"all", ctre::case_insensitive>(type)) {
                options.omnibus_type = omnibus::ALL;
            } else {
                log_error("Unrecognized omnibus value: ", type);
                return std::unexpected(std::make_error_code(std::errc::invalid_argument));
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
                case omnibus::PART5:
                    options.title = std::make_optional<std::string>(DEFAULT_OMNIBUS_PART5_TITLE);
                    break;
                case omnibus::ALL:
                    options.title = std::make_optional<std::string>(DEFAULT_OMNIBUS_TITLE);
                    break;
            }
            log_info("Omnibus title defaulted to: ", options.title.value());
        }
    }

    options.cover = utils::find_if_optarg<fs::path>(args_options, "--cover="sv);
    if (options.cover) {
        const auto path = *options.cover;
        std::error_code ec;

        auto status = fs::status(path, ec);
        if (ec) {
            log_error("Cover ", path, " can't be read: ", ec);
            return std::unexpected(ec);
        }

        if (!fs::exists(status)) {
            log_error("Cover ", path, " doesn't exist ", ec);
            if (ec) return std::unexpected(ec);
            return std::unexpected(std::make_error_code(std::errc::no_such_file_or_directory));
        }

        if (!fs::is_regular_file(status)) {
            log_error("Cover ", path, " isn't a regular file");
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }

        if (!ctre::match<"[.]jpg", ctre::case_insensitive>(path.extension().string())) {
            log_error("Cover ", path, " isn't a .jpg");
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        options.cover = path;
    }
    if (auto it = find_if (args_options, [](const auto& opt){ return opt.starts_with("--compression-level="sv); }); it != args_options.end()) {
        auto pos = it->find("="sv);
        auto level = fs::path(it->substr(pos+1));
        if (level == "smallest"sv) {
            options.compression_level = MZ_BEST_COMPRESSION;
        } else if (level == "fastest"sv) {
            options.compression_level = MZ_BEST_SPEED;
        } else {
            auto l = std::stoi(level);
            options.compression_level = std::clamp<mz_uint>(l, MZ_NO_COMPRESSION, MZ_UBER_COMPRESSION);
        }
        log_info("Compression level set to ", *options.compression_level);
    }
    if (find (args_options, "--check"sv) != args_options.end()) {
        auto map = get_updated();
        if (map.size() > 0) {
            log_info("Got updates.json, size ", map.size());
        } else {
            log_info("Failed to get updates.json");
        }
        log_info("This executable works.");
        options.command = args::command_e::DUMP;
    }
    if (options.omnibus_type) {
        options.do_nested = true;
        if (find (args_options, "--no-nested"sv) != args_options.end()) {
            options.do_nested = false;
        }
    }

    if (!options.prefix && !options.suffix) {
        options.prefix = std::make_optional<std::string>(DEFAULT_PREFIX);
    }
    if (!options.compression_level) {
        options.compression_level = DEFAULT_COMPRESSION_LEVEL;
    }


    if (args::command_e::DUMP == options.command) {
        const auto num_files = std::distance(args_files.begin(), args_files.end());
        if (num_files == 0) {
            usage(argv[0]);
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }
        auto it = args_files.begin();

        if (2 == num_files) {
            options.dump_volume = *it++;
            options.input_file = *it;
        } else {
            options.input_file = *it;
        }

        if (auto res = verify_input_file(options.input_file); !res.has_value()) {
            return res;
        }
    }

    if (args::command_e::NORMAL == options.command) {
        if (std::distance(args_files.begin(), args_files.end()) != 2) {
            usage(argv[0]);
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }

        auto it = args_files.begin();
        options.input_dir = *it++;
        options.output_dir = *it;

        if (auto res = verify_input_directory(options.input_dir); !res.has_value()) {
            return res;
        }
        log_verbose("Verified input dir: ", options.input_dir.string());
        if (auto res = verify_output_directory(options.input_dir, options.output_dir); !res) {
            return std::unexpected(res.error());
        }
        log_verbose("Verified output dir: ", options.output_dir.string());
    }
    return {};
}

std::ostream&
operator<<(std::ostream& os, args::command_e c)
{
    switch (c) {
        case args::command_e::DUMP: os << "Command::DUMP"; break;
        case args::command_e::NORMAL: os << "Command::NORMAL"; break;
        case args::command_e::TEST: os << "Command::TEST"; break;
        default:
            os << "Command:UNIMPLMENTED?"; break;
    }
    return os;
}
