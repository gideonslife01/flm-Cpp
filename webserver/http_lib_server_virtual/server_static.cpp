#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

static std::string read_file(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

static std::string sanitize_path(const std::string& raw) {
    if (raw.empty() || raw[0] != '/') return "/index.html";
    if (raw.find("..") != std::string::npos) return "/index.html";
    if (raw.find('\\') != std::string::npos) return "/index.html";
    if (raw.back() == '/') return raw + "index.html";
    if (raw == "/") return "/index.html";
    return raw;
}

static std::string mime_type(const std::string& path) {
    auto pos = path.find_last_of('.');
    if (pos == std::string::npos) return "application/octet-stream";
    auto ext = path.substr(pos + 1);
    if (ext == "html") return "text/html; charset=utf-8";
    if (ext == "css") return "text/css; charset=utf-8";
    if (ext == "js") return "application/javascript; charset=utf-8";
    if (ext == "json") return "application/json; charset=utf-8";
    if (ext == "png") return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "svg") return "image/svg+xml";
    if (ext == "ico") return "image/x-icon";
    if (ext == "txt") return "text/plain; charset=utf-8";
    return "application/octet-stream";
}

int main() {
    // 인증서 경로 / certificate path
    httplib::SSLServer svr("./fullchain.pem", "./privkey.pem");

    if (!svr.is_valid()) {
        std::cerr << "SSL server setup failed\n";
        return 1;
    }

    // 호스트별 루트 폴더 매핑 (절대경로 권장)
    // Mapping root folders by host (absolute paths recommended)
    std::unordered_map<std::string, std::string> roots = {
        {"host1.domain.org", "/home/username/http_lib_server_virtual/www"},
        {"silverhand.domain.org", "/home/username/http_lib_server_virtual/www2"}
    };

    svr.set_pre_routing_handler([&](const auto& req, auto& res) {
        std::string host = req.get_header_value("Host");
        if (auto pos = host.find(':'); pos != std::string::npos) {
            host = host.substr(0, pos);
        }

        auto it = roots.find(host);
        if (it == roots.end()) {
            res.status = 404;
            res.set_content("Unknown host", "text/plain; charset=utf-8");
            return httplib::Server::HandlerResponse::Handled;
        }

        std::string rel = sanitize_path(req.path);
        std::string full = it->second + rel;

        auto body = read_file(full);
        if (body.empty()) {
            res.status = 404;
            res.set_content("Not Found", "text/plain; charset=utf-8");
        } else {
            res.set_content(body, mime_type(full).c_str());
        }
        return httplib::Server::HandlerResponse::Handled;
    });

    std::cout << "Listening on https://0.0.0.0:5080\n";
    svr.listen("0.0.0.0", 5080);
}
