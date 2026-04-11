#include "../httplib.h"
#include <iostream>

int main() {
    httplib::Client cli("http://127.0.0.1:5080");

    if (auto res = cli.Get("/")) {
        std::cout << "status: " << res->status << "\n";
        std::cout << "body: " << res->body << "\n";
    } else {
        std::cerr << "request failed\n";
    }
}
