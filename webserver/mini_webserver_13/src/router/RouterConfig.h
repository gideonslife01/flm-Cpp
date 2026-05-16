// RouterConfig.h
#pragma once
#include "Router.h"
#include <map>
#include <string>
// ❗️
class RouterConfig {
public:
    static void init_http_routers();
    static void init_https_routers();
    static void init_reverse_routers();

    static std::map<std::string, Router*> vhost_routers;
    static std::map<std::string, Router*> vhost_https_routers;
    static std::map<std::string, Router*> vhost_reverse_routers;

    static Router server1_router;
    static Router server2_router;
    static Router server3_router;
    static Router virtual_router;
};
