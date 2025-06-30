#include "logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>

int main() {
    Logger logger;

    std::cout << "Starting Test." << std::endl;


    auto log_info = std::chrono::high_resolution_clock::now();
    logger.log(LogLevel::INFO, "Testing INFO: int value %d. latency: %s", 123,
               (std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - log_info).count()) + " ns").c_str());

    auto log_debug = std::chrono::high_resolution_clock::now();
    logger.log(LogLevel::DEBUG, "Testing DEBUG: float values x=%f, y=%f. latency: %s", 3.14, 2.71,
               (std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - log_debug).count()) + " ns").c_str());

    auto log_warning = std::chrono::high_resolution_clock::now();
    logger.log(LogLevel::WARNING, "Testing WARNING: user '%s' from IP: %s. latency: %s", "Shashank", "127.0.0.1",
               (std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - log_warning).count()) + " ns").c_str());

    auto log_error = std::chrono::high_resolution_clock::now();
    logger.log(LogLevel::ERROR, "Testing ERROR: Code %d, description: %s. latency: %s", 100, " Server Timeout Error",
               (std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - log_error).count()) + " ns").c_str());

    auto log_critical = std::chrono::high_resolution_clock::now();
    logger.log(LogLevel::CRITICAL, "Testing CRITICAL: large value %lld. latency: %s", 9876543210987654321LL,
               (std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - log_critical).count()) + " ns").c_str());

    auto log_bool = std::chrono::high_resolution_clock::now();
    logger.log(LogLevel::INFO, "Testing INFO: boolean value %s. latency: %s", true,
               (std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - log_bool).count()) + " ns").c_str());

    std::string std_str_arg = "This is string.";
    auto start_log_std_string = std::chrono::high_resolution_clock::now();
    logger.log(LogLevel::DEBUG, "Testing DEBUG: string value: %s. latency: %s", std_str_arg,
               (std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start_log_std_string).count()) + " ns").c_str());

    std::string complex_str1 = "Complex String";
    const char* complex_str2 = "C-style String";
    int complex_int = 12345;
    double complex_double = 987.654;
    auto log_complex = std::chrono::high_resolution_clock::now();
    logger.log(LogLevel::WARNING, "Testing WARNING: StdStr: %s, CStr: %s, Int: %d, Double: %f. Latency: %s",
               complex_str1, complex_str2, complex_int, complex_double,
               (std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - log_complex).count()) + " ns").c_str());


    std::cout << "Tests END, check application.log file for results." << std::endl;

    return 0;
}
