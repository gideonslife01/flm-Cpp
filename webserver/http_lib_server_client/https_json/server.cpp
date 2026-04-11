#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../httplib.h"
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

int main() {
    httplib::SSLServer svr("./fullchain.pem", "./privkey.pem");

    if (!svr.is_valid()) {
        std::cerr << "SSL server setup failed\n";
        return 1;
    }

    svr.Get("/api", [](const httplib::Request&, httplib::Response& res) {
        json j = {
            {"ok", true},
            {"msg", "hello"},
            {"value", 123}
        };
        res.set_content(j.dump(), "application/json");
    });

    std::cout << "Listening on https://0.0.0.0:5080\n";
    svr.listen("0.0.0.0", 5080);
}
