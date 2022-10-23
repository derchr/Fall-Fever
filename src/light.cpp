#include "light.h"
#include "transform.h"

static auto light_active(float illuminance) -> bool
{
    return std::abs(illuminance) >= std::numeric_limits<float>::epsilon();
}

static auto point_light_uniform_base(std::size_t n) -> std::string
{
    return "u_pointLight[" + std::to_string(n) + "].";
}

void Light::update_lights(entt::registry &registry, Shader &shader)
{
    shader.bind();

    // Directional light
    {
        auto directional_lights_view =
            registry.view<DirectionalLight const, GlobalTransform const>();

        entt::entity entity = directional_lights_view.front();
        auto [directional_light, global_transform] = directional_lights_view.get(entity);

        glm::vec4 unit_vector{1.0, 0.0, 0.0, 0.0};
        glm::vec3 direction = glm::vec3(global_transform.transform * unit_vector);

        shader.set_uniform("u_directionalLight.isActive",
                           light_active(directional_light.illuminance));
        shader.set_uniform("u_directionalLight.direction", direction);
        shader.set_uniform("u_directionalLight.color",
                           (directional_light.color * directional_light.illuminance).to_vec3());
    }

    // Point lights
    {
        auto point_lights_view = registry.view<PointLight const, GlobalTransform const>();
        std::size_t num = 0;
        for (auto [entity, point_light, global_transform] : point_lights_view.each()) {
            shader.set_uniform((point_light_uniform_base(num) + "isActive").c_str(),
                               light_active(point_light.intensity));
            shader.set_uniform((point_light_uniform_base(num) + "position").c_str(),
                               global_transform.position());
            shader.set_uniform((point_light_uniform_base(num) + "color").c_str(),
                               (point_light.color * point_light.intensity).to_vec3());

            num++;
        }
    }

    Shader::unbind();
}
