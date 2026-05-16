#include "rtmp_server.h"
#include "rtmp_session.h"
#include <iostream>

RtmpServer::RtmpServer(asio::io_context& io_context, short port)
    : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    do_accept();
}

void RtmpServer::do_accept() {
    acceptor_.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "\n=== New OBS Connection ===\n";
                std::make_shared<RtmpSession>(std::move(socket))->start();
            }
            do_accept();
        });
}
