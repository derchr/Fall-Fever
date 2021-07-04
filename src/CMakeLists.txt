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
    FrameBuffer.cpp
    Widget.cpp
    Screen.cpp
    Menu.cpp
    JsonParser.cpp
    Helper.cpp
)

target_link_libraries(
    Fall-Fever PRIVATE 
    glad
    glm
    stb
    jsoncpp
    ${GLFW3_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    imgui
    pthread
)

target_compile_options(Fall-Fever PRIVATE -Wall -Wextra -pedantic)
