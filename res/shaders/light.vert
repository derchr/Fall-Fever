#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_modelViewProjMatrix;

void main() {
    gl_Position = u_modelViewProjMatrix * vec4(a_position, 1.0f);
}
