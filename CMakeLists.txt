cmake_minimum_required(VERSION 3.10)

project(Fall-Fever)

find_package(glfw3 3.3 REQUIRED)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Fall-Fever main.cpp)

target_link_libraries(Fall-Fever glfw)

target_compile_options(Fall-Fever PRIVATE -Wall -Wextra -pedantic)
