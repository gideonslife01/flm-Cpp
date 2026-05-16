// RouterConfig.cpp
#include "RouterConfig.h"

Router RouterConfig::server1_router("../public1");
Router RouterConfig::server2_router("../public2");
Router RouterConfig::server3_router("../public");
Router RouterConfig::virtual_router("../public_virtual");

std::map<std::string, Router*> RouterConfig::vhost_routers;
std::map<std::string, Router*> RouterConfig::vhost_https_routers;
std::map<std::string, Router*> RouterConfig::vhost_reverse_routers;

void RouterConfig::init_http_routers() {
    server1_router.add_route("GET","/test1", "test1.html");
    server1_router.add_route("POST","/test2", "test2.html");

    virtual_router.add_route("GET","/test1", "test1.html");
    virtual_router.add_route("POST","/test2", "test2.html");

    vhost_routers = {
        {"yourmainhost.domain.org", &server1_router},
        {"yourvirtualhost.domain.org", &virtual_router},
        {"localhost", &server1_router},
        {"127.0.0.1", &server1_router},
        {"192.168.0.10", &server1_router}
    };
}

void RouterConfig::init_https_routers() {
    server2_router.add_route("GET","/test1", "test1.html");
    server2_router.add_route("POST","/test2", "test2.html");

    virtual_router.add_route("GET","/test1", "test1.html");
    virtual_router.add_route("POST","/test2", "test2.html");

    vhost_https_routers = {
        {"yourmainhost.domain.org", &server2_router},
        {"yourvirtualhost.domain.org", &virtual_router},
        {"localhost", &server2_router},
        {"127.0.0.1", &server2_router},
        {"192.168.0.10", &server2_router}
    };
}

void RouterConfig::init_reverse_routers() {
    server3_router.add_route("GET","/api", "index.html");

    virtual_router.add_route("GET","/test1", "test1.html");
    virtual_router.add_route("POST","/test2", "test2.html");

    vhost_reverse_routers = {
        {"yourmainhost.domain.org", &server3_router},
        {"yourvirtualhost.duckdns.org", &virtual_router},
        {"localhost", &server3_router},
        {"127.0.0.1", &server3_router},
        {"192.168.0.10", &server3_router}
    };
}
