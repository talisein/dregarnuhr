#pragma once

#include <filesystem>
#include <optional>
#include <regex>
#include "outcome/result.hpp"
#include "volumes.h"

namespace fs = std::filesystem;
namespace outcome = OUTCOME_V2_NAMESPACE;
using outcome::result;

struct args
{
    enum class command
    {
        NORMAL,
        DUMP,
        TEST
    } command;


    std::optional<std::string> dump_volume;
    std::optional<std::string> prefix;
    std::optional<std::string> suffix;
    std::optional<std::string> title;
    fs::path input_file;
    fs::path input_dir;
    fs::path output_dir;
    std::optional<fs::path> cover;
    bool output_created;
    bool verbose;
    std::optional<std::size_t> size_filter;
    std::optional<std::regex> name_filter;
    std::optional<int> jpg_quality;
    std::optional<int> jpg_scale;
    std::optional<omnibus> omnibus_type;
};


const args* get_options();

result<void> parse(int argc, char **argv);
