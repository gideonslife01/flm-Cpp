#include <iostream>
#include <sys/socket.h> // 소켓 API
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {

    // 소켓 생성,0은 프로토콜 자동 선택
    // // Create socket, 0 for automatic protocol selection
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 모든 IP 허용 / Allow all IPs
    address.sin_port = htons(5080); // 포트 8080 / Port 8080

    bind(server_fd, (struct sockaddr*)&address, sizeof(address)); // 주소 바인딩 / Address Binding
    listen(server_fd, 3); // 접속 대기 / Waiting for connection

    std::cout << "listening..." << std::endl;

    int new_socket = accept(server_fd, nullptr, nullptr); // 연결 수락 / Accept Connection

    char buffer[1024] = {0};
    read(new_socket, buffer, 1024); // 데이터 수신 / Receive Data
    std::cout << "수신된 메시지 / Received Message: " << buffer << std::endl;

    const char* hello = "Hello from server";
    send(new_socket, hello, strlen(hello), 0); // 응답 전송 / Send Response

    close(new_socket);
    close(server_fd);
    return 0;
}
