#include "Server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <functional> // 추가

#define BACKLOG 10 // 대기 큐 크기 / wait queue size

http::Server::Server(const std::string& ip, int port)
    : ip_address(ip), port_number(port) {}

void http::Server::run_listener() {
    //std::cout << "[INFO] Server listening on " << ip_address << ":" << port_number << std::endl;

    // --- 1. 소켓 생성 (Socket Creation) --- //
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // --- 2. 바인딩 (Binding) --- //
    struct sockaddr_in address;
    address.sin_family = AF_INET;                // IPv4
    address.sin_addr.s_addr = INADDR_ANY;        // 모든 인터페이스 IP 사용 / Use all interface IPs
    address.sin_port = htons(port_number);       // 포트를 네트워크 바이트 순서로 변환 / Convert ports to network byte order

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // --- 3. 리스닝 시작 (setsockopt: listen_socket(socket)) --- //
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port_number << "..." << std::endl;

    // --- 4. 메인 루프 (Accept loop): 클라이언트 연결을 무한히 기다림 / Infinitely waiting for client connections --- //

    // 무한루프에서 종료신호를 받으면 루프를 종료 / Exit the loop when the termination signal is received
    while (running.load()) {

        // 루프탈출 / Loop escape
        if (!running.load()) {
            break;  // 종료 신호 받으면 루프 종료 / Terminate the loop when the termination signal is received.
        }

        // 종료신호 여부를 확인하기 위해 0.1초 대기 / Wait for 0.1 seconds to check the termination signal
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 부하 방지를 위해 대기

        // IPv4구조체 / Structure for IPv4 address
        struct sockaddr_in client_address;
        socklen_t addrlen = sizeof(client_address);

        // accept() 함수가 블로킹(Blocking) 상태에서 클라이언트 연결을 기다림
        // The accept() function waits for client connections in a blocking state.
        int client_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);
        if (running.load() && client_socket < 0) {

            // 루프탈출 / Loop escape
            if (!running.load()) {
                break;  // 소켓 닫혀서 실패한 경우 종료 / Terminate if failed due to socket closing
            }

            perror("accept failed");
            continue; // 실패했으면 다음 루프를 돌며 재시도 / Retry in the next loop if it fails
        }
        // 클라이언트 접속 / Client connected
        std::thread client_thread([this, client_socket]() {

            //외부에서 메세지 바꾸기 / Change message externally
            this->handle_client(client_socket);
        });
        // 스레드 - 백터에 저장 / Thread - Stored in a vector
        client_threads.push_back(std::move(client_thread));

        //client_thread.detach(); // 이전코드 / Previous code
    }
}

// 커맨드 입력 루프
void http::Server::run_cmd(){
    std::string command;
    while (true) {
        std::cout << "CMD> "; // 명령어 프롬프트 표시 / Show command prompt

        // std::getline을 사용하여 공백을 포함한 전체 문자열을 한 번에 받습니다.
        // Use std::getline to get the entire string including spaces at once.
        if (!std::getline(std::cin, command)) {
            // EOF (Ctrl+C 등)로 입력을 받을 수 없는 상황이 발생하면 종료
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
}

// 람다식으로 외부에서 내용 바꾸기
// Change content from the outside using a lambda expression
void http::Server::handle_client(int client_socket) {
    char buffer[1024];
    // -- debuging --
    //std::cout << "[Connection] New client connected." << std::endl;

    // 1. 데이터 수신 / receive data
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::string request = buffer;
        // -- debuging --
        //std::cout << "[Received] " << request.substr(0, 30) << "..." << std::endl;

        std::string response;
        if (logic_handler) {

            // main에서 보낸 람다식 실행
            // request는 버퍼의 값...
            // Execute the lambda expression sent from main
            // request is the buffer value...
            response = logic_handler(request);

        } else {
            response = "<h1>No Logic Handler set!</h1>";
        }

        //std::string response = html_content; // main.cpp에서 넘어온 헤더 적용

        write(client_socket, response.c_str(), response.size());
    }

    // 2. 연결 종료 (if문 밖으로 이동하여 무조건 닫히게 설정)
    // 2. Close connection (move outside the if statement and set to close unconditionally)
    close(client_socket);
    // -- debuging --
    //std::cout << "[Connection] Client disconnected." << std::endl;
}


bool http::Server::start() {
    // 서버가 실제로 작동하는 메인 함수 / Main function that actually runs the server
    try {
        if (!running.load()) {
            running.store(true);
            run_listener();
            return false;
        }

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Server failed to start: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void http::Server::stop() {
    try {
        if (running.load() && server_fd != -1) { //
            // 플래그를 false로 설정하여 루프 종료 신호 전송
            // Set flag to false to send loop termination signal
            running.store(false);
            close(server_fd);
            // 실제로는 여기서 소켓을 닫거나 리소스 해제를 수행합니다.
            // In practice, close the socket or release resources here.
            server_fd = -1; // 닫았으므로 초기화 / Initialize to -1 after closing

            // 모든 클라이언트 스레드 종료 대기
            // Wait for all client threads to terminate
            for (auto& t : client_threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
            client_threads.clear();

            std::cout << "[Server] Shutdown initiated." << std::endl;
        }
    } catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception during shutdown: " << e.what() << std::endl;
    } catch (...) {
            std::cerr << "[ERROR] Unknown exception during shutdown." << std::endl;
    }
}
