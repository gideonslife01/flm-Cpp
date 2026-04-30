#include "Server.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>

int main() {

    // 1. 서버 인스턴스 생성 / Create server instance
    Server my_server("127.0.0.1", 5080);
    std::thread server_thread(&Server::start, &my_server); // 서버 시작 / Start server
    std::cout << "--- Starting Server Application ---" << std::endl;

    // 2. 메인 스레드는 사용자 입력 대기 / The main thread waits for user input❗️
    // 1)엔터입력시 서버종료하기 / Shut down the server when Enter is pressed
    // std::cin.get();
    //
    // 2)/exit입력시 서버 종료하기 / Shut down the server by entering /exit
    std::string command;
    while (true) {
        std::cout << "CMD> "; // 명령어 프롬프트 표시 / Show command prompt

        // std::getline을 사용하여 공백을 포함한 전체 문자열을 한 번에 받습니다.
        // Use std::getline to get the entire string including spaces at once.
        if (!std::getline(std::cin, command)) {
            // EOF (Ctrl+D 등)로 입력을 받을 수 없는 상황이 발생하면 종료
            break;
        }

        // 입력된 명령어를 확인합니다. / Check the input command
        if (command.empty()) {
            // 아무것도 입력하지 않았으면 그냥 넘어갑니다./ If you don't enter anything, it will just skip.
            continue;
        } else if (command == "/exit" || command == "exit") {

            std::cout << "[INFO] 종료 명령을 받았습니다. 서비스 종료를 시작합니다.\n";
            std::cout << "[INFO] Received a shutdown command. Starting service shutdown.\n";
            break; // 루프 종료 / Loop End

        } else if (command == "/status") {
            std::cout << "[STATUS] 서비스는 정상적으로 작동 중입니다.\n";
            std::cout << "[STATUS] The service is running normally.\n";
        } else {
            std::cout << "[ERROR] 알 수 없는 명령어입니다. (/exit, /status)\n";
            std::cout << "[ERROR] Unknown command. (/exit, /status)\n";
        }
    }
    // 3. 사용자 요청에 의해 서버를 안전하게 종료 ❗️
    // 3. Safely shut down the server at the user's request ❗️
    my_server.stop();

    // 2. 종료 로직 (서버가 종료되지 않기 떄문에 실행되지 않음,나중을 위한 종료 로직)
    // Termination logic (not executed because the server has not terminated, termination logic for later)
    server_thread.join();
    my_server.stop();

    return 0;
}
