#include "Server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>

//#define PORT 5080 ❗️
#define BACKLOG 10 // 대기 큐 크기 / wait queue size

Server::Server(const std::string& ip, int port)
    : ip_address(ip), port_number(port) {}

void Server::run_listener() {
    //std::cout << "[INFO] Server listening on " << ip_address << ":" << port_number << std::endl;

    // --- 1. 소켓 생성 (Socket Creation) --- //
    //int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // 멤버 변수로 변경 / Change to member variable❗️
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    //if (server_fd == 0) { ❗️
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
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스 IP 사용 / Use all interface IPs
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
    while (true) {

        // IPv4구조체 / Structure for IPv4 address
        struct sockaddr_in client_address;

        // 주소 구조체가 메모리에서 차지하는 바이트 크기를 저장하는 변수의 타입
        // Type of the variable storing the byte size occupied by the address structure in memory
        socklen_t addrlen = sizeof(client_address);

        // accept() 함수가 블로킹(Blocking) 상태에서 클라이언트 연결을 기다림
        // The accept() function waits for client connections in a blocking state.
        int client_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);
        if (client_socket < 0) {
            perror("accept failed");
            continue; // 실패했으면 다음 루프를 돌며 재시도 / Retry in the next loop if it fails
        }

        // 동시성 처리: 새로운 연결이 오면 새로운 쓰레드를 생성하여 처리
        // Concurrency Handling: When a new connection arrives, create a new thread to handle it.
        //std::thread client_thread(Server::handle_client, client_socket);
        //
        // 람다 {[](){...}} 는 현재 스레드가 Server 객체(this)의 맥락을 유지하며 실행되도록 보장합니다.
        // Lambda {[](){...}} ensures that the current thread executes while maintaining the context of the Server object (this).
        std::thread client_thread([this, client_socket]() {
            this->handle_client(client_socket);
        });
        // 쓰레드를 분리하고 메인 루프를 계속 실행하게 함
        // Detach the thread so that the main loop continues running.
        client_thread.detach();
    }
}
//❗️
void Server::handle_client(int client_socket) {
    char buffer[1024];
    std::cout << "[Connection] New client connected." << std::endl;

    // 1. 데이터 수신 / receive data (Read)
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // C-string
        std::string request = buffer;
        std::cout << "[Received] " << request.substr(0, 30) << "..." << std::endl;

        // 2. 응답 전송 / send response(Write)
        const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<h1>👋Hi~ Hi~ Hi~~~!</h1><p>Welcome to the miniWebserver!</p>";
        write(client_socket, response, strlen(response));
    }

    // 3. 연결 종료 / close connection
    close(client_socket);
    std::cout << "[Connection] Client disconnected." << std::endl;
}

bool Server::start() {
    // 서버가 실제로 작동하는 메인 함수 / Main function that actually runs the server
    try {
        run_listener();
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Server failed to start: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void Server::stop() {
    // 서버를 종료하는 함수 / Function to stop the server
    //close(server_fd);❗️
    if (server_fd != -1) {
        // 멤버 변수에 저장된 값을 사용하여 종료합니다. / Use the value stored in the member variable to stop the server
        close(server_fd);
        server_fd = -1; // 닫았으므로 초기화 / Initialize to -1 after closing
    }
}
