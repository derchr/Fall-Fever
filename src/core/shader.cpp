#include "shader.h"

#include <fmt/format.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

Shader::Shader(std::string_view name, std::filesystem::path const& directory)
    : program(glCreateProgram())
{
    std::filesystem::path vertex_shader_path = directory / name;
    vertex_shader_path.concat(".vert");

    std::filesystem::path frag_shader_path = directory / name;
    frag_shader_path.concat(".frag");

    std::string vertex_shader_source = parse(vertex_shader_path);
    std::string fragment_shader_source = parse(frag_shader_path);

    GLuint vertex_shader = compile(vertex_shader_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile(fragment_shader_source, GL_FRAGMENT_SHADER);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (linked == 0) {
        spdlog::warn(R"(Failed to link Shader "{}")", name);
    }

#ifdef NDEBUG
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
#endif

    spdlog::trace(R"(Loaded Shader "{}")", name);
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

void Shader::bind() const
{
    glUseProgram(program);
}

void Shader::unbind()
{
    glUseProgram(0);
}

auto Shader::parse(std::filesystem::path const& path) -> std::string
{
    std::fstream file;
    file.open(path, std::ios::in);

    if (!file.is_open()) {
        spdlog::critical(R"(Shader "{}" not found!)", path.string());
        std::terminate();
    }

    return {(std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>())};
}

auto Shader::compile(std::string_view source, GLenum type) -> GLuint
{
    GLuint program = glCreateShader(type);
    auto const* src = source.data();
    glShaderSource(program, 1, &src, nullptr);
    glCompileShader(program);

    int result{};
    glGetShaderiv(program, GL_COMPILE_STATUS, &result);

    if (result != GL_TRUE) {
        int length{};
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &length);

        std::string message;
        message.reserve(static_cast<unsigned>(length));

        glGetShaderInfoLog(program, length, &length, message.data());
        spdlog::error("Shader compilation failed: {}", message);

        return 0;
    }

    return program;
}

auto Shader::retrieveUniformLocation(std::string_view uniform_name) const -> GLint
{
    if (uniform_location_cache.find(uniform_name.data()) != uniform_location_cache.end()) {
        return uniform_location_cache[uniform_name.data()];
    }

    GLint location = glGetUniformLocation(program, uniform_name.data());

    if (location != -1) {
        uniform_location_cache[uniform_name.data()] = location;
    } else {
        spdlog::warn(R"(Uniform "{}" not found.)", uniform_name);
    }

    return location;
}

template <typename T> void Shader::set_uniform(std::string_view name, T value) const
{
    GLint location = retrieveUniformLocation(name);

    if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, int>) {
        glUniform1i(location, (int)value);
    } else if constexpr (std::is_same_v<T, unsigned>) {
        glUniform1ui(location, value);
    } else if constexpr (std::is_same_v<T, float>) {
        glUniform1f(location, value);
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
        glUniform2f(location, value.x, value.y); // NOLINT(cppcoreguidelines-pro-type-union-access)
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
        glUniform3f(
            location, value.x, value.y, value.z); // NOLINT(cppcoreguidelines-pro-type-union-access)
    } else if constexpr (std::is_same_v<T, glm::mat3>) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    } else if constexpr (std::is_same_v<T, glm::mat4>) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

template void Shader::set_uniform(std::string_view, bool) const;
template void Shader::set_uniform(std::string_view, int) const;
template void Shader::set_uniform(std::string_view, unsigned) const;
template void Shader::set_uniform(std::string_view, float) const;
template void Shader::set_uniform(std::string_view, glm::vec2) const;
template void Shader::set_uniform(std::string_view, glm::vec3) const;
template void Shader::set_uniform(std::string_view, glm::mat3) const;
template void Shader::set_uniform(std::string_view, glm::mat4) const;
