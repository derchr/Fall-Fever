#include "Widget.h"
#include "Menu.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Window.h"

#include <GLFW/glfw3.h>

Widget::Widget(Prototype prototype, Texture *texture)
    : m_position(prototype.position), m_dimensions(prototype.dimensions), m_uniqueName(prototype.name),
      m_callbackId(prototype.callBackId)
{
    m_widgetTextures.push_back(texture);
}

Widget::~Widget()
{
    for (auto &texture : m_widgetTextures)
        delete texture;
}

std::string Widget::getUniqueName()
{
    return m_uniqueName;
}

uint16_t Widget::getCallbackId()
{
    return m_callbackId;
}

void Widget::draw(ShaderProgram *shaderProgram)
{
    shaderProgram->bind();
    shaderProgram->setUniform("u_widgetData.position", m_position);
    shaderProgram->setUniform("u_widgetData.dimensions", m_dimensions);

    GLint wireframe;
    glGetIntegerv(GL_POLYGON_MODE, &wireframe);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_widgetTextures[0]->textureId());

    GLint location = glGetUniformLocation(shaderProgram->getShaderProgramId(), "u_texture");
    glUniform1i(location, 0);

    // A VAO is necessary although no data is stored in it
    GLuint temp_vao;
    glGenVertexArrays(1, &temp_vao);
    glBindVertexArray(temp_vao);
    glDrawArrays(GL_POINTS, 0, 3);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, wireframe);

    shaderProgram->unbind();
}

bool Widget::isHovered(Window *window)
{
    double xpos, ypos, width, height;
    glfwGetCursorPos(window->getGLFWwindow(), &xpos, &ypos);
    width = window->getWindowWidth();
    height = window->getWindowHeight();

    double xrel = xpos / width;
    double yrel = -ypos / height + 1;

    bool isHovered = false;
    if (xrel >= m_position.x && xrel <= m_position.x + m_dimensions.x && yrel >= m_position.y &&
        yrel <= m_position.y + m_dimensions.x)
        isHovered = true;

    return isHovered;
}
