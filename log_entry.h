#pragma once

#include <cstdint>
#include <array>
#include <chrono>
#include <thread>
#include <cstring>
#include <type_traits>
#include <cstdio>

static constexpr size_t MAX_ARG_DATA_SIZE = 256;
static constexpr size_t MAX_ARGS = 8;

enum class LogLevel : uint8_t {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

enum ArgType : uint8_t {
    ARG_INT,
    ARG_DOUBLE,
    ARG_C_STR,
    ARG_LONG_LONG,
    ARG_BOOL,
    ARG_UNKNOWN
};

struct ArgInfo {
    ArgType type;
    uint16_t offset;
    uint16_t size;
};

struct alignas(64) LogEntry {
    uint64_t timestamp;
    uint32_t thread_id;
    LogLevel level;
    const char* format_str;

    std::array<char, MAX_ARG_DATA_SIZE> arg_buffer;
    uint16_t arg_data_size;

    std::array<ArgInfo, MAX_ARGS> arg_infos;
    uint8_t num_args;

    void captureTimestamp() {
        timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    }

    static uint32_t getCurrentThreadId() {
        return static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }

    void init(LogLevel log_level, const char* fmt) {
        thread_id = getCurrentThreadId();
        level = log_level;
        captureTimestamp();
        format_str = fmt;
        arg_data_size = 0;
        num_args = 0;
    }

    template<typename T>
    void serialize_arg(const T& arg) {
        if (arg_data_size + sizeof(T) > MAX_ARG_DATA_SIZE || num_args >= MAX_ARGS) {
            return;
        }
        
        memcpy(arg_buffer.data() + arg_data_size, &arg, sizeof(T));
        
        arg_infos[num_args].offset = arg_data_size;
        arg_infos[num_args].size = sizeof(T);

        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
            if constexpr (std::is_same_v<T, long long>) {
                arg_infos[num_args].type = ARG_LONG_LONG;
            } else {
                arg_infos[num_args].type = ARG_INT;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            arg_infos[num_args].type = ARG_BOOL;
        } else if constexpr (std::is_floating_point_v<T>) {
            arg_infos[num_args].type = ARG_DOUBLE;
        } else {
            arg_infos[num_args].type = ARG_UNKNOWN;
        }

        arg_data_size += sizeof(T);
        num_args++;
    }

    void serialize_arg(const char* arg) {
        size_t len = strlen(arg) + 1;
        if (arg_data_size + len > MAX_ARG_DATA_SIZE || num_args >= MAX_ARGS) {
            return;
        }

        memcpy(arg_buffer.data() + arg_data_size, arg, len);
        arg_infos[num_args].offset = arg_data_size;
        arg_infos[num_args].size = len;
        arg_infos[num_args].type = ARG_C_STR;

        arg_data_size += len;
        num_args++;
    }

    void serialize_arg(const std::string& arg) {
        serialize_arg(arg.c_str());
    }

    void serialize_args() {
    }

    template<typename T, typename... Rest>
    void serialize_args(T&& first, Rest&&... rest) {
        serialize_arg(std::forward<T>(first));
        if constexpr (sizeof...(rest) > 0) {
            serialize_args(std::forward<Rest>(rest)...);
        }
    }

    const char* LogLevelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }

    void format_and_write(FILE* file) const {
        char formatted_message[1024];
        char* current_pos = formatted_message;
        int remaining_size = sizeof(formatted_message);

        int written = snprintf(current_pos, remaining_size, "[%lu] [%u] [%s] ", 
                               timestamp, thread_id, LogLevelToString(level));
        if (written > 0) {
            current_pos += written;
            remaining_size -= written;
        }

        const char* fmt_ptr = format_str;
        int arg_idx = 0;

        while (*fmt_ptr != '\0' && remaining_size > 0) {
            if (*fmt_ptr == '%') {
                fmt_ptr++;
                if (arg_idx < num_args) {
                    const ArgInfo& info = arg_infos[arg_idx];
                    const char* arg_data = arg_buffer.data() + info.offset;

                    switch (info.type) {
                        case ARG_INT:
                            written = snprintf(current_pos, remaining_size, "%d", *reinterpret_cast<const int*>(arg_data));
                            break;
                        case ARG_DOUBLE:
                            written = snprintf(current_pos, remaining_size, "%f", *reinterpret_cast<const double*>(arg_data));
                            break;
                        case ARG_C_STR:
                            written = snprintf(current_pos, remaining_size, "%s", reinterpret_cast<const char*>(arg_data));
                            break;
                        case ARG_LONG_LONG:
                            written = snprintf(current_pos, remaining_size, "%lld", *reinterpret_cast<const long long*>(arg_data));
                            break;
                        case ARG_BOOL:
                            written = snprintf(current_pos, remaining_size, "%s", *reinterpret_cast<const bool*>(arg_data) ? "true" : "false");
                            break;
                        default:
                            written = snprintf(current_pos, remaining_size, "<UNKNOWN>");
                            break;
                    }
                    if (written > 0) {
                        current_pos += written;
                        remaining_size -= written;
                    }
                    arg_idx++;
                } else {
                    written = snprintf(current_pos, remaining_size, "<MISSING_ARG>");
                    if (written > 0) {
                        current_pos += written;
                        remaining_size -= written;
                    }
                }
            } else {
                *current_pos = *fmt_ptr;
                current_pos++;
                remaining_size--;
            }
            fmt_ptr++;
        }
        *current_pos = '\0';

        fprintf(file, "%s\n", formatted_message);
    }
};