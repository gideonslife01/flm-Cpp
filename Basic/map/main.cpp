#include <iostream>
#include <map>
#include <string>

int main() {

    // std::map 선언: 도시 이름 -> 도시 설명
    // 키는 std::string, 값도 std::string
    // std::map declaration: City Name -> City Description
    // Keys are std::strings, values ​​are also std::strings
    std::map<std::string, std::string> route_map;

    // 데이터 삽입 / Insert data
    route_map["Seoul"] = "Capital of South Korea";
    route_map["Busan"] = "Famous port city";
    route_map["Jeju"] = "Popular island tourist destination";

    // 맵 내용을 출력 / Print map contents
    for (const auto& pair : route_map) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    // 특정 키 검색 예제 / Specific key search example
    std::string city = "Seoul";
    auto iter = route_map.find(city);
    if (iter != route_map.end()) {
        std::cout << city << " 정보/info.: " << iter->second << std::endl;
    } else {
        std::cout << city << " 정보를 찾을 수 없습니다. / Can not find information" << std::endl;
    }

    return 0;
}
