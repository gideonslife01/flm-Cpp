#include "Server.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>

int main() {

    // 1. 서버 인스턴스 생성 / Create server instance
    Server my_server("127.0.0.1", 5080);
    std::thread server_thread(&Server::start, &my_server); // 서버 시작 / Start server ❗️
    std::cout << "--- Starting Server Application ---" << std::endl;

    // 2. 종료 로직 (서버가 종료되지 않기 떄문에 실행되지 않음,나중을 위한 종료 로직)
    // Termination logic (not executed because the server has not terminated, termination logic for later)
    server_thread.join();
    my_server.stop();

    return 0;
}
