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
    resources/Resource.cpp
    resources/ResourceHandler.cpp
    resources/Texture.cpp
    resources/Model.cpp
    resources/CubeMap.cpp
    util/Log.cpp
)

target_compile_features(fever_engine PUBLIC cxx_std_20)
target_compile_definitions(fever_engine PRIVATE SPDLOG_FMT_EXTERNAL)

target_link_libraries(
    fever_engine PUBLIC 
    glad
    glm
    glfw
    glm
    fmt
    pthread
    spdlog
)

add_executable(Fall-Fever
    main.cpp
)

target_link_libraries(Fall-Fever PRIVATE fever_engine)
