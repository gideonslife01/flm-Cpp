#include "httplib.h"
#include <chrono>
#include <string>
#include <thread>

int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"(
<!doctype html><html><body>
<h1>SSE Test</h1>
<pre id="log"></pre>
<script>
const log = document.getElementById('log');
const es = new EventSource('/events'); // same-origin
es.onopen = () => log.textContent += "[connected]\n";
es.onmessage = (e) => log.textContent += e.data + "\n";
es.onerror = () => log.textContent += "[reconnecting]\n";
</script>
</body></html>
)", "text/html; charset=utf-8");
    });

    svr.Get("/events", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");
        res.set_chunked_content_provider("text/event-stream; charset=utf-8",
            [n = 0](size_t, httplib::DataSink& sink) mutable {
                std::string msg = "data: tick " + std::to_string(n++) + "\n\n";
                sink.write(msg.c_str(), msg.size());
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return true; // 계속 연결 유지
            });
    });

    svr.set_keep_alive_max_count(1000);
    svr.set_keep_alive_timeout(60);

    std::cout << "SSE server listening on http://0.0.0.0:5080\n";
    svr.listen("0.0.0.0", 5080);
    return 0;
}
