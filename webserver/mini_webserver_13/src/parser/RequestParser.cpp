#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include "RequestParser.h"

//❗️ 요청 문자열을 파싱하는 함수 (요청 라인, 헤더, 본문 분리)
// Function to parse the request string (separate request line, header, and body)
HttpRequest RequestParser::parse(const std::string& raw_request) {
    HttpRequest req;
    std::istringstream req_stream(raw_request);
    std::string line;

    // 1. 요청 라인 파싱 (예: "GET /path HTTP/1.1")
    // Parsing request lines (e.g., "GET /path HTTP/1.1")
    if (!std::getline(req_stream, line)) {
        return req;  // 빈 요청 처리 / Empty request processing
    }
    std::istringstream line_stream(line);
    line_stream >> req.method >> req.path >> req.http_version;

    // 2. 헤더 파싱 전에 남은 개행문자 제거용
    // For removing remaining newline characters before header parsing
    std::string dummy;
    std::getline(req_stream, dummy);  // 빈 줄 또는 남은 개행 소비 / Consuming empty lines or remaining newlines

    // 3. 헤더 파싱 / Header parsing
    while (std::getline(req_stream, line) && !line.empty() && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) continue;

        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        // 공백 및 개행문자 제거 / Remove whitespace and newline characters
        key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        req.headers[key] = value;

        if (key == "Host" || key == "host") {
            req.host = value;
        }
    }

    // 4. 본문 파싱 (헤더 뒤 남은 내용 모두) / Parsing the main content (all remaining content after the header)
    std::string body;
    while (std::getline(req_stream, line)) {
        body += line + "\n";
    }
    if (!body.empty() && body.back() == '\n') {
        body.pop_back();  // 마지막 개행 제거 / Remove last newline
    }
    req.body = body;

    return req;
}
