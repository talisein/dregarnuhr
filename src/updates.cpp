#include <sstream>
#undef CPPHTTPLIB_BROTLI_SUPPORT
#undef CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "updates.h"
#include "log.h"

update_map_t get_updated()
{
    update_map_t map;
    httplib::Client cli("http://aoabmonitor.talinet.net");
    cli.set_follow_location(true);
    auto res = cli.Get("/updates.json");
    if (res->status < 200 || res->status >= 300) {
        log_error("Couldn't fetch updates.json: ", res->status);
        return map;
    }
    std::istringstream iss(res->body);
    for (std::string line; std::getline(iss, line);) {
        if (line.size() < 2) continue;
        auto begin = line.find_first_of('"');
        auto end = line.find_last_of('"');
        auto slug = line.substr(begin+1, end - begin - 1);
        auto ts = std::stoll(line.substr(line.find_last_of(':')+2));
        map.emplace(get_volume_from_slug(slug), std::chrono::seconds(ts));
    }
    return map;
}
