#include <iostream>
#include <string>

#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
    shaderProgramId = createShader(vertexShaderFilename, fragmentShaderFilename);
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

GLuint ShaderProgram::createShader(const char* vertexShaderFilename, const char* framentShaderFilename) {
    std::string vertexShaderSource = parse(vertexShaderFilename);
    std::string fragmentShaderSource = parse(framentShaderFilename);

    GLuint program = glCreateProgram();
    GLuint vs = compile(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fs = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    #ifdef _RELEASE
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
    #endif

    return program;
}

std::string ShaderProgram::parse(const char* filename) {
    FILE* file;
    file = fopen(filename, "rb");
    if(!file) {
        std::cout << "File " << filename << " not found!" << std::endl;
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

void ShaderProgram::setBool(const char *name, bool value) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniform1i(location, (int)value);
}

void ShaderProgram::setInt(const char *name, int value) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniform1i(location, value);
}

void ShaderProgram::setFloat(const char *name, float value) const {
    GLint location = glGetUniformLocation(shaderProgramId, name);
    glUniform1f(location, value);
}
