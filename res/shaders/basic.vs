#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;

out vec4 v_color;
out vec2 v_texCoord;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(a_position, 1.0f);
    v_texCoord = a_texCoord;
    v_color = a_color;
}
