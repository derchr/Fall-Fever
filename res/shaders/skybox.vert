#version 330 core

layout(location = 0) in vec3 a_position;

out vec3 v_texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 u_viewProjectionMatrix;

void main() {
    
    gl_Position = u_viewProjectionMatrix * vec4(a_position, 1.0);

    v_texCoord = a_position;

}  