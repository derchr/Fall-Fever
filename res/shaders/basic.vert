#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;

out vec3 v_normal;
out vec2 v_texCoord;

out vec3 v_fragmentPosition;
out vec4 v_fragmentPositionLightSpace;

uniform mat4 u_modelViewProjMatrix;
uniform mat4 u_modelMatrix;

uniform mat4 u_lightViewProjMatrix;

void main() {
    gl_Position = u_modelViewProjMatrix * vec4(a_position, 1.0f);

    v_fragmentPosition = vec3(u_modelMatrix * vec4(a_position, 1.0f));
    v_fragmentPositionLightSpace = u_lightViewProjMatrix * vec4(v_fragmentPosition, 1.0);
    
    v_normal = a_normal;
    v_texCoord = a_texCoord;
}
