#include <iostream>
#include <thread>

void worker(int id) {
    std::cout << "Worker " << id << " started.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Worker " << id << " finished.\n";
}

int main() {
    for(int i = 0; i < 3; ++i) {
        std::thread t(worker, i);

        // 스레드를 분리하여 독립 실행
        // Separate threads for independent execution
        t.detach();  
    }
    
    std::cout << "Main thread finished without waiting.\n";
    // 메인 스레드가 여기서 종료되어도 worker 스레드는 백그라운드에서 계속 동작할 수 있음
    // Even if the main thread terminates here, the worker thread can continue to run in the background

    // 스레드가 작업 할 시간 확보용(디버깅 목적)
    // To secure time for the thread to work (for debugging purposes)
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    return 0;
}