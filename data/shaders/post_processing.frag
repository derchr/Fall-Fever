#version 330 core

const float GAMMA = 2.2f;
const float EXPOSURE = 1.0f;

layout(location = 0) out vec4 f_color;

in vec2 v_tex_coords;

uniform sampler2D u_texture;

void main() {

    vec3 fragmentColor = vec3(texture2D(u_texture, v_tex_coords));

    // Exposure tone mapping
    fragmentColor = vec3(1.0) - exp(-fragmentColor * EXPOSURE);

    // Gamma correction
    fragmentColor = pow(fragmentColor, vec3(1.0/GAMMA));

    f_color = vec4(fragmentColor, 1.0f);

}
