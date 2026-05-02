#include "Server.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <map>

// -- mini_webserver_4 -- //
//
// URL 경로에 따라 HTML을 리턴하는 간단한 라우터 함수❗️
// 마임타입이 없으나 정상 작동함. 추후 마임타입추가 예정
// A simple router function that returns HTML based on the URL path❗️
// Works correctly even without MIME types. MIME types will be added later.
//
std::string router(const std::string& request) {
    // 요청 문자열에서 경로(Path) 추출 / Extract path from request string
    // "GET /login HTTP/1.1" -> "/login"만 잘라냄 / Extract "/login" from "GET /login HTTP/1.1"
    size_t start = request.find(" ") + 1;
    size_t end = request.find(" ", start);
    if (start == std::string::npos || end == std::string::npos) return "<h1>Error</h1>";

    std::string path = request.substr(start, end - start);
    std::cout << "[Router] Client requested path: " << path << std::endl;

    // 라우팅
    if (path == "/" || path == "/index") {
        return "<h1>🏠 메인 페이지/Main Page</h1><p>환영합니다! 여기는 홈입니다./ Welcome! This is Home.</p>"
               "<h1>👋Hello~~~! 🤗 </h1>"
               "<a href='/login'>로그인하러 가기 / Go to log in</a>";
    }
    else if (path == "/love") {
        return "<h1>🔑 로그인 페이지/Login Page</h1><p>아이디와 비번을 입력하세요./Please enter your ID and password.</p>"
               "<button onclick='location.href=\"/\"'>홈으로/Home</button>";
    }
    else if (path == "/8") {
        return "<h1>📊 데이터 현황/Data Status</h1><p>현재 서버는 정상 작동 중입니다./The server is currently running.</p>";
    }
    else {
        // 404 Not Found
        return "<h1 style='color:red;'>404 Not Found</h1>";
    }
}

int main() {

    // 서버 인스턴스 생성 / Create server instance
    Server my_server("127.0.0.1", 5080);

    // main에서 브라우저에 출력될 내용을 결정,람다로 브라우저에 띄울 내용을 전달❗️
    // 서버 시작전에 메세지 라우터 셋팅,set_handler에 router(req)를 리턴하는 익명함수 실행
    // Determine the content to be displayed in the browser in main, pass the content to be displayed in the browser to Lambda❗️
    // Set up the message router before starting the server, execute the anonymous function that returns router(req) in set_handler
    //
    my_server.set_handler([&](const std::string& req) {
        return router(req);
    });

    // 서버시작 / Start server
    std::thread server_thread(&Server::start, &my_server); // 서버 시작 / Start server
    std::cout << "--- Starting Server Application ---" << std::endl;


    std::cout << "==============================================\n";


    // -----------------------------------------------------------------------
    //  서버 종료
    // -----------------------------------------------------------------------


    // 2. 메인 스레드는 사용자 입력 대기 / The main thread waits for user input❗️
    // 1)엔터입력시 서버종료하기 / Shut down the server when Enter is pressed
    // std::cin.get();
    //
    // 2)/exit입력시 서버 종료하기 / Shut down the server by entering /exit
    std::string command;
    while (true) {
        std::cout << "CMD> "; // 명령어 프롬프트 표시 / Show command prompt

        // std::getline을 사용하여 공백을 포함한 전체 문자열을 한 번에 받습니다.
        // Use std::getline to get the entire string including spaces at once.
        if (!std::getline(std::cin, command)) {
            // EOF (Ctrl+D 등)로 입력을 받을 수 없는 상황이 발생하면 종료
            break;
        }

        // 입력된 명령어를 확인합니다. / Check the input command
        if (command.empty()) {
            // 아무것도 입력하지 않았으면 그냥 넘어갑니다./ If you don't enter anything, it will just skip.
            continue;
        } else if (command == "/exit" || command == "exit") {

            std::cout << "[INFO] 종료 명령을 받았습니다. 서비스 종료를 시작합니다.\n";
            std::cout << "[INFO] Received a shutdown command. Starting service shutdown.\n";
            break; // 루프 종료 / Loop End

        } else if (command == "/status") {
            std::cout << "[STATUS] 서비스는 정상적으로 작동 중입니다.\n";
            std::cout << "[STATUS] The service is running normally.\n";
        } else {
            std::cout << "[ERROR] 알 수 없는 명령어입니다. (/exit, /status)\n";
            std::cout << "[ERROR] Unknown command. (/exit, /status)\n";
        }
    }

    // 3. 사용자 요청에 의해 서버를 안전하게 종료 ❗️
    // 3. Safely shut down the server at the user's request ❗️
    my_server.stop();

    // 4. 종료 로직 (서버가 종료되지 않기 떄문에 실행되지 않음,나중을 위한 종료 로직)
    // Termination logic (not executed because the server has not terminated, termination logic for later)
    server_thread.join();
    my_server.stop();

    return 0;
}
