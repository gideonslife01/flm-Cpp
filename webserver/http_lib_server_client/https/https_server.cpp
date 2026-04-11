#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../httplib.h"
#include <iostream>

int main() {
    httplib::SSLServer svr("./fullchain.pem", "./privkey.pem");

    if (!svr.is_valid()) {
        std::cerr << "SSL server setup failed\n";
        return 1;
    }

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("<h1>Hello HTTPS</h1>", "text/html");
    });

    std::cout << "Listening on https://0.0.0.0:5080\n";
    svr.listen("0.0.0.0", 5080);
}
