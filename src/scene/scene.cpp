#include "scene.h"

// void Scene::spawn_scenes(entt::registry& registry)
// {
//     auto scene_view = registry.view<entt::resource<Scene>>();
//     for (auto [entity, scene] : scene_view.each()) {
//         registry.erase<entt::resource<Scene>>(entity);

//         for (auto [id, storage] : scene->registry.storage()) {
//             if (auto* other = registry.storage(id); other && storage.contains(entity)) {
//                 other->push(copy, storage.value(entity));
//             }
//         }
//     }
// }

// Notes:
// By loading a scene from disk, spawn it directly into the world.
// This is less flexible but easier to implement

// Use bevy format: file.gltf#Scene0 otherwise default scene...
