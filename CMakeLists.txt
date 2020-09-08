cmake_minimum_required(VERSION 3.10)

project(Fall-Fever)

if(WIN32)
    set(GLFW3_ROOT "${PROJECT_SOURCE_DIR}/glfw")
    include_directories(${PROJECT_SOURCE_DIR}/glfw/include)
endif(WIN32)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(GLFW3 REQUIRED)
find_package(OpenGL REQUIRED)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

if(!WIN32)
    add_subdirectory(${PROJECT_SOURCE_DIR}/tools)
endif(!WIN32)