# cmake최소버전지정 / Specify minimum CMake version
cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름과 버전 설정 / Project name and version settings
project(mini_webserver_12 VERSION 1.0)

# C++ 표준 버전 지정(c++20) / C++ Standard Version Specification (C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Openssl
include_directories(/opt/homebrew/opt/openssl@3/include)
link_directories(/opt/homebrew/opt/openssl@3/lib)

# 실행 파일 생성 대상 소스 파일 나열 / List source files to be used for creating executable files
add_executable(main
    Main.cpp
    Router.cpp
    Server.cpp
    HttpsServer.cpp
    Proxy.cpp
)

# Openssl + main 링크/link
target_link_libraries(main ssl crypto)
