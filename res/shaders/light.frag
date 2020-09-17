#version 330 core

layout(location = 0) out vec4 f_color;

uniform vec3 v_lightColor;

void main() {
    f_color = vec4(v_lightColor, 1.0f);
}
