#version 330 core

layout(location = 0) out vec4 f_color;

in vec3 v_texCoord;

uniform samplerCube u_skybox;

void main() {


    vec3 fragmentColor = vec3(texture(u_skybox, v_texCoord));

    f_color = vec4(fragmentColor, 1.0f);

}  