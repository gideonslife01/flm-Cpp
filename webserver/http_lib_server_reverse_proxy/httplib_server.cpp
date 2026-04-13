#include "httplib.h"
#include <iostream>

int main()
{
  httplib::Server svr;

  svr.Get("/",[](const httplib::Request &req, httplib::Response &res){
      res.set_content("<h1>Hi! silver hand!!!👋</h1>", "text/html; charset=utf-8");
  });

  svr.Get("/api",[](const httplib::Request &req, httplib::Response &res){
   //res.set_content("Hello Red Dead Redemption!","text/plain");
   res.set_content("<h1>👉🏻Hello Red Dead Redemption-reverse proxy ok!!</h1>", "text/html; charset=utf-8");
  });

  std::cout << "cpp-httplib server listening on http://localhost:3000" << std::endl;
  svr.listen("0.0.0.0", 3000);

  return 0;
}
