#include "Proxy.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <chrono>     // timeout
#include <fcntl.h>
#include <string>

// 간단한 timeout 설정 함수
// bool set_nonblocking(int sock) {
//     int flags = fcntl(sock, F_GETFL, 0);
//     return fcntl(sock, F_SETFL, flags | O_NONBLOCK) != -1;
// }

std::string forward_request_to_backend(const std::string& backend_ip,
                                       int backend_port,
                                       const std::string& client_request) {

    // IPV4로 소켓 연결 / Socket connection via IPv4
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }

    /*
    10초 connect timeout
    - 10은 초(seconds)를 의미하고, 0은 마이크로초(microseconds)를 의미합니다..
    - setsockopt() 함수는 소켓의 다양한 옵션을 설정합니다.
    - SO_RCVTIMEO: 소켓에서 데이터를 수신할 때 최대 대기 시간을 설정하는 옵션입니다.
                   즉, 10초 이내에 데이터가 오지 않으면 수신 함수(recv 등)가 타임아웃됩니다.
    - SO_SNDTIMEO: 데이터를 전송할 때 최대 대기 시간을 지정합니다.
                   10초 이내에 전송이 되지 않으면 타임아웃 처리합니다.

    10 second connect timeout
    - 10 represents seconds, and 0 represents microseconds.
    - The setsockopt() function sets various options for a socket.
    - SO_RCVTIMEO: This option sets the maximum waiting time when receiving data from a socket.
                   In other words, if data does not arrive within 10 seconds, the receive function (such as recv) times out.
    - SO_SNDTIMEO: Specifies the maximum waiting time when transmitting data.
                   If transmission does not occur within 10 seconds, a timeout is triggered.
   */
    struct timeval tv = {10, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET; //IPV4
    serv_addr.sin_port = htons(backend_port); // Host TO Network Short(big endian)

    // inet_pton : 텍스트형식을 이진 IP주소로변환 / Convert text format to binary IP address
    // backend_ip.c_str() : IP 문자열 / IP string
    // &serv_addr.sin_addr) : 저장될 위치 / Location to be saved
    //
    if (inet_pton(AF_INET, backend_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        close(sock);
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }

    // 소켓으로 지정된 주소에 접속시도 실패시 오류 반환
    // Returns an error if the attempt to connect to the address specified by the socket fails.
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return "HTTP/1.1 504 Gateway Timeout\r\n\r\n";
    }

    // 전체 데이터 전송 보장 / Full data transfer guaranteed
    size_t total_sent = 0; // 전송된 바이트 누적 / Accumulation of transferred bytes

    // 요청 크기만큼 전송 반복 / Repeat transmission for the requested size
    while (total_sent < client_request.size()) {
        // client_request.data() + total_sent
        // total_sent로 보낸 데이터의 다음 메모리 위치
        // Next memory location of the data sent with total_sent
        //
        // 데이터 전송이 완료된 위치에 커서가 있고 여기서 다시 데이터 크기만큼 전송
        // The cursor is at the location where data transmission is complete,
        // and from here, data of the size is transmitted again.
        ssize_t sent = send(sock, client_request.data() + total_sent,
                           client_request.size() - total_sent, 0);
        if (sent < 0) {
            close(sock);
            return "HTTP/1.1 502 Bad Gateway\r\n\r\n";
        }
        total_sent += sent;
    }

    // 응답 수신 / Receive response
    // html파일 내용 읽어오기 / Read HTML file content
    char buffer[8192];
    std::string response;
    response.reserve(65536);  // 대략적인 초기 용량

    // 반복적으로 한번에 8192만큼 데이터 읽어와서 response변수에 저장하기
    // Repeatedly read 8192 bytes of data at a time and store them in the response variable
    ssize_t received;
    while ((received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        response.append(buffer, received);
    }

    close(sock);

    if (received < 0 && response.empty()) {
        return "HTTP/1.1 502 Bad Gateway\r\n\r\n";
    }

    // 응답이 아예 없으면 에러
    // Error handling if there is no response at all
    if (response.empty()) {
        return "HTTP/1.1 502 Bad Gateway\r\n\r\n";
    }

    return response;
}

// 리버스 프록시 / Reverse proxy❗️
std::string handle_reverse_proxy(const std::string& req, const HttpRequest& parsed_req) {
    std::string method = parsed_req.method;
    std::string path = parsed_req.path;

    if ((method == "GET" || method == "POST") &&
        (path == "/api" || path.find("/api/") == 0)) {

        std::string backend_ip = "127.0.0.1";
        int backend_port = 8080;
        std::string modified_req = req;

        // /api → / 로 경로 변경 /
        // Change path from /api to /
        if (path == "/api") {
            size_t pos = modified_req.find(" /api ");
            if (pos != std::string::npos) {
                modified_req.replace(pos, 5, " /");
            }
        } else if (path.find("/api/") == 0) {
            size_t pos = modified_req.find("/api/");
            if (pos != std::string::npos) {
                modified_req.replace(pos, 5, "/");
            }
        }

        // Host 헤더 수정 / Edit Host header
        size_t host_pos = modified_req.find("\r\nHost:");
        if (host_pos == std::string::npos) {
            host_pos = modified_req.find("\r\nhost:");
        }
        if (host_pos != std::string::npos) {
            size_t line_end = modified_req.find("\r\n", host_pos + 1);
            if (line_end != std::string::npos) {
                modified_req.replace(host_pos, line_end - host_pos, "\r\nHost: 127.0.0.1:8080");
            }
        }

        // Connection 헤더 close로 설정
        // Set Connection header to close
        size_t conn_pos = modified_req.find("Connection:");
        if (conn_pos != std::string::npos) {
            size_t line_end = modified_req.find("\r\n", conn_pos);
            if (line_end != std::string::npos) {
                modified_req.replace(conn_pos, line_end - conn_pos, "Connection: close");
            }
        }

        return forward_request_to_backend(backend_ip, backend_port, modified_req);
    }

    // 조건 미충족 시 빈 문자열 반환하거나 원 요청 그대로 반환할 수 있음
    // If conditions are not met, it can return an empty string or the original request as is
    return std::string{};
}
