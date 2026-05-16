// RequestParser.h ❗️
#pragma once
#include <string>
#include <map>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string host;
};

class RequestParser {
public:
    static HttpRequest parse(const std::string& raw_request);
};
