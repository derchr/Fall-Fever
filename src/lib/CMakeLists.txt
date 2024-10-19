add_library(fever_core
    components/transform.cpp
    core/graphics/framebuffer.cpp
    core/graphics/image.cpp
    core/graphics/material.cpp
    core/graphics/mesh.cpp
    core/camera.cpp
    core/game_loop.cpp
    core/glad.cpp
    core/light.cpp
    core/render.cpp
    core/shader.cpp
    core/time.cpp
    input/input.cpp
    scene/gltf.cpp
    scene/gltf_loader.cpp
    util/log.cpp
    window/window.cpp
)

target_compile_features(fever_core PUBLIC cxx_std_20)
target_include_directories(fever_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    fever_core PUBLIC
    glad
    glfw
    EnTT::EnTT
    pthread
    spdlog
    glm::glm
    fx-gltf::fx-gltf
    nlohmann_json::nlohmann_json
    stb
)
