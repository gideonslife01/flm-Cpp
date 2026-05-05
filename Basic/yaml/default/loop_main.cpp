#include <iostream>
#include <yaml-cpp/yaml.h>

int main() {
    YAML::Node config = YAML::LoadFile("loop_config.yaml");

    // 서버 리스트 읽기
    const YAML::Node& servers = config["server"];
    for (std::size_t i = 0; i < servers.size(); i++) {
        std::string host = servers[i]["host"].as<std::string>();
        int port = servers[i]["port"].as<int>();
        std::cout << "Server " << i + 1 << " Host: " << host << ", Port: " << port << std::endl;
    }

    // 데이터베이스 설정 읽기
    std::string db_user = config["database"]["user"].as<std::string>();
    std::string db_password = config["database"]["password"].as<std::string>();
    std::string db_name = config["database"]["dbname"].as<std::string>();

    std::cout << "Database User: " << db_user << ", DB Name: " << db_name << std::endl;

    return 0;
}


// 컴파일
// g++ -std=c++17 -I/opt/homebrew/Cellar/yaml-cpp/0.9.0/include -L/opt/homebrew/Cellar/yaml-cpp/0.9.0/lib -lyaml-cpp loop_main.cpp -o loop_main
