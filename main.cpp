#include "logger.h"
#include <iostream>

int main() {
    Logger logger;
    std::cout << "Application started. Logging to application.log" << std::endl;
    logger.log(LogLevel::INFO, "Application started successfully.");

    // Your application logic would go here

    logger.log(LogLevel::INFO, "Application is shutting down.");
    std::cout << "Application finished." << std::endl;
    return 0;
}
