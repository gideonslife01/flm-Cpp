#include "httplib.h"
#include <atomic>
#include <iostream>
#include <string>
#include <thread>

int main() {
    httplib::ws::WebSocketClient ws("ws://localhost:5080/ws");
    if (!ws.connect()) {
        std::cerr << "connect failed\n";
        return 1;
    }

    std::atomic<bool> running{true};

    // 수신 스레드 / receive thread
    std::thread reader([&]() {
        std::string msg;
        while (running && ws.read(msg)) {
            std::cout << "[recv] " << msg << '\n';
        }
        running = false;
    });

    std::cout << "connected. type message and press Enter.\n";
    std::cout << "type /quit to exit.\n";

    // 송신 루프 / Send loop
    std::string line;
    while (running && std::getline(std::cin, line)) {
        if (line == "/quit") break;
        // ws.send(line);
        ws.send("⭐️" + line);
    }

    running = false;
    ws.close();
    if (reader.joinable()) reader.join();

    std::cout << "closed\n";
    return 0;
}
