#include "Controller.h"

#include <GLFW/glfw3.h>
#include <iostream>

auto main(int argc, char **argv) -> int
{
    // Suppress warning about unused variable
    (void)argc;
    (void)argv;

#ifndef NDEBUG
    std::cout << "[Debug Mode]" << std::endl;
#endif

    {
        // Create controller
        Controller controller;
        controller.run();
    }

    glfwTerminate();
    return 0;
}
