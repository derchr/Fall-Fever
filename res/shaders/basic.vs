#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec4 a_color;

out vec3 v_normal;
out vec2 v_texCoord;
out vec4 v_color;

out vec3 v_fragmentPosition;

uniform mat4 u_modelViewProjMatrix;
uniform mat4 u_modelMatrix;

void main() {
    gl_Position = u_modelViewProjMatrix * vec4(a_position, 1.0f);

    v_fragmentPosition = vec3(u_modelMatrix * vec4(a_position, 1.0f));
    
    v_normal = a_normal;
    v_texCoord = a_texCoord;
    v_color = a_color;
}
