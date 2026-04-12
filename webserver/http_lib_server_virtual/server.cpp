#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>
#include <unordered_map>

int main() {
    // https setting
    httplib::SSLServer svr("./fullchain.pem", "./privkey.pem");

    if (!svr.is_valid()) {
        std::cerr << "SSL server setup failed\n";
        return 1;
    }

    // 가상호스트 라우팅 / Virtual Host Routing
    svr.set_pre_routing_handler([&](const auto& req, auto& res) {

        std::string host = req.get_header_value("Host");  // Host 기준
        if (auto pos = host.find(':'); pos != std::string::npos) {
            host = host.substr(0, pos);
        }

        if (host == "host1.domain.org") {
            res.set_content("<h1>Hello VirtuslHost 😉</h1>", "text/html; charset=utf-8");
            return httplib::Server::HandlerResponse::Handled;
        }
        if (host == "silverhand.domain.org") {
            res.set_content("<h1>Hello Silverhand 👋</h1>", "text/html; charset=utf-8");
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });

    // 공통 라우트 (호스트 못 맞춘 경우) / Common route (if host does not match)
    svr.Get("/", [](const auto& req, auto& res) {
        res.set_content("Default vhost", "text/plain");
    });

    std::cout << "Server Listening: https://0.0.0.0:5080" << std::endl;
    svr.listen("0.0.0.0", 5080);
}
