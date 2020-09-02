#pragma once

#include <glad/glad.h>
#include <string>

class ShaderProgram {

public:

    ShaderProgram(const char* vertexShaderPath, const char* framentShaderPath);
    ~ShaderProgram();

    void bind();
    void unbind();

    // May be rewritten...
    void setBool(const char *name, bool value) const;  
    void setInt(const char *name, int value) const;   
    void setFloat(const char *name, float value) const;

    GLuint getShaderProgramId() { return shaderProgramId; }

private:

    std::string parse(const char* filename);
    GLuint compile(std::string shaderSource, GLenum type);
    GLuint createShader(const char* vertexShaderPath, const char* framentShaderPath);

    GLuint shaderProgramId;

};