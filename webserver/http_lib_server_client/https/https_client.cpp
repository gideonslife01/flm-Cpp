#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../httplib.h"
#include <iostream>

int main() {
    httplib::Client cli("https://yourhost.domain.org:5080");

    if (auto res = cli.Get("/")) {
        std::cout << res->status << "\n";
        std::cout << res->body << "\n";
    } else {
        std::cerr << "request failed\n";
    }
}
