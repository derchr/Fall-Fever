#include "Controller.h"
#include "log.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <spdlog/spdlog.h>

auto main() -> int
{
    Log::initialize();

    // Initialize GLFW
    if (glfwInit() == 0) {
        spdlog::critical("Could not initialize GLFW");
        return -1;
    }

    {
        // Create controller
        Controller controller;
        controller.run();
    }

    glfwTerminate();
    return 0;
}
