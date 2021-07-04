#pragma once

#include <GLFW/glfw3.h>
#include <chrono>
#include <stdint.h>
#include <string>

namespace Helper {
void sleep(uint32_t us);

void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                       const void *userParam);

class Timer
{
public:
    Timer(const std::string &name);
    ~Timer();

private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start, end;
    std::chrono::duration<float> duration;
};

} // namespace Helper
