add_library(fever_engine
    components/transform.cpp
    core/graphics/framebuffer.cpp
    core/graphics/image.cpp
    core/graphics/material.cpp
    core/graphics/mesh.cpp
    core/camera.cpp
    core/glad.cpp
    core/light.cpp
    core/render.cpp
    core/shader.cpp
    input/input.cpp
    scene/gltf_loader.cpp
    scene/scene.cpp
    util/log.cpp
    window/Window.cpp
)

target_compile_features(fever_engine PUBLIC cxx_std_20)
target_include_directories(fever_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(fever_engine PRIVATE SPDLOG_FMT_EXTERNAL)

target_link_libraries(
    fever_engine PUBLIC
    glad
    glfw
    EnTT::EnTT
    fmt
    pthread
    spdlog
    fx-gltf::fx-gltf
    stb
)

add_executable(Fall-Fever
    bin/main.cpp
    bin/Controller.cpp
)

target_link_libraries(Fall-Fever PRIVATE fever_engine)
