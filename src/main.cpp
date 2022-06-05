#include <GLFW/glfw3.h>
#include <iostream>

#include "Controller.h"

auto main(int argc, char **argv) -> int
{
    // Suppress warning about unused variable
    (void)argc;
    (void)argv;

#ifdef _DEBUG
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
