#include "ShaderProgram.h"

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

ShaderProgram::ShaderProgram(const std::string &name, const std::string &vertexShaderPath,
                             const std::string &fragmentShaderPath)
    : m_uniqueName(name)
{
    std::string vertexShaderSource = parse(vertexShaderPath.c_str());
    std::string fragmentShaderSource = parse(fragmentShaderPath.c_str());

    m_shaderProgramId = glCreateProgram();
    GLuint vs = compile(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fs = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(m_shaderProgramId, vs);
    glAttachShader(m_shaderProgramId, fs);

    glLinkProgram(m_shaderProgramId);

    GLint isLinked = 0;
    glGetProgramiv(m_shaderProgramId, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        std::cout << "Failed to link shaderProgram: " << vertexShaderPath << ", " << fragmentShaderPath << std::endl;
    }

#ifdef _RELEASE
    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
#endif
}

ShaderProgram::ShaderProgram(const std::string &name, const std::string &vertexShaderPath,
                             const std::string &geometryShaderPath, const std::string &fragmentShaderPath)
    : m_uniqueName(name)
{
    std::string vertexShaderSource = parse(vertexShaderPath.c_str());
    std::string geometryShaderSource = parse(geometryShaderPath.c_str());
    std::string fragmentShaderSource = parse(fragmentShaderPath.c_str());

    m_shaderProgramId = glCreateProgram();
    GLuint vs = compile(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint gs = compile(geometryShaderSource, GL_GEOMETRY_SHADER);
    GLuint fs = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(m_shaderProgramId, vs);
    glAttachShader(m_shaderProgramId, gs);
    glAttachShader(m_shaderProgramId, fs);

    glLinkProgram(m_shaderProgramId);

    GLint isLinked = 0;
    glGetProgramiv(m_shaderProgramId, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        std::cout << "Failed to link shaderProgram: " << vertexShaderPath << ", " << geometryShaderPath << ", "
                  << fragmentShaderPath << std::endl;
    }

#ifdef _RELEASE
    glDetachShader(program, vs);
    glDetachShader(program, gs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(gs);
    glDeleteShader(fs);
#endif
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_shaderProgramId);
}

void ShaderProgram::bind()
{
    glUseProgram(m_shaderProgramId);
}

void ShaderProgram::unbind()
{
    glUseProgram(0);
}

std::string ShaderProgram::parse(const std::string &filename)
{
    std::fstream shaderfile;
    shaderfile.open(filename, std::ios::in);

    if (!shaderfile.is_open()) {
        std::cerr << "Shader " << filename << " not found!" << std::endl;
        exit(-1);
    }

    std::string contents((std::istreambuf_iterator<char>(shaderfile)), (std::istreambuf_iterator<char>()));

    return contents;
}

GLuint ShaderProgram::compile(const std::string &shaderSource, GLenum type)
{
    GLuint shaderId = glCreateShader(type);
    const char *src = shaderSource.c_str();
    glShaderSource(shaderId, 1, &src, 0);
    glCompileShader(shaderId);

    int result;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char *message = new char[length];
        glGetShaderInfoLog(shaderId, length, &length, message);
        std::cout << "Shader compile error: " << message << std::endl;
        delete[] message;
        return 0;
    }
    return shaderId;
}

GLint ShaderProgram::retrieveUniformLocation(const std::string &name) const
{
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
        return m_uniformLocationCache[name];

    GLint location = glGetUniformLocation(m_shaderProgramId, name.c_str());
    m_uniformLocationCache[name] = location;

    return location;
}

void ShaderProgram::setUniform(const std::string &name, bool value) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform1i(location, (int)value);
}

void ShaderProgram::setUniform(const std::string &name, int value) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string &name, float value) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string &name, glm::vec3 vector) const
{
    GLint location = retrieveUniformLocation(name);
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void ShaderProgram::setUniform(const std::string &name, glm::mat3 matrix) const
{
    GLint location = retrieveUniformLocation(name);
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::setUniform(const std::string &name, glm::mat4 matrix) const
{
    GLint location = retrieveUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

GLuint ShaderProgram::getShaderProgramId()
{
    return m_shaderProgramId;
}

const std::string &ShaderProgram::getUniqueName()
{
    return m_uniqueName;
}
