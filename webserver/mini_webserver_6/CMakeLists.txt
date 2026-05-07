# cmake최소버전지정 / Specify minimum CMake version
cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름과 버전 설정 / Set project name and version
project(mini_webserver_6 VERSION 1.0)

# C++ 표준 버전 지정(c++20) / C++ Standard Version Specification (C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 실행 파일 이름, 소스 파일 / Executable file name, source file
add_executable(main
    Main.cpp
    Router.cpp
    Server.cpp
)
