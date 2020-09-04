#version 330 core

layout(location = 0) out vec4 f_color;

in vec4 v_color;
in vec2 v_texCoord;

uniform sampler2D u_texture_diffuse0;
uniform sampler2D u_texture_diffuse1;

uniform float mix_val;

void main() {
    //f_color = v_color;
    vec4 texColor1 = texture(u_texture_diffuse0, v_texCoord);
    vec4 texColor2 = texture(u_texture_diffuse1, v_texCoord);
    f_color = mix(texColor1, texColor2, mix_val);
}
