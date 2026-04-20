#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>

#define PORT 5080
#define BACKLOG 10 // 대기 큐 크기 / wait queue size

// 클라이언트와의 통신을 담당하는 함수 (각 사용자를 위한 별도의 쓰레드)
// Function responsible for communication with clients (separate thread for each user)
void handle_client(int client_socket) {
    char buffer[1024];
    std::cout << "[Connection] New client connected." << std::endl;

    // 1. 데이터 수신 / receive data (Read)
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // C-string
        std::string request = buffer;
        std::cout << "[Received] " << request.substr(0, 30) << "..." << std::endl;

        // 2. 응답 전송 / send response(Write)
        const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<h1>👋Hi~ Hi~ Hi~~~!</h1><p>Welcome to the miniWebserver!</p>";
        write(client_socket, response, strlen(response));
    }

    // 3. 연결 종료 / close connection
    close(client_socket);
    std::cout << "[Connection] Client disconnected." << std::endl;
}

int main() {
    // --- 1. 소켓 생성 (Socket Creation) --- //
    // AF_INET(Address Family Internet):
    // IPv4 사용, SOCK_STREAM: TCP 연결 방식 사용, 0: 프로토콜 기본 사용(기본값 사용)
    // Use IPv4, SOCK_STREAM: Use TCP connection method, 0: Use default protocol (use default)
    //
    // AF_INET6: IPv6 사용, SOCK_DGRAM : UDP 연결 방식 사용 / Use UDP connection method
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        // perror() : 어떤 시스템 함수가 실패할 경우 내가 정의한 오류 메세지와 운영체제가 정의한 오류 메세지를 함께 출력해줌.
        perror("socket failed");
        /*
         exit(1)의 동작 결과는 exit(EXIT_FAILURE)가 의도하는 동작 결과와 동일합니다. 즉, 프로그램이 '실패' 상태로 종료된다는 의미가 같습니다.
         exit(0)의 동작 결과는 exit(EXIT_SUCCESS)가 의도하는 동작 결과와 동일합니다. 즉, 프로그램이 '성공' 상태로 종료된다는 의미가 같습니다.
         The result of exit(1) is the same as the intended result of exit(EXIT_FAILURE). In other words, it means the same thing: the program terminates in a 'failed' state.
         The result of exit(0) is the same as the intended result of exit(EXIT_SUCCESS). In other words, it means the same thing: the program terminates in a 'successful' state.
         */
        exit(EXIT_FAILURE);
    }

    // 포트 재사용 옵션 설정 (같은 포트를 재사용 가능하게 함)
    /* setsockopt() : 소켓에설정적용하기 / Applying settings to the socket
     * setsockopt(int sockfd,           // 1. 소켓 식별자 / socket identifier
                int level,              //.2. 프로토콜 레벨 (어느 계층의 설정인지) / Protocol level (which layer's setting)
                int optionname,         // 3. 설정할 옵션의 이름 (어떤 설정을 할지) / Name of the option to set (which setting to make)
                const void *optionval,  // 4. 옵션의 실제 값 (값을 1로 설정할지) / The actual value of the option (whether to set the value to 1)
                socklen_t optionlen);   // 5. 옵션 값의 길이 / Length of the option value

       SOL_SOCKET :
        소켓 통로 자체의 전반적인 옵션 및 속성(소켓통신을 위한 기본 값)
        Overall options and properties of the socket channel itself (default values ​​for socket communication)

       SO_REUSEADDR :
        지금 이 포트는 임시적으로 점유된 상태일지라도, 이 주소를 다시 사용하도록 요청하는 것
        Requesting to use this address again, even if this port is currently temporarily occupied

       opt = 1 : 포트 재사용 허용 / Allow port reuse

    */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // --- 2. 바인딩 (Binding) --- //
    // struct sockaddr_in :
    //   IPv4 주소만 다루기 위한 구체적인 구조체입니다.
    //   This is a specific structure for handling only IPv4 addresses.
    //
    // struct sockaddr_in6 :
    //   IPv6 주소만 다루기 위한 구체적인 구조체입니다.
    //   This is a specific structure designed to handle only IPv6 addresses.
    //
    // struct sockaddr :
    //   이 구조체는 특정 IP 버전(IPv4인지 IPv6인지)에 관계없이 사용할 수 있도록 설계되었습니다.
    //   This structure is designed to be usable regardless of the specific IP version (whether IPv4 or IPv6).

    struct sockaddr_in address;
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스 IP 사용 / Use all interface IPs
    address.sin_port = htons(PORT);       // 포트를 네트워크 바이트 순서로 변환 / Convert ports to network byte order

    // 바로 위에서 선언한 구조체 address에 소켓을 바인딩함.
    // Bind the socket to the structure address declared just above.
    //
    // (struct sockaddr *)&address : &address변수의 값을 (struct sockaddr *) 타입으로 타입 캐스팅함.
    // (struct sockaddr *)&address : Type casts the value of the &address variable to the type (struct sockaddr *).

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // --- 3. 리스닝 (Listening) --- //
    // 최대 대기할 연결 요청 수를 지정 (BACKLOG)
    // Specify the maximum number of connection requests to wait for (BACKLOG)
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // --- 4. 무한 루프: 연결 수락 및 처리 / Accept and handle incoming connections indefinitely --- //
    while (true) {
        // IPv4구조체
        struct sockaddr_in client_address;
        // 주소 구조체가 메모리에서 차지하는 바이트 크기를 저장하는 변수의 타입
        socklen_t addrlen = sizeof(client_address);

        // accept() 함수가 블로킹(Blocking) 상태에서 클라이언트 연결을 기다림
        int client_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);
        if (client_socket < 0) {
            perror("accept failed");
            continue; // 실패했으면 다음 루프를 돌며 재시도 / Retry in the next loop if it fails
        }

        // 5. 동시성 처리: 새로운 연결이 오면 새로운 쓰레드를 생성하여 처리
        // Concurrency Handling: When a new connection arrives, create a new thread to handle it.
        std::thread client_thread(handle_client, client_socket);
        // 쓰레드를 분리하고 메인 루프를 계속 실행하게 함
        // Detach the thread so that the main loop continues running.
        client_thread.detach();
    }

    // 이 부분은 무한 루프가 도달하지 않습니다. / This part is not reached by an infinite loop.
    close(server_fd);
    return 0;
}
