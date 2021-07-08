#include "Widget.h"
#include "Menu.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Window.h"

#include <GLFW/glfw3.h>

Widget::Widget(std::string &name, Texture *texture, float p_x, float p_y, float p_w, float p_h, uint16_t callbackId)
    : m_posX(p_x), m_posY(p_y), m_width(p_w), m_height(p_h), m_uniqueName(name), m_callbackId(callbackId)
{
    m_widgetTextures.push_back(texture);

    double widgetVerticesData[12] = {
        2 * (m_posX + m_width) - 1.0f,
        2 * (m_posY)-1.0f,
        0.0f, // Bottom right
        2 * (m_posX)-1.0f,
        2 * (m_posY + m_height) - 1.0f,
        0.0f, // Top left
        2 * (m_posX)-1.0f,
        2 * (m_posY)-1.0f,
        0.0f, // Bottom left
        2 * (m_posX + m_width) - 1.0f,
        2 * (m_posY + m_height) - 1.0f,
        0.0f // Top right
    };

    unsigned int widgetIndicesData[6] = {0, 1, 2, 0, 3, 1};

    float widgetTextureCoordinates[8] = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};

    m_widgetVertices = VertexArray::createVertices(widgetVerticesData, 12, widgetTextureCoordinates);
    m_widgetIndices.assign(widgetIndicesData, widgetIndicesData + 6);
    m_widgetMesh = new Mesh(m_widgetVertices, m_widgetIndices, m_widgetTextures);
    m_widgetMesh->initializeOnGPU();
}

Widget::~Widget()
{
    delete m_widgetMesh;
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
    m_widgetMesh->draw(shaderProgram);
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
    if (xrel >= m_posX && xrel <= m_posX + m_width && yrel >= m_posY && yrel <= m_posY + m_height)
        isHovered = true;

    return isHovered;
}
