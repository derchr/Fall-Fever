#include "controller.h"
#include "util/log.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

auto main(int argc, char *argv[]) -> int
{
    auto arguments = std::span(argv, argc);

    Log::initialize();

    // Initialize GLFW
    if (glfwInit() == 0) {
        spdlog::critical("Could not initialize GLFW");
        return -1;
    }

    {
        // Create controller
        Controller controller(arguments[1]);
        controller.run();
    }

    glfwTerminate();
    return 0;
}
