#version 330 core

struct Material {
    sampler2D u_texture_diffuse0;
    sampler2D u_texture_diffuse1;
};

layout(location = 0) out vec4 f_color;

in vec4 v_color;
in vec2 v_texCoord;

uniform Material material;

uniform float mix_val;

void main() {
    //f_color = v_color;
    vec4 texColor1 = texture(material.u_texture_diffuse0, v_texCoord);
    vec4 texColor2 = texture(material.u_texture_diffuse1, v_texCoord);
    f_color = mix(texColor1, texColor2, mix_val);
}
