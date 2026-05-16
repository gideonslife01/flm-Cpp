#include "rtmp_server.h"
#include <iostream>

int main() {
    try {
        asio::io_context io_context;
        RtmpServer s(io_context, 1935);
        std::cout << "RTMP Server listening on 1935...\n";
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
