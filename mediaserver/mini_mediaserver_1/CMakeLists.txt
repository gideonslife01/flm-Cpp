cmake_minimum_required(VERSION 3.16)
project(media_server_1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# asio 경로/Path
find_path(ASIO_INCLUDE_DIR asio.hpp
    PATHS /opt/homebrew/include /usr/local/include
    REQUIRED
)

add_executable(media_server
    main.cpp
    src/rtmp_server.cpp
    src/rtmp_session.cpp
    src/amf0.cpp
    src/flv_writer.cpp
)

target_include_directories(media_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${ASIO_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(media_server PRIVATE
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Mac에서 경고 제거 / Remove warnings from Mac
if(APPLE)
    target_compile_definitions(media_server PRIVATE ASIO_DISABLE_STD_EXPERIMENTAL_STRING_VIEW)
endif()
