#version 330 core

layout(location = 0) out vec4 f_color;

in vec2 v_texCoord;

struct Material
{
    sampler2D texture_diffuse0;
};
uniform Material u_material;

void main()
{
    f_color = texture(u_material.texture_diffuse0, v_texCoord);
}
