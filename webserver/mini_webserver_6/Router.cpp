#include "Router.h"
#include <iostream>
#include <string>
#include <utility> // std::pair 사용
//❗️
#include <fstream>
#include <sstream>
#include <filesystem>


// 생성자: 클래스 내부 구현시 Router:: 제거
// Constructor: Remove Router:: when implementing internally in the class
Router::Router() {
    // 초기화 로직
    // initialization logic
}
// -- 이전 로직 -- //
// 경로에 따라 마임타입과 본문을 리턴하는 함수
// std::pair<std::string, std::string> Router::router(const std::string& request) {
//     size_t start = request.find(" ") + 1;
//     size_t end = request.find(" ", start);

//     if (start == std::string::npos || end == std::string::npos) {
//         return {"text/html", "<h1>Error</h1>"};
//     }

//     std::string path = request.substr(start, end - start);
//     std::cout << "[Router] Client requested path: " << path << std::endl;

//     std::string mime_type = "text/html; charset=UTF-8";
//     std::string body;

//     if (path == "/" || path == "/index") {
//         body = "<h1>🏠 메인 페이지</h1><a href='/login'>로그인</a>";
//     }
//     else if (path == "/login") {
//         body = "<h1>🔑 로그인 페이지</h1>";
//     }
//     else if (path == "/style.css") {
//         mime_type = "text/css";
//         body = "body { background-color: powderblue; }";
//     }
//     else if (path == "/data") {
//         mime_type = "application/json";
//         body = "{\"status\": \"running\", \"code\": 200}";
//     }
//     else {
//         body = "<h1 style='color:red;'>404 Not Found</h1>";
//     }


//     return {mime_type, body};
// }


// -- ends_with : c++20이상부터 시용가능 -- //
// -- public 디렉토리에 html 문서 사용 가능 설정(정적파일 서빙)❗️ -- //
// -- ends_with : Available from C++20 -- //
// -- Enable HTML documents in public directory (serving static files) -- //
std::pair<std::string, std::string> Router::router(const std::string& request) {
    size_t start = request.find(" ") + 1;
    size_t end = request.find(" ", start);
    if (start == std::string::npos || end == std::string::npos) {
        return {"text/html", "<h1>Error</h1>"};
    }
    std::string path = request.substr(start, end - start);
    std::cout << "[Router] Client requested path: " << path << std::endl;

    // 기본 문서 루트 설정 / Set default document root
    std::string doc_root = "../public";
    std::string full_path = doc_root + path;

    // 경로가 / 이면 index.html로 매핑
    // If the path is /, map to index.html
    if (path == "/") {
        full_path = doc_root + "/index.html";
    }else if(path == "/test1"){
        full_path = doc_root + "/test1.html";
    }else if(path == "/test2"){
        full_path = doc_root + "/test2.html";
    }else{
        full_path = doc_root + "/index.html";
    }
    if (!std::filesystem::exists(full_path)) {
        return {"text/html", "<h1 style='color:red;'>404 Not Found</h1>"};
    }

    // 파일 읽기 / read file
    std::ifstream file(full_path, std::ios::binary);
    if (!file) {
        return {"text/html", "<h1>500 Internal Server Error</h1>"};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string body = buffer.str();

    // 파일 확장자에 따른 마임타입 지정
    // Specify MIME type based on file extension
    std::string mime_type = "text/html; charset=UTF-8";
    if (full_path.ends_with(".css")) mime_type = "text/css";
    else if (full_path.ends_with(".js")) mime_type = "application/javascript";
    else if (full_path.ends_with(".json")) mime_type = "application/json";
    else if (full_path.ends_with(".png")) mime_type = "image/png";
    else if (full_path.ends_with(".jpg")) mime_type = "image/jpg";

    // 나중에 더 추가하기... / Add more later...
    return {mime_type, body};
}
