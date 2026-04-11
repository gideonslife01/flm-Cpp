#include "../httplib.h"
#include <iostream>

int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello HTTP", "text/plain");
    });

    std::cout << "Listening on http://0.0.0.0:5080\n";
    svr.listen("0.0.0.0", 5080);
}
