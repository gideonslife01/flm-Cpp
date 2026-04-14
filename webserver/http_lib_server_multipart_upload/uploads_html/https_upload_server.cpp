#define CPPHTTPLIB_OPENSSL_SUPPORT //
#include "../httplib.h"
#include <fstream>
#include <iostream>

int main() {
    //httplib::Server svr;
    httplib::SSLServer svr("./fullchain.pem", "./privkey.pem");

    if (!svr.is_valid()) {
        std::cerr << "SSL server setup failed\n";
        return 1;
    }
    // html upload form
    if (!svr.set_mount_point("/", "./www")) {
        std::cerr << "경로 폴더를 찾을 수 없습니다! 경로를 확인하세요." << std::endl;
        return 1;
    }

    svr.Post("/upload", [&](const httplib::Request& req, httplib::Response& res,
                            const httplib::ContentReader& content_reader) {
        if (!req.is_multipart_form_data()) {
            res.status = 400;
            res.set_content("Not multipart/form-data", "text/plain");
            return;
        }

        std::string filename;
        std::ofstream ofs;

        content_reader(
            [&](const httplib::FormData& item) {
                if (item.name != "file") return true;

                filename = httplib::sanitize_filename(item.filename);
                if (filename.empty()) {
                    res.status = 400;
                    res.set_content("Invalid filename", "text/plain");
                    return false;
                }
                // file upload directory
                ofs.open("../uploads/" + filename, std::ios::binary);
                if (!ofs) {
                    res.status = 500;
                    res.set_content("Failed to open file", "text/plain");
                    return false;
                }
                return true;
            },
            [&](const char* data, size_t len) {
                if (ofs) ofs.write(data, len);
                return true;
            }
        );

        if (ofs) ofs.close();

        res.set_content("Upload OK", "text/plain");
    });

    std::cout << "Upload server listening on https://0.0.0.0:5080\n";
    svr.listen("0.0.0.0", 5080);
}
