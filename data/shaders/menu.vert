#version 330 core

struct WidgetData
{
    vec2 position;
    vec2 dimensions;
};
uniform WidgetData u_widgetData;

void main()
{
    float glPosX = u_widgetData.position.x * 2.0f - 1.0f;
    float glPosY = u_widgetData.position.y * 2.0f - 1.0f;

    gl_Position = vec4(glPosX, glPosY, 0, 1);
}
