#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class ShaderProgram {

public:

    ShaderProgram(const char* vertexShaderPath, const char* framentShaderPath);
    ~ShaderProgram();

    void bind();
    void unbind();

    // May be rewritten...
    void setUniform(const char *name, bool value) const;  
    void setUniform(const char *name, int value) const;   
    void setUniform(const char *name, float value) const;
    void setUniform(const char *name, glm::vec3 vector) const;
    void setUniform(const char *name, glm::mat4 matrix) const;

    GLuint getShaderProgramId() { return shaderProgramId; }

private:

    std::string parse(const char* filename);
    GLuint compile(std::string shaderSource, GLenum type);
    GLuint createShader(const char* vertexShaderPath, const char* framentShaderPath);

    GLuint shaderProgramId;

};