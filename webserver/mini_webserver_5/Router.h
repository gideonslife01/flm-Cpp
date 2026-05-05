#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <map>
#include <functional>
#include <utility> // std::pair 사용을 위해 추가 / add std::pair for use

class Router {
    public:
        Router(); // 생성자 선언 / constructor declaration
        std::pair<std::string, std::string> router(const std::string& request); // 함수 선언 / function declaration❗️
    };

#endif // ROUTER_H
