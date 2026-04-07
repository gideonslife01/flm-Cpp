// 표준라이브러리 폴더안에 있는경우
// If it is inside the standard library folder
// #include <httplib.h>
//
#include "httplib.h"
#include <iostream>

int main()
{
  httplib::Server svr;

  svr.Get("/",[](const httplib::Request &req, httplib::Response &res){
   res.set_content("Hello httplib!","text/plain");
  });

  svr.Get("/hi", [](const httplib::Request &req, httplib::Response &res) {
    res.set_content("<h1>Hi! silver hand!!!👋</h1>", "text/html; charset=utf-8");
  });

  std::cout << "cpp-httplib server listening on http://localhost:8080" << std::endl;
  svr.listen("0.0.0.0", 8080);

  return 0;
}
