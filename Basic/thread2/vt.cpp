#include <iostream>
#include <thread>
#include <vector>

void worker(int id) {
    std::cout << "Worker " << id << " started.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Worker " << id << " finished.\n";
}

int main() {
    std::vector<std::thread> threads;

    for(int i = 0; i < 3; ++i) {
        std::thread t(worker, i);

        // 스레드 객체를 벡터에 저장
        // Store thread objects in a vector
        threads.push_back(std::move(t));  
    }
    
    // 모든 스레드가 종료될 때까지 기다림
    // Wait until all threads terminate
    for(auto &t : threads) {
        if(t.joinable()) {
            t.join();
        }
    }

    // 벡터에서 스레드 객체 제거
    // Remove thread objects from vector
    threads.clear(); 
    std::cout << "All workers completed.\n";
    return 0;
}