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
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr); // 서버 IP 설정 / Set Server IP

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); // 서버 접속 / Connect to Server

    // const char* message = "Hello from client";
    // send(sock, message, strlen(message), 0); // 메시지 전송

    // char buffer[1024] = {0};
    // read(sock, buffer, 1024); // 서버 응답 수신
    // std::cout << "서버 응답: " << buffer << std::endl;


    while (true) {
        std::string msg;
        std::cout << "보낼 메시지 / Message to send (종료/Quit: quit): ";
        std::getline(std::cin, msg); // 키보드 입력 받기 / Receiving keyboard input

        // quit가 입력되면... / When quit is entered...
        if (msg == "quit") break;

        send(sock, msg.c_str(), msg.length(), 0); // 메세지를 서버로 전송 / Send message to server

        char buffer[1024] = {0};
        int valread = read(sock, buffer, 1024); // 서버 응답 대기 / Waiting for server response

        if (valread <= 0) {
            std::cout << "서버와의 연결이 끊겼습니다. / Server connection closed." << std::endl;
            break;
        }
        std::cout << "서버 응답 / Server response:: " << buffer << std::endl;
    }



    close(sock); // 소켓 종료 / Socket close
    return 0;
}
