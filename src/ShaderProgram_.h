#pragma once

#include <filesystem>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <unordered_map>

class ShaderProgram
{
public:
    struct Prototype
    {
        std::string name;
        std::string vertexPath;
        std::string fragmentPath;
    };

    ShaderProgram(Prototype prototype);
    ~ShaderProgram();

    void bind() const;
    static void unbind();

    auto retrieveUniformLocation(std::string_view uniform_name) const -> GLint;

    // May be rewritten...
    void set_uniform(const std::string &name, bool value) const;
    void set_uniform(const std::string &name, int value) const;
    void set_uniform(const std::string &name, unsigned int value) const;
    void set_uniform(const std::string &name, float value) const;
    void set_uniform(const std::string &name, glm::vec2 vector) const;
    void set_uniform(const std::string &name, glm::vec3 vector) const;
    void set_uniform(const std::string &name, glm::mat3 matrix) const;
    void set_uniform(const std::string &name, glm::mat4 matrix) const;

    auto getShaderProgramId() const -> GLuint;

private:
    static auto parse(const std::filesystem::path &path) -> std::string;
    static auto compile(const std::string &shaderSource, GLenum type) -> GLuint;

    GLuint m_shaderProgramId;
    mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
};
