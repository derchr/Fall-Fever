#include "material.h"
#include "ShaderProgram.h"

GpuMaterial::GpuMaterial(Material const &material)
{
    int texture_unit_counter = 0;

    if (material.base_color_texture.has_value()) {
        Binding binding{.uniform_name = "u_material.texture_diffuse",
                        .texture_unit = texture_unit_counter++};
        base_color_texture = std::make_pair(GpuImage(material.base_color_texture.value()), binding);
    }

    if (material.normal_map_texture.has_value()) {
        Binding binding{.uniform_name = "u_material.texture_normal",
                        .texture_unit = texture_unit_counter++};
        normal_map_texture = std::make_pair(GpuImage(material.normal_map_texture.value()), binding);
    }
}
void GpuMaterial::bind(ShaderProgram const &shader_program) const
{
    auto bind_texture = [&shader_program](auto const &texture) {
        if (texture.has_value()) {
            shader_program.setUniform(texture->second.uniform_name, texture->second.texture_unit);
            glActiveTexture(GL_TEXTURE0 + texture->second.texture_unit);
            glBindTexture(GL_TEXTURE_2D, texture->first.texture);
        }
    };

    bind_texture(base_color_texture);
    bind_texture(normal_map_texture);
}
