#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

class ShaderProgram
{
public:
    ShaderProgram(const std::string &name, const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
    ShaderProgram(const std::string &name, const std::string &vertexShaderPath, const std::string &geometryShaderPath, const std::string &fragmentShaderPath);
    ~ShaderProgram();

    void bind();
    void unbind();

    GLint retrieveUniformLocation(const std::string& name) const;
    // May be rewritten...
    void setUniform(const std::string& name, bool value) const;
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, glm::vec3 vector) const;
    void setUniform(const std::string& name, glm::mat3 matrix) const;
    void setUniform(const std::string& name, glm::mat4 matrix) const;

public:
    GLuint getShaderProgramId();
    std::string getUniqueName();

private:
    std::string parse(const std::string& filename);
    GLuint compile(const std::string& shaderSource, GLenum type);

private:
    GLuint shaderProgramId;
    std::string unique_name;
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;
};
