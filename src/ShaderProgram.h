#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
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
        std::string geometryPath;
    };

    ShaderProgram(Prototype prototype);

    ~ShaderProgram();

    void bind();
    void unbind();

    GLint retrieveUniformLocation(const std::string &name) const;
    // May be rewritten...
    void setUniform(const std::string &name, bool value) const;
    void setUniform(const std::string &name, int value) const;
    void setUniform(const std::string &name, float value) const;
    void setUniform(const std::string &name, glm::vec3 vector) const;
    void setUniform(const std::string &name, glm::mat3 matrix) const;
    void setUniform(const std::string &name, glm::mat4 matrix) const;

    GLuint getShaderProgramId();
    const std::string &getUniqueName();

private:
    std::string parse(const std::string &filename);
    GLuint compile(const std::string &shaderSource, GLenum type);

    GLuint m_shaderProgramId;
    std::string m_uniqueName;
    mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;
};
