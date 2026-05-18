#include <iostream>
#include <memory>
#include <fstream>
#include <string>

class Logger {
    std::ofstream file_;
public:
    Logger(const std::string& filename) {
        file_.open(filename);
        file_ << "=== 로그 시작 / Log Start ===\n";
        std::cout << filename << " 파일 열림 / File Open\n";
    }

    void log(const std::string& msg) {
        file_ << msg << "\n";
        std::cout << "로그 기록 / log record: " << msg << "\n";
    }

    ~Logger() {
        file_ << "=== 로그 종료 / log exit ===\n";
        file_.close();
        std::cout << "파일 닫힘 / file close\n";
    }
};

int main() {
    {
        // 여기서 파일 열림 /  Open file here
        auto logger = std::make_unique<Logger>("app.log");
        logger->log("프로그램 시작 / Start program");
        logger->log("작업 중.../ Working...");
    } // 여기 나가면 자동으로 파일 닫힘 / The file closes automatically when you leave here

    std::cout << "main 끝/end\n";
    return 0;
}
