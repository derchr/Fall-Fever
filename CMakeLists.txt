cmake_minimum_required(VERSION 3.10)

project(Fall-Fever)

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

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
    glfw
    glad
    stb
    ${OPENGL_LIBRARIES}
    assimp
)

target_compile_options(Fall-Fever PRIVATE -Wall -Wextra -pedantic)
