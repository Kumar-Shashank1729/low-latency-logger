#pragma once

#include <cstdint>
#include <array>
#include <chrono>
#include <thread>

enum class LogLevel : uint8_t {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct alignas(64) LogEntry {
    uint64_t timestamp;
    uint32_t thread_id;
    LogLevel level;
    uint16_t message_length;
    static constexpr size_t MAX_MESSAGE_LENGTH = 48;
    std::array<char, MAX_MESSAGE_LENGTH> message;

    void captureTimestamp() {
        timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    }

    void setMessage(const char* msg, size_t length) {
        message_length = length < MAX_MESSAGE_LENGTH ? length : MAX_MESSAGE_LENGTH - 1;
        for (size_t i = 0; i < message_length; ++i) {
            message[i] = msg[i];
        }
        message[message_length] = '\0';
    }

    static uint32_t getCurrentThreadId() {
        return static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }

    void init(LogLevel log_level, const char* msg) {
        thread_id = getCurrentThreadId();
        level = log_level;
        captureTimestamp();
        setMessage(msg, std::char_traits<char>::length(msg));
    }
};

static_assert(sizeof(LogEntry) == 64, "LogEntry must be exactly 64 bytes");