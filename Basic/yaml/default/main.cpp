#include <iostream>
#include <yaml-cpp/yaml.h>

int main() {
    try {
        YAML::Node config = YAML::LoadFile("../config.yaml");

        std::string host = config["server"]["host"].as<std::string>();
        int port = config["server"]["port"].as<int>();

        std::string db_user = config["database"]["user"].as<std::string>();
        std::string db_password = config["database"]["password"].as<std::string>();
        std::string db_name = config["database"]["dbname"].as<std::string>();

        std::cout << "Server Host: " << host << std::endl;
        std::cout << "Server Port: " << port << std::endl;
        std::cout << "Database User: " << db_user << std::endl;
        std::cout << "Database Name: " << db_name << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error loading config.yaml: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


// 컴파일
// g++ -std=c++17 -I/opt/homebrew/Cellar/yaml-cpp/0.9.0/include -L/opt/homebrew/Cellar/yaml-cpp/0.9.0/lib -lyaml-cpp main.cpp -o main
