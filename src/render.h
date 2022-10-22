#pragma once

#include "camera.h"
#include "material.h"
#include "mesh.h"

#include <entt/entt.hpp>

namespace Render {
    
void render(entt::registry &registry)
{
    auto mesh_view = registry.view<GpuMesh const, GpuMaterial const, GlobalTransform const>();
    auto camera_view = registry.view<Camera const, GlobalTransform const>();
    auto camera_entity = camera_view.front();
    auto [camera, camera_transform] = camera_view.get(camera_entity);
    glm::mat4 view_projection_matrix =
        camera.projection_matrix() * Camera::view_matrix(camera_transform);

    for (auto [entity, mesh, material, transform] : mesh_view.each()) {
        auto shader = material.shader;
        shader->bind();

        // Bind textures
        material.bind();

        // Bind modelview matrix uniform
        {
            glm::mat4 modelViewProj = view_projection_matrix * transform.transform;
            shader->set_uniform("u_modelViewProjMatrix", modelViewProj);
            shader->set_uniform("u_modelMatrix", transform.transform);
            shader->set_uniform("u_viewPosition", camera_transform.position());
        }

        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indices_count, mesh.indices_type, nullptr);
        glBindVertexArray(0);

        Shader::unbind();
    }
}

} // namespace Render
