cmake_minimum_required(VERSION 3.10)
project(low_latency_logger)

set(CMAKE_CXX_STANDARD 17)

add_library(low_latency_core STATIC
    logger.cpp
    log_entry.h
    lock_free_queue.h
)

target_link_libraries(low_latency_core PUBLIC pthread)

add_executable(low_latency_app main.cpp)
target_link_libraries(low_latency_app PRIVATE low_latency_core)

add_executable(logger_tests logger_test_suite.cpp)
target_link_libraries(logger_tests PRIVATE low_latency_core)