#include <iostream>
#include <thread> // 스레드 헤더 / thread header
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

// 클라이언트와 통신하기 위한 함수
// A function to communicate with the client
void handle_client(int client_socket) {
    char buffer[1024];
    while (true) {

        // 버퍼를 모두 0으로 초기화해서 기존 쓰레기 값 지우기
        // Initialize buffer to all 0s to clear existing garbage values
        memset(buffer, 0, 1024);
        ssize_t valread = read(client_socket, buffer, 1024);

        if (valread <= 0) {
            std::cout << "클라이언트 접속 종료 / Client connection terminated " << std::endl;
            break;
        }

        std::cout << "수신된 메시지 / Received message: " << buffer << std::endl;
        send(client_socket, buffer, valread, 0); // send
    }
    close(client_socket);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 포트가 이미 사용 중이어도 즉시 재사용 가능하게 설정
    // Configure port to be immediately reusable even if it is already in use
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(5080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed"); // 에러 내용 출력 / Error details output
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "서버 정상 시작, 접속 대기 중 / Server started normally, waiting for connection ..." << std::endl;

    while (true) {
        int client_sock = accept(server_fd, nullptr, nullptr);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }
        std::cout << "새 클라이언트 연결됨 / New Client Connected: " << client_sock << ")" << std::endl;
        std::thread(handle_client, client_sock).detach();
    }
    return 0;
}
