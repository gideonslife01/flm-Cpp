#include "../httplib.h"
#include <fstream>
#include <iostream>

int main() {
    httplib::Client cli("http://localhost:5080");

    // 업로드 항목
    httplib::UploadFormData item;
    item.name = "file";
    item.filename = "test.txt";
    item.content_type = "text/plain";

    // 파일 읽기
    std::ifstream ifs("test.txt", std::ios::binary);
    if (!ifs) {
        std::cerr << "cannot open file\n";
        return 1;
    }
    item.content.assign((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());

    httplib::UploadFormDataItems items;
    items.push_back(item);

    httplib::Headers headers; // 필요 시 Authorization 등 추가

    if (auto res = cli.Post("/upload", headers, items)) {
        std::cout << "status: " << res->status << "\n";
        std::cout << res->body << "\n";
    } else {
        std::cerr << "request failed\n";
    }
}
