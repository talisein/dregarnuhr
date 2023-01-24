#pragma once

#include <filesystem>
#include <optional>
#include <regex>
#include <string>
#include <expected>
#include "outcome/result.hpp"
#include "volumes.h"

namespace fs = std::filesystem;
namespace outcome = OUTCOME_V2_NAMESPACE;
using outcome::result;

struct args
{
    enum class command_e
    {
        NORMAL,
        DUMP,
        TEST
    } command;


    std::optional<std::string> dump_volume;
    std::optional<std::string> prefix;
    std::optional<std::string> suffix;
    std::optional<std::string> title;
    std::optional<fs::path> cover;
    fs::path input_file;
    fs::path input_dir;
    fs::path output_dir;
    bool output_created;
    bool verbose;
    bool do_nested;
    std::optional<std::size_t> size_filter;
    std::optional<std::regex> name_filter;
    std::optional<int> jpg_quality;
    std::optional<int> jpg_scale;
    std::optional<omnibus> omnibus_type;
    std::optional<unsigned int> compression_level;
};

std::ostream& operator<<(std::ostream& os, args::command_e c);

const args* get_options();

std::expected<void, std::error_code> parse(int argc, char **argv);
