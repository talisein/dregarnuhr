#pragma once

#include <filesystem>
#include <optional>
#include "outcome/result.hpp"


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
    fs::path input_file;
    fs::path input_dir;
    fs::path output_dir;
    bool output_created;
    bool verbose;
};


extern args options;

result<void> parse(int argc, char **argv);
