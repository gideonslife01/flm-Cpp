#include "Server.h"
#include "Router.h"
#include "HttpsServer.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <map>
#include <utility> // std::pair 사용을 위해 추가 / Added for using std::pair

// -- 멀티 서버 / Multi Server -- //
int main() {

    // -- 라우트 설정 변경 / Change route settings -- //
    Router server1_router("../public1");
    server1_router.add_route("GET","/test1", "test1.html");
    server1_router.add_route("POST","/test2", "test2.html");
    // https 라우터
    Router server2_router("../public2");
    server2_router.add_route("GET","/test1", "test1.html");
    server2_router.add_route("POST","/test2", "test2.html");


    // --- 서버 1 설정 (5080 포트) --- //
    // --- Server 1 Configuration (Port 5080) --- //
    http::Server server1("127.0.0.1", 5080);
    server1.set_handler([&](const std::string& req) {
        // method 검사 / method check
        std::istringstream req_stream(req);
        std::string method, path, http_version;
        req_stream >> method >> path >> http_version;

        std::pair<std::string, std::string> result;

        if (method == "GET" || method == "POST" || method == "PUT" || method == "DELETE" || method == "FETCH") {
            result = server1_router.router(method, path);
        }
        else {
            return std::string(
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Allow: GET, POST, PUT, DELETE, FETCH\r\n"
                "Connection: close\r\n\r\n"
                "Method Not Allowed"
            );
        }

        // 라우터로부터 마임 타입과 본문을 받음
        // Receive MIME type and body from router
        const auto& [mime, body] = result;

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


    // --- 서버 2 설정 (6443 포트) --- // ❗️
    // --- Server 2 Configuration (Port 6443) --- //
    https::Server server2("127.0.0.1", 6443);
    //
    // SSL 초기화 및 인증서,개인키 로드
    // SSL Initialization and Loading Certificate and Private Key
    if (!server2.initialize_ssl("../cert/cert.pem", "../cert/key.pem")) {
        std::cerr << "SSL initialization failed. Exiting.\n";
        return -1;
    }

    /*
    * Let's Encrypt 적용시(중간인증서(chain),개인키(privkey.pem))
    * When applying Let's Encrypt (intermediate certificate (chain), private key (privkey.pem))
    * if (!server3.initialize_ssl("../cert/fullchain.pem", "../cert/privkey.pem")) {
        std::cerr << "SSL initialization failed. Exiting.\n";
        return -1;
    }
    */

    server2.set_handler([&](const std::string& req) {

        std::istringstream req_stream(req);
        std::string method, path, http_version;
        req_stream >> method >> path >> http_version;

        std::pair<std::string, std::string> result;

        if (method == "GET" || method == "POST" || method == "PUT" || method == "DELETE" || method == "FETCH") {
            result = server2_router.router(method, path);
        }
        else {
            return std::string(
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Allow: GET, POST, PUT, DELETE\r\n"
                "Connection: close\r\n\r\n"
                "Method Not Allowed"
            );
        }

        // 라우터로부터 마임 타입과 본문을 받음
        // Receive MIME type and body from router
        const auto& [mime, body] = result;

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


    // 각 서버별 스레드 시작 / Start threads per server
    std::thread server_thread(&http::Server::start, &server1); // http
    std::thread server_thread2(&https::Server::start, &server2); // https

    std::cout << "--- Starting Server Application ---" << std::endl;

    // 0.2초 딜레이로 Server listening on port 메세지 출력후 run_cmd()실행
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 메인 루프 (명령어 입력) / Main loop (command input)
    server1.run_cmd();

    // 서버 정리 / Server Close
    server1.stop();
    server2.stop();

    if (server_thread.joinable()) {
        server_thread.join();
    }
    if (server_thread2.joinable()) {
        server_thread2.join();
    }
    std::cout << "[MAIN] Server Application Finished." << std::endl;
    return 0;
}
