#pragma once

#include <optional>
#include <string_view>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <span>
#include <map>
#include <memory>
#include <variant>
#include <ranges>

#include "outcome/result.hpp"
namespace outcome = OUTCOME_V2_NAMESPACE;

enum class volume
{
    P1V1,
    P1V2,
    P1V3,
    P2V1,
    P2V2,
    P2V3,
    P2V4,
    P3V1,
    P3V2,
    P3V3,
    P3V4,
    P3V5,
    P4V1,
    P4V2,
    P4V3,
    P4V4,
    P4V5,
    P4V6,
    P4V7,
    P4V8,
    P4V9,
    P5V1,
    P5V2,
    P5V3,
    P5V4,
    P5V5,
    P5V6,
    P5V7,
    FB1,
    FB2,
    FB3,
    RA1,
    SSC1,
    M1V1,
    M1V2,
    M1V3,
    M1V4,
    M1V5,
    M1V6,
    M1V7,
    M2V1,
    M2V2,
    M2V3,
    M2V4,
    M2V5,
    M2V6,
    M2V7,
    M3V1,
    M3V2,
    SSBDOVA1,    // Otto P1V3
    SSJBUNKO1,   // Benno P1V2
    SSTEASET,    // Ferdinand & Rozemyne P3V1
    SSDRAMACD2,  // Tulli P4V3
    SSDRAMACD3,  // Hartmut @ Name Swearing P4V7
    SSDRAMACD4,  // Ferdinand Hair Stick P4V9
    SSTOBBONUS1, // Raimund P4V6
    SSUP1,       // Anastasius/Hannelore P5V1
    UFTSS1,    // Unofficial Fan Translated Side Stories (compilation of above)
};

enum class omnibus
{
    PART1,
    PART2,
    PART3,
    PART4,
    PART5,
    ALL
};

std::string_view to_string_view(volume vol);
std::string_view to_string_view(omnibus vol);

std::ostream& operator<<(std::ostream& os, volume v);
std::ostream& operator<<(std::ostream& os, omnibus v);
std::ostream& operator<<(std::ostream& os, const std::variant<omnibus, volume>& v);

volume get_volume_from_slug(std::string_view sv);
std::string_view get_slug_from_volume(volume vol);

outcome::result<volume> get_volume_from_uid(std::string_view uid) noexcept;
outcome::result<std::string_view> get_uid_from_volume(volume vol) noexcept;
