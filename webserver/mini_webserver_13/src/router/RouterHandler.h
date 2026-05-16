#pragma once

#include <string>
#include <utility>
#include "../parser/RequestParser.h"
#include "RouterConfig.h"

class RouterHandler {
public:
    // 라우터 선택, 요청 처리, 응답 생성 및 반환을 모두 수행하는 함수 ❗️
    // A function that performs router selection, request processing, and response generation and return
    std::string process_request(const HttpRequest& parsed_req, const std::string& server_id);
};
