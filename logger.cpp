#include "logger.h"
#include <iostream>

void Logger::info(const char* msg) {
    std::cout << "[INFO] " << msg << std::endl;
}