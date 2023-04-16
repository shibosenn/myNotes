//当多个线程同时写入同一个日志文件时，可能会产生竞争条件，导致日志信息写入不完整或者出现重复。为了解决这个问题，可以实现一个线程安全的日志系统。下面是一个简单的实现：

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <thread>

class Logger {
public:
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    void WriteLog(const std::string& message) {
        std::lock_guard<std::mutex> guard(mutex_);
        file_ << message << std::endl;
    }

private:
    Logger() {
        file_.open("log.txt", std::ios::out | std::ios::app);
    }

    ~Logger() {
        file_.close();
    }

private:
    std::mutex mutex_;
    std::ofstream file_;
};

int main() {
    for (int i = 0; i < 5; ++i) {
        std::thread t([&]() {
            Logger& logger = Logger::GetInstance();
            logger.WriteLog("Thread " + std::to_string(i) + " is running.");
        });
        t.join();
    }
}