#include "../router/RouterHandler.h"
#include "../router/RouterConfig.h"

// 서버 라우터 맵 / server router map
std::unordered_map<std::string, Router*> server_routers = {
    {"server1", &RouterConfig::server1_router},
    {"server2", &RouterConfig::server2_router},
    {"server3", &RouterConfig::server3_router},
};

// 라우터 선택,메소드 검증,헤더와 마임타입 리턴
// Select router, validate method, return header and MIME type
std::string RouterHandler::process_request(const HttpRequest& parsed_req, const std::string& server_id="server1") {
    std::pair<std::string, std::string> result;
    std::string host = parsed_req.host;

    // Host 정규화 (포트 제거) / Host Normalization (Port Removal)
    std::string host_normalized = host;
    size_t pos = host.find(':');
    if (pos != std::string::npos) {
        host_normalized = host.substr(0, pos);
    }

    // 기본 라우터 설정 / Default router settings
    // Router* router_ptr = &RouterConfig::server1_router;
    // auto it = RouterConfig::vhost_routers.find(host_normalized);
    // if (it != RouterConfig::vhost_routers.end()) {
    //     router_ptr = it->second;
    // }

    // 서버 식별자로 라우터 선택 / Select router by server identifier
    Router* router_ptr = nullptr;
    auto it = server_routers.find(server_id);
    if (it != server_routers.end()) {
        router_ptr = it->second;
    } else {

        // 서버 id 존재하지 않은 경우 기본 라우터로 설정
        // Set as default router if server ID does not exist
        router_ptr = &RouterConfig::server1_router;
    }

    // HTTP 메서드 검증 / HTTP method validation
    const auto& method = parsed_req.method;
    const auto& path = parsed_req.path;

    if (method == "GET" || method == "POST" || method == "PUT" ||
        method == "DELETE" || method == "FETCH") {
        result = router_ptr->router(method, path);
    } else {
        return std::string(
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Allow: GET, POST, PUT, DELETE, FETCH\r\n"
            "Connection: close\r\n\r\n"
            "Method Not Allowed"
        );
    }

    // 라우터가 반환한 마임 타입과 본문
    // MIME type and body returned by the router
    const auto& [mime, body] = result;

    // HTTP 응답 헤더 생성
    // Generate HTTP response headers
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + mime + "\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    response += "Connection: close\r\n\r\n";

    // 본문 추가 / Add body
    response += body;

    return response;
}
