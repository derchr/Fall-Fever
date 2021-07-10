#include "Handler.h"
#include "Window.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Imgui::Handler::Handler(GLFWwindow *window) : m_GLFWwindow(window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.IniFilename = nullptr;

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_GLFWwindow, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

Imgui::Handler::~Handler()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Imgui::Handler::addImguiWindow(std::shared_ptr<Window> window)
{
    m_windows.push_back(window);
}

void Imgui::Handler::renderWindows()
{
    for (auto window : m_windows)
        window->render();
}