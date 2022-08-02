#include <ranges>
#include <cinttypes>
#include <cctype>
#include <charconv>

#include "httplib.h"
#include "updates.h"
#include "log.h"

namespace {
    update_map_t parse(std::string_view body)
    {
        update_map_t map;
        for (const auto line : std::views::split(body, '\n')) {
            auto colon_split = std::views::split(line, ':');
            auto colon_it = std::ranges::begin(colon_split);
            if (colon_it == std::ranges::end(colon_split)) continue;
            auto quote_split = std::views::split(*colon_it, '"');
            auto quote_it = std::ranges::next(std::ranges::begin(quote_split), 1, std::ranges::end(quote_split));
            if (quote_it == std::ranges::end(quote_split)) continue;
            auto slug_r = *quote_it;
            auto slug_sv = std::string_view(std::ranges::begin(slug_r), std::ranges::end(slug_r));
            std::ranges::advance(colon_it, 1, std::ranges::end(colon_split));
            if (colon_it == std::ranges::end(colon_split)) continue;
            auto no_ws = std::views::drop_while(*colon_it, [](unsigned char c) {return std::isspace(c);});
            std::chrono::seconds::rep ts;
            auto [ptr, ec] = std::from_chars(std::to_address(std::ranges::begin(no_ws)),
                                             std::to_address(std::ranges::end(no_ws)), ts);
            if (ec != std::errc()) continue;
            auto it = map.emplace(get_volume_from_slug(slug_sv), std::chrono::seconds(ts));
            log_verbose("Emplaced ", it.first->first, ": ", it.first->second.time_since_epoch());
        }
        return map;
    }
}


update_map_t get_updated()
{
// Support https if someone builds from source
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    httplib::Client cli("https://aoabmonitor.talinet.net");
#else
    httplib::Client cli("http://aoabmonitor.talinet.net");
#endif
    cli.set_follow_location(true);
    cli.set_connection_timeout(5, 0); // 5 seconds
    cli.set_read_timeout(5, 0);
    cli.set_write_timeout(5, 0);
    auto res = cli.Get("/updates.json");
    if (!res) {
        log_error("Couldn't fetch updates.json: ", res.error());
        return {};
    }

    return parse(res->body);
}
