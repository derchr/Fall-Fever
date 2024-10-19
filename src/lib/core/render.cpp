#include "render.h"
#include "core/camera.h"
#include "core/graphics/material.h"
#include "core/graphics/mesh.h"
#include "core/shader.h"

#include <spdlog/spdlog.h>

void Render::render(entt::registry& registry)
{
    auto mesh_view = registry.view<GpuMesh const, GpuMaterial const, GlobalTransform const>();
    auto camera_view = registry.view<Camera const, GlobalTransform const>();
    auto camera_entity = camera_view.front();
    
    if (camera_entity == entt::null) {
        spdlog::debug("No camera entity found");
        return;
    }

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
