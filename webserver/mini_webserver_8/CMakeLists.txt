# cmake최소버전지정
cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름과 버전 설정
project(mini_webserver_8 VERSION 1.0)

# C++ 표준 버전 지정(c++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 실행 파일 생성 대상 소스 파일 나열
add_executable(main
    Main.cpp
    Router.cpp
    Server.cpp
)
