add_executable(Fall-Fever
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
    # imgui/EntityWindow.cpp
    # imgui/GeneralInfoWindow.cpp
    # imgui/Handler.cpp
    # imgui/Window.cpp
)

target_link_libraries(
    Fall-Fever PRIVATE 
    glad
    glm
    stb
    ${GLFW3_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    imgui
    pthread
    spdlog
)

target_compile_options(Fall-Fever PRIVATE -Wall -Wextra -pedantic -fno-exceptions)
