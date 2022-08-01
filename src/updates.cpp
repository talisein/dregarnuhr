#include <sstream>
#include "httplib.h"
#include "updates.h"
#include "log.h"

update_map_t get_updated()
{
    update_map_t map;
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
