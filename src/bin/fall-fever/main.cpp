#include "controller.h"
#include "util/log.h"

#include <GLFW/glfw3.h>
#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

auto main(int argc, char* argv[]) -> int
{
    Log::initialize();

    argparse::ArgumentParser program("Fall-Fever");
    program.add_argument("model").help("model file to load");

    try {
        program.parse_args(argc, argv);
    } catch (std::exception const& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    auto model = program.get<std::string>("model");

    // Initialize GLFW
    if (glfwInit() == 0) {
        spdlog::critical("Could not initialize GLFW");
        return -1;
    }

    {
        // Create controller
        Controller controller(model);
        controller.run();
    }

    glfwTerminate();
    return 0;
}
