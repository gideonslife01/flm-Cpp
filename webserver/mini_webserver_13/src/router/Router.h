#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <map>
#include <functional>
#include <utility> // std::pair 사용을 위해 추가 / Added for using std::pair

class Router {
    public:

    // html문서경로 초기화 / Reset HTML document path
    Router(std::string root = "../public") : doc_root(root) {}

     // 함수 선언 / function declaration
    //std::pair<std::string, std::string> router(const std::string& request);
    //
    // 추가
    std::pair<std::string, std::string> router(const std::string& method, const std::string& path);


    // 서버별 라우트 지정 / Specify routes per server
    // 서버별로 경로를 추가할 수 있는 함수 / A function that can add paths per server
    // void add_route(const std::string& path, const std::string& file_name) {
    //     route_map[path] = file_name;
    // }
    //
    // 메소드 추가 / method added
    void add_route(const std::string& method, const std::string& path, const std::string& file_name) {
        route_map[method][path] = file_name;
    }

    private:
        std::string doc_root; // html문서 경로 저장 / Save HTML document path

        // 경로(URL)와 파일명을 저장하는 맵
        // A map that stores paths (URLs) and filenames
        //std::map<std::string, std::string> route_map;
        //
        // method,path,filename -
        std::map<std::string, std::map<std::string, std::string>> route_map;

};

#endif // ROUTER_H
