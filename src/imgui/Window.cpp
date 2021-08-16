#include "Window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Imgui::Window::Window(const std::string &title) : m_title(title)
{}

void Imgui::Window::render()
{
    ImGui::Begin(m_title.c_str());
    addWidgets();
    ImGui::End();
}