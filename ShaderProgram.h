#pragma once

#include <glad/glad.h>
#include <string>

class ShaderProgram {

public:

    ShaderProgram(const char* vertexShaderFilename, const char* framentShaderFilename);
    ~ShaderProgram();

    void bind();
    void unbind();

private:

    std::string parse(const char* filename);
    GLuint compile(std::string shaderSource, GLenum type);
    GLuint createShader(const char* vertexShaderFilename, const char* framentShaderFilename);

    GLuint shaderProgramId;

};