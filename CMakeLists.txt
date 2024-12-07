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

find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(fx-gltf REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)

add_library(fever_core
    src/components/transform.cpp
    src/core/application.cpp
    src/core/camera.cpp
    src/core/glad.cpp
    src/core/graphics/framebuffer.cpp
    src/core/graphics/image.cpp
    src/core/graphics/material.cpp
    src/core/graphics/mesh.cpp
    src/core/light.cpp
    src/core/render.cpp
    src/core/shader.cpp
    src/core/time.cpp
    src/input/input.cpp
    src/scene/gltf.cpp
    src/scene/gltf_loader.cpp
    src/util/log.cpp
    src/window/window.cpp
)

target_compile_features(fever_core PUBLIC cxx_std_20)
target_include_directories(fever_core PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(
    fever_core PUBLIC
    glad
    stb
    glfw
    EnTT::EnTT
    spdlog::spdlog
    glm::glm
    fx-gltf::fx-gltf
    nlohmann_json::nlohmann_json
)

add_subdirectory(${PROJECT_SOURCE_DIR}/apps)
