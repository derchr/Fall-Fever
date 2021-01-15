#include "Widget.h"
#include "VertexArray.h"

Widget::Widget(Texture *texture, float p_x, float p_y, float p_w, float p_h) :
    x(p_x), y(p_y), w(p_w), h(p_h)
{
    widgetTextures.push_back(texture);
    
    const double ofst = 0.005;
    
    double widgetVerticesData[12] = {
        x + w + ofst,       -1.0f + 2*y - ofst, 0.0f,   // Bottom right
        -1.0f + 2*x - ofst, y + h + ofst,       0.0f,   // Top left
        -1.0f + 2*x-ofst,   -1.0f + 2*y - ofst, 0.0f,   // Bottom left
        x + w + ofst,       y + h + ofst,       0.0f    // Top right
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

void Widget::draw(ShaderProgram *shaderProgram)
{
    shaderProgram->bind();
    widgetMesh->draw(shaderProgram);
    shaderProgram->unbind();
}
