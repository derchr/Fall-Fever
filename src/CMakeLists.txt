add_library(fever_engine
    main.cpp
    Controller.cpp
    Window.cpp
    scene.cpp
    framebuffer.cpp
    log.cpp
    image.cpp
    mesh.cpp
    glad.cpp
    gltf_loader.cpp
    material.cpp
    camera.cpp
    shader.cpp
    transform.cpp
    light.cpp
    input.cpp
)

target_compile_features(fever_engine PUBLIC cxx_std_20)
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
    main.cpp
)

target_link_libraries(Fall-Fever PRIVATE fever_engine)
