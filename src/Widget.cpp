#include "Widget.h"
#include "VertexArray.h"

Widget::Widget(std::string &name, Texture *texture, float p_x, float p_y, float p_w, float p_h) :
    x(p_x),
    y(p_y),
    w(p_w),
    h(p_h),
    unique_name(name)
{
    widgetTextures.push_back(texture);
    
    double widgetVerticesData[12] = {
        2 * (x + w) - 1.0f,  2 * (y) - 1.0f,      0.0f,   // Bottom right
        2 * (x) - 1.0f,      2 * (y + h) - 1.0f,  0.0f,   // Top left
        2 * (x) - 1.0f,      2 * (y) - 1.0f,      0.0f,   // Bottom left
        2 * (x + w) - 1.0f,  2 * (y + h) - 1.0f,  0.0f    // Top right
    };
    
    unsigned int widgetIndicesData[6] = {
        0, 1, 2,
        0, 3, 1
    };
    
    float widgetTextureCoordinates[8] = {
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f
    };
    
    widgetVertices = VertexArray::createVertices(widgetVerticesData, 12, widgetTextureCoordinates);
    widgetIndices.assign(widgetIndicesData, widgetIndicesData + 6);
    widgetMesh = new Mesh(widgetVertices, widgetIndices, widgetTextures);
}

Widget::~Widget()
{
    delete widgetMesh;
}

std::string Widget::getUniqueName()
{
    return unique_name;
}

void Widget::draw(ShaderProgram *shaderProgram)
{
    shaderProgram->bind();
    widgetMesh->draw(shaderProgram);
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
    if(xrel >= x && xrel <= x + w && yrel >= y && yrel <= y + h)
        isHovered = true;

    return isHovered;
}
