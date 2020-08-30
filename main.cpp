//#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Controller.h"

int main(int argc, char** argv) {
    // Suppress warning about unused variable
    (void)argc; (void)argv;

    // Create window
    Controller *mainController = new Controller();

    mainController->run();

    delete mainController;
    return 0;
}
