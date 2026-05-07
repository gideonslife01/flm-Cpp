#include "Server.h" // 소켓서버 / Socket Server
#include "Router.h" // 라우터 / Router❗️
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <map>
#include <utility> // std::pair 사용을 위해 추가 / Added for using std::pair ❗️


int main() {
    Server my_server("127.0.0.1", 5080);
    Router my_router; // 라우터 인스턴스 / router instance ❗️

    // 2. 핸들러 설정 (바디만리턴)
    // my_server.set_handler([&](const std::string& req) {
    //     auto [mime, body] = my_router.router(req);

    //     // [참고] 만약 Server 클래스가 헤더까지 처리하게 수정했다면
    //     // 여기서 mime도 같이 넘겨줘야 하지만, 현재는 body만 리턴하는 구조입니다.
    //     return body;
    // });
    //
    // 2. 핸들러 설정 - 마임타입적용(마임타입과 바디가 포함된 헤더리턴) ❗️
    // 2. Handler Settings - Apply MIME Type (Returns header containing MIME type and body)
    my_server.set_handler([&](const std::string& req) {

        // 라우터로부터 마임 타입과 본문을 받음
        // Receive MIME type and body from router
        auto [mime, body] = my_router.router(req);

        // HTTP 응답 헤더 생성
        // Generate HTTP response headers
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + mime + "\r\n";
        response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n"; // 헤더와 본문 구분자 / Header and body separator

        // 본문 추가 / Add body
        response += body;

        // 최종 응답 반환 / Return final response
        return response;
    });

    // 3. 서버 엔진 시작 (스레드)
    // 3. Start server engine (thread)
    std::thread server_thread(&Server::start, &my_server);
    std::cout << "--- Starting Server Application ---" << std::endl;

    // 4. 커맨드 입력 루프 (스레드로 돌리지 말고 메인에서 직접 실행) ❗️
    // 메인 스레드가 여기서 입력을 대기하므로 프로그램이 바로 종료되지 않습니다.
    // 4. Command Input Loop (Do not run as a thread, execute directly in Main)
    // The main thread waits for input here, so the program does not terminate immediately.
    my_server.run_cmd();

    // 5. run_cmd()를 빠져나왔다는 것은 사용자가 exit를 쳤다는 뜻이므로 정리 시작
    // 5. Exiting run_cmd() means the user typed exit, so start cleaning up.
    my_server.stop();

    if (server_thread.joinable()) {
        server_thread.join();
    }

    std::cout << "[MAIN] Server Application Finished." << std::endl;
    return 0;
}
