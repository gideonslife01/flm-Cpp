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
    // http router
    Router server1_router("../public1");
    server1_router.add_route("GET","/test1", "test1.html");
    server1_router.add_route("POST","/test2", "test2.html");

    // https router
    Router server2_router("../public2");
    server2_router.add_route("GET","/test1", "test1.html");
    server2_router.add_route("POST","/test2", "test2.html");

    // virtusl host 라우터 / virtual host router❗️
    Router virtual_router("../public_virtual");
    virtual_router.add_route("GET","/test1", "test1.html");
    virtual_router.add_route("POST","/test2", "test2.html");

    // virtual host http(5080) server host map❗️
    static std::map<std::string, Router*> vhost_routers = {
        {"yourmainhost.domain.org", &server1_router},
        {"yourvirtualhost.domain.org", &virtual_router},
        {"localhost", &server1_router},
        {"127.0.0.1", &server1_router},
        {"192.168.0.100", &server1_router}
    };

    // virtual host https(6443) server host map❗️
    static std::map<std::string, Router*> vhost_https_routers = {
        {"yourmainhost.domain.org", &server2_router},
        {"yourvirtualhost.domain.org", &virtual_router},
        {"localhost", &server2_router},
        {"127.0.0.1", &server2_router},
        {"192.168.0.100", &server2_router}
    };

    // --- 서버 1 설정 (5080 포트) --- //
    // --- Server 1 Configuration (Port 5080) --- //
    http::Server server1("127.0.0.1", 5080);
    server1.set_handler([&](const std::string& req) {
        // method 검사 / method check
        std::istringstream req_stream(req);
        std::string method, path, http_version;
        req_stream >> method >> path >> http_version;

        std::pair<std::string, std::string> result;

        // 요청 헤더 전체 읽기 및 Host 헤더 파싱❗️
        // Read all request headers and parse Host headers
        std::string line;
        std::string host;

        // 헤더에서 개행문자 제거용 ❗️
        // For removing newline characters from headers
        std::string dummy;
        std::getline(req_stream, dummy);

        // 요청 헤더 파싱(req stream의 값을 읽어서 line 변수에 한 줄씩 복사)❗️
        // Parse request headers (read values ​​from the req stream and copy them line by line to the line variable)
        while (std::getline(req_stream, line) && line != "\r" && !line.empty()) {
            // "Host:" 찾기 (대소문자 무관하게 검사하는 것이 안전함)
            if (line.compare(0, 5, "Host:") == 0 || line.compare(0, 5, "host:") == 0) {
                host = line.substr(5);
                // 공백 및 \r 제거
                host.erase(0, host.find_first_not_of(" \t"));
                size_t last = host.find_last_not_of(" \t\r\n");
                if (last != std::string::npos) host.erase(last + 1);

                // std::cout << "[DEBUG] Host Found: " << host << std::endl << std::flush;
            }
        }

        // 포트 번호 제거하여 도메인명만 추출❗️
        // Remove port number to extract only the domain name
        std::string host_normalized = host;
        size_t pos = host.find(':');
        if (pos != std::string::npos) {
            host_normalized = host.substr(0, pos);
        }

        // 라우터 선택: 해당 호스트의 라우터 찾기, 없으면 기본 라우터❗️
        // Select Router: Find the router for the host, if none exists, use the default router
        Router* router_ptr = &server1_router; // 기본값 / default value
        auto it = vhost_routers.find(host_normalized);
        if (it != vhost_routers.end()) {
            router_ptr = it->second;
        }

        if (method == "GET" || method == "POST" || method == "PUT" || method == "DELETE" || method == "FETCH") {
            // 이전코드 / previous code
            //result = server1_router.router(method, path);
            //
            // virtual host 적용 / apply virtual host❗️
            result = router_ptr->router(method, path);

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


    // --- 서버 2 설정 (6443 포트) --- //
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


        // 요청 헤더 전체 읽기 및 Host 헤더 파싱❗️
        // Read all request headers and parse Host headers
        std::string line;
        std::string host;

        // 헤더에서 개행문자 제거용 ❗️
        // For removing newline characters from headers
        std::string dummy;
        std::getline(req_stream, dummy);

        // 요청 헤더 파싱(req stream의 값을 읽어서 line 변수에 한 줄씩 복사)❗️
        // Parse request headers (read values from the req stream and copy them line by line to the line variable)
        while (std::getline(req_stream, line) && line != "\r" && !line.empty()) {
            // "Host:" 찾기 (대소문자 무관하게 검사하는 것이 안전함)
            if (line.compare(0, 5, "Host:") == 0 || line.compare(0, 5, "host:") == 0) {
                host = line.substr(5);
                // 공백 및 \r 제거
                host.erase(0, host.find_first_not_of(" \t"));
                size_t last = host.find_last_not_of(" \t\r\n");
                if (last != std::string::npos) host.erase(last + 1);

                // std::cout << "[DEBUG] https Host Found: " << host << std::endl << std::flush;
            }
        }

        // 포트 번호 제거하여 도메인명만 추출❗️
        // Remove port number to extract only the domain name
        std::string host_normalized = host;
        size_t pos = host.find(':');
        if (pos != std::string::npos) {
            host_normalized = host.substr(0, pos);
        }

        // 라우터 선택: 해당 호스트의 라우터 찾기, 없으면 기본 라우터❗️
        // Select Router: Find the router for the host, if none exists, use the default router
        Router* router_ptr = &server2_router; // 기본값 / default value
        auto it = vhost_https_routers.find(host_normalized);
        if (it != vhost_https_routers.end()) {
            router_ptr = it->second;
        }


        if (method == "GET" || method == "POST" || method == "PUT" || method == "DELETE" || method == "FETCH") {

            // 이전코드 / previous code
            //result = server2_router.router(method, path);
            //
            // virtual host 적용 / apply virtual host❗️
            result = router_ptr->router(method, path);
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


    // 각 서버별 스레드 시작 / Start threads per server
    std::thread server_thread(&http::Server::start, &server1); // http
    std::thread server_thread2(&https::Server::start, &server2); // https

    std::cout << "--- Starting Server Application ---" << std::endl;

    // 0.2초 딜레이로 Server listening on port 메세지 출력후 run_cmd()실행
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 메인 루프 (명령어 입력) / Main loop (command input)
    server1.run_cmd();

    // 서버 정리 / Server Close ❗️
    // server1.stop();
    // server2.stop();
    try {
        server1.stop();
        server2.stop();
    } catch (const std::exception& e) {
        std::cerr << "Stop error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error during stop" << std::endl;
    }

    if (server_thread.joinable()) {
        server_thread.join();
    }
    if (server_thread2.joinable()) {
        server_thread2.join();
    }

    std::cout << "[MAIN] Server Application Finished." << std::endl;

    return 0;
}
