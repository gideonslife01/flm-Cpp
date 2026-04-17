#include "httplib.h"
#include <atomic>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "usage: ./target_server <server_id> <port>\n";
        return 1;
    }

    const std::string server_id = argv[1];
    const int port = std::stoi(argv[2]);

    httplib::Server svr;
    std::atomic<unsigned long long> hit{0};

    svr.Get("/", [&](const httplib::Request&, httplib::Response& res) {
        auto n = ++hit;
        std::string html =
            "<!doctype html><html><head><meta charset='utf-8'><title>Target</title></head><body>"
            "<h1>Target Server: " + server_id + "</h1>"
            "<p>Port: " + std::to_string(port) + "</p>"
            "<p>Hit Count: " + std::to_string(n) + "</p>"
            "<p>브라우저에서 새로고침하면 LB가 다른 서버로 보낼 수 있습니다.</p>"
            "<p>If you refresh the browser, the LB may send it to a different server.</p>"
            "</body></html>";

        res.set_content(html, "text/html; charset=utf-8");
    });

    std::cout << "target " << server_id << " listening on http://0.0.0.0:" << port << "\n";
    svr.listen("0.0.0.0", port);
}
