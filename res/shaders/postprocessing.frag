#version 330 core

const float GAMMA = 2.0f;

layout(location = 0) out vec4 f_color;

in vec2 v_tex_coords;

uniform sampler2D u_texture;

void main() {

    vec3 fragmentColor = vec3(texture2D(u_texture, v_tex_coords));

    // Gamma correction
    fragmentColor = pow(fragmentColor, vec3(1.0/GAMMA));

    f_color = vec4(fragmentColor, 1.0f);

}