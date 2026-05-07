#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <map>
#include <functional>
#include <utility> // std::pair 사용을 위해 추가 / Added for using std::pair

class Router {
    public:
        // 생성자 선언 / Constructor Declaration
        Router();

        // 함수 선언 / Function Declaration❗️
        std::pair<std::string, std::string> router(const std::string& request);
};

#endif // ROUTER_H
