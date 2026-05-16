#pragma once
#include <asio.hpp>

class RtmpServer {
public:
    RtmpServer(asio::io_context& io_context, short port);
private:
    void do_accept();
    asio::ip::tcp::acceptor acceptor_;
};
