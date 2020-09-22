add_executable(Fall-Fever
    main.cpp
    Controller.cpp
    Window.cpp
    EventHandler.cpp
    ShaderProgram.cpp
    VertexArray.cpp
    Texture.cpp
    Camera.cpp
    Mesh.cpp
    Model.cpp
    Entity.cpp
    Light.cpp
    World.cpp
    Framebuffer.cpp
    Shadows.cpp
)

target_link_libraries(
    Fall-Fever PRIVATE 
    glad
    glm
    stb
    ${GLFW3_LIBRARY}
    ${OPENGL_LIBRARIES}
    imgui
)

target_compile_options(Fall-Fever PRIVATE -Wall -Wextra -pedantic)
