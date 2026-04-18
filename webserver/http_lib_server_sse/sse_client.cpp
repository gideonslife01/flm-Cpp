#include "httplib.h"

int main() {
    httplib::Client cli("http://localhost:5080");
    httplib::sse::SSEClient sse(cli, "/events");

    sse.on_message([](const httplib::sse::SSEMessage &msg) {
        std::cout << "Event: " << msg.event << std::endl;
        std::cout << "Data: " << msg.data << std::endl;
    });

    sse.start();  // Blocking, with auto-reconnect
    return 0;
}
