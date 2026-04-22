#include <string>

class Server {
public:
    // 생성자: 서버 초기화 (IP, Port)
    // Constructor: Initialize server (IP, Port)
    Server(const std::string& ip, int port);

    // 서버 소켓 파일 디스크립터를 저장할 멤버 변수
    // Member variable to store server socket file descriptors
    int server_fd = -1; // ❗️

    // 서버 실행 함수: 리스닝을 시작하고 메인 루프를 돌림
    // Server execution function: Starts listening and runs the main loop
    bool start();

    // 서버 종료 (필요하다면)
    // Shut down the server (if necessary)
    void stop();

private:
    std::string ip_address;
    int port_number;

    // 실제 리스닝 및 연결 수신 로직 (내부 구현)
    // Actual listening and connection reception logic (internal implementation)
    void run_listener();

    // 연결 처리 로직 (스레드 풀 또는 별도 핸들러에게 위임)
    // Connection handling logic (delegated to thread pool or separate handler)
    void handle_client(int client_socket);
};
