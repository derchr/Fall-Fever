#include "controller.h"
#include "util/log.h"

#include <GLFW/glfw3.h>
#include <cxxopts.hpp>
#include <iostream>
#include <spdlog/spdlog.h>

auto main(int argc, char* argv[]) -> int
{
    Log::initialize();

    cxxopts::Options options("Fall-Fever", "A brief description");

    // clang-format off
    options.add_options()
        ("model", "Model file to load", cxxopts::value<std::string>())
        ("h,help", "Print usage")
    ;
    // clang-format on

    options.parse_positional({"model"});

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    std::string model;
    if (result.count("model"))
        model = result["model"].as<std::string>();

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
