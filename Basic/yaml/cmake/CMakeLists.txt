cmake_minimum_required(VERSION 3.15)  # 최소 cmake 버전 명시 / Specify minimum cmake version

project(MyProject)

find_library(YAML_CPP_LIB yaml-cpp PATHS /opt/homebrew/Cellar/yaml-cpp/0.9.0/lib)
include_directories(/opt/homebrew/Cellar/yaml-cpp/0.9.0/include)

add_executable(main main.cpp)
target_link_libraries(main ${YAML_CPP_LIB})
