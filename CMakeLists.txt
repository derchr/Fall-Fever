cmake_minimum_required(VERSION 3.20)

project(
    Fall-Fever
    VERSION 0.1.0
    DESCRIPTION "A fruit collecting game."
    LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
