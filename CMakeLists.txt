cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0135 NEW)

project(
    Fall-Fever
    VERSION 0.1.0
    DESCRIPTION "A fruit collecting game."
    LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# EnTT
FetchContent_Declare(
    entt
    URL https://github.com/skypjack/entt/archive/refs/tags/v3.11.1.tar.gz
)

FetchContent_MakeAvailable(entt)

# GLFW
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
)

option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
FetchContent_MakeAvailable(glfw)


# fx-gltf
FetchContent_Declare(
    fx-gltf
    URL https://github.com/jessey-git/fx-gltf/archive/refs/tags/v2.0.0.tar.gz
)

option(FX_GLTF_BUILD_TESTS "" OFF)
option(FX_GLTF_INSTALL "" OFF)
FetchContent_MakeAvailable(fx-gltf)

find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
