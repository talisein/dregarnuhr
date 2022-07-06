#pragma once

#include <type_traits>
#include <concepts>
#include <optional>
#include <memory>
#include <iostream>
#include "args.h"

template <typename T>
void _log_helper(std::ostream& out, bool autonl, T t)
{
    if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::error_code>) {
        out << t << ' ' << t.message();
    } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::system_error>) {
        out << t.code() << ' ' << t.what();
    } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, char>) {
        if (!autonl || '\n' == t) {
            out << t;
        } else {
            out << t << '\n';
        }
    } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, char*>) {
        std::string_view sv {t};
        if (!autonl || (!sv.empty() && '\n' == sv.back())) {
            out << sv;
        } else {
            out << sv << '\n';
        }
        if (0 != strcmp("\n", t)) {
            out << t;
            if (autonl)
                out << '\n';
        } else if (autonl) {
            out << '\n';
        }
    } else {
        out << t;
        if (autonl)
            out << '\n';
    }
}

template <typename T, typename... Args>
void _log_helper(std::ostream& out, bool autonl, T t, Args... argz)
{
    out << t;
    _log_helper(out, autonl, argz...);
}

template<typename... Args>
void log_error(Args... argz) {
    std::clog << "Error: ";
    _log_helper(std::clog, true, argz...);
}

template<typename... Args>
void log_info(Args... argz) {
    _log_helper(std::cout, true, argz...);
}


template<typename... Args>
void log_verbose_ml(Args... argz) {
    if (get_options()->verbose) {
        _log_helper(std::cout, false, argz...);
    }
}

template<typename... Args>
void log_verbose(Args... argz) {
    if (get_options()->verbose) {
        _log_helper(std::cout, true, argz...);
    }
}
