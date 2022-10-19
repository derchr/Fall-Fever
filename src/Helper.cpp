#include "Helper.h"
#include "util/Log.h"

#include <algorithm>

void Helper::gl_debug_callback(GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei length,
                               const GLchar *message,
                               const void *userParam)
{
    (void)length;
    (void)userParam;

    const char *_source;
    const char *_type;
    const char *_severity;

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

    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
        Log::logger().debug("[OpenGL Debug Message]\n"
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

Helper::Timer::Timer(const std::string &name) : m_name(name)
{
    m_start = std::chrono::high_resolution_clock::now();
}

Helper::Timer::~Timer()
{
    m_end = std::chrono::high_resolution_clock::now();

    m_duration = m_end - m_start;
    float ms = m_duration.count() * 1000.0f;

    Log::logger().info("Timer {} took {}", m_name, ms);
}
