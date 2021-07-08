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

    double widgetVerticesData[12] = {
        2 * (m_position.x + m_dimensions.x) - 1.0f,
        2 * (m_position.y) - 1.0f,
        0.0f, // Bottom right
        2 * (m_position.x) - 1.0f,
        2 * (m_position.y + m_dimensions.y) - 1.0f,
        0.0f, // Top left
        2 * (m_position.x) - 1.0f,
        2 * (m_position.y) - 1.0f,
        0.0f, // Bottom left
        2 * (m_position.x + m_dimensions.x) - 1.0f,
        2 * (m_position.y + m_dimensions.y) - 1.0f,
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
    if (xrel >= m_position.x && xrel <= m_position.x + m_dimensions.x && yrel >= m_position.y &&
        yrel <= m_position.y + m_dimensions.x)
        isHovered = true;

    return isHovered;
}
