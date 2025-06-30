#include "logger.h"
#include "log_entry.h"
#include <chrono>
#include <thread>
#include <cstdio>
#include <stdexcept>

Logger::Logger(const char* filename) : done_(false) {
    file_ = fopen(filename, "w");
    if (!file_) {
        perror("Failed to open log file");
        throw std::runtime_error("Failed to open log file");
    }
    io_thread_ = std::thread([this] { this->run(); });
}

Logger::~Logger() {
    done_ = true;
    io_thread_.join();
    fclose(file_);
}

void Logger::run() {
    LogEntry entry;
    while (!done_ || !log_queue_.empty()) {
        if (log_queue_.try_pop(entry)) {
            entry.format_and_write(file_);
            fflush(file_);
        } else if (!done_) {
            std::this_thread::yield();
        }
    }
}
