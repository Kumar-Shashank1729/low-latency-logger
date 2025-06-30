#!/bin/bash

echo "Configuring project with CMake..."
cmake -S . -B build
echo "Building project..."

if cmake --build build; then
    echo -e "\e[32mBuild successful!\e[0m"
    echo "Running executable..."
    rm -f application.log
    ./build/logger_tests
else
    echo -e "\e[31mBuild failed!\e[0m"
    echo "Check the error messages above."
    exit 1
fi