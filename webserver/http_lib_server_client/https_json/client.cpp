#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../httplib.h"
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

int main() {
    httplib::Client cli("https://yourhost.domain.org:5080");

    if (auto res = cli.Get("/api")) {
        json j = json::parse(res->body);
        std::cout << "ok: " << j["ok"] << "\n";
        std::cout << "msg: " << j["msg"] << "\n";
        std::cout << "value: " << j["value"] << "\n";
    } else {
        std::cerr << "request failed\n";
    }
}
