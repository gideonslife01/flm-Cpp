#include <string>
#include <atomic> // Atomic Flag 사용을 위해 추가 / Added for using Atomic Flag❗️
#include <functional> // 추가 / Add❗️

class Server {
public:
    // 생성자: 서버 초기화 (IP, Port)
    // Constructor: Initialize server (IP, Port)
    Server(const std::string& ip, int port);

    // 소멸자: 객체가 파괴될 때 서버를 안전하게 종료합니다.❗️
    // Destructor: Safely shuts down the server when the object is destroyed.
    ~Server() {
        stop();
    }

    // 서버 소켓 파일 디스크립터를 저장할 멤버 변수
    // Member variable to store server socket file descriptors
    int server_fd = -1;

    // 서버 실행 함수: 리스닝을 시작하고 메인 루프를 돌림
    // Server execution function: Starts listening and runs the main loop
    bool start();

    // 서버 종료 (필요하다면)
    // Shut down the server (if necessary)
    void stop();

    // -- 브라우저 메세지 바꾸기❗️ --
    // string(요청)을 받아서 string(응답내용)을 뱉는 함수를 담는 그릇
    std::function<std::string(const std::string&)> logic_handler;

    // main에서 람다를 넣어줄 함수(Main.cpp에서사용)
    void set_handler(std::function<std::string(const std::string&)> handler) {
        logic_handler = handler;
    }

    void handle_client_test(int client_socket);
    //

private:
    std::string ip_address;
    int port_number;

    // 서버가 실행 상태인지 확인하는 플래그
    // Flag to check if the server is running
    std::atomic<bool> running{false};// ❗️

    // 실제 리스닝 및 연결 수신 로직 (내부 구현)
    // Actual listening and connection reception logic (internal implementation)
    void run_listener();

    // 연결 처리 로직 (스레드 풀 또는 별도 핸들러에게 위임)
    // Connection handling logic (delegated to thread pool or separate handler)
    void handle_client(int client_socket);

};
