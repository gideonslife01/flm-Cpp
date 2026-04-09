#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>

int main( ) {
    // 인증서와 키 파일 경로를 지정합니다.
    // Specify the certificate and key file paths.
    httplib::SSLServer svr("./cert.pem", "./key.pem" );

    if (!svr.is_valid()) {
        std::cerr << "SSL 서버 설정 실패 / SSL server setup failed!" << std::endl;
        return 1;
    }

    svr.Get("/", [](const httplib::Request&, httplib::Response& res ) {
        res.set_content("<h1> 👉🏻 HTTPS Server is Running! 🔥 </h1>", "text/html; charset=utf-8");
    });

    std::cout << "Server Listening: https://0.0.0.0:5080" << std::endl;
    svr.listen("0.0.0.0", 5080 );

    return 0;
}
