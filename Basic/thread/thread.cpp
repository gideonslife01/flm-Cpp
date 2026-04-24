// 콘솔에 메시지를 출력할 수 있습니다.
// You can print messages to the console.
#include <iostream> 

// 쓰레드를 생성하고 관리할 수 있습니다.
// You can create and manage threads.
#include <thread> 

// 시간 지연을 위해 필요합니다, 시간 관련 기능을 제공합니다.
// It is needed for time delay, and provides time-related functions.
#include <chrono> 

// 1. 쓰레드에서 실행될 함수 정의
// Define a function that will be executed in the thread
void worker_function(int id, const std::string& message, int delay_seconds) {

    std::cout << "Worker [" << id << "] 이 작업을 시작합니다. 메시지: " << message << std::endl;
    std::cout << "Worker [" << id << "] Starting this job. Message: " << message << "\n" << std::endl;
    
    // 지정된 시간 동안 대기 
    // Wait for a specified amount of time 
    std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));
    
    std::cout << "Worker [" << id << "] 작업 완료. 시간 경과: " << delay_seconds << "초.\n";
    std::cout << "Worker [" << id << "] Task completed. Time elapsed: " << delay_seconds << " seconds.\n";
}

int main() {

    std::cout << "--- Thread Start---\n";

    // 2. std::thread 객체 생성 및 함수 실행
    // Create std::thread objects and execute the function

    // std::thread(함수 포인터, 인자1, 인자2, ...) 형태로 사용합니다.
    // You can use std::thread in the form of std::thread(function pointer, arg1, arg2, ...)


    // Worker 1 스레드 생성 
    // Create Worker 1 thread ⭐️
    std::thread t1(worker_function, 1, "frist background work", 3);
    
    // Worker 2 스레드 생성 (t1과 동시에 실행됨)
    // Create Worker 2 thread (runs concurrently with t1) ⭐️
    std::thread t2(worker_function, 2, "second background work", 1);

    // 3. 쓰레드 시작 후 메인 스레드가 즉시 다음 코드를 실행함을 확인
    std::cout << "\n[Main Thread] 두 개의 쓰레드를 성공적으로 시작했습니다.\n";
    std::cout << "[Main Thread] 쓰레드들이 돌아가도록 기다리는 동안, 메인 스레드는 다른 작업을 수행합니다.\n";
    std::cout << "\n[Main Thread] Two threads started successfully.\n";
    std::cout << "[Main Thread] While waiting for the threads to return, the main thread performs other tasks.\n";
    std::cout << "\n";
    
    // 4. join()을 사용하여 쓰레드가 끝날 때까지 메인 스레드를 대기시킵니다.
    // Use join() to make the main thread wait until the threads finish.

    // 만약 join()을 사용하지 않고 프로그램이 종료되면, 쓰레드는 강제 종료될 위험이 있습니다.
    // If you do not use join() and the program ends, there is a risk that the threads will be forcibly terminated.
    
    // t1이 끝날 때까지 기다림
    // Wait for t1 to finish
    t1.join(); 
    
    // t2가 끝날 때까지 기다림
    // Wait for t2 to finish
    t2.join();

    // 모든 쓰레드가 끝난 후에 실행되는 코드
    // Code that runs after all threads have completed
    std::cout << "--- Thread End ---\n";

    return 0;
}

