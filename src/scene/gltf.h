#pragma once

#include "components/transform.h"
#include "core/graphics/material.h"
#include "core/graphics/mesh.h"

#include <entt/entt.hpp>
#include <fx/gltf.h>
#include <optional>
#include <vector>

class Mesh;
class Scene;
class Material;
class Image;

struct GltfPrimitive
{
    entt::resource<Mesh> mesh;
    entt::resource<Material> material;
};

struct GltfMesh
{
    std::vector<GltfPrimitive> primitives;
};

struct GltfCamera
{
    std::variant<fx::gltf::Camera::Perspective, fx::gltf::Camera::Orthographic> projection;
};

struct GltfNode
{
    std::string name;
    Transform transform;
    std::optional<entt::resource<GltfMesh>> mesh;
    std::optional<GltfCamera> camera;
    std::vector<entt::resource<GltfNode>> children;
};

struct Gltf
{
    std::vector<entt::resource<Material>> materials;
    std::vector<entt::resource<GltfMesh>> meshes;
    std::vector<entt::resource<GltfNode>> nodes;
    std::vector<entt::resource<Scene>> scenes;

    std::optional<entt::resource<Scene>> default_scene;
    fx::gltf::Document document;
};
