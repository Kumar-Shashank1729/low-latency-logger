#pragma once

#include <thread>
#include <atomic>
#include <cstdio>
#include <stdexcept>
#include <chrono>
#include <string>
#include "lock_free_queue.h"
#include "log_entry.h"

class Logger {
public:
    explicit Logger(const char* filename = "application.log");
    ~Logger();

    template <typename... Args>
    void log(LogLevel level, const char* format, Args&&... args) {
        LogEntry entry;
        entry.init(level, format);
        entry.serialize_args(std::forward<Args>(args)...);
        
        int backoff = 1;
        while (!log_queue_.try_push(entry)) {
            if (backoff < 1000) {
                std::this_thread::sleep_for(std::chrono::microseconds(backoff));
                backoff *= 2;
            } else {
                std::this_thread::yield();
            }
        }
    }

private:
    void run();

    DefaultLogQueue log_queue_;
    std::atomic<bool> done_;
    std::thread io_thread_;
    FILE* file_;
    std::string write_buffer_;
};