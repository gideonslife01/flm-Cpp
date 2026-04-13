#include "httplib.h"
#include <iostream>

static void copy_response(const httplib::Result& r, httplib::Response& res) {
    if (!r) {
        res.status = 502;
        res.set_content("Backend server is unreachable.", "text/plain; charset=utf-8");
        return;
    }
    res.status = r->status;

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
    httplib::Server svr;

    // /api 와 /api/... 모두 처리
    svr.Get(R"(/api.*)", [](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Proxying: " << req.method << " " << req.path << "\n";

        httplib::Client cli("localhost", 3000);

        if (auto backend_res = cli.Get(req.path.c_str())) {
            copy_response(backend_res, res);
        } else {
            res.status = 502;
            res.set_content("Backend server is unreachable.", "text/plain; charset=utf-8");
        }
    });

    std::cout << "Proxy server listening on http://0.0.0.0:5080\n";
    svr.listen("0.0.0.0", 5080);

    return 0;
}
