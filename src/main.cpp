#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "Controller.h"

int main(int argc, char **argv)
{
    // Suppress warning about unused variable
    (void)argc;
    (void)argv;

#ifdef _DEBUG
    std::cout << "[Debug Mode]" << std::endl;
#endif

    // Create controller
    Controller *mainController = new Controller();

    const char *fps_env = std::getenv("MAXFPS");
    if (fps_env) {
        uint16_t maxfps = std::stoul(fps_env);
        mainController->setMaxFps(maxfps);
        std::cout << "[Warning] Default max FPS overridden with " << maxfps << " by environment." << std::endl;
    }

    mainController->run();

    delete mainController;
    return 0;
}
