#include "logger.h"
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    Logger log;
    log.info("Logger initialized.");
    return 0;
}