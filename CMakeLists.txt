cmake_minimum_required(VERSION 3.10)
project(low_latency_logger)

set(CMAKE_CXX_STANDARD 17)

add_executable(low_latency_logger
    main.cpp
    logger.cpp
)