add_executable(Fall-Fever
    main.cpp
    Controller.cpp
    Window.cpp
    EventHandler.cpp
    ShaderProgram.cpp
    VertexArray.cpp
    Camera.cpp
    Mesh.cpp
    Entity.cpp
    Light.cpp
    Scene.cpp
    FrameBuffer.cpp
    Widget.cpp
    Screen.cpp
    Menu.cpp
    JsonParser.cpp
    Helper.cpp
    imgui/EntityWindow.cpp
    imgui/GeneralInfoWindow.cpp
    imgui/Handler.cpp
    imgui/Window.cpp
    util/Log.cpp
    resources/Resource.cpp
    resources/ResourceHandler.cpp
    resources/Texture.cpp
    resources/Model.cpp
    resources/CubeMap.cpp
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
    spdlog
)

target_compile_options(Fall-Fever PRIVATE -Wall -Wextra -pedantic)
