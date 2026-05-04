#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성 / create socket

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr); // 서버 IP 설정 / Server IP settings

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // 서버 접속 / server connection

    const char* message = "Hello from client";
    send(sock, message, strlen(message), 0); // 메세지 전송 / Send Message

    char buffer[1024] = {0};
    read(sock, buffer, 1024); // 서버 응답 수신 / Receive Server Response
    std::cout << "서버 응답 / Server Response: " << buffer << std::endl;

    close(sock); // 소켓 종료 / Close Socket
    return 0;
}
