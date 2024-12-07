cmake_minimum_required(VERSION 3.24)

project(
    Fall-Fever
    VERSION 0.1.0
    DESCRIPTION "A fruit collecting game."
    LANGUAGES C CXX
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(PROJECT_IS_TOP_LEVEL)
    include(cmake/CPM.cmake)

    CPMAddPackage(NAME EnTT URL "https://github.com/skypjack/entt/releases/download/v3.13.2/entt-v3.13.2.tar.gz")
    CPMAddPackage(NAME nlohmann_json URL "https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz")
    CPMAddPackage(NAME glfw3 URL "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip" OPTIONS "GLFW_BUILD_DOCS OFF" "GLFW_INSTALL OFF")
    CPMAddPackage(NAME fx-gltf URL "https://github.com/jessey-git/fx-gltf/archive/refs/tags/v2.0.0.tar.gz" OPTIONS "FX_GLTF_BUILD_TESTS OFF" "FX_GLTF_INSTALL OFF")
    CPMAddPackage(NAME glm URL "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz")
    CPMAddPackage(NAME spdlog URL "https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.tar.gz")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/apps)
