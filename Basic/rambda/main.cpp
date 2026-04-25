#include <iostream>
#include <string>
#include <functional>
#include <vector>

class Server {
public:
    // 로직을 담을 함수 저장소 (람다를 담는 그릇)
    // Function repository to hold logic (container for Lambda)
    std::function<std::string(const std::string&)> logic_handler;

    // 핸들러 설정 메서드(logic_handler에서 외부 함수 handler 연결)
    // handler함수에는 익명함수로써 문자열 입력받고 router함수 리턴
    // Handler configuration method (connecting the external function handler in logic_handler)
    // The handler function receives a string as an anonymous function and returns the router function
    void set_handler(std::function<std::string(const std::string&)> handler) {
        logic_handler = handler;
    }

    // 클라이언트 요청을 처리하는 가상 메서드
    // Virtual method that handles client requests
    void receive_request(const std::string& message) {
        std::cout << "[Server] 요청 받음 / Requested: " << message << std::endl;

        if (logic_handler) {
            // 외부에서 정의한 람다 함수가 여기서 실행됩니다!
            // login_handler는 router 함수의 값을 리턴
            // The externally defined Lambda function is executed here!
            // login_handler returns the value of the router function
            std::string response = logic_handler(message);
            std::cout << "[Server] 응답 보냄 / Reply sent: " << response << "\n" << std::endl;
        } else {
            std::cout << "[Server] 처리할 핸들러가 없습니다/There is no handler to process..\n" << std::endl;
        }
    }
};

std::string router(const std::string& msg) {
    if (msg == "aloy") return "horizon";
    if (msg == "silverhand") return "cyberpunk";
    return "unknown command";
}

int main() {
    Server my_sv;

    // 람다 함수를 사용하여 로직 연결 / Connecting logic using lambda functions
    my_sv.set_handler([&](const std::string& req) -> std::string {
        return router(req);
    });

    // 요청 / requests
    my_sv.receive_request("aloy");
    my_sv.receive_request("silverhand");
    my_sv.receive_request("npc");

    return 0;
}
