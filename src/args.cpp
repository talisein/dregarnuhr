#include <ranges>
#include <vector>
#include <algorithm>
#include <iostream>
#include <system_error>
#include <cctype>
#include "outcome/try.hpp"
#include "args.h"
#include "log.h"

struct args options;

namespace {
    void usage(const char *prg)
    {
        log_info("Usage: ", fs::path(prg).filename().string(), " input_dir output_dir\n\n",
                 "input_dir : Directory where Ascendence of a Bookworm .epub files reside.\n",
                 "output_dir: Directory to output new .epub files. Must not be input_dir.");
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

        if (fs::equivalent(input_dir, output_dir)) {
            log_error("Input and output directories are the same. This isn't allowed.\n");
            return std::errc::invalid_argument;
        }

        bool output_exists = fs::exists(output_dir, ec);
        OUTCOME_TRY(handle_error(ec));
        if (!output_exists) {
            log_info("Output directory ", output_dir, " doesn't exist. Creating.\n");
            options.output_created = fs::create_directory(output_dir, ec);
            if (ec) {
                std::system_error e{ec};
                log_error("Failed to create output directory ", output_dir, ": ", e.code(), ' ', e.what());
                return ec;
            }
        } else {
            if (!fs::is_directory(output_dir, ec) || ec) {
                OUTCOME_TRY(handle_error(ec));
                log_error("Output directory ", output_dir, " exists but isn't a directory.\n");
                return std::errc::not_a_directory;
            }
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
        OUTCOME_TRY(verify_output_directory(options.input_dir, options.output_dir));
    }
    return outcome::success();
}
