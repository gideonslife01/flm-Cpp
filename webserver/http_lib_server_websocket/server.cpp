#include "httplib.h"
#include <iostream>
#include <string>

/* --- 소켓 기본형, 접속자에게만 메세지 전송 / Basic socket type, sends messages only to the connected user --- */
int main() {
    httplib::Server svr;
    // 브라우저 / Browser
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"(
        <!doctype html>
        <html><body>
        <h1>WebSocket Browser Test</h1>
        <input id="msg" value="hello">
        <button id="send">Send</button>
        <pre id="log"></pre>
        <script>
        const log = document.getElementById('log');
        const ws = new WebSocket('ws://localhost:5080/ws');

        ws.onopen = () => log.textContent += '[connected]\n';
        ws.onmessage = (e) => log.textContent += '[recv] ' + e.data + '\n';
        ws.onclose = () => log.textContent += '[closed]\n';
        ws.onerror = () => log.textContent += '[error]\n';

        document.getElementById('send').onclick = () => {
        const v = document.getElementById('msg').value;
        ws.send(v);
        log.textContent += '[send] ' + v + '\n';
        };
        </script>
        </body></html>
        )", "text/html; charset=utf-8");
    });

    // 웹소켓 / websocket
    svr.WebSocket("/ws", [](const httplib::Request&, httplib::ws::WebSocket& ws) {
        // 클라이언트에서 받은 메세지를 다시 모든 클라이언트에게 전송
        // Send received message from client to all clients
        std::string msg;
        while (ws.read(msg)) {
            ws.send("Echo: " + msg);
        }
    });

    std::cout << "WebSocket server: ws://0.0.0.0:5080/ws\n";
    svr.listen("0.0.0.0", 5080);
}
