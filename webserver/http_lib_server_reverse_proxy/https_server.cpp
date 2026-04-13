#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>

static void copy_response(const httplib::Result& r, httplib::Response& res) {
    if (!r) {
        res.status = 502;
        res.set_content("Backend server is unreachable.", "text/plain; charset=utf-8");
        return;
    }
    res.status = r->status;

    // hop-by-hop 헤더는 제외
    for (const auto& h : r->headers) {
        if (h.first == "Connection" || h.first == "Keep-Alive" ||
            h.first == "Proxy-Authenticate" || h.first == "Proxy-Authorization" ||
            h.first == "TE" || h.first == "Trailers" ||
            h.first == "Transfer-Encoding" || h.first == "Upgrade") {
            continue;
        }
        res.set_header(h.first.c_str(), h.second.c_str());
    }

    res.body = r->body;
}

int main() {
    httplib::SSLServer svr("./fullchain.pem", "./privkey.pem");

    if (!svr.is_valid()) {
        std::cerr << "SSL server setup failed\n";
        return 1;
    }

    // 모든 /api/... 요청을 프록시 처리
    svr.Get(R"(/api.*)", [](const httplib::Request& req, httplib::Response& res) {
        httplib::Client cli("localhost", 3000);

        if (auto backend_res = cli.Get(req.path.c_str())) {
            copy_response(backend_res, res);
        } else {
            res.status = 502;
            res.set_content("Backend server is unreachable.", "text/plain; charset=utf-8");
        }
    });

    std::cout << "Proxy server listening on https://0.0.0.0:5080\n";
    svr.listen("0.0.0.0", 5080);

    return 0;
}
