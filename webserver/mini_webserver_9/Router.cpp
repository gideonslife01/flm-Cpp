#include "Router.h"
#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <filesystem>

//
// Constructor - 주석처리 가능 / Commentable
// Router::Router(std::string root) : doc_root(root) {
//
    // 생성자 몸체는 비워두어도 초기화 리스트(: doc_root(root))가 작동합니다.
    // The initialization list (: doc_root(root)) works even if the constructor body is left empty.

// }

// -- ends_with : c++20이상부터 시용가능 -- //
// -- public 디렉토리에 html 문서 사용 가능 설정(정적파일 서빙) -- //

// -- ends_with : Available from C++20 -- //
// -- Enable HTML documents in public directory (serving static files) -- //
std::pair<std::string, std::string> Router::router(const std::string& method, const std::string& path) {

    std::string target_file;
    if(route_map.count(method) && route_map[method].count(path)) {
        target_file = route_map[method][path];
    }

    // 기본 경로("/") 처리
    // Handling default path ("/")
    else if (path == "/") {
        target_file = "index.html";
    }
    // 등록되지 않은 경로는 그냥 경로 이름대로 파일 시도 (또는 404)
    // For unregistered paths, just attempt the file as the path name (or 404)
    else {
        target_file = path;
    }

    std::string full_path = doc_root + "/" + target_file;

    // 404
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

    // 필요에 따라 추가 / Add as needed
    return {mime_type, body};
}
