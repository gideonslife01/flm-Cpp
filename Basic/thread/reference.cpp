// 콘솔에 메시지를 출력할 수 있습니다.
// You can print messages to the console.
#include <iostream> 

// 쓰레드를 생성하고 관리할 수 있습니다.
// You can create and manage threads.
#include <thread> 

// 시간 지연을 위해 필요합니다, 시간 관련 기능을 제공합니다.
// It is needed for time delay, and provides time-related functions.
#include <chrono> 

void worker_fun1(std::string msg){
    
    std::cout << "Worker 1: " << msg << "\n"<< std::endl;

    // 시간 지연 / time delay
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

// refrence error
// void worker_fun2(std::string& msg){
//     std::cout << "Worker 1: " << msg << "\n"<< std::endl;
// }

// const reference
void worker_fun2(const std::string& msg){
    std::cout << "Worker 1: " << msg << "\n"<< std::endl;
}

int main() {

    // Thread
    std::thread t1(worker_fun1, "Hello, Silverhand~~ !");
    worker_fun2("hi");
    t1.join();

    // function
    worker_fun2("aloy~~👋");

    return 0;   
}