add_library(
    imgui STATIC
    imgui.cpp
    imgui_widgets.cpp
    imgui_draw.cpp
    imgui_demo.cpp
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
)

target_link_libraries(imgui glad)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
