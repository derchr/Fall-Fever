#include "glad.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

static void gl_debug_callback(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei length,
                              GLchar const* message,
                              void const* userParam)
{
    (void)length;
    (void)userParam;

    char const* _source;
    char const* _type;
    char const* _severity;

    // Remove unwanted newline characters from message string
    std::string _message = message;
    _message.erase(std::remove(_message.begin(), _message.end(), '\n'), _message.end());

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UNDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
        spdlog::debug("[OpenGL Debug Message]\n"
                      "Message: {}\n"
                      "Source: {}\n"
                      "Type: {}\n"
                      "ID: {}\n"
                      "Severity: {}\n",
                      _message,
                      _source,
                      _type,
                      id,
                      _severity);
    }
}

void init_glad()
{
    // Initialize GLAD
    if (gladLoadGL(glfwGetProcAddress) == 0) {
        spdlog::critical("Failed to initialize GLAD");
        std::quick_exit(-1);
    }

#ifndef NDEBUG
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    auto const* gl_version = reinterpret_cast<char const*>(glGetString(GL_VERSION));
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
    spdlog::debug("OpenGL version: {}", gl_version);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&gl_debug_callback, nullptr);
#endif

    // Enable z buffer
    glEnable(GL_DEPTH_TEST);

    // Enable face culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);

    // Enable multisampling (a bit redundant because most graphics drivers do this automatically)
    glEnable(GL_MULTISAMPLE);

    // Disable VSync
    glfwSwapInterval(0);
}
