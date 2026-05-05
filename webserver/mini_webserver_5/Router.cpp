#include <iostream>
#include <string>
#include <utility> // std::pair 사용 / Use std::pair
#include "Router.h"


// 생성자 / Constructor
Router::Router() {
// 초기화 로직 / initialization logic
}

// 경로에 따라 마임타입과 본문을 리턴하는 함수- 기능 추가 ❗️
// Function to return MIME type and body based on path - Feature added
std::pair<std::string, std::string> Router::router(const std::string& request) {
    size_t start = request.find(" ") + 1;
    size_t end = request.find(" ", start);

    if (start == std::string::npos || end == std::string::npos) {
        return {"text/html", "<h1>Error</h1>"};
    }

    std::string path = request.substr(start, end - start);
    std::cout << "[Router] Client requested path: " << path << std::endl;

    std::string mime_type = "text/html; charset=UTF-8";
    std::string body;

    // 라우터 / router
    if (path == "/" || path == "/index") {
        body = "<h1>🏠 메인 페이지/main page</h1><a href='/login'>로그인/login</a>";
    }
    else if (path == "/login") {
        body = "<h1>🔑 로그인 페이지/login page</h1>";
    }
    else if (path == "/style.css") {
        mime_type = "text/css"; //❗️
        body = "body { background-color: powderblue; }";
    }
    else if (path == "/data") {
        mime_type = "application/json"; //❗️
        body = "{\"status\": \"running\", \"code\": 200}";
    }
    else {
        body = "<h1 style='color:red;'>404 Not Found</h1>";
    }
    return {mime_type, body};
}
