#include "httplib.h"
#include <atomic>
#include <iostream>
#include <string>
#include <vector>

struct Backend {
    std::string host;
    int port;
};

static void copy_response(const httplib::Result& r, httplib::Response& res) {
    if (!r) {
        res.status = 502;
        res.set_content("Bad Gateway", "text/plain; charset=utf-8");
        return;
    }

    res.status = r->status;
    for (const auto& h : r->headers) {
        // hop-by-hop headers 제외
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
    httplib::Server lb;

    std::vector<Backend> backends = {
        {"127.0.0.1", 3001},
        {"127.0.0.1", 3002},
        {"127.0.0.1", 3003}
    };

    std::atomic<size_t> rr{0};

    auto next_backend = [&]() -> Backend {
        size_t idx = rr.fetch_add(1, std::memory_order_relaxed) % backends.size();
        return backends[idx];
    };

    auto proxy = [&](const httplib::Request& req, httplib::Response& res) {
        Backend b = next_backend();
        httplib::Client cli(b.host, b.port);

        httplib::Headers headers = req.headers;
        headers.erase("Host");

        std::cout << "[LB] " << req.method << " " << req.path
                  << " -> " << b.host << ":" << b.port << "\n";

        if (req.method == "GET") {
            copy_response(cli.Get(req.path.c_str(), headers), res);
        } else if (req.method == "POST") {
            copy_response(cli.Post(req.path.c_str(), headers, req.body,
                                   req.get_header_value("Content-Type").c_str()), res);
        } else if (req.method == "PUT") {
            copy_response(cli.Put(req.path.c_str(), headers, req.body,
                                  req.get_header_value("Content-Type").c_str()), res);
        } else if (req.method == "PATCH") {
            copy_response(cli.Patch(req.path.c_str(), headers, req.body,
                                    req.get_header_value("Content-Type").c_str()), res);
        } else if (req.method == "DELETE") {
            copy_response(cli.Delete(req.path.c_str(), headers, req.body,
                                     req.get_header_value("Content-Type").c_str()), res);
        } else {
            res.status = 405;
            res.set_content("Method Not Allowed", "text/plain; charset=utf-8");
        }
    };

    lb.Get(R"(.*)", proxy);
    lb.Post(R"(.*)", proxy);
    lb.Put(R"(.*)", proxy);
    lb.Patch(R"(.*)", proxy);
    lb.Delete(R"(.*)", proxy);

    std::cout << "Round-robin LB listening on http://0.0.0.0:5080\n";
    lb.listen("0.0.0.0", 5080);
}
