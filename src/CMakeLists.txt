add_library(fever_engine
    main.cpp
    Controller.cpp
    Window.cpp
    ShaderProgram.cpp
    VertexArray.cpp
    Camera.cpp
    Mesh.cpp
    Entity.cpp
    Light.cpp
    Scene.cpp
    FrameBuffer.cpp
    Helper.cpp
    resources/Texture.cpp
    resources/Model.cpp
    util/Log.cpp
    image.cpp
    gltf_loader.cpp
)

target_compile_features(fever_engine PUBLIC cxx_std_20)
target_compile_definitions(fever_engine PRIVATE SPDLOG_FMT_EXTERNAL)

target_link_libraries(
    fever_engine PUBLIC
    glad
    glm
    glfw
    EnTT::EnTT
    fmt
    pthread
    spdlog
    fx-gltf::fx-gltf
    stb
    sail
)

add_executable(Fall-Fever
    main.cpp
)

target_link_libraries(Fall-Fever PRIVATE fever_engine)
