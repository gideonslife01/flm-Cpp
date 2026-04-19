#include "httplib.h"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

/* ---- 모든 소켓 접속자에게 메세지 전송 / Send message to all socket connected users---- */
std::mutex g_mtx;

// g_clients 모든 웹소켓 클라이언트 / g_clients all WebSocket clients
std::vector<httplib::ws::WebSocket*> g_clients;

void broadcast(const std::string& msg) {

    // 스레드에서 같은데이터 건드릴때 충돌막는 안전장치
    // A safety mechanism to prevent conflicts when threads access the same data
    std::lock_guard<std::mutex> lock(g_mtx);
    // 모든 소켓 접속자에게 메세지전달 / Deliver message to all socket users
    for (auto* c : g_clients) {
        c->send(msg);
    }
}

int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"(
        <!doctype html><html><body>
        <input id="msg" value="hello aloy~~👋"><button id="send">Send</button>
        <pre id="log"></pre>
        <script>
        const log = document.getElementById('log');
        const ws = new WebSocket('ws://localhost:5080/ws');
        ws.onmessage = (e) => log.textContent += '[recv] ' + e.data + '\n';
        document.getElementById('send').onclick = () => {
        const v = document.getElementById('msg').value;
        const vplus = "👉🏻" +  v;

        //ws.send(v);
        ws.send(vplus);
        log.textContent += '[send] ' + v + '\n';
        };
        </script></body></html>
        )", "text/html; charset=utf-8");
    });

    svr.WebSocket("/ws", [](const httplib::Request&, httplib::ws::WebSocket& ws) {
        {
            std::lock_guard<std::mutex> lock(g_mtx);
            g_clients.push_back(&ws);
        }

        std::string msg;
        while (ws.read(msg)) {
            broadcast(msg); // 모두에게 전송 / send to everyone
        }

        {
            std::lock_guard<std::mutex> lock(g_mtx);
            g_clients.erase(std::remove(g_clients.begin(), g_clients.end(), &ws), g_clients.end());
        }
    });

    std::cout << "ws server: http://localhost:5080\n";
    svr.listen("0.0.0.0", 5080);
}
