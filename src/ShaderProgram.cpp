#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath) {

    std::string vertexShaderSource = parse(vertexShaderPath);
    std::string fragmentShaderSource = parse(fragmentShaderPath);

    shaderProgramId = glCreateProgram();
    GLuint vs = compile(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fs = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(shaderProgramId, vs);
    glAttachShader(shaderProgramId, fs);

    glLinkProgram(shaderProgramId);

    GLint isLinked = 0;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &isLinked);
    if(!isLinked) std::cout << "Failed to link shaderProgram: " << vertexShaderPath << ", " << fragmentShaderPath << std::endl;

    #ifdef _RELEASE
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
    #endif

}

ShaderProgram::ShaderProgram(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath) {

    std::string vertexShaderSource = parse(vertexShaderPath);
    std::string geometryShaderSource = parse(geometryShaderPath);
    std::string fragmentShaderSource = parse(fragmentShaderPath);

    shaderProgramId = glCreateProgram();
    GLuint vs = compile(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint gs = compile(geometryShaderSource, GL_GEOMETRY_SHADER);
    GLuint fs = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(shaderProgramId, vs);
    glAttachShader(shaderProgramId, gs);
    glAttachShader(shaderProgramId, fs);

    glLinkProgram(shaderProgramId);

    GLint isLinked = 0;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &isLinked);
    if(!isLinked) std::cout << "Failed to link shaderProgram: " << vertexShaderPath << ", " << geometryShaderPath << ", " << fragmentShaderPath << std::endl;

    #ifdef _RELEASE
    glDetachShader(program, vs);
    glDetachShader(program, gs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(gs);
    glDeleteShader(fs);
    #endif

}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(shaderProgramId);
}

void ShaderProgram::bind() {
    glUseProgram(shaderProgramId);
}

void ShaderProgram::unbind() {
    glUseProgram(0);
}

std::string ShaderProgram::parse(const char* filename) {
    FILE* file;
    file = fopen(filename, "rb");
    if(!file) {
        std::cout << "Shader " << filename << " not found!" << std::endl;
        exit(-1);
    }

    std::string contents;
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);
    contents.resize(filesize);

    fread(&contents[0], 1, filesize, file);
    fclose(file);

    return contents;
}

GLuint ShaderProgram::compile(std::string shaderSource, GLenum type) {
    GLuint shaderId = glCreateShader(type);
    const char* src = shaderSource.c_str();
    glShaderSource(shaderId, 1, &src, 0);
    glCompileShader(shaderId);

    int result;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if(result != GL_TRUE) {
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* message = new char[length];
        glGetShaderInfoLog(shaderId, length, &length, message);
        std::cout << "Shader compile error: " << message << std::endl;
        delete[] message;
        return 0;
    }
    return shaderId;
}

void ShaderProgram::setUniform(const char *name, bool value) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniform1i(location, (int)value);
}

void ShaderProgram::setUniform(const char *name, int value) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(const char *name, float value) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(const char *name, glm::vec3 vector) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void ShaderProgram::setUniform(const char *name, glm::mat3 matrix) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::setUniform(const char *name, glm::mat4 matrix) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
