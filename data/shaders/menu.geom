#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

struct WidgetData
{
    vec2 position;
    vec2 dimensions;
};
uniform WidgetData u_widgetData;

out vec2 v_texCoord;

void main()
{
    float glWidth = u_widgetData.dimensions.x * 2.0f;
    float glHeight = u_widgetData.dimensions.y * 2.0f;

    vec4 offset = vec4(0.0, 0.0, 0.0, 0.0);
    gl_Position = gl_in[0].gl_Position + offset;
    v_texCoord = vec2(0.0, 0.0);
    EmitVertex();

    offset = vec4(glWidth, 0.0, 0.0, 0.0);
    gl_Position = gl_in[0].gl_Position + offset;
    v_texCoord = vec2(1.0, 0.0);
    EmitVertex();

    offset = vec4(0.0, glHeight, 0.0, 0.0);
    gl_Position = gl_in[0].gl_Position + offset;
    v_texCoord = vec2(0.0, 1.0);
    EmitVertex();

    offset = vec4(glWidth, glHeight, 0.0, 0.0);
    gl_Position = gl_in[0].gl_Position + offset;
    v_texCoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}