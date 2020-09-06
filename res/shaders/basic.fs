#version 330 core

layout(location = 0) out vec4 f_color;

struct Material {
    sampler2D u_texture_diffuse0;
    sampler2D u_texture_diffuse1;
    sampler2D u_texture_specular0;
    sampler2D u_texture_specular1;
};

in vec4 v_color;
in vec2 v_texCoord;

uniform vec3 v_objectColor;
uniform vec3 v_lightColor;

uniform Material material;


void main() {
    //f_color = v_color;
    vec4 texColor1 = texture(material.u_texture_diffuse0, v_texCoord);
    //vec4 texColor2 = texture(material.u_texture_diffuse1, v_texCoord);
    //f_color = vec4(v_objectColor * v_lightColor, 1.0f);
    f_color = texColor1;
}
