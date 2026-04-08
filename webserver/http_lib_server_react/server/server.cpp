#include "httplib.h"
#include <iostream>

int main( ) {
    httplib::Server svr;

    // 기본 라우트 / default route
    svr.Get("/", [](const httplib::Request&, httplib::Response& res ) {
        res.set_content("<h3>hihi! Silver Hand ~~ 👋</h3>", "text/html; charset=utf-8");
    });

    // 리액트 경로 / react path
    if (!svr.set_mount_point("/react", "../www/dist")) {
        std::cerr << "리액트 경로 폴더를 찾을 수 없습니다! 경로를 확인하세요./React path folder not found! Please check the path." << std::endl;
        return 1;
    }

    // API 라우트 / api route
    svr.Get("/api/hello", [](const httplib::Request&, httplib::Response& res ) {
        res.set_content("{\"message\": \"API Server 😉\"}", "application/json; charset=utf-8");
    });

    // 404 에러 핸들러 / 404 error handler
    svr.set_error_handler([](const httplib::Request& req, httplib::Response& res ) {
        if (res.status == 404) {
            // 서버 터미널에 에러 기록 (관리자 확인용)
            std::cerr << "[404 Error] 요청된 경로를 찾을 수 없음: " << req.path << std::endl;

            // 브라우저 화면에 안내 메시지 전송 (사용자 확인용)
            res.set_content(
                "<h1>페이지를 찾을 수 없습니다 / The page cannot be found.😭</h1>"
                "<p>요청하신 '" + req.path + "' 경로는 존재하지 않습니다.</p>"
                "<p>The requested path '" + req.path + "' does not exist.</p>",
                "text/html; charset=utf-8"
            );
        }
    });

    std::cout << "Server Listen http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080 );

    return 0;
}
