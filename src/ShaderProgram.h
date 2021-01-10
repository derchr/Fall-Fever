#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class ShaderProgram
{
public:
    ShaderProgram(std::string &name, std::string &vertexShaderPath, std::string &fragmentShaderPath);
    ShaderProgram(std::string &name, std::string &vertexShaderPath, std::string &geometryShaderPath, std::string &fragmentShaderPath);
    ~ShaderProgram();

    void bind();
    void unbind();

    // May be rewritten...
    void setUniform(const char *name, bool value) const;
    void setUniform(const char *name, int value) const;
    void setUniform(const char *name, float value) const;
    void setUniform(const char *name, glm::vec3 vector) const;
    void setUniform(const char *name, glm::mat3 matrix) const;
    void setUniform(const char *name, glm::mat4 matrix) const;

    GLuint getShaderProgramId()
    {
        return shaderProgramId;
    }

    std::string getUniqueName()
    {
        return unique_name;
    }

private:
    std::string parse(const char *filename);
    GLuint compile(std::string shaderSource, GLenum type);

    GLuint shaderProgramId;
    std::string unique_name;
};
