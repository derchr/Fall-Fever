cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0135 NEW)

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

include(FetchContent)

# EnTT
FetchContent_Declare(
    entt
    SYSTEM
    URL https://github.com/skypjack/entt/releases/download/v3.13.2/entt-v3.13.2.tar.gz
)

FetchContent_MakeAvailable(entt)

# GLFW
FetchContent_Declare(
    glfw
    SYSTEM
    URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
)

# nlohmann-json
FetchContent_Declare(
    json
    SYSTEM
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_MakeAvailable(json)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_INSTALL OFF)
FetchContent_MakeAvailable(glfw)


# fx-gltf
FetchContent_Declare(
    fx-gltf
    SYSTEM
    URL https://github.com/jessey-git/fx-gltf/archive/refs/tags/v2.0.0.tar.gz
)

set(FX_GLTF_BUILD_TESTS OFF)
set(FX_GLTF_INSTALL OFF)
FetchContent_MakeAvailable(fx-gltf)

# glm
FetchContent_Declare(
    glm
    SYSTEM
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.tar.gz
)

FetchContent_MakeAvailable(glm)

# glm
FetchContent_Declare(
    spdlog
    SYSTEM
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.tar.gz
)

FetchContent_MakeAvailable(spdlog)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
