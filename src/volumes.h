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
    FB1,
    FB2,
    FB3,
    RA1,
    SSC1,
    MP1V1,
    MP1V2,
    MP1V3,
    MP1V4,
    MP1V5,
    MP1V6,
    MP1V7,
    MP2V1,
    MP2V2,
    MP2V3,
    MP2V4,
    MP2V5,
    MP2V6,
    MP2V7,
    SSBDOVA1,    // Otto P1V3
    SSJBUNKO1,   // Benno P1V2
    SSTEASET,    // Ferdinand & Rozemyne P3V1
    SSWN1,       // Hartmut P3V1
    SSWN2,       // Charlotte P4V4
    SSDRAMACD2,  // Tulli P4V3
    SSDRAMACD3,  // Hartmut @ Name Swearing P4V7
    SSDRAMACD4,  // Ferdinand Hair Stick P4V9
    SSTOBBONUS1, // Raimund P4V6
    SSUP1,       // Anastasius/Hannelore P5V1
    SSUFTSS1,    // Unofficial Fan Translated Side Stories (compilation of above)
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
