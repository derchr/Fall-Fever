#include "Controller.h"
#include "Helper.h"
#include "util/Log.h"

#include <GLFW/glfw3.h>
#include <iostream>

auto main() -> int
{
#ifndef NDEBUG
    std::cout << "[Debug Mode]" << std::endl;
#endif

    // Initialize GLFW
    if (glfwInit() == 0) {
        Log::logger().critical("Could not initialize GLFW");
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
