#ifndef PROXY_H
#define PROXY_H

#include <string>
#include "../parser/RequestParser.h"

// 리버스 프록시 요청 함수 선언 / Declaration of reverse proxy request function
// backend_ip: 백엔드 서버 IP 주소 / Backend server IP address,
// backend_port: 포트번호 / port number
// client_request: 클라이언트에서 받은 HTTP 요청 전체 문자열 / The entire string of the HTTP request received from the client
// 반환: 백엔드로부터 받은 HTTP 응답 문자열
// Return: HTTP response string received from the backend
std::string forward_request_to_backend(const std::string& backend_ip, int backend_port, const std::string& client_request);
// 리버스 프록시 / Reverse proxy❗️
std::string handle_reverse_proxy(const std::string& req, const HttpRequest& parsed_req);
#endif // PROXY_H
