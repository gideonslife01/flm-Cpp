#include <iostream>
#include <memory>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

// --
/*
 - std::make_shared<Session>(std::move(socket)) :
   한 곳만 포인터 공유설정 / Pointer sharing settings for only one location

 - : public std::enable_shared_from_this<Session> :
   shared_from_this(); 사용을 위해 상속
   Inheritance for using shared_from_this();

 - shared_from_this :
   class Session 객체를 복사해서 저장
   Copy and save the class Session object
   ** this가 소멸될 수 있어서 안전한 세션연결 유지를 위해 Session클래스를 하나 더 복사함
      Since 'this' can be destroyed,
      an additional copy of the Session class is made to maintain a safe session connection. **
 */
class Session : public std::enable_shared_from_this<Session> {
    tcp::socket socket_;
    char data_[1024];

public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    void do_read() {

        // 람다 살아있는 동안 Session 안 죽음 / The Session does not die as long as Lambda is alive
        // 이 클래스 객체를 복사해서 저장 / Copy and save this class object
        auto self = shared_from_this();

        /*
         * async_read_some(buffer(data_), 람다/rambda)
         * 1) 데이터 오면 data_에 채워놓고 람다 실행 예약만 한다.
         * When data arrives, fill it into data_ and just schedule the Lambda execution.
         *
         * 2) OS가 소켓에 데이터 들어오는 거 감지 → data_ 배열에 len바이트만큼 복사해 둔다.
         * The OS detects incoming data in the socket and copies len bytes into the data_ array.
         *
         * 3) 복사가 끝났으면 람다 실행
         * Run Lambda once copying is finished
         *
         * 4) 이미 data_에 들어와 있는 변수를 람다에서 사용
         * Using variables already in data_ in Lambda
         */
        socket_.async_read_some(boost::asio::buffer(data_),
            [this, self](boost::system::error_code ec, size_t len) {
                if (!ec) {
                    std::cout << "받음/Receive: " << std::string(data_, len) << "\n";
                    do_write(len); // 에코 / echo
                }
            });
    }

    void do_write(size_t len) {
        auto self = shared_from_this();
        boost::asio::async_write(socket_, boost::asio::buffer(data_, len),
            [this, self](boost::system::error_code ec, size_t) {
                if (!ec) do_read(); // 읽기 / read
            });
    }
};

// --
class Server {
    // socket() + bind() + listen()수행 / Execute socket() + bind() + listen()
    tcp::acceptor acceptor_; // private
public:
    Server(boost::asio::io_context& io, short port)
        : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            // 람다함수 / Lambda function
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    // 에러가 없으면 소켓을 세션에 넘기고 start 호출, 소켓은 복사 안됨
                    // If there are no errors, pass the socket to the session and call start;
                    // the socket is not copied
                    std::make_shared<Session>(std::move(socket))->start();
                }
                // 다음 클라이언트 계속 받음
                // Continue accepting next client
                //
                // 무한루프 / infinite loop
                do_accept();
            });
    }
};

int main() {
    try {
        boost::asio::io_context io;
        Server s(io, 12345);
        std::cout << "서버 시작. 포트 12345 / Start server. Port 12345\n";
        io.run();
    } catch (std::exception& e) {
        std::cerr << "에러/Error: " << e.what() << "\n";
    }
    return 0;
}
