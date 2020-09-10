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
    Entity.cpp
)

target_link_libraries(
    Fall-Fever PRIVATE 
    glad
    stb
    ${GLFW3_LIBRARY}
    ${OPENGL_LIBRARIES}
    imgui
)

target_compile_options(Fall-Fever PRIVATE -Wall -Wextra -pedantic)
