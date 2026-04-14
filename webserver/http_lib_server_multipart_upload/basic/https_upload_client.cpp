#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../httplib.h"
#include <fstream>
#include <iostream>

int main() {
    // HTTPS client
    httplib::SSLClient cli("host.domain.org", 5080);

    // 업로드 항목/ upload item
    httplib::UploadFormData item;
    item.name = "file";
    item.filename = "test.txt";
    item.content_type = "text/plain";

    std::ifstream ifs("test.txt", std::ios::binary);
    if (!ifs) {
        std::cerr << "cannot open file\n";
        return 1;
    }
    item.content.assign((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());

    httplib::UploadFormDataItems items;
    items.push_back(item);

    httplib::Headers headers;

    if (auto res = cli.Post("/upload", headers, items)) {
        std::cout << "status: " << res->status << "\n";
        std::cout << res->body << "\n";
    } else {
        std::cerr << "request failed\n";
    }
}
