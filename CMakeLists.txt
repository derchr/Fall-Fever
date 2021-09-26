cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Fall-Fever)

if(WIN32)
    set(GLFW3_ROOT "${PROJECT_SOURCE_DIR}/glfw")
    include_directories(${PROJECT_SOURCE_DIR}/glfw/include)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(GLFW3 REQUIRED)
find_package(OpenGL REQUIRED)
#find_package(Freetype REQUIRED) Will be used in the future

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# Colored output with Ninja
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

if(NOT WIN32)
    add_subdirectory(${PROJECT_SOURCE_DIR}/tools)
endif()
