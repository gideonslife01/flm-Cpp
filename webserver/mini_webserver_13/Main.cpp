// Main.cpp
// 헤더 파일 위치 변경 ❗️
// Change header file location
#include "src/network/Server.h" // http server
#include "src/network/HttpsServer.h" //https server
#include "src/router/Router.h"
#include "src/router/RouterConfig.h"
#include "src/router/RouterHandler.h"
#include "src/proxy/Proxy.h" // reverse proxy
#include "src/parser/RequestParser.h" // request parsing

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <map>
#include <utility> // std::pair 사용을 위해 추가 / Added for using std::pair

// -- 멀티 서버 / Multi Server -- //
int main() {

    // -- 라우트 설정 변경 / Change route settings ❗️-- //
    // 기존 설정 RouterConfig.cpp로 이동함.
    RouterConfig::init_http_routers();
    RouterConfig::init_https_routers();
    RouterConfig::init_reverse_routers();
    RouterHandler router_handler; // ❗️

    // --- 서버 1 설정 (5080 포트) --- //
    // --- Server 1 Configuration (Port 5080) --- //
    http::Server server1("127.0.0.1", 5080);
    server1.set_handler([&](const std::string& req) {

        // -- 1.method 검사 / method check -- //
        //  요청 파싱 / Request parsing ❗️
        HttpRequest parsed_req = RequestParser::parse(req);

        // -- 2.리버스 프록시 적용 / Apply reverse proxy -- //❗️
        // 간단히 /api/ 경로를 리버스 프록시로 판단
        // Simply treat the /api/ path as a reverse proxy

        std::string response = handle_reverse_proxy(req, parsed_req);
        if (!response.empty()) {
            return response;
        }

        // -- 3.메인호스트,가상호스트 라우터 선택 / Select Main Host, Virtual Host, and Router -- //❗️
        // & 요청 헤더 전체 읽기 및 Host 헤더 파싱
        // Read all request headers and parse Host headers
        response = router_handler.process_request(parsed_req,"server1");
        return response;
    });


    // --- 서버 2 설정 (6443 포트) --- //
    // --- Server 2 Configuration (Port 6443) --- //
    https::Server server2("127.0.0.1", 6443);
    //
    // SSL 초기화 및 인증서,개인키 로드
    // SSL Initialization and Loading Certificate and Private Key
    // Lets Encrypt : if (!server3.initialize_ssl("../cert/fullchain.pem", "../cert/privkey.pem")) {
    if (!server2.initialize_ssl("../cert/cert.pem", "../cert/key.pem")) {
        std::cerr << "SSL initialization failed. Exiting.\n";
        return -1;
    }

    server2.set_handler([&](const std::string& req) {

        HttpRequest parsed_req = RequestParser::parse(req);
        std::string response = handle_reverse_proxy(req, parsed_req);
        if (!response.empty()) {
            return response;
        }
        response = router_handler.process_request(parsed_req,"server2");
        return response;
    });


    // --- 서버 3 설정 (5080 포트) --- //
    // --- Server 3 Configuration (Port 8080) --- //
    http::Server server3("127.0.0.1", 8080);
    server3.set_handler([&](const std::string& req) {
        HttpRequest parsed_req = RequestParser::parse(req);
        std::string response = handle_reverse_proxy(req, parsed_req);
        if (!response.empty()) {
            return response;
        }
        response = router_handler.process_request(parsed_req,"server3");
        return response;
    });


    // 각 서버별 스레드 시작 / Start threads per server
    std::thread server_thread(&http::Server::start, &server1);   // http
    std::thread server_thread2(&https::Server::start, &server2); // https
    std::thread server_thread3(&http::Server::start, &server3);  // for reverse proxy

    std::cout << "--- Starting Server Application ---" << std::endl;

    // 0.2초 딜레이로 Server listening on port 메세지 출력후 run_cmd()실행
    // Print "Server listening on port" after a 0.2-second delay, then execute run_cmd()
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 메인 루프 (명령어 입력) / Main loop (command input)
    server1.run_cmd();

    // 서버 정리 / Server Close
    try {
        server1.stop();
        server2.stop();
        server3.stop();
    } catch (const std::exception& e) {
        std::cerr << "Stop error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error during stop" << std::endl;
    }
    // 스레드 정리 / Thread join
    if (server_thread.joinable()) {
        server_thread.join();
    }
    if (server_thread2.joinable()) {
        server_thread2.join();
    }
    if (server_thread3.joinable()) {
        server_thread3.join();
    }
    std::cout << "[MAIN] Server Application Finished." << std::endl;

    return 0;
}
