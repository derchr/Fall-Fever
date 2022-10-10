#include "ShaderProgram.h"
#include "util/Log.h"

#include <fmt/format.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram(Prototype prototype) : m_shaderProgramId(glCreateProgram())
{
    std::string vertexShaderSource = parse(prototype.vertexPath).value();
    std::string fragmentShaderSource = parse(prototype.fragmentPath).value();

    GLuint vertexShader = compile(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(m_shaderProgramId, vertexShader);
    glAttachShader(m_shaderProgramId, fragmentShader);

    glLinkProgram(m_shaderProgramId);

    GLint isLinked = 0;
    glGetProgramiv(m_shaderProgramId, GL_LINK_STATUS, &isLinked);
    if (isLinked == 0) {
        Log::logger().warn(R"(Failed to link shaderProgram "{}", "{}")", prototype.vertexPath, prototype.fragmentPath);
    }

#ifdef _RELEASE
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
#endif

    Log::logger().info(R"(Loaded shaderprogram "{}")", prototype.name);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_shaderProgramId);
}

void ShaderProgram::bind() const
{
    glUseProgram(m_shaderProgramId);
}

void ShaderProgram::unbind()
{
    glUseProgram(0);
}

auto ShaderProgram::parse(const std::filesystem::path &path) -> std::optional<std::string>
{
    std::fstream shaderfile;
    shaderfile.open(path, std::ios::in);

    if (!shaderfile.is_open()) {
        Log::logger().warn("Shader \"{}\" not found", path.c_str());
        return {};
    }

    std::string contents((std::istreambuf_iterator<char>(shaderfile)), (std::istreambuf_iterator<char>()));

    return contents;
}

auto ShaderProgram::compile(const std::string &shaderSource, GLenum type) -> GLuint
{
    GLuint shaderId = glCreateShader(type);
    const char *src = shaderSource.c_str();
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    int result{};
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);

    if (result != GL_TRUE) {
        int length{};
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);

        std::string message;
        message.reserve(static_cast<unsigned>(length));

        glGetShaderInfoLog(shaderId, length, &length, message.data());
        Log::logger().error("Shader compilation failed: {}", message);

        return 0;
    }

    return shaderId;
}

auto ShaderProgram::retrieveUniformLocation(std::string_view uniform_name) const -> GLint
{
    if (m_uniformLocationCache.find(uniform_name.data()) != m_uniformLocationCache.end()) {
        return m_uniformLocationCache[uniform_name.data()];
    }

    GLint location = glGetUniformLocation(m_shaderProgramId, uniform_name.data());
    m_uniformLocationCache[uniform_name.data()] = location;

    return location;
}

void ShaderProgram::setUniform(const std::string &name, bool value) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform1i(location, (int)value);
}

void ShaderProgram::setUniform(const std::string &name, int value) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string &name, float value) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string &name, glm::vec2 vector) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform2f(location, vector.x, vector.y);
}

void ShaderProgram::setUniform(const std::string &name, glm::vec3 vector) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void ShaderProgram::setUniform(const std::string &name, glm::mat3 matrix) const
{
    GLint location = retrieveUniformLocation(name);
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::setUniform(const std::string &name, glm::mat4 matrix) const
{
    GLint location = retrieveUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

auto ShaderProgram::getShaderProgramId() const -> GLuint
{
    return m_shaderProgramId;
}
