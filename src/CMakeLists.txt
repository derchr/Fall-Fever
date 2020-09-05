add_executable(Fall-Fever
    main.cpp
    Controller.cpp
    Window.cpp
    EventHandler.cpp
    ShaderProgram.cpp
    VertexBuffer.cpp
    Texture.cpp
    Camera.cpp
    Mesh.cpp
    Model.cpp
)

target_link_libraries(
    Fall-Fever PRIVATE 
    glad
    stb
    glfw
    ${OPENGL_LIBRARIES}
    assimp
)

target_compile_options(Fall-Fever PRIVATE -Wall -Wextra -pedantic)
