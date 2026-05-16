// ServerHandler.cpp
#include "ServerHandler.h"
#include "RequestParser.h"
#include "RouterManager.h"
#include "src/proxy/Proxy.h"

std::string handle_request(const std::string& raw_request) {
    HttpRequest request = RequestParser::parse(raw_request);

    // 리버스 프록시 적용 / Apply reverse proxy❗️
    // 간단히 /api/ 경로를 리버스 프록시로 판단
    // Simply treat the /api/ path as a reverse proxy
    if ((method == "GET" || method == "POST") &&
        (path == "/api" || path.find("/api/") == 0)) {

        std::string backend_ip = "127.0.0.1";
        int backend_port = 8080;

        std::string modified_req = req;

        // /api → / 로 변경 (백엔드가 루트 경로로 받도록)
        if (path == "/api") {
            size_t pos = modified_req.find(" /api ");
            if (pos != std::string::npos) {
                modified_req.replace(pos, 5, " /");
            }
        }
        else if (path.find("/api/") == 0) {
            size_t pos = modified_req.find("/api/");
            if (pos != std::string::npos) {
                modified_req.replace(pos, 5, "/");
            }
        }

        // Host 헤더 수정 / Modify Host header
        size_t host_pos = modified_req.find("\r\nHost:");
        if (host_pos == std::string::npos) {
            host_pos = modified_req.find("\r\nhost:");
        }
        if (host_pos != std::string::npos) {
            size_t line_end = modified_req.find("\r\n", host_pos + 1);
            if (line_end != std::string::npos) {
                modified_req.replace(host_pos, line_end - host_pos, "\r\nHost: 127.0.0.1:8080");
            }
        }

        // Connection: close
        size_t conn_pos = modified_req.find("Connection:");
        if (conn_pos != std::string::npos) {
            size_t line_end = modified_req.find("\r\n", conn_pos);
            if (line_end != std::string::npos) {
                modified_req.replace(conn_pos, line_end - conn_pos, "Connection: close");
            }
        }

        return forward_request_to_backend(backend_ip, backend_port, modified_req);
    }

    Router* router = RouterManager::get_router(request.host);
    auto [mime, body] = router->router(request.method, request.path);

    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + mime + "\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    response += "Connection: close\r\n\r\n";
    response += body;

    return response;
}
